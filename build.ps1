<#
.SYNOPSIS
    PyroProgression Mod Build Script (MSVC x64) - Modernized via CMake Presets
.DESCRIPTION
    Script de automação para compilação unificada com MSVC x64 de PyroProgression.dll e testes.
    Utiliza o sistema de CMakePresets.json do CMake 3.25+.
.EXAMPLE
    .\build.ps1
    .\build.ps1 -Target mod
    .\build.ps1 -Target test
    .\build.ps1 -InstallPath "C:\Program Files (x86)\Steam\steamapps\common\Cube World"
#>

[CmdletBinding()]
param (
    [ValidateSet("all", "mod", "test", "clean")]
    [string]$Target = "all",

    [ValidateSet("Release", "Debug", "RelWithDebInfo")]
    [string]$BuildType = "Release",

    [string]$InstallPath = ""
)

$ErrorActionPreference = "Stop"

$isWindows = $true
if ($PSVersionTable.PSVersion.Major -ge 6) {
    $isWindows = $IsWindows
}

# Map to the unified CMake Preset names based on Platform
$presetName = ""
$testPresetName = ""

if ($isWindows) {
    if ($env:VSCMD_ARG_TGT_ARCH -ne "x64" -or -not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
        $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
        $vcvars = $null
        if (Test-Path $vswhere) {
            $vsPath = & $vswhere -latest -prerelease -property installationPath
            if ($vsPath -and (Test-Path "$vsPath\VC\Auxiliary\Build\vcvars64.bat")) {
                $vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
            }
        }
        if (-not $vcvars) {
            $candidate = Get-ChildItem -Path "C:\Program Files\Microsoft Visual Studio", "C:\Program Files (x86)\Microsoft Visual Studio" -Filter "vcvars64.bat" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
            if ($candidate) { $vcvars = $candidate.FullName }
        }
        if ($vcvars) {
            Write-Host "Inicializando ambiente MSVC x64: $vcvars" -ForegroundColor Cyan
            cmd.exe /c "call `"$vcvars`" >nul 2>&1 && set" | ForEach-Object {
                if ($_ -match "^(.*?)=(.*)$") {
                    [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
                }
            }
        }
    }
    $testPresetName = "windows-test"
    if ($Target -eq "test" -or $BuildType -eq "Debug") {
        $presetName = "windows-debug"
    } else {
        $presetName = "windows-release"
    }
} else {
    $testPresetName = "macos-test"
    $presetName = "macos-debug" # Debug is used for macOS testing
    
    if ($Target -eq "mod") {
        Write-Error "O target 'mod' não é suportado nativamente no macOS/Linux. Apenas o target 'test' ou 'all' estão disponíveis para execução de testes unitários."
    }
    if ($InstallPath -ne "") {
        Write-Warning "Instalação desativada: O target de instalação só é suportado no Windows onde o PyroProgression é compilado."
        $InstallPath = ""
    }
}

$buildDir = "build/$presetName"

if ($Target -eq "clean") {
    if (Test-Path "build") {
        Write-Host "Limpando diretórios de build..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force "build"
        Write-Host "Build limpo com sucesso." -ForegroundColor Green
    }
    exit 0
}

Write-Host "Configurando CMake usando o Preset: $presetName..." -ForegroundColor Cyan
cmake --preset $presetName

switch ($Target) {
    "all" {
        Write-Host "Compilando projeto usando o Preset: $presetName..." -ForegroundColor Cyan
        cmake --build --preset $presetName --parallel
    }
    "mod" {
        Write-Host "Compilando target PyroProgression..." -ForegroundColor Cyan
        cmake --build --preset $presetName --target PyroProgression
    }
    "test" {
        Write-Host "Compilando e executando testes via CTest..." -ForegroundColor Cyan
        cmake --build --preset $presetName --target pyro_tests
        ctest --preset $testPresetName
    }
}

if ($InstallPath -ne "") {
    if (-not (Test-Path $InstallPath)) {
        Write-Error "Diretório de instalação não encontrado: $InstallPath"
    }

    $modsDir = $InstallPath
    if (Test-Path (Join-Path $InstallPath "Mods")) {
        $modsDir = Join-Path $InstallPath "Mods"
    }

    Write-Host "Instalando PyroProgression.dll em $modsDir..." -ForegroundColor Magenta

    $dllCandidates = @(
        (Join-Path $buildDir "src/PyroProgression.dll"),
        (Join-Path $buildDir "src/$BuildType/PyroProgression.dll"),
        (Join-Path $PSScriptRoot "dist/PyroProgression.dll")
    )

    $dllPath = $dllCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

    if ($dllPath) {
        Copy-Item -Path $dllPath -Destination $modsDir -Force
        Write-Host " -> Copiado PyroProgression.dll ($dllPath) para $modsDir" -ForegroundColor Green
    } else {
        Write-Warning "PyroProgression.dll não encontrado nos caminhos candidatos."
    }
}

Write-Host "Processo concluído com sucesso!" -ForegroundColor Green
