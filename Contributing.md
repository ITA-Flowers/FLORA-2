# Contributing Guidelines

> **IMPORTANT:** *This is a private, copyrighted project. Contributions are by invitation only.*

## Getting Started

1. Make sure you have access to the repository.

2. Set up your development environment following the instructions in the main `README.md`.

3. Familiarize yourself with the project structure and coding standards.

## Development Workflow

### Branching Strategy

We follow a modified Gitflow workflow:

- `master` - Production-ready code, protected branch
- `develop` - Integration branch for features, protected branch
- `feature/*` - Feature branches (e.g., feature/optical-flow-optimization)
- `bugfix/*` - Bug fix branches
- `hotfix/*` - Urgent fixes for production

### Making Changes

1. Create a new branch from develop for your changes

2. Make your changes, following the coding standards

3. Write or update tests for your changes

4. Ensure all tests pass locally

5. Update documentation as needed

6. Commit your changes with clear, descriptive commit messages

### Commit Messages

**Format**:

``` text
[Component] Short summary (max 50 chars)

More detailed explanation if necessary. Wrap lines at 72 characters.
Explain what the change does and why it's needed.

Fixes #123 (reference issues if applicable)
```

**Short example**:

``` text
[Dead Reckoning] Kalman filter implementation
```

**Long example**:

``` text
[OpticalFlow] Improve performance of feature tracking

Optimized the feature tracking algorithm to reduce processing time by 30%
by implementing a more efficient matching strategy.

Fixes #45
```

### Pull Requests

1. Push your branch to the repository

2. Create a pull request to the develop branch

3. Fill out the PR template completely

4. Assign reviewers (at least one other team member)

5. Address all review comments

6. Ensure CI checks pass

7. PR will be merged by the reviewer after approval

## Code Review Process

### For Authors

- Break down large changes into smaller, focused PRs

- Explain your approach and any important decisions

- Be responsive to feedback

- Don't take criticism personally

### For Reviewers

- Review PRs in a timely manner (within 2 business days)

- Be respectful and constructive

- Focus on substance over style

- Consider both correctness and maintainability

- Use the "Request changes" option sparingly

## Coding Standards

### C++

- Follow the Google C++ Style Guide (with exceptions noted below)
- Use C++17 features appropriately
- Class names: `CamelCase`
- Method names: `camelCase`
- Variable names: `snake_case`
- Constants: `kConstantName`
- Indentation: 4 spaces (no tabs)
- Line length: 120 characters maximum
- Always use braces for control structures
- Place opening brace on the same line
- Document public APIs using Doxygen-style comments

### Python

- Follow PEP 8
- Use type hints (PEP 484)
- Document functions with docstrings (PEP 257)
- Use f-strings for string formatting
- Maximum line length: 88 characters (Black formatter default)

### Testing Requirements

- All new code must have corresponding tests
- Unit tests for individual components
- Integration tests for interacting components
- Maintain or improve code coverage
- Tests must be fast and deterministic

### Documentation

- Update documentation for all user-facing changes
- Document public APIs with detailed descriptions, parameters, and return values
- Include examples for complex functionality
- Keep design documents up-to-date

### Performance Considerations

- Be mindful of performance in critical paths
- Consider resource constraints of target platforms
- Document performance characteristics of algorithms
- Add benchmarks for performance-critical code

## License and Copyright

All contributions to this project are subject to the project's copyright. By contributing, you agree that your work becomes part of the project and subject to the same copyright terms.