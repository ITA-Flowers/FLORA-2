# PowerShell script to run a square flight mission in AirSim with data collection

# Set the output directory
$TimeStamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$OUTPUT_DIR = "..\..\data\airsim\square-flight_$TimeStamp"
$SQUARE_SIZE = 80  # meters
$ALTITUDE = 100     # meters
$SPEED = 10         # m/s

# Print start message
Write-Host "===================================================================" -ForegroundColor Cyan
Write-Host "Starting Square Flight Mission with data collection" -ForegroundColor Cyan
Write-Host "Output directory: $OUTPUT_DIR" -ForegroundColor Cyan
Write-Host "Square size: $SQUARE_SIZE meters" -ForegroundColor Cyan
Write-Host "Flight altitude: $ALTITUDE meters" -ForegroundColor Cyan
Write-Host "Flight speed: $SPEED m/s" -ForegroundColor Cyan
Write-Host "===================================================================" -ForegroundColor Cyan

# Check if AirSim is running
Write-Host "Checking if AirSim is running..." -ForegroundColor Yellow
$airsim_status = Get-NetTCPConnection -LocalPort 41451 -ErrorAction SilentlyContinue
if ($null -eq $airsim_status) {
    Write-Host "ERROR: AirSim does not appear to be running. Please start AirSim before running this script." -ForegroundColor Red
    exit 1
}
Write-Host "AirSim is running. Continuing..." -ForegroundColor Green

# Wait a bit to ensure AirSim is fully loaded
Write-Host "Waiting a moment to ensure AirSim is fully initialized..." -ForegroundColor Yellow
Start-Sleep -Seconds 3

# Create output directory
if (-not (Test-Path $OUTPUT_DIR)) {
    New-Item -ItemType Directory -Path $OUTPUT_DIR -Force | Out-Null
    Write-Host "Created output directory: $OUTPUT_DIR" -ForegroundColor Green
}

# Run the square flight script
Write-Host "Starting square flight..." -ForegroundColor Yellow
try {
    python ./scenarios/square_flight.py `
        --output $OUTPUT_DIR `
        --size $SQUARE_SIZE `
        --altitude $ALTITUDE `
        --speed $SPEED
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Square flight failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit 1
    }
}
catch {
    Write-Host "ERROR: Square flight failed with exception: $_" -ForegroundColor Red
    exit 1
}

# Check if the flight was successful
if (Test-Path "$OUTPUT_DIR\square_flight.mp4") {
    Write-Host "===================================================================" -ForegroundColor Cyan
    Write-Host "Square flight mission completed successfully!" -ForegroundColor Green
    Write-Host "Data has been collected and saved to: $OUTPUT_DIR" -ForegroundColor Green
    Write-Host "Video of the flight: $OUTPUT_DIR\square_flight.mp4" -ForegroundColor Green
    Write-Host "===================================================================" -ForegroundColor Cyan
}
else {
    Write-Host "WARNING: Flight completed but video may not have been created." -ForegroundColor Yellow
    Write-Host "Check the data in: $OUTPUT_DIR" -ForegroundColor Yellow
}