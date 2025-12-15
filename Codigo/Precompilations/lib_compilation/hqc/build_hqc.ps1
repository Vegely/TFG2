# Set up the root build directory
$rootBuildDir = "build"
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

# Build HQC library
Write-Host "`n=== Configuring HQC build ===`n"
cmake -S . -B $rawBuildDir -DHQC_BUILD_KAT_TESTS=OFF
cmake --build $rawBuildDir --config Release

Write-Host "`n=== HQC build completed successfully ===`n"