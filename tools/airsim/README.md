# AirSim

This directory contains Python scripts for running and collecting data from flight simulations in AirSim. These tools help automate test flights, gather sensor data, and evaluate navigation algorithms in a controlled virtual environment.

## Directory Structure

- `/settings` - AirSim settings for specific scenarios

- `/utils` - Auxillary Python classes like data collectors or video assembling scripts

## Prerequisites

- **AirSim** (with Unreal Engine **4.27**)

- Python **3.9**

- Required Python packages in `requirements.txt`

## Setup 

1. Install required packages:

``` bash
pip install -r requirements.txt
```

2. Configure AirSim:

    - Make sure AirSim is correctly installed with Unreal Engine
    - Copy the appropriate settings from settings/ to your AirSim settings file

## Usage

1. Run the AirSim

Example:

``` powershell
Blocks.exe -windowed -ResX=1024 -ResY=768
```

2. Run script

Example:

``` powershell
./run_square_flight.ps1
```