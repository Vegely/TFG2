# Build script for Kyber1024 library
# This script builds the Kyber1024 implementation with KYBER_K=4

# Set up the root build directory
$rootBuildDir = "build_kyber1024"
$rawBuildDir = "$rootBuildDir\raw_build"

# Clean existing build directories if they exist
if (Test-Path $rawBuildDir) {
    Remove-Item -Recurse -Force $rawBuildDir
}
if (-not (Test-Path $rootBuildDir)) {
    New-Item -ItemType Directory -Path $rootBuildDir | Out-Null
}

# Explicitly create raw build directory
New-Item -ItemType Directory -Path $rawBuildDir | Out-Null

# Build Kyber1024 (raw_build)
Write-Host "`n=== Configuring Kyber1024 build (KYBER_K=4) ===`n" -ForegroundColor Green
cmake -S . -B $rawBuildDir -DKYBER_K=4 -DKYBER_BUILD_KAT_TESTS=OFF

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`n=== Building Kyber1024 ===`n" -ForegroundColor Green
cmake --build $rawBuildDir --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "`n=== Kyber1024 build completed successfully ===`n" -ForegroundColor Green
Write-Host "Built libraries:" -ForegroundColor Yellow
Write-Host "  - Static library: $rawBuildDir\libkyber1024.a (Linux/macOS) or kyber1024.lib (Windows)" -ForegroundColor Cyan
Write-Host "  - Shared library: $rawBuildDir\libkyber1024.so (Linux), libkyber1024.dylib (macOS), or kyber1024.dll (Windows)" -ForegroundColor Cyan