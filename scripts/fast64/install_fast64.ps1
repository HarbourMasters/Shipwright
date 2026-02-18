param(
    [string]$InstallRoot = "Scene_Editor/tools/fast64",
    [string]$PinnedTag = "v2.1.0",
    [string]$RepoUrl = "https://github.com/Fast-64/fast64.git"
)

$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss zzz"
    Write-Output "[$timestamp] $Message"
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "git command not found. Install Git first."
}

$lockPath = "Scene_Editor/fast64.lock.json"

if (-not (Test-Path $InstallRoot)) {
    Write-Step "Cloning Fast64 repository to $InstallRoot"
    git clone $RepoUrl $InstallRoot | Out-Null
} else {
    Write-Step "Fast64 directory already exists. Fetching latest refs."
    git -C $InstallRoot fetch --all --tags --prune | Out-Null
}

Write-Step "Checking out pinned tag $PinnedTag"
git -C $InstallRoot checkout "tags/$PinnedTag" --force | Out-Null

$lock = [ordered]@{
    testedTag    = $PinnedTag
    updatedAtUtc = (Get-Date).ToUniversalTime().ToString("o")
    notes        = "Pinned by scripts/fast64/install_fast64.ps1"
}

$lockJson = $lock | ConvertTo-Json -Depth 5
$lockJson | Set-Content -Path $lockPath -NoNewline

Write-Step "Updated lock file: $lockPath"
Write-Step "Fast64 pinned setup complete."
