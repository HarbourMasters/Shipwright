[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [string[]]$Files = @(),
    [int]$MaxFiles = 8,
    [int]$MaxSnippetsPerFile = 3,
    [int]$SnippetLines = 220,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($MaxFiles -lt 1 -or $MaxSnippetsPerFile -lt 1 -or $SnippetLines -lt 1) {
    Write-Error "Invalid context-pack budget values."
    exit 1
}

$defaults = @(
    "docs/agents/project_state.md",
    "docs/agents/memory.index.json"
)

$selected = @()
$selected += $defaults
$selected += $Files
$selected = $selected | Select-Object -Unique | Select-Object -First $MaxFiles

if ($DryRun) {
    Write-Host "[DryRun] Selected files:"
    $selected | ForEach-Object { Write-Host $_ }
    exit 0
}

$outLines = New-Object System.Collections.Generic.List[string]
$outLines.Add("# Context Pack")
$outLines.Add("generated=$(Get-Date -Format o)")
$outLines.Add("maxFiles=$MaxFiles maxSnippetsPerFile=$MaxSnippetsPerFile snippetLines=$SnippetLines")
$outLines.Add("")

foreach ($rel in $selected) {
    $full = Join-Path $RepoRoot $rel
    if (-not (Test-Path $full)) {
        continue
    }
    $outLines.Add("## file: $rel")
    $content = Get-Content -Path $full -Encoding UTF8
    $total = $content.Count
    $chunkCount = [Math]::Min($MaxSnippetsPerFile, [Math]::Ceiling($total / [double]$SnippetLines))
    for ($chunk = 0; $chunk -lt $chunkCount; $chunk++) {
        $start = $chunk * $SnippetLines
        $end = [Math]::Min($start + $SnippetLines - 1, $total - 1)
        $outLines.Add('```text')
        for ($i = $start; $i -le $end; $i++) {
            $outLines.Add($content[$i])
        }
        $outLines.Add('```')
    }
    $outLines.Add("")
}

$parent = Split-Path -Parent $OutputPath
if (-not [string]::IsNullOrWhiteSpace($parent) -and -not (Test-Path $parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
}

Set-Content -Path $OutputPath -Value $outLines -Encoding UTF8
Write-Host "Context pack generated: $OutputPath"
exit 0


