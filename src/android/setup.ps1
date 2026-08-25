[CmdletBinding()]
param(
    [switch]$AcceptLicenses,
    [switch]$Force
)

$ErrorActionPreference = "Stop"
& (Join-Path $PSScriptRoot "tools\setup-toolchain.ps1") `
    -AcceptLicenses:$AcceptLicenses `
    -Force:$Force

