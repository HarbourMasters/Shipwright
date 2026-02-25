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
    }
}

function Add-IndexListValue {
    param(
        [hashtable]$Table,
        [string]$Key,
        [string]$Value
    )

    if (-not $Table.ContainsKey($Key)) {
        $Table[$Key] = New-Object System.Collections.ArrayList
    }
    [void]$Table[$Key].Add($Value)
}

function Read-JsonLinesFile {
    param(
        [string]$Path
    )

    $entries = @()
    if (-not (Test-Path $Path)) {
        return $entries
    }

    $lineNo = 0
    foreach ($line in Get-Content -Path $Path -Encoding UTF8) {
        $lineNo++
        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }
        $obj = $line | ConvertFrom-Json
        $entries += [PSCustomObject]@{
            Entry = $obj
            Line = $lineNo
            Source = [IO.Path]::GetFileName($Path)
        }
    }

    return $entries
}

$paths = Get-AgentsPaths

if (-not (Test-Path $paths.AgentsDir)) {
    throw "Agents directory not found: $($paths.AgentsDir)"
}

$byId = @{}
$byTag = @{}
$byTopic = @{}
$byStatus = @{
    open = (New-Object System.Collections.ArrayList)
    done = (New-Object System.Collections.ArrayList)
    deprecated = (New-Object System.Collections.ArrayList)
}
$ranges = New-Object System.Collections.ArrayList

$currentEntries = Read-JsonLinesFile -Path $paths.Log

foreach ($row in $currentEntries) {
    $entry = $row.Entry
    $id = [string]$entry.id

    if ([string]::IsNullOrWhiteSpace($id)) {
        continue
    }

    $byId[$id] = [ordered]@{
        topic = [string]$entry.topic
        status = [string]$entry.status
        line = [int]$row.Line
        source = [string]$row.Source
    }

    Add-IndexListValue -Table $byTopic -Key ([string]$entry.topic) -Value $id
    Add-IndexListValue -Table $byStatus -Key ([string]$entry.status) -Value $id

    foreach ($tag in @($entry.tags)) {
        $tagValue = [string]$tag
        if (-not [string]::IsNullOrWhiteSpace($tagValue)) {
            Add-IndexListValue -Table $byTag -Key $tagValue -Value $id
        }
    }
}

if (Test-Path $paths.Manifest) {
    $manifest = Get-Content -Path $paths.Manifest -Raw -Encoding UTF8 | ConvertFrom-Json
    foreach ($segment in @($manifest.segments)) {
        [void]$ranges.Add([ordered]@{
            name = [string]$segment.name
            fromId = [string]$segment.fromId
            toId = [string]$segment.toId
            count = [int]$segment.count
            sha256 = [string]$segment.sha256
        })
    }
}

$index = [ordered]@{
    version = 1
    lastUpdatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")
    byId = $byId
    byTag = $byTag
    byTopic = $byTopic
    byStatus = $byStatus
    ranges = @($ranges)
}

$index | ConvertTo-Json -Depth 12 | Set-Content -Path $paths.Index -Encoding UTF8
Write-Host "Rebuilt memory index: $($paths.Index)"
