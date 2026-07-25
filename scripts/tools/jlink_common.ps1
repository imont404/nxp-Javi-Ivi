<#
.SYNOPSIS
    Shared J-Link probe selection for the AVC flash and RTT wrappers.

.DESCRIPTION
    Dot-source this and call Resolve-JLinkSerial.

    Every student kit has a different J-Link serial, so nothing here is
    hardcoded to one board. Resolution order:

      1. An explicit -UsbSerial argument.
      2. $env:AVC_JLINK_SERIAL.
      3. Auto-detect, if exactly one SEGGER probe is attached.

    If more than one probe is attached and none was specified, this throws and
    lists what it found rather than guessing. That case is not hypothetical:
    a workstation driving two boards will silently flash the wrong one if the
    tooling picks whichever probe enumerates first.

    A student with a single kit never configures anything.
#>

function Get-AttachedJLinkSerial {
    <#
    .SYNOPSIS
        Return the serial numbers of all attached SEGGER probes.
    .DESCRIPTION
        Reads them from PnP instance IDs, so it does not open or disturb a
        probe that another process is already using.
    #>
    $serials = @()

    try {
        $devices = Get-PnpDevice -PresentOnly -ErrorAction Stop |
            Where-Object { $_.InstanceId -match '^USB\\VID_1366&PID_[0-9A-Fa-f]{4}\\[0-9]+$' }

        foreach ($d in $devices) {
            $sn = ($d.InstanceId -split '\\')[-1]
            # Instance IDs are zero-padded; J-Link tools want the bare number.
            $trimmed = $sn.TrimStart('0')
            if (-not [string]::IsNullOrWhiteSpace($trimmed)) {
                $serials += $trimmed
            }
        }
    } catch {
        Write-Warning "Could not enumerate J-Link probes: $($_.Exception.Message)"
        return @()
    }

    return @($serials | Sort-Object -Unique)
}

function Resolve-JLinkSerial {
    param(
        [string]$Requested = "",
        [switch]$AllowAny
    )

    if (-not [string]::IsNullOrWhiteSpace($Requested)) {
        return $Requested.Trim()
    }

    if (-not [string]::IsNullOrWhiteSpace($env:AVC_JLINK_SERIAL)) {
        Write-Host "J-Link serial from AVC_JLINK_SERIAL" -ForegroundColor DarkGray
        return $env:AVC_JLINK_SERIAL.Trim()
    }

    $found = @(Get-AttachedJLinkSerial)

    if ($found.Count -eq 1) {
        Write-Host "J-Link auto-detected: $($found[0])" -ForegroundColor DarkGray
        return $found[0]
    }

    if ($found.Count -eq 0) {
        if ($AllowAny) {
            Write-Warning "No J-Link detected; letting the tool choose."
            return ""
        }
        throw "No J-Link probe detected. Connect the FRDM-MCXN947 debug USB port, or pass -UsbSerial."
    }

    throw @"
More than one J-Link probe is attached and no serial was specified:

  $($found -join "`n  ")

Refusing to guess, because picking the wrong probe would flash or halt someone
else's board. Choose one explicitly:

  .\flash.ps1 -UsbSerial <serial> ...
  .\rtt.ps1   -UsbSerial <serial> ...

or set it once for the session:

  `$env:AVC_JLINK_SERIAL = "<serial>"
"@
}
