# build_kat.ps1
param (
    [ValidateSet("512", "768", "1024")]
    [string]$SecurityLevel = "512"
)

# Map security level to KYBER_K values
$kyberKMap = @{
    "512" = "2"
    "768" = "3"
    "1024" = "4"
}
$kyberK = $kyberKMap[$SecurityLevel]

# Define build folder
$buildDir = "build_kat_$SecurityLevel"

# Clean build folder
if (Test-Path $buildDir) {
    Remove-Item $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null
Push-Location $buildDir

# Run CMake
cmake .. -DKYBER_K=$kyberK

# Build the project
cmake --build . --config Release

Pop-Location

Write-Host "Build completed for Kyber-$SecurityLevel (K=$kyberK)"
