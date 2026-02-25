param(
    [string]$Id,
    [string]$Tag,
    [string]$Topic,
    [ValidateSet("open", "done", "deprecated")]
    [string]$Status,
    [DateTime]$From,
    [DateTime]$To,
    [int]$Limit = 50
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-AgentsPaths {
    $repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
    $agentsDir = Join-Path $repoRoot "docs\agents"
    return @{
        AgentsDir = $agentsDir
        Log = Join-Path $agentsDir "memory.log"
        Manifest = Join-Path $agentsDir "archive.manifest.json"
        Archive = Join-Path $agentsDir "archive"
    }
}

function Read-Jsonl {
    param(
        [string[]]$Lines
    )
    $items = @()
    foreach ($line in $Lines) {
        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }
        try {
            $items += ($line | ConvertFrom-Json)
        } catch {
            Write-Warning "Skipping invalid JSONL line."
        }
    }
    return $items
}

function Read-GZipLines {
    param(
        [string]$Path
    )

    $buffer = New-Object System.Collections.Generic.List[string]
    $fileStream = [System.IO.File]::OpenRead($Path)
    try {
        $gzipStream = New-Object System.IO.Compression.GZipStream($fileStream, [IO.Compression.CompressionMode]::Decompress)
        try {
            $reader = New-Object System.IO.StreamReader($gzipStream)
            try {
                while (-not $reader.EndOfStream) {
                    $buffer.Add($reader.ReadLine())
                }
            } finally {
                $reader.Dispose()
            }
        } finally {
            $gzipStream.Dispose()
        }
    } finally {
        $fileStream.Dispose()
    }
    return $buffer
}

function Read-ZstLines {
    param(
        [string]$Path
    )

    $zstd = Get-Command zstd -ErrorAction SilentlyContinue
    if ($null -eq $zstd) {
        Write-Warning "zstd not found. Skipping $Path"
        return @()
    }

    $output = & $zstd.Source "-dc" $Path
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Failed to decompress zstd segment: $Path"
        return @()
    }

    return @($output -split "`r?`n")
}

function Load-AllEntries {
    param(
        [hashtable]$Paths
    )

    $entries = @()

    if (Test-Path $Paths.Log) {
        $entries += Read-Jsonl -Lines (Get-Content -Path $Paths.Log -Encoding UTF8)
    }

    if (Test-Path $Paths.Manifest) {
        $manifest = Get-Content -Path $Paths.Manifest -Raw -Encoding UTF8 | ConvertFrom-Json
        foreach ($segment in @($manifest.segments)) {
            $segmentPath = Join-Path $Paths.Archive ([string]$segment.name)
            if (-not (Test-Path $segmentPath)) {
                continue
            }

            if ($segmentPath.EndsWith(".jsonl", [System.StringComparison]::OrdinalIgnoreCase)) {
                $entries += Read-Jsonl -Lines (Get-Content -Path $segmentPath -Encoding UTF8)
            } elseif ($segmentPath.EndsWith(".gz", [System.StringComparison]::OrdinalIgnoreCase)) {
                $entries += Read-Jsonl -Lines (Read-GZipLines -Path $segmentPath)
            } elseif ($segmentPath.EndsWith(".zst", [System.StringComparison]::OrdinalIgnoreCase)) {
                $entries += Read-Jsonl -Lines (Read-ZstLines -Path $segmentPath)
            }
        }
    }

    return $entries
}

$paths = Get-AgentsPaths
$entries = Load-AllEntries -Paths $paths

$filtered = $entries

if (-not [string]::IsNullOrWhiteSpace($Id)) {
    $filtered = $filtered | Where-Object { $_.id -eq $Id }
}
if (-not [string]::IsNullOrWhiteSpace($Tag)) {
    $filtered = $filtered | Where-Object { @($_.tags) -contains $Tag }
}
if (-not [string]::IsNullOrWhiteSpace($Topic)) {
    $filtered = $filtered | Where-Object { $_.topic -eq $Topic }
}
if (-not [string]::IsNullOrWhiteSpace($Status)) {
    $filtered = $filtered | Where-Object { $_.status -eq $Status }
}
if ($PSBoundParameters.ContainsKey("From")) {
    $filtered = $filtered | Where-Object { [DateTime]$_.ts -ge $From.ToUniversalTime() }
}
if ($PSBoundParameters.ContainsKey("To")) {
    $filtered = $filtered | Where-Object { [DateTime]$_.ts -le $To.ToUniversalTime() }
}

$filtered = $filtered | Sort-Object { [DateTime]$_.ts } -Descending | Select-Object -First $Limit
$filtered | ConvertTo-Json -Depth 8
