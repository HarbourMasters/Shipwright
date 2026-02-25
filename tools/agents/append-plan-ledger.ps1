[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [ValidateSet("new", "update")]
    [string]$Mode = "new",
    [string]$PlanId = "",
    [string]$Title = "",
    [string]$Summary = "",
    [ValidateSet("planned", "in_progress", "blocked", "done", "canceled")]
    [string]$Status = "planned",
    [ValidateSet("engine", "docs", "mod", "tooling", "mixed")]
    [string]$Scope = "tooling",
    [string[]]$Milestones = @(),
    [string]$Note = "",
    [string[]]$Tags = @(),
    [string[]]$Refs = @(),
    [switch]$WriteMemory,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-Paths {
    param([string]$Root)
    $resolvedRoot = (Resolve-Path -Path $Root).Path
    $agentsDir = Join-Path $resolvedRoot "docs\agents"
    @{
        RepoRoot = $resolvedRoot
        AgentsDir = $agentsDir
        PlansPath = Join-Path $agentsDir "Plans.md"
        MemoryScript = Join-Path $resolvedRoot "tools\agents\append-memory.ps1"
    }
}

function Ensure-PlansFile {
    param([string]$Path)
    if (Test-Path $Path) {
        return
    }
    $template = @'
# Plans Ledger

This file is the fixed, append-only plan ledger for this repository.
'@
    Set-Content -Path $Path -Value $template -Encoding UTF8
}

function Get-NextPlanId {
    param([string]$PlansPath)
    $dayPrefix = [DateTime]::UtcNow.ToString("yyyyMMdd")
    $prefix = "PLN-$dayPrefix-"
    $max = 0
    if (Test-Path $PlansPath) {
        $lines = Get-Content -Path $PlansPath -Encoding UTF8
        foreach ($line in $lines) {
            if ($line -match "^\#\# \[(PLN-\d{8}-(\d{4}))\]") {
                $id = $Matches[1]
                if ($id.StartsWith($prefix)) {
                    $seqText = $Matches[2]
                    $seq = 0
                    if ([int]::TryParse($seqText, [ref]$seq)) {
                        if ($seq -gt $max) {
                            $max = $seq
                        }
                    }
                }
            }
        }
    }
    "PLN-{0}-{1:D4}" -f $dayPrefix, ($max + 1)
}

function Truncate-Summary {
    param([string]$Text, [int]$MaxLen = 180)
    if ([string]::IsNullOrWhiteSpace($Text)) { return "" }
    if ($Text.Length -le $MaxLen) { return $Text }
    return $Text.Substring(0, $MaxLen - 3) + "..."
}

$paths = Resolve-Paths -Root $RepoRoot
if (-not (Test-Path $paths.AgentsDir)) {
    Write-Error "Agents directory not found: $($paths.AgentsDir)"
    exit 1
}

Ensure-PlansFile -Path $paths.PlansPath

$utcNow = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")
$resolvedPlanId = $PlanId

if ($Mode -eq "new") {
    if ([string]::IsNullOrWhiteSpace($Title)) {
        Write-Error "Title is required for mode=new."
        exit 1
    }
    if ([string]::IsNullOrWhiteSpace($Summary)) {
        Write-Error "Summary is required for mode=new."
        exit 1
    }
    $resolvedPlanId = Get-NextPlanId -PlansPath $paths.PlansPath
}
else {
    if ([string]::IsNullOrWhiteSpace($PlanId)) {
        Write-Error "PlanId is required for mode=update."
        exit 1
    }
    if ([string]::IsNullOrWhiteSpace($Note)) {
        Write-Error "Note is required for mode=update."
        exit 1
    }
}

$entry = New-Object System.Collections.Generic.List[string]
$entry.Add("")

if ($Mode -eq "new") {
    $entry.Add("## [$resolvedPlanId] $Title")
    $entry.Add("- createdUtc: $utcNow")
    $entry.Add("- status: $Status")
    $entry.Add("- scope: $Scope")
    $entry.Add("- summary: $Summary")
    $entry.Add("- milestones:")
    if ($Milestones.Count -eq 0) {
        $entry.Add("  1. define milestones")
    }
    else {
        for ($i = 0; $i -lt $Milestones.Count; $i++) {
            $entry.Add(("  {0}. {1}" -f ($i + 1), $Milestones[$i]))
        }
    }
    if ($Tags.Count -gt 0) {
        $entry.Add("- tags: " + ($Tags -join ", "))
    }
    $entry.Add("- refs:")
    if ($Refs.Count -eq 0) {
        $entry.Add("  - docs/agents/project_state.md")
    }
    else {
        foreach ($ref in $Refs) {
            $entry.Add("  - $ref")
        }
    }
}
else {
    $entry.Add("## [$resolvedPlanId][UPDATE] $utcNow")
    $entry.Add("- status: $Status")
    $entry.Add("- note: $Note")
    $entry.Add("- refs:")
    if ($Refs.Count -eq 0) {
        $entry.Add("  - docs/agents/project_state.md")
    }
    else {
        foreach ($ref in $Refs) {
            $entry.Add("  - $ref")
        }
    }
}

if ($DryRun) {
    Write-Host "[DryRun] Would append to $($paths.PlansPath):"
    $entry | ForEach-Object { Write-Host $_ }
    Write-Host "[DryRun] planId=$resolvedPlanId"
    exit 0
}

Add-Content -Path $paths.PlansPath -Value $entry -Encoding UTF8

if ($WriteMemory -and (Test-Path $paths.MemoryScript)) {
    $memoryKind = if ($Mode -eq "new") { "task" } else { "note" }
    $memoryTopic = "plan:$resolvedPlanId"
    $memorySummary = if ($Mode -eq "new") {
        Truncate-Summary -Text "Plan created: $Title | $Summary"
    }
    else {
        Truncate-Summary -Text "Plan update: $resolvedPlanId | $Note"
    }
    $memoryStatus = if ($Status -eq "done") { "done" } elseif ($Status -eq "canceled") { "deprecated" } else { "open" }
    & $paths.MemoryScript `
        -Kind $memoryKind `
        -Topic $memoryTopic `
        -Summary $memorySummary `
        -Details "Plans ledger entry generated by append-plan-ledger.ps1 ($Mode)." `
        -Scope $Scope `
        -Status $memoryStatus `
        -Tags @("plans", "ledger", "agents") `
        -Refs @("docs/agents/Plans.md")
}

Write-Host "Appended plan ledger entry: planId=$resolvedPlanId mode=$Mode"
exit 0
