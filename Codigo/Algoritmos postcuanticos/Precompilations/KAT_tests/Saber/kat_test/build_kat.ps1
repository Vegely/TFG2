# build_kat.ps1
param (
    [ValidateSet("light", "saber", "fire")]
    [string]$SecurityLevel = "fire"
)

# Map security level to SABER_L values
$saberLMap = @{
    "light" = "2"
    "saber" = "3"
    "fire" = "4"
}
$saberL = $saberLMap[$SecurityLevel]

# Define build folder
$buildDir = "build_kat_$SecurityLevel"

# Clean build folder
if (Test-Path $buildDir) {
    Remove-Item $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null
Push-Location $buildDir

# Run CMake
cmake .. -DSABER_L=$saberL

# Build the project
cmake --build . --config Release

Pop-Location

Write-Host "Build completed for SABER-$SecurityLevel (L=$saberL)"