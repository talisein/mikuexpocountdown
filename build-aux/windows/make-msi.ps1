<#
    Stages the app (stage.ps1) and wraps the stage in a per-user .msi with
    WiX (mikuexpocountdown.wxs): build-msvc\mikuexpo-countdown-<version>-x64.msi.
    Run through gtkenv.cmd:

        C:\dev\gtkenv.cmd powershell -ExecutionPolicy Bypass -File build-aux\windows\make-msi.ps1

    wix comes from ~\AsusMiku\setup-installer-tools.ps1.
#>
[CmdletBinding()]
param(
    [string]$BuildDir
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

function Write-Step { param([string]$m) Write-Host "==> $m" -ForegroundColor Cyan }

function Invoke-Native {
    param([scriptblock]$Command)
    $prevEap = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try { & $Command 2>&1 | ForEach-Object { "$_" } }
    finally { $ErrorActionPreference = $prevEap }
}

# Windows PowerShell 5.1 leaves $PSScriptRoot empty in param defaults.
if (-not $BuildDir) { $BuildDir = Join-Path $PSScriptRoot '..\..\build-msvc' }
$BuildDir = (Resolve-Path $BuildDir).Path
$stage = Join-Path $BuildDir 'stage'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

# A global tool lands in ~\.dotnet\tools, which an ssh session that started
# before setup-installer-tools.ps1 ran does not have on PATH. Its apphost
# finds the SDK through DOTNET_ROOT.
$wix = Join-Path $env:USERPROFILE '.dotnet\tools\wix.exe'
if (-not (Test-Path $wix)) { throw "no $wix; run setup-installer-tools.ps1 from the desktop" }
if (-not $env:DOTNET_ROOT) { $env:DOTNET_ROOT = [Environment]::GetEnvironmentVariable('DOTNET_ROOT', 'User') }

& (Join-Path $PSScriptRoot 'stage.ps1') -BuildDir $BuildDir -Stage $stage
if ($LASTEXITCODE -ne 0) { throw 'stage.ps1 failed' }

# MSI versions are numeric: major.minor.build.
$info = @(Invoke-Native { meson introspect --projectinfo $BuildDir }) -join "`n" | ConvertFrom-Json
$version = $info.version
if ($version -notmatch '^\d+\.\d+\.\d+$') { throw "project version '$version' is not numeric x.y.z, which an MSI needs" }

Write-Step "wix build $version"
$msi = Join-Path $BuildDir "mikuexpo-countdown-$version-x64.msi"
$out = @(Invoke-Native { & $wix build -arch x64 -pdbtype none `
            -d "Version=$version" -d "StageDir=$stage" `
            -d "IconFile=$(Join-Path $repo 'res\dance._39music.MikuExpoCountdown.ico')" `
            -o $msi (Join-Path $PSScriptRoot 'mikuexpocountdown.wxs') })
$out | ForEach-Object { Write-Host "    $_" }
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $msi)) { throw 'wix build failed' }

Write-Step 'ICE validation'
# ICE38, ICE64 and ICE91 assume a per-machine package: they want every
# component under the user profile keyed by an HKCU value and every
# directory there listed for removal. Harvested files are keyed by
# themselves, and a per-user package only ever installs for one user.
$out = @(Invoke-Native { & $wix msi validate -sice ICE38 -sice ICE64 -sice ICE91 $msi })
$out | ForEach-Object { Write-Host "    $_" }
if ($LASTEXITCODE -ne 0) { throw 'ICE validation failed' }

Write-Step ("{0} ({1:N1} MB)" -f $msi, ((Get-Item $msi).Length / 1MB))
