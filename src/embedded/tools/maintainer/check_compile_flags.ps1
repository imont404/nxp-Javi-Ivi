param(
    [Parameter(Mandatory = $true)]
    [string]$CompileCommands,

    [string]$ExpectedOptimization = "-O2"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $CompileCommands)) {
    throw "Compile command database not found: $CompileCommands"
}

$commands = Get-Content -Raw -LiteralPath $CompileCommands | ConvertFrom-Json
$cCommands = @($commands | Where-Object { [System.IO.Path]::GetExtension($_.file) -eq ".c" })
$errors = New-Object System.Collections.Generic.List[string]

foreach ($entry in $cCommands) {
    $optimization = @(
        [regex]::Matches(
            $entry.command,
            '(?<!\S)-O(?:0|1|2|3|s|g|fast)(?!\S)'
        ) | ForEach-Object { $_.Value }
    )

    if (($optimization.Count -ne 1) -or ($optimization[0] -ne $ExpectedOptimization)) {
        $errors.Add("$($entry.file): optimization flags [$($optimization -join ', ')]")
    }

    foreach ($requiredFlag in @("-g3", "-gdwarf-4", "-fno-builtin")) {
        if ($entry.command -notmatch "(?<!\S)$([regex]::Escape($requiredFlag))(?!\S)") {
            $errors.Add("$($entry.file): missing $requiredFlag")
        }
    }
}

if ($cCommands.Count -eq 0) {
    throw "Compile command database contains no C sources: $CompileCommands"
}

if ($errors.Count -gt 0) {
    throw "Competition compile-flag contract failed:`n  $($errors -join "`n  ")"
}

Write-Host "Verified $($cCommands.Count) C compile commands: $ExpectedOptimization, debug symbols, retained -fno-builtin." -ForegroundColor Green
