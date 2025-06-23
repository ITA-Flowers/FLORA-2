# Tools

This directory contains utility scripts and tools for the UAV Optical Flow Navigation System. These tools are primarily written in Python and help with data preprocessing, visualization, and evaluation.

## Directory Structure

- [`/airsim`](airsim/) - AirSim drone flight scenarios

## Setup

These tools require **Python 3.12 or newer**. To set up the environment:

``` bash
# Create a virtual environment
python -m venv venv

# Activate
# -- On Linux
source venv/bin/activate # OR on Windows: venv/Scripts/activate

# Install dependencies
pip install -r requirements.txt
```

## Using Tools

> To do ...

---

## Adding New Tools

When adding new tools:

1. Place them in the appropriate subdirectory

2. Add clear documentation with examples in the script header

3. Use argparse for command-line arguments

4. Update this README.md with usage examples

5. Add any new dependencies to requirements.txt

## Coding Standards

- Follow PEP 8 style guidelines

- Include docstrings for all functions and classes

- Add appropriate error handling

- Write unit tests for complex functionality