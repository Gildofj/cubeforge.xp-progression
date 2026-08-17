<#
.SYNOPSIS
    Compilacao e empacotamento automatizado do PyroProgression.
.DESCRIPTION
    Script PowerShell para compilar a DLL do mod, rodar testes unitarios e organizar os binarios.
.EXAMPLE
    .\build.ps1
    .\build.ps1 -Test
    .\build.ps1 -Clean
#>

[CmdletBinding()]
param (
    [ValidateSet("Release", "Debug", "RelWithDebInfo")]
    [string]$Config = "Release",

    [switch]$Test,
    [switch]$Clean,
    [string]$InstallPath = ""
)

$ErrorActionPreference = "Stop"
$RepoRoot = $PSScriptRoot
$BuildDir = Join-Path $RepoRoot "build"
$DistDir = Join-Path $RepoRoot "dist"

Write-Host "=======================================================" -ForegroundColor Cyan
Write-Host "         PyroProgression Build Pipeline                " -ForegroundColor Cyan
Write-Host "=======================================================" -ForegroundColor Cyan

# 1. Clean if requested
if ($Clean) {
    Write-Host "`n[*] Limpando diretorios de compilacao..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
    if (Test-Path $DistDir) { Remove-Item -Recurse -Force $DistDir }
    Write-Host "[OK] Limpeza concluida!" -ForegroundColor Green
    if (-not $Test -and ($PSBoundParameters.Count -eq 1)) { return }
}

# 2. Check submodule
$SubmoduleDir = Join-Path $RepoRoot "CWSDK"
if (-not (Test-Path (Join-Path $SubmoduleDir "CMakeLists.txt"))) {
    Write-Host "`n[*] Inicializando submodulo CWSDK..." -ForegroundColor Yellow
    git submodule update --init --recursive
}

# 3. Configure CMake
Write-Host "`n[*] Configurando CMake (x64, Config: $Config)..." -ForegroundColor Yellow
cmake -B $BuildDir -S $RepoRoot -A x64

if ($LASTEXITCODE -ne 0) {
    Write-Error "Falha na configuracao do CMake."
    exit $LASTEXITCODE
}

# 4. Build Mod DLL
Write-Host "`n[*] Compilando PyroProgression.dll..." -ForegroundColor Yellow
cmake --build $BuildDir --config $Config --target PyroProgression

if ($LASTEXITCODE -ne 0) {
    Write-Error "Falha na compilacao do PyroProgression."
    exit $LASTEXITCODE
}

# 5. Copy to dist/
if (-not (Test-Path $DistDir)) {
    New-Item -ItemType Directory -Path $DistDir | Out-Null
}

$DllOutput = Join-Path $BuildDir "$Config\PyroProgression.dll"
if (Test-Path $DllOutput) {
    Copy-Item -Path $DllOutput -Destination (Join-Path $DistDir "PyroProgression.dll") -Force
    Write-Host "`n[OK] DLL gerada com sucesso:" -ForegroundColor Green
    Write-Host "     -> $(Join-Path $DistDir 'PyroProgression.dll')" -ForegroundColor White
}

# 6. Optional: Run tests if requested
if ($Test) {
    Write-Host "`n[*] Compilando e executando testes..." -ForegroundColor Yellow
    cmake --build $BuildDir --config $Config --target pyro_tests
    if ($LASTEXITCODE -eq 0) {
        & "$BuildDir\$Config\pyro_tests.exe"
    } else {
        Write-Error "Falha na compilacao dos testes."
        exit $LASTEXITCODE
    }
}

# 7. Optional: Install to Cube World Mods/ folder
if ($InstallPath -and (Test-Path $InstallPath)) {
    Write-Host "`n[*] Copiando DLL para destino de instalacao: $InstallPath..." -ForegroundColor Yellow
    Copy-Item -Path (Join-Path $DistDir "PyroProgression.dll") -Destination $InstallPath -Force
    Write-Host "[OK] Instalado em $InstallPath" -ForegroundColor Green
}

Write-Host "`n[OK] Build concluida com sucesso!`n" -ForegroundColor Green
