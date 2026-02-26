Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-AgentsPaths {
    $repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
    $agentsDir = Join-Path $repoRoot "docs\agents"
    return @{
        AgentsDir = $agentsDir
        Log = Join-Path $agentsDir "memory.log"
        Index = Join-Path $agentsDir "memory.index.json"
        Manifest = Join-Path $agentsDir "archive.manifest.json"
        Archive = Join-Path $agentsDir "archive"
    }
}

function Fail {
    param([string]$Message)
    Write-Error $Message
    exit 1
}

function Validate-RequiredFields {
    param(
        [object]$Entry,
        [int]$Line
    )
    $required = @("id", "ts", "kind", "topic", "tags", "scope", "summary", "details", "refs", "status", "supersedes")
    foreach ($field in $required) {
        if (-not $Entry.PSObject.Properties.Name.Contains($field)) {
            Fail "memory.log line $Line missing field '$field'."
        }
    }
}

$paths = Get-AgentsPaths
if (-not (Test-Path $paths.AgentsDir)) { Fail "Agents directory not found." }
if (-not (Test-Path $paths.Log)) { Fail "Missing memory.log." }
if (-not (Test-Path $paths.Index)) { Fail "Missing memory.index.json." }
if (-not (Test-Path $paths.Manifest)) { Fail "Missing archive.manifest.json." }

$allowedKinds = @("decision", "incident", "task", "note")
$allowedStatus = @("open", "done", "deprecated")

$entries = @()
$seenIds = @{}
$lineNo = 0
foreach ($line in Get-Content -Path $paths.Log -Encoding UTF8) {
    $lineNo++
    if ([string]::IsNullOrWhiteSpace($line)) { continue }
    $entry = $null
    try {
        $entry = $line | ConvertFrom-Json
    } catch {
        Fail "memory.log line $lineNo is not valid JSON."
    }

    Validate-RequiredFields -Entry $entry -Line $lineNo

    $id = [string]$entry.id
    if ($seenIds.ContainsKey($id)) {
        Fail "Duplicate id in memory.log: $id"
    }
    $seenIds[$id] = $true

    if (-not ($allowedKinds -contains [string]$entry.kind)) {
        Fail "Invalid kind '$($entry.kind)' at line $lineNo."
    }
    if (-not ($allowedStatus -contains [string]$entry.status)) {
        Fail "Invalid status '$($entry.status)' at line $lineNo."
    }

    $null = [DateTime]$entry.ts
    $entries += $entry
}

$entryIds = @{}
foreach ($entry in $entries) {
    $entryIds[[string]$entry.id] = $true
}

foreach ($entry in $entries) {
    foreach ($sup in @($entry.supersedes)) {
        $supId = [string]$sup
        if (-not [string]::IsNullOrWhiteSpace($supId) -and -not $entryIds.ContainsKey($supId)) {
            Write-Warning "Entry $($entry.id) supersedes unknown id '$supId' (may exist in archive)."
        }
    }
}

$index = Get-Content -Path $paths.Index -Raw -Encoding UTF8 | ConvertFrom-Json
if ([int]$index.version -ne 1) {
    Fail "Unsupported memory.index.json version: $($index.version)"
}

foreach ($id in $entryIds.Keys) {
    if (-not $index.byId.PSObject.Properties.Name.Contains($id)) {
        Fail "Index missing id from memory.log: $id"
    }
}

$manifest = Get-Content -Path $paths.Manifest -Raw -Encoding UTF8 | ConvertFrom-Json
if ([int]$manifest.version -ne 1) {
    Fail "Unsupported archive.manifest.json version: $($manifest.version)"
}

foreach ($segment in @($manifest.segments)) {
    $name = [string]$segment.name
    $path = Join-Path $paths.Archive $name
    if (-not (Test-Path $path)) {
        Fail "Archive segment listed but missing file: $name"
    }

    $expectedHash = [string]$segment.sha256
    if (-not [string]::IsNullOrWhiteSpace($expectedHash)) {
        $actualHash = (Get-FileHash -Path $path -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($actualHash -ne $expectedHash.ToLowerInvariant()) {
            Fail "SHA256 mismatch for archive segment $name"
        }
    }
}

Write-Host "Memory validation passed."
