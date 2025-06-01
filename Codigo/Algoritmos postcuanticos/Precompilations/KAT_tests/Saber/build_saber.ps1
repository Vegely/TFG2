# Set up the root build directory
$rootBuildDir = "build"
$rawBuildDir = "$rootBuildDir\saber_build"

# Clean existing build directories if they exist
if (Test-Path $rawBuildDir) {
    Remove-Item -Recurse -Force $rawBuildDir
}
if (-not (Test-Path $rootBuildDir)) {
    New-Item -ItemType Directory -Path $rootBuildDir | Out-Null
}

# Explicitly create raw build directory
New-Item -ItemType Directory -Path $rawBuildDir | Out-Null

# Build SABER library
Write-Host "`n=== Configuring SABER build ===`n"
cmake -S . -B $rawBuildDir -DSABER_L=4
cmake --build $rawBuildDir --config Release

Write-Host "`n=== SABER build completed successfully ===`n"