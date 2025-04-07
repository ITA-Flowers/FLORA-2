# Tests

This directory contains unit and integration tests for the UAV Optical Flow Navigation System.

## Directory Structure

- `/unit` - Unit tests for individual components

    - `/core` - Tests for math core components
    
    - `/nav-dr` - Tests for dead reckoning algorithms
  
    - `/nav-of` - Tests for optical flow processing
  
    - `/nav-sf` - Tests for sensor fusion algorithms
  
    - `/io` - Tests for input/output operations

- `/integration` - Integration tests that validate multiple components working together

## Test Organization

We organize tests using both directories and CTest labels:

- **Directory organization**: Tests are organized in directories corresponding to system components

    - `/unit` - Tests for individual components
    - `/integration` - Tests that validate multiple components together

- **Test labels**: We use CTest labels to categorize tests across directories

    - `UnitTests` - Fast, focused tests of individual components
    - `IntegrationTests` - Tests of component interactions

Labels allow running specific types of tests regardless of their directory location.

## Adding Test Labels

Labels are defined in `CMakeLists.txt` for each test target:

``` cmake
add_app_test(matrix_tests unit/core/MatrixTests.cpp "UnitTests;Core") # Test type: Unit; Package: Core
```

> **Tip**: You can add multiple labels to one test suite with listing them and separate with ';'.

## Running Tests

We use Google Test framework for C++ testing. To run the tests:

``` bash
# Build the tests
cd build
cmake ..
make tests

# Run all tests
ctest

# Run specific test suite
ctest -R DeadReckoningTests

# Run with verbose output
ctest -V -R OpticalFlowTests

# Run parallel tests (e.g., using 4 cores)
ctest -j4

# Run tests with labels
ctest -L UnitTests
ctest -L IntegrationTests

ctest -T Test
```

## Adding New Tests

1. Create a new test file in the appropriate subdirectory

    - Name the file as <component_name>_test.cpp
    
    - For a new component, create a corresponding subdirectory if needed


2. Write test cases following the Google Test format:

``` cpp
#include "gtest/gtest.h"
#include "component_to_test.h"

TEST(TestSuiteName, TestName) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(expected, actual);
}
```

3. Add the test file to the appropriate `CMakeLists.txt`

## Continuous Integration

All tests are automatically run on each pull request. PRs cannot be merged if tests are failing.

## Writing Good Tests

- Each test should focus on a single functionality

- Use appropriate test fixtures to reduce code duplication

- Mock external dependencies when necessary

- Test both normal cases and edge cases

- Keep tests independent from each other

- Make test failures descriptive to aid debugging