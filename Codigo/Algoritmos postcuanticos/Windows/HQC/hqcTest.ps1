if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
}

New-Item -ItemType Directory -Path "build" | Out-Null
Set-Location "build"

cmake --build . --config Release