[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$PlanId = "",
    [int]$Limit = 20,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($Limit -lt 1) {
    Write-Error "Limit must be >= 1."
    exit 1
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$plansPath = Join-Path $resolvedRoot "docs\agents\Plans.md"
if (-not (Test-Path $plansPath)) {
    Write-Error "Plans ledger not found: $plansPath"
    exit 1
}

$lines = Get-Content -Path $plansPath -Encoding UTF8
$entries = New-Object System.Collections.Generic.List[object]
$current = $null
$lineNumber = 0

foreach ($line in $lines) {
    $lineNumber++
    if ($line -match "^\#\# \[(PLN-\d{8}-\d{4})\](\[UPDATE\])?\s*(.*)$") {
        if ($null -ne $current) {
            $entries.Add($current)
        }
        $current = [ordered]@{
            planId = $Matches[1]
            isUpdate = -not [string]::IsNullOrWhiteSpace($Matches[2])
            title = $Matches[3].Trim()
            status = ""
            createdUtc = ""
            summary = ""
            line = $lineNumber
        }
        continue
    }

    if ($null -ne $current) {
        if ($line -match "^- status:\s*(.+)$" -and [string]::IsNullOrWhiteSpace($current.status)) {
            $current.status = $Matches[1].Trim()
        }
        if ($line -match "^- createdUtc:\s*(.+)$" -and [string]::IsNullOrWhiteSpace($current.createdUtc)) {
            $current.createdUtc = $Matches[1].Trim()
        }
        if ($line -match "^- summary:\s*(.+)$" -and [string]::IsNullOrWhiteSpace($current.summary)) {
            $current.summary = $Matches[1].Trim()
        }
    }
}

if ($null -ne $current) {
    $entries.Add($current)
}

$result = $entries
if (-not [string]::IsNullOrWhiteSpace($PlanId)) {
    $result = @($result | Where-Object { $_.planId -eq $PlanId })
}

$result = @($result | Select-Object -Last $Limit)

if ($DryRun) {
    Write-Host "[DryRun] plansPath=$plansPath totalEntries=$($entries.Count) returned=$($result.Count)"
}

$result | ConvertTo-Json -Depth 6
exit 0
