# build_kat.ps1
param (
    [ValidateSet("128", "192", "256")]
    [string]$SecurityLevel = "256"
)

# Define build folder
$buildDir = "build_kat_hqc_$SecurityLevel"

# Clean build folder
if (Test-Path $buildDir) {
    Remove-Item $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null
Push-Location $buildDir

# Run CMake
cmake .. -DHQC_LEVEL=$SecurityLevel

# Build the project
cmake --build . --config Release

Pop-Location

Write-Host "Build completed for HQC-$SecurityLevel"