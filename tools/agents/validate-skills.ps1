[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$MirrorDir = "",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($MirrorDir)) {
    $MirrorDir = Join-Path $RepoRoot "docs\agents\skills"
}

if (-not (Test-Path $MirrorDir)) {
    Write-Error "Skills mirror not found: $MirrorDir"
    exit 1
}

$errors = New-Object System.Collections.Generic.List[string]
$skills = Get-ChildItem -Path $MirrorDir -Directory

foreach ($skill in $skills) {
    $skillMd = Join-Path $skill.FullName "SKILL.md"
    if (-not (Test-Path $skillMd)) {
        $errors.Add("Missing SKILL.md: $($skill.Name)")
        continue
    }
    $content = Get-Content -Raw -Path $skillMd -Encoding UTF8
    if ($content -notmatch "(?s)^---\s*name:\s*[a-z0-9\-]+\s*description:\s*.+?---") {
        $errors.Add("Invalid frontmatter in $($skill.Name)/SKILL.md")
    }
}

$manifestPath = Join-Path $RepoRoot "docs\agents\skills.manifest.json"
$indexPath = Join-Path $RepoRoot "docs\agents\skills.index.json"
if (-not (Test-Path $manifestPath)) { $errors.Add("Missing skills.manifest.json") }
if (-not (Test-Path $indexPath)) { $errors.Add("Missing skills.index.json") }

if ($DryRun) {
    Write-Host "[DryRun] Skills checked: $($skills.Count) errors: $($errors.Count)"
}

if ($errors.Count -gt 0) {
    $errors | ForEach-Object { Write-Error $_ }
    exit 1
}

Write-Host "Skills validation passed: $($skills.Count) skill(s)."
exit 0

