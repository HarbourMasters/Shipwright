Using Namespace System
$url = "https://github.com/llvm/llvm-project/releases/download/llvmorg-14.0.6/LLVM-14.0.6-win64.exe"
$llvmInstallerPath = ".\LLVM-14.0.6-win64.exe"
$clangFormatFilePath = ".\clang-format.exe"
$requiredVersion = "clang-format version 14.0.6"
$currentVersion = ""

function Test-7ZipInstalled {
    $sevenZipPath = "C:\Program Files\7-Zip\7z.exe"
    return Test-Path $sevenZipPath -PathType Leaf
}

if (Test-Path $clangFormatFilePath) {
    $currentVersion = & $clangFormatFilePath --version
    if (-not ($currentVersion -eq $requiredVersion)) {
        # Delete the existing file if the version is incorrect
        Remove-Item $clangFormatFilePath -Force
    }
}

if (-not (Test-Path $clangFormatFilePath) -or ($currentVersion -ne $requiredVersion)) {
    if (-not (Test-7ZipInstalled)) {
        Write-Host "7-Zip is not installed. Please install 7-Zip and run the script again."
        exit
    }

    $wc = New-Object net.webclient
    $wc.Downloadfile($url, $PSScriptRoot + $llvmInstallerPath)

    $sevenZipPath = "C:\Program Files\7-Zip\7z.exe"
    $specificFileInArchive = "bin\clang-format.exe"
    & "$sevenZipPath" e $llvmInstallerPath $specificFileInArchive

    Remove-Item $llvmInstallerPath -Force
}

$basePath = (Resolve-Path .).Path
$files = Get-ChildItem -Path $basePath\soh -Recurse -File `
    | Where-Object { ($_.Extension -eq '.c' -or $_.Extension -eq '.cpp' -or `
                      (($_.Extension -eq '.h' -or $_.Extension -eq '.hpp') -and `
                       (-not ($_.FullName -like "*\soh\src\*" -or $_.FullName -like "*\soh\include\*")))) -and `
                     (-not ($_.FullName -like "*\soh\assets\*" -or $_.FullName -like "*\soh\build\*")) }

for ($i = 0; $i -lt $files.Length; $i++) {
    $file = $files[$i]
    $relativePath = $file.FullName.Substring($basePath.Length + 1)
    Write-Host "Formatting [$($i+1)/$($files.Length)] $relativePath"
    .\clang-format.exe -i $file.FullName
}
