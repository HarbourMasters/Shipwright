[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$MirrorDir = "",
    [string]$GlobalSkillsDir = "C:\Users\leolo\.codex\skills",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($MirrorDir)) {
    $MirrorDir = Join-Path $RepoRoot "docs\agents\skills"
}

if (-not (Test-Path $MirrorDir)) {
    Write-Error "Mirror skills directory not found: $MirrorDir"
    exit 1
}

if (-not (Test-Path $GlobalSkillsDir)) {
    if ($DryRun) {
        Write-Host "[DryRun] Would create global skills dir: $GlobalSkillsDir"
    }
    else {
        New-Item -ItemType Directory -Force -Path $GlobalSkillsDir | Out-Null
    }
}

$skills = Get-ChildItem -Path $MirrorDir -Directory
foreach ($skill in $skills) {
    $dest = Join-Path $GlobalSkillsDir $skill.Name
    if ($DryRun) {
        Write-Host "[DryRun] Sync $($skill.FullName) -> $dest"
        continue
    }
    if (Test-Path $dest) {
        Remove-Item -Path $dest -Recurse -Force
    }
    Copy-Item -Path $skill.FullName -Destination $dest -Recurse -Force
}

Write-Host "Synced $($skills.Count) skills to $GlobalSkillsDir"
exit 0

