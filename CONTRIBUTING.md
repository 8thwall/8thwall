# Contributing to 8th Wall

Thank you for your interest in contributing to 8th Wall! This document explains how to get involved.

## AI-Generated Code

Contributors may use AI tools to assist with contributions. By submitting a PR you are certifying that:

- You have reviewed any AI-generated code in full
- You take responsibility for its correctness and licensing
- You are not knowingly submitting code derived from incompatible licenses

## How to Contribute

### Reporting Bugs

- Check existing issues before opening a new one
- Include steps to reproduce, expected behavior, and actual behavior
- Include your environment details (OS, browser, version, etc.)

### Suggesting Features

- GitHub Discussions are the preferred way to discuss feature requests
- Describe the use case, not just the feature
- Be open to feedback and alternative approaches

### Submitting a Pull Request

1. Fork the repo and clone your fork onto your local machine
1. See the below instructions for local development setup instructions
2. Make your changes
3. Add or update tests as needed
4. Ensure tests and linting passes
5. Push your branch to your fork
6. Open a PR against the `main` branch, following [pull request best practices](https://github.com/8thwall/archive/blob/main/wiki/engineering-quality/pull-request-best-practices.md).

## Local Development

Setup instructions are still being finalized, and may vary depending on the area of the repository you're working in.

The main requirements are:

1. Installing Bazel through [Bazelisk](https://github.com/bazelbuild/bazelisk).
2. Installing [Buildifier 5.1.0](https://github.com/bazelbuild/buildtools/releases/5.1.0)
3. Installing node through [nvm](https://github.com/nvm-sh/nvm)
4. Setting up a virtualenv: 
   1. Install python3.11 (On mac, `brew install python@3.11`)
   2. `python3.11 -m venv ~/venv-8thwall`
   3. `source ~/venv-8thwall/bin/activate`
   4. `pip install -r requirements.txt`
5. Set up root dependencies `./scripts/apply-npm-rule npm-eslint .`

If there is a snag, additional setup instructions are [available in the archive](https://github.com/8thwall/archive?tab=readme-ov-file#pre-bootstrap-script) but may not all be applicable.

Some projects may have their own setup steps, for example running `npm install` in project folders. Check the README.md files in any folders you plan to work in.

## Code Style

Please run the following before sending Pull Requests.

```bash
./scripts/lint.sh
```

A more detailed style guide is available [here](https://github.com/8thwall/archive/blob/main/wiki/engineering-quality/code-style-guide.md)

## Review Process

- All PRs require at least one maintainer approval before merging
- The Technical Steering Committee (TSC) reviews and approves significant changes
- Maintainers may request changes or close PRs that are out of scope

## Community

Have a question or want to discuss an idea before opening an issue? Use [GitHub Discussions](https://github.com/orgs/8thwall/discussions).
