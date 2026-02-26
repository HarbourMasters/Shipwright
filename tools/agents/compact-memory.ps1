param(
    [Parameter(Mandatory = $true)]
    [DateTime]$BeforeDate,

    [string]$SegmentName,

    [ValidateSet("zstd", "gzip", "none")]
    [string]$Compression = "zstd"
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

function Ensure-Dir {
    param([string]$Path)
    if (-not (Test-Path $Path)) {
        New-Item -ItemType Directory -Path $Path | Out-Null
    }
}

function Compress-GZip {
    param(
        [string]$Source,
        [string]$Destination
    )
    $inStream = [System.IO.File]::OpenRead($Source)
    try {
        $outStream = [System.IO.File]::Create($Destination)
        try {
            $gzip = New-Object System.IO.Compression.GZipStream($outStream, [IO.Compression.CompressionLevel]::Optimal)
            try {
                $inStream.CopyTo($gzip)
            } finally {
                $gzip.Dispose()
            }
        } finally {
            $outStream.Dispose()
        }
    } finally {
        $inStream.Dispose()
    }
}

function Compute-Sha256 {
    param([string]$Path)
    return (Get-FileHash -Path $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

$paths = Get-AgentsPaths
Ensure-Dir -Path $paths.Archive

if (-not (Test-Path $paths.Log)) {
    throw "memory.log not found: $($paths.Log)"
}

$beforeUtc = $BeforeDate.ToUniversalTime()
$oldRows = New-Object System.Collections.ArrayList
$newRows = New-Object System.Collections.ArrayList

foreach ($line in Get-Content -Path $paths.Log -Encoding UTF8) {
    if ([string]::IsNullOrWhiteSpace($line)) {
        continue
    }
    $obj = $line | ConvertFrom-Json
    $ts = [DateTime]$obj.ts
    if ($ts -lt $beforeUtc) {
        [void]$oldRows.Add($line)
    } else {
        [void]$newRows.Add($line)
    }
}

if ($oldRows.Count -eq 0) {
    Write-Host "No entries older than $($beforeUtc.ToString("u")) to compact."
    exit 0
}

$oldObjects = @($oldRows | ForEach-Object { $_ | ConvertFrom-Json })
$fromId = [string]$oldObjects[0].id
$toId = [string]$oldObjects[$oldObjects.Count - 1].id

if ([string]::IsNullOrWhiteSpace($SegmentName)) {
    $SegmentName = "memory-$($fromId)-$($toId).jsonl"
}

$plainSegmentPath = Join-Path $paths.Archive $SegmentName
$oldRows | Set-Content -Path $plainSegmentPath -Encoding UTF8
$storedPath = $plainSegmentPath
$compressionUsed = "none"

if ($Compression -eq "zstd") {
    $zstd = Get-Command zstd -ErrorAction SilentlyContinue
    if ($null -ne $zstd) {
        $zstPath = "$plainSegmentPath.zst"
        & $zstd.Source "-q" "-f" $plainSegmentPath "-o" $zstPath
        if ($LASTEXITCODE -eq 0) {
            Remove-Item -Path $plainSegmentPath -Force
            $storedPath = $zstPath
            $compressionUsed = "zstd"
        }
    }
}

if ($Compression -eq "gzip" -or ($Compression -eq "zstd" -and $compressionUsed -eq "none")) {
    $gzPath = "$plainSegmentPath.gz"
    Compress-GZip -Source $plainSegmentPath -Destination $gzPath
    Remove-Item -Path $plainSegmentPath -Force
    $storedPath = $gzPath
    $compressionUsed = "gzip"
}

$newRows | Set-Content -Path $paths.Log -Encoding UTF8

$manifest = @{
    version = 1
    segments = @()
}
if (Test-Path $paths.Manifest) {
    $manifest = Get-Content -Path $paths.Manifest -Raw -Encoding UTF8 | ConvertFrom-Json
}

$segmentEntry = [ordered]@{
    name = [IO.Path]::GetFileName($storedPath)
    fromId = $fromId
    toId = $toId
    count = $oldRows.Count
    sha256 = Compute-Sha256 -Path $storedPath
    createdUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")
    compression = $compressionUsed
}

$segments = New-Object System.Collections.ArrayList
foreach ($existing in @($manifest.segments)) {
    [void]$segments.Add($existing)
}
[void]$segments.Add($segmentEntry)

$outManifest = [ordered]@{
    version = 1
    segments = @($segments)
}
$outManifest | ConvertTo-Json -Depth 8 | Set-Content -Path $paths.Manifest -Encoding UTF8

$rebuildScript = Join-Path $PSScriptRoot "rebuild-index.ps1"
if (Test-Path $rebuildScript) {
    & $rebuildScript
}

Write-Host "Compacted $($oldRows.Count) entries into $([IO.Path]::GetFileName($storedPath))"
