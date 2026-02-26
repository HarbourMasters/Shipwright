param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("decision", "incident", "task", "note")]
    [string]$Kind,

    [Parameter(Mandatory = $true)]
    [string]$Topic,

    [Parameter(Mandatory = $true)]
    [string]$Summary,

    [string]$Details = "",
    [ValidateSet("open", "done", "deprecated")]
    [string]$Status = "open",
    [string]$Scope = "docs",
    [string[]]$Tags = @(),
    [string[]]$Refs = @(),
    [string[]]$Supersedes = @()
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-AgentsPaths {
    $repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
    $agentsDir = Join-Path $repoRoot "docs\agents"
    return @{
        RepoRoot = $repoRoot
        AgentsDir = $agentsDir
        Log = Join-Path $agentsDir "memory.log"
        Index = Join-Path $agentsDir "memory.index.json"
    }
}

function Get-NextId {
    param(
        [string]$LogPath
    )

    $utcNow = [DateTime]::UtcNow
    $dayPrefix = $utcNow.ToString("yyyyMMdd")
    $idPrefix = "mem-$dayPrefix-"
    $maxSeq = 0

    if (Test-Path $LogPath) {
        foreach ($line in Get-Content -Path $LogPath -Encoding UTF8) {
            if ([string]::IsNullOrWhiteSpace($line)) {
                continue
            }
            $obj = $line | ConvertFrom-Json
            if ($obj.id -like "$idPrefix*") {
                $suffix = $obj.id.Substring($idPrefix.Length)
                $seq = 0
                if ([int]::TryParse($suffix, [ref]$seq)) {
                    if ($seq -gt $maxSeq) {
                        $maxSeq = $seq
                    }
                }
            }
        }
    }

    $next = $maxSeq + 1
    return "$idPrefix{0:D6}" -f $next
}

$paths = Get-AgentsPaths

if (-not (Test-Path $paths.AgentsDir)) {
    throw "Agents directory not found: $($paths.AgentsDir)"
}

$id = Get-NextId -LogPath $paths.Log
$ts = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")

$entry = [ordered]@{
    id = $id
    ts = $ts
    kind = $Kind
    topic = $Topic
    tags = @($Tags)
    scope = $Scope
    summary = $Summary
    details = $Details
    refs = @($Refs)
    status = $Status
    supersedes = @($Supersedes)
}

$jsonLine = $entry | ConvertTo-Json -Compress -Depth 8
Add-Content -Path $paths.Log -Value $jsonLine -Encoding UTF8

$rebuildScript = Join-Path $PSScriptRoot "rebuild-index.ps1"
if (Test-Path $rebuildScript) {
    & $rebuildScript
}

Write-Host "Appended memory entry: $id"
