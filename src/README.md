# Source Code

This directory contains the main C++ source code for the **FLORA**.

## Directory Structure

- `/app` - Program entrypoint with argument parser, config and main file

- `/core` - Math core data structures and types like Matrix, Vector, Quaternion, etc.

- `/nav-dr` - Implementation of dead reckoning algorithms

- `/nav-of` - Optical flow detection and processing

- `/nav-sf` - Data fusion algorithms

- `/io` - Input/output operations

## Adding New Components

1. Create header and implementation files in the appropriate subdirectory

2. Update the corresponding `CMakeLists.txt` file

3. Document the public API in the header file

4. Add unit tests for the new component in the /tests directory

## Coding Standards

All source code must follow the project's coding standards as defined in the [`CONTRIBUTING.md`](../Contributing.md) file located in the root directory. Key points:

- Use consistent naming conventions

- Document all public functions and classes

- Write unit tests for all new functionality

- Handle errors appropriately

## Building

