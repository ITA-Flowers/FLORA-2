#!/bin/bash

MODE=$1
if [[ -z "$MODE" ]]; then
  echo "Usage: $0 <mode: square|circle|straight>"
  exit 1
fi

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
OUTPUT_DIR="../../data/simulation/${MODE}-flight_$TIMESTAMP"
FLIGHT_SCRIPT="../flight_controller.py"
SIZE=30
ALTITUDE=40
SPEED=15

CYAN='\033[0;36m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' 

echo -e "${CYAN}==================================================================="
echo -e "Starting $MODE Flight Mission with data collection"
echo -e "Output directory: $OUTPUT_DIR"
echo -e "Flight size: $SIZE meters"
echo -e "Flight altitude: $ALTITUDE meters"
echo -e "Flight speed: $SPEED m/s"
echo -e "===================================================================${NC}"

echo -e "${YELLOW}Checking if AirSim is running...${NC}"
if ! lsof -i :41451 &>/dev/null; then
  echo -e "${RED}ERROR: AirSim is not running. Please start it first.${NC}"
  exit 1
fi
echo -e "${GREEN}AirSim is running. Continuing...${NC}"

echo -e "${YELLOW}Waiting for AirSim to fully initialize...${NC}"
sleep 3

if [ ! -d "$OUTPUT_DIR" ]; then
  mkdir -p "$OUTPUT_DIR"
  echo -e "${GREEN}Created output directory: $OUTPUT_DIR${NC}"
fi

echo -e "${YELLOW}Starting $MODE flight...${NC}"
python3 "$FLIGHT_SCRIPT" \
  --mode "$MODE" \
  --output "$OUTPUT_DIR" \
  --size "$SIZE" \
  --altitude "$ALTITUDE" \
  --speed "$SPEED"

VIDEO_PATH="$OUTPUT_DIR/${MODE}_flight.mp4"
if [ -f "$VIDEO_PATH" ]; then
  echo -e "${CYAN}==================================================================="
  echo -e "${GREEN}$MODE flight mission completed successfully!"
  echo -e "Data collected at: $OUTPUT_DIR"
  echo -e "Video saved at: $VIDEO_PATH"
  echo -e "===================================================================${NC}"
else
  echo -e "${YELLOW}WARNING: Flight finished, but video not found."
  echo -e "Check collected data manually at: $OUTPUT_DIR${NC}"
fi
