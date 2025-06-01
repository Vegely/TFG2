if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
}

New-Item -ItemType Directory -Path "build" | Out-Null
Set-Location "build"
cmake .. 
cmake --build . --config Release