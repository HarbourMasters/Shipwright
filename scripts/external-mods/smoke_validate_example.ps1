param(
    [string]$ExampleRoot = "docs/examples/external_mods/kokiri_parkour",
    [string]$OutputZip = "build-smoke/kokiri_parkour.zip"
)

$ErrorActionPreference = "Stop"

function Write-Step([string]$Message) {
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss zzz"
    Write-Output "[$timestamp] $Message"
}

if (!(Test-Path $ExampleRoot)) {
    throw "Example root not found: $ExampleRoot"
}

$manifestPath = Join-Path $ExampleRoot "mod.json"
$scriptPath = Join-Path $ExampleRoot "scripts/init.json"

$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$script = Get-Content $scriptPath -Raw | ConvertFrom-Json

if ($manifest.apiVersion -ne 1) {
    throw "mod.json apiVersion must be 1"
}
if ([string]::IsNullOrWhiteSpace($manifest.entryScript)) {
    throw "mod.json entryScript is required"
}
if ($script.apiVersion -ne 1) {
    throw "entry script apiVersion must be 1"
}

New-Item -ItemType Directory -Path ([System.IO.Path]::GetDirectoryName($OutputZip)) -Force | Out-Null
if (Test-Path $OutputZip) {
    Remove-Item $OutputZip -Force
}

Compress-Archive -Path (Join-Path $ExampleRoot "*") -DestinationPath $OutputZip -Force

Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [IO.Compression.ZipFile]::OpenRead((Resolve-Path $OutputZip))
try {
    $entries = $zip.Entries.FullName
    if (($entries | Where-Object { $_ -eq "mod.json" }).Count -eq 0) {
        throw "Zip does not contain mod.json at root"
    }
} finally {
    $zip.Dispose()
}

Write-Step "Smoke validation passed."
