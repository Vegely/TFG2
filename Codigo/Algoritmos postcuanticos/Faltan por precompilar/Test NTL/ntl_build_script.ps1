# Script simple para compilar y ejecutar NTL
param([string]$BuildType = "Debug", [switch]$Clean)

Write-Host "Compilando NTL GF(2) Operations..." -ForegroundColor Cyan

# Limpiar si se solicita
if ($Clean -and (Test-Path "build")) {
    Remove-Item -Recurse -Force "build"
    Write-Host "Directorio build limpiado" -ForegroundColor Green
}

# Crear directorio build
if (!(Test-Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }

# Configurar y compilar
Set-Location "build"
try {
    cmake -DCMAKE_BUILD_TYPE=$BuildType ..
    if ($LASTEXITCODE -ne 0) { throw "Error en configuración CMake" }
    
    cmake --build . --config $BuildType
    if ($LASTEXITCODE -ne 0) { throw "Error en compilación" }
    
    Write-Host "`nEjecutando tests..." -ForegroundColor Yellow
    
    # Buscar el ejecutable en ubicaciones típicas de Visual Studio
    $exePaths = @(
        ".\$BuildType\ntl_test.exe",
        ".\Debug\ntl_test.exe", 
        ".\Release\ntl_test.exe",
        ".\ntl_test.exe"
    )
    
    $exe = $null
    foreach ($path in $exePaths) {
        if (Test-Path $path) {
            $exe = $path
            break
        }
    }
    
    if (!$exe) { 
        Write-Host "Buscando ejecutable recursivamente..." -ForegroundColor Yellow
        $exe = Get-ChildItem -Recurse -Filter "ntl_test.exe" | Select-Object -First 1 -ExpandProperty FullName
    }
    
    if (!$exe) { throw "No se encontró ntl_test.exe" }
    
    Write-Host "Ejecutable encontrado: $exe" -ForegroundColor Green
    & $exe
    $result = $LASTEXITCODE
    
    if ($result -eq 0) {
        Write-Host "`n¡TODOS LOS TESTS PASARON!" -ForegroundColor Green
    } else {
        Write-Host "`nALGUNOS TESTS FALLARON" -ForegroundColor Red
    }
} catch {
    Write-Host "ERROR: $_" -ForegroundColor Red
    $result = 1
} finally {
    Set-Location ".."
}

exit $result