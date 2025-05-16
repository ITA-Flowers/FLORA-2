if ($args.Length -ge 2 -and $args[0] -eq "--") {
    $Mode = $args[1]
} elseif ($args.Length -eq 1) {
    $Mode = $args[0]
} elseif (-not $Mode) {
    Write-Host "Usage: ./run_flight_controller.ps1 -- square|circle|straight OR -Mode <value>" -ForegroundColor Yellow
    exit 1
}

# Settings
$TimeStamp = Get-Date -Format "yyyyMMdd_HHmmss"
$OUTPUT_DIR = "..\..\..\data\simulation\$Mode-flight_$TimeStamp"
$FLIGHT_SCRIPT = "..\flight_controller.py"
$SIZE = 50
$ALTITUDE = 40
$SPEED = 10

# Color functions
function Write-Cyan   { param($msg) Write-Host $msg -ForegroundColor Cyan }
function Write-Yellow { param($msg) Write-Host $msg -ForegroundColor Yellow }
function Write-Green  { param($msg) Write-Host $msg -ForegroundColor Green }
function Write-Red    { param($msg) Write-Host $msg -ForegroundColor Red }

# Header
Write-Cyan "==================================================================="
Write-Cyan "Starting $Mode Flight Mission with data collection"
Write-Cyan "Output directory: $OUTPUT_DIR"
Write-Cyan "Flight size: $SIZE meters"
Write-Cyan "Flight altitude: $ALTITUDE meters"
Write-Cyan "Flight speed: $SPEED m/s"
Write-Cyan "==================================================================="

# Check if AirSim is running
Write-Yellow "Checking if AirSim is running..."
$airsim_status = Get-NetTCPConnection -LocalPort 41451 -ErrorAction SilentlyContinue
if ($null -eq $airsim_status) {
    Write-Red "ERROR: AirSim is not running. Please start it first."
    exit 1
}
Write-Green "AirSim is running. Continuing..."

# Wait for AirSim to be ready
Write-Yellow "Waiting a moment for AirSim to initialize..."
Start-Sleep -Seconds 3

# Create output directory
if (-not (Test-Path $OUTPUT_DIR)) {
    New-Item -ItemType Directory -Path $OUTPUT_DIR -Force | Out-Null
    Write-Green "Created output directory: $OUTPUT_DIR"
}

# Run the flight
Write-Yellow "Starting $Mode flight..."
try {
    python $FLIGHT_SCRIPT `
        --mode $Mode `
        --output $OUTPUT_DIR `
        --size $SIZE `
        --altitude $ALTITUDE `
        --speed $SPEED

    if ($LASTEXITCODE -ne 0) {
        Write-Red "ERROR: Flight failed with exit code $LASTEXITCODE"
        exit 1
    }
}
catch {
    Write-Red "ERROR: Flight failed with exception: $_"
    exit 1
}

# Check for video
$VIDEO_PATH = "$OUTPUT_DIR\${Mode}_flight.mp4"
if (Test-Path $VIDEO_PATH) {
    Write-Cyan "==================================================================="
    Write-Green "$Mode flight mission completed successfully!"
    Write-Green "Data saved at: $OUTPUT_DIR"
    Write-Green "Video path: $VIDEO_PATH"
    Write-Cyan "==================================================================="
}
else {
    Write-Yellow "WARNING: Flight completed, but no video found."
    Write-Yellow "Check manually at: $OUTPUT_DIR"
}
