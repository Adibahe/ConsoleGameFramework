param (
    [string]$Run
)

# --- Paths ---
$Root   = Split-Path -Parent $MyInvocation.MyCommand.Path
$Src    = Join-Path $Root "src"
$Build  = Join-Path $Root "builds"
$Header = Join-Path $Root "headers"

# --- Ensure build directory exists ---
if (-not (Test-Path $Build)) {
    New-Item -ItemType Directory -Path $Build | Out-Null
}

# --- Check for g++ ---
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $gpp) {
    Write-Error "g++ not found. Make sure MinGW or MSYS2 is on PATH."
    exit 1
}

# --- Compile all .c and .cpp files ---
Get-ChildItem $Src -File | Where-Object {
    $_.Extension -in ".c", ".cpp"
} | ForEach-Object {

    $srcFile = $_.FullName
    $exeName = [System.IO.Path]::GetFileNameWithoutExtension($_.Name) + ".exe"
    $outExe  = Join-Path $Build $exeName

    Write-Host "Compiling $($_.Name) -> builds\$exeName"

    & g++ `
        $srcFile `
        -o $outExe `
        -I"$Header" `
        -mwindows `
}
# --- Run executable if argument provided ---
if ($Run) {
    if (-not $Run.EndsWith(".exe")) {
        $Run += ".exe"
    }

    $exePath = Join-Path $Build $Run

    if (Test-Path $exePath) {
        Write-Host "Running $Run"
        & $exePath
    }
    else {
        Write-Error "Executable not found: $exePath"
    }
}
