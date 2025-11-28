# Contributing to R-Type

Welcome to the R-Type project! This guide will help you understand how to contribute effectively to the project, particularly regarding our commit message conventions.

## Table of Contents

1. [Commit Message Convention](#commit-message-convention)
2. [Commit Types](#commit-types)
3. [Commit Structure](#commit-structure)
4. [Examples](#examples)
5. [Best Practices](#best-practices)
6. [Programming Case Types](#programming-case-types)
7. [FAQ](#faq)

---

## Commit Message Convention

We follow the **Conventional Commits 1.0.0** specification to maintain a clear and structured commit history. This convention makes it easier to:

- Automatically generate CHANGELOGs
- Determine semantic version bumps
- Communicate changes clearly to the team
- Trigger automated build and publish processes
- Navigate and understand project history

---

## Commit Types

### Required Types

| Type | Description | Semantic Versioning |
|------|-------------|---------------------|
| `feat` | Introduces a new feature to the codebase | MINOR version bump |
| `fix` | Patches a bug in the codebase | PATCH version bump |

### Additional Types

| Type | Description | Example Use Case |
|------|-------------|------------------|
| `docs` | Documentation changes only | Update README, add comments |
| `style` | Code style changes (formatting, whitespace) | Run code formatter, fix indentation |
| `refactor` | Code refactoring without feature or bug changes | Rename variables, restructure code |
| `perf` | Performance improvements | Optimize algorithms, reduce memory usage |
| `test` | Add or update tests | Add unit tests, fix test cases |
| `build` | Changes to build system or dependencies | Update CMakeLists.txt, add libraries |
| `ci` | Changes to CI/CD configuration | Update GitHub Actions, Docker configs |
| `chore` | Other changes that don't modify src or test files | Update .gitignore, maintenance tasks |

### Breaking Changes

A **BREAKING CHANGE** introduces an incompatible API change and correlates with a MAJOR version bump. It can be indicated in two ways:

1. Add `!` after the type/scope: `feat!:` or `feat(api)!:`
2. Add a `BREAKING CHANGE:` footer in the commit body

---

## Commit Structure

### Basic Format

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

### Components

#### 1. Type (REQUIRED)

A noun describing the nature of the change: `feat`, `fix`, `docs`, etc.

#### 2. Scope (REQUIRED)

A noun describing the section of the codebase affected, enclosed in parentheses:

```
feat(server): add new matchmaking system
fix(client): resolve rendering issue
docs(api): update protocol documentation
```

**Common scopes in R-Type:**
- `server` - Server-side changes
- `client` - Client-side changes
- `engine` - Game engine modifications
- `network` - Networking layer changes
- `api` - API changes
- `ui` - User interface updates

#### 3. Description (REQUIRED)

A short summary of the changes:

- **Must** immediately follow the colon and space
- Use imperative mood: "add" not "added" or "adds"
- Don't capitalize the first letter
- No period (.) at the end
- Maximum 72 characters recommended

✅ **Good:**
```
fix(main): resolve memory leak in entity manager
feat(dev): add support for custom player skins
```

❌ **Bad:**
```
fix(main): Fixed a bug.
feat(dev): Adding new features to the game.
```

#### 4. Body (OPTIONAL)

Provides additional context about the changes:

- **Must** begin one blank line after the description
- Free-form text, can span multiple paragraphs
- Explain **what** and **why**, not **how**

#### 5. Footer(s) (OPTIONAL)

Additional metadata, provided one blank line after the body:

- References to issues: `Refs: #123`
- Breaking changes: `BREAKING CHANGE: description`
- Reviewers: `Reviewed-by: Name`

---

## Examples

### Simple Commit (No Body)

```bash
git commit -m "feat(lang): add French language support"
```

```bash
git commit -m "perf(engine): optimize entity collision detection"
```

### Commit with Body

```bash
git commit -m "fix(dev): resolve player synchronization issues

Introduce request ID and reference to latest state update.
Dismiss incoming updates that are outdated.

Refs: #456"
```

### Breaking Change (with !)

```bash
git commit -m "feat(api)!: change protocol to binary format"
```

### Breaking Change (with footer)

```bash
git commit -m "feat(dev): redesign configuration system

BREAKING CHANGE: configuration file format changed from JSON to YAML.
Users must migrate their config files."
```

### Complete Example

```bash
git commit -m "fix(server): prevent racing of client requests

Introduce a request ID and a reference to latest request. Dismiss
incoming responses other than from latest request.

Remove timeouts which were used to mitigate the racing issue but are
obsolete now.

Reviewed-by: John Doe
Refs: #789"
```

---

## Best Practices

### ✅ DO

1. **Keep commits atomic**: One logical change per commit
2. **Write clear descriptions**: Be specific about what changed
3. **Use the imperative mood**: "add feature" not "added feature"
4. **Reference issues**: Include issue numbers in footers when applicable
5. **Be consistent**: Follow the convention for every commit

```bash
# ✅ Good - Clear and specific
git commit -m "feat(client): add fullscreen toggle in settings menu"

# ✅ Good - Proper scope usage
git commit -m "fix(network): resolve packet loss on high latency"

# ✅ Good - Breaking change properly indicated
git commit -m "refactor(api)!: rename Player class to GameEntity"
```

### ❌ DON'T

1. **Vague descriptions**: Avoid generic messages
2. **Mix multiple changes**: Don't combine unrelated changes
3. **Ignore the format**: Always follow the convention
4. **Write novels**: Keep descriptions concise

```bash
# ❌ Bad - Too vague
git commit -m "fix stuff"

# ❌ Bad - No type
git commit -m "updated the server code"

# ❌ Bad - Mixed changes
git commit -m "feat: add login system, fix bug, update docs"

# ❌ Bad - Wrong mood
git commit -m "feat: added new feature"
```

---

## FAQ

### What if my commit fits multiple types?

Go back and split your changes into multiple commits. This is a benefit of Conventional Commits - it encourages organized commits.

```bash
# Split into separate commits:
git add src/server/auth.cpp
git commit -m "feat(server): add authentication system"

git add tests/auth_test.cpp
git commit -m "test(server): add tests for authentication"

git add docs/auth.md
git commit -m "docs(docs): add authentication documentation"
```

### Should I use uppercase or lowercase for types?

Use **lowercase** for consistency. While any casing is allowed by the spec, our project uses lowercase.

```bash
# ✅ Correct
git commit -m "feat(main): add new feature"

# ❌ Avoid
git commit -m "FEAT(main): add new feature"
```

### What if I made a mistake in my commit message?

**Before pushing:**
```bash
# Amend the last commit message
git commit --amend -m "fix(client): correct typo in shader code"

# Or edit multiple commits interactively
git rebase -i HEAD~3
```

**After pushing:**
- Contact the team lead for guidance
- Minor mistakes in commit messages are not critical

### Do all contributors need to follow this?

Yes! All commits to the repository should follow the Conventional Commits specification. If you're squashing commits during a pull request merge, the final squashed commit must follow the convention.

### How do I handle revert commits?

Use the `revert` type and reference the commit being reverted:

```bash
git commit -m "revert(render): remove experimental rendering feature

Refs: 676104e"
```

---

## Workflow Example

Here's a complete workflow for contributing:

1. **Create a feature branch:**
   ```bash
   git checkout -b feat/add-multiplayer-lobby
   ```

2. **Make your changes and commit following the convention:**
   ```bash
   git add src/server/lobby.cpp
   git commit -m "feat(server): add multiplayer lobby system"
   
   git add tests/lobby_test.cpp
   git commit -m "test(server): add lobby system tests"
   
   git add docs/lobby.md
   git commit -m "docs(docs): add lobby system documentation"
   ```

3. **Push your branch:**
   ```bash
   git push origin feat/add-multiplayer-lobby
   ```

4. **Create a Pull Request** on GitHub following the PR template

5. **Wait for review and merge**

---

## Programming Case Types

| Type | Case used | Example |
|------|-------------|------------------|
| `class` and `struct` | 👨‍🔬 PascalCase `PascalCase` | `class EntityManager` or `struct MovementPattern` |
| Methods and variables | 🐫 `camelCase` | `std::string getEntityName()` or `int entityId` |
| Variables in `class` | 🐫 `camelCase` but with a `_` before the variable name | `std::size_t _entityId` |

---

## Need Help?

If you have questions about contributing:

1. **Open an issue** on GitHub with the `question` label
2. **Ask in the team chat**
3. **Check existing commits** for examples: `git log --oneline`

Thank you for contributing to R-Type! 🚀
