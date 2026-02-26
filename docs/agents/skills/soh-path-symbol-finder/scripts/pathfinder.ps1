[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [Parameter(Mandatory = $true)]
    [string]$Query,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($DryRun) {
    Write-Host "[DryRun] Would search for query: $Query"
    exit 0
}

$rg = Get-Command rg -ErrorAction SilentlyContinue
if ($null -eq $rg) {
    Write-Error "rg not found on PATH."
    exit 1
}

Push-Location $RepoRoot
try {
    $raw = rg --line-number --no-heading --hidden --glob '!build/**' --glob '!.git/**' --glob '!x64/Release/**' $Query
    $results = @()
    foreach ($line in $raw) {
        $parts = $line -split ":", 3
        if ($parts.Count -eq 3) {
            $results += [PSCustomObject]@{
                path = $parts[0]
                line = [int]$parts[1]
                text = $parts[2]
            }
        }
    }
    $results | Select-Object -First 200 | ConvertTo-Json -Depth 5
    exit 0
}
finally {
    Pop-Location
}

