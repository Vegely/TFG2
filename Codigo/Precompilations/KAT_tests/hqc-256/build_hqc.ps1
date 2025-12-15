# Set up the root build directory
$rootBuildDir = "build"
$rawBuildDir = "$rootBuildDir\hqc_build"

# Clean existing build directories if they exist
if (Test-Path $rawBuildDir) {
    Remove-Item -Recurse -Force $rawBuildDir
}
if (-not (Test-Path $rootBuildDir)) {
    New-Item -ItemType Directory -Path $rootBuildDir | Out-Null
}

# Explicitly create raw build directory
New-Item -ItemType Directory -Path $rawBuildDir | Out-Null

# Build HQC-256 library
Write-Host "`n=== Configuring HQC-256 build ===`n"
cmake -S . -B $rawBuildDir
cmake --build $rawBuildDir --config Release

Write-Host "`n=== HQC-256 build completed successfully ===`n"