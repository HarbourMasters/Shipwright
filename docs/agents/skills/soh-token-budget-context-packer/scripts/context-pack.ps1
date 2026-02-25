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
    Write-Error "Invalid budget values."
    exit 1
}

$defaultFiles = @(
    "docs/agents/project_state.md",
    "docs/agents/memory.index.json"
)

$all = @()
$all += $defaultFiles
$all += $Files
$all = $all | Select-Object -Unique | Select-Object -First $MaxFiles

if ($DryRun) {
    $all | ConvertTo-Json
    exit 0
}

$buffer = New-Object System.Collections.Generic.List[string]
$buffer.Add("# Context Pack")
$buffer.Add("generated=$(Get-Date -Format o)")
$buffer.Add("maxFiles=$MaxFiles maxSnippetsPerFile=$MaxSnippetsPerFile snippetLines=$SnippetLines")
$buffer.Add("")

foreach ($rel in $all) {
    $full = Join-Path $RepoRoot $rel
    if (-not (Test-Path $full)) {
        continue
    }
    $buffer.Add("## file: $rel")
    $lines = Get-Content -Path $full -Encoding UTF8
    $total = $lines.Count
    $chunks = [Math]::Min($MaxSnippetsPerFile, [Math]::Ceiling($total / [double]$SnippetLines))
    for ($c = 0; $c -lt $chunks; $c++) {
        $start = $c * $SnippetLines
        $end = [Math]::Min($start + $SnippetLines - 1, $total - 1)
        $buffer.Add('```text')
        for ($i = $start; $i -le $end; $i++) {
            $buffer.Add($lines[$i])
        }
        $buffer.Add('```')
    }
    $buffer.Add("")
}

$outDir = Split-Path -Parent $OutputPath
if (-not [string]::IsNullOrWhiteSpace($outDir) -and -not (Test-Path $outDir)) {
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null
}

Set-Content -Path $OutputPath -Value $buffer -Encoding UTF8
Write-Host "Context pack written to $OutputPath"
exit 0


