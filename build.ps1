<#
.SYNOPSIS
    xp-progression Mod Build Script (MSVC x64) - Modernized via CMake Presets
.DESCRIPTION
    Script de automação para compilação unificada com MSVC x64 de xp-progression.dll e testes.
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

$runningOnWindows = $true
if ($PSVersionTable.PSVersion.Major -ge 6) {
    $runningOnWindows = $IsWindows
}

# Map to the unified CMake Preset names based on Platform
$presetName = ""
$testPresetName = ""

if ($runningOnWindows) {
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
        Write-Warning "Instalação desativada: O target de instalação só é suportado no Windows onde o xp-progression é compilado."
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
        Write-Host "Compilando target xp-progression..." -ForegroundColor Cyan
        cmake --build --preset $presetName --target xp-progression
    }
    "test" {
        Write-Host "Compilando e executando testes via CTest..." -ForegroundColor Cyan
        cmake --build --preset $presetName --target xp_progression_tests
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

    Write-Host "Instalando xp-progression.dll em $modsDir..." -ForegroundColor Magenta

    $dllCandidates = @(
        (Join-Path $buildDir "src/xp-progression.dll"),
        (Join-Path $buildDir "src/$BuildType/xp-progression.dll"),
        (Join-Path $PSScriptRoot "dist/xp-progression.dll")
    )

    $dllPath = $dllCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

    if ($dllPath) {
        Copy-Item -Path $dllPath -Destination $modsDir -Force
        Write-Host " -> Copiado xp-progression.dll ($dllPath) para $modsDir" -ForegroundColor Green
    } else {
        Write-Warning "xp-progression.dll não encontrado nos caminhos candidatos."
    }
}

Write-Host "Processo concluído com sucesso!" -ForegroundColor Green
