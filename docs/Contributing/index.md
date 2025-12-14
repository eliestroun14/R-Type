# Contributing to R-Type

Welcome! This guide explains the different ways you can contribute to the R-Type project and helps you find the right resources for your contribution type.

## Ways to Contribute

### 1. 📝 Improve Documentation

Help us keep our documentation clear, accurate, and comprehensive. No coding required!

**What you can do:**

- Fix typos or unclear explanations
- Add missing documentation sections
- Improve code examples and tutorials
- Update outdated information
- Translate documentation (future)

**Where to start:**

- Read the [Documentation Guide](Documentation/index.md) to understand our documentation standards
- Check the [Writing Guide](Documentation/writing-guide.md) for style and formatting rules
- Look for issues labeled `documentation` on GitHub

**Tools needed:**

- Git (for version control)
- Text editor or Markdown editor
- GitHub account

---

### 2. 🐛 Report Bugs

Found something broken? Help us fix it by reporting bugs clearly and completely.

**What makes a good bug report:**

- Clear description of the issue
- Steps to reproduce
- Expected vs actual behavior
- Environment details (OS, compiler version, etc.)
- Screenshots or error messages (if applicable)

**Where to report:**

- Open an issue on GitHub with the `bug` label
- Include as much detail as possible
- Check if the bug is already reported before opening a new issue

---

### 3. ✨ Suggest Features

Have an idea to make R-Type better? Share your feature requests!

**What we're interested in:**

- New game engine features
- Better logging capabilities
- Network improvements
- Performance enhancements
- Developer experience improvements

**How to suggest:**

- Open an issue on GitHub with the `enhancement` label
- Describe the feature and why it would be useful
- Provide examples or use cases if possible
- Discuss with maintainers before starting implementation

---

### 4. 💻 Write Code

Contribute code improvements, new features, or bug fixes.

**Before you start:**

- Check [Expectations](Expectations/expectations.md) for project requirements
- Understand the project structure and architecture
- Read the [Game Engine documentation](../GameEngine&Architecture/index.md)
- Look for issues labeled `good first issue` or `help wanted`

**What we need:**

- Bug fixes with tests
- New features with documentation
- Performance improvements
- Code refactoring (with justification)
- Build system improvements

**Code contribution process:**

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Make your changes
4. Follow project coding standards
5. Add/update tests
6. Update documentation if needed
7. Push to your fork
8. Create a Pull Request

**Code quality expectations:**

- Code must be well-documented
- Tests must pass
- Follow C++ best practices (C++17/20)
- Maintain consistency with existing code style
- No external dependencies without justification

---

### 5. 🧪 Write Tests

Help us maintain code quality by writing and improving tests.

**Test types we need:**

- Unit tests for individual components
- Integration tests for system interactions
- Performance tests for critical paths

**Where to start:**

- Read [Why Google Test?](../Tests/Overview/index.md) to understand our testing framework
- Check [How to add new tests](../Tests/index.md)
- Look at existing tests for patterns

**What to test:**

- ECS components and systems
- Logger functionality
- Network protocols
- Game logic edge cases

---

### 6. 🔍 Code Review

Review pull requests and help maintain code quality.

**How to participate:**

- Review open PRs on GitHub
- Suggest improvements
- Test the changes
- Check documentation updates
- Ensure code quality standards

---

## Getting Started

### Step 1: Understand the Project

- Read the [Game Engine & Architecture overview](../GameEngine&Architecture/index.md)
- Understand the [ECS system](../GameEngine&Architecture/ECS/index.md)
- Explore the [Logger system](../Logger/index.md)

### Step 2: Choose Your Contribution Type

Pick from the types listed above based on your interests and skills.

### Step 3: Check Requirements

- For **documentation**: Follow the [Writing Guide](Documentation/writing-guide.md)
- For **code**: Check [Expectations](Expectations/expectations.md)
- For **bugs/features**: Comment on the issue first

### Step 4: Make Your Changes

- Create a branch or fork
- Make your improvements
- Test thoroughly
- Write clear commit messages

### Step 5: Submit Your Contribution

- Push to your fork/branch
- Create a Pull Request
- Provide clear description
- Link related issues
- Be patient for review

---

## Questions?

- Check existing GitHub issues
- Read the relevant documentation
- Ask in pull request comments
- Contact project maintainers

## Code of Conduct

Be respectful and constructive. We welcome everyone regardless of experience level!

---

## Recognition

All contributors are valued! Your contributions help make R-Type better.

Thank you for contributing! 🙏
