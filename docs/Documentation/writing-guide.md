# Contributing to R-Type Documentation

This guide explains how to add and organize documentation for the R-Type project. Documentation is automatically deployed to ReadTheDocs on every push to the main branch.

## Table of Contents

1. [Project Structure](#project-structure)
2. [Adding a New Page](#adding-a-new-page)
3. [Routing System (Navigation)](#routing-system-navigation)
4. [Markdown Syntax](#markdown-syntax)
5. [Advanced Features](#advanced-features)
6. [Contribution Workflow](#contribution-workflow)

---

## Project Structure

```
R-Type/
├── mkdocs.yml           # Main configuration
└── docs/                # 📁 All documentation files go here
    ├── index.md         # Homepage
    ├── contributing.md
    ├── requirements.txt # Python dependencies
    ├── extra.css        # Custom CSS styles
    ├── getting-started/
    │   ├── index.md
    │   └── install.md
    ├── guide/
    │   ├── basics/
    │   └── advanced/
    └── api/
        └── reference.md
```

**⚠️ Important**: All your Markdown files must be in the `docs/` folder.

---

## Adding a New Page

### Step 1: Create the Markdown File

Create a new `.md` file in the appropriate folder:

```bash
# Example: Add a page about network protocol
touch docs/architecture/protocol.md
```

### Step 2: Write the Content

Edit the file with your content:

```markdown
# Network Protocol

This document describes the UDP communication protocol used by R-Type.

## Overview

The protocol is binary and UDP-based...

## Packet Structure

### Connection Packet

| Field | Type  | Size   | Description |
|-------|-------|--------|-------------|
| ID    | uint8 | 1 byte | Identifier  |
| ...   | ...   | ...    | ...         |
```

### Step 3: Add the Page to Navigation

Open `mkdocs.yml` and add your page to the `nav` section:

```yaml
nav:
  - Home: index.md
  
  - Architecture:
    - architecture/index.md
    - Protocol: architecture/protocol.md  # ← Your new page
    - Server: architecture/server.md
```

### Step 4: Commit and Push

Once your changes are made, commit and push:

```bash
git add docs/architecture/protocol.md mkdocs.yml
git commit -m "docs: add network protocol documentation"
git push
```

**ReadTheDocs will automatically rebuild the documentation** and it will be available in a few minutes at `https://r-type-rennes.readthedocs.io/en/latest` or `https://r-type-rennes.readthedocs.io/en/dev`

---

## Routing System (Navigation)

Routing defines how pages appear in the sidebar menu. Everything is configured in `mkdocs.yml` under the `nav` section.

### Basic Syntax

```yaml
nav:
  - Displayed Title: path/to/file.md
```

### Structure Types

#### 1. Simple Page

```yaml
nav:
  - Home: index.md
  - Contributing: contributing.md
```

**Result**: Two links at the same level in the sidebar.

---

#### 2. Section with Sub-pages

```yaml
nav:
  - Getting Started:
    - Installation: getting-started/install.md
    - Configuration: getting-started/config.md
```

**Result**: "Getting Started" section in **bold** (expandable) containing 2 sub-pages.

> **💡 Why bold?** The `navigation.sections` feature automatically bolds sections containing sub-pages to improve readability.

---

#### 3. Section with Index Page

```yaml
nav:
  - API Reference:
    - api/index.md          # Page displayed when clicking "API Reference"
    - Classes: api/classes.md
    - Functions: api/functions.md
```

**Result**: Clicking "API Reference" displays `api/index.md`, while still providing access to sub-pages.

> **⚠️ Important**: Requires the `navigation.indexes` feature (already enabled in our config).

---

#### 4. Nested Sections (Multi-level)

```yaml
nav:
  - Documentation:
    - Getting Started:
      - docs/getting-started/index.md
      - Installation: docs/getting-started/install.md
    - Advanced Topics:
      - Architecture: docs/advanced/architecture.md
      - Performance: docs/advanced/performance.md
```

**Result**: Multi-level hierarchy with sections and sub-sections.

---

#### 5. External Link

```yaml
nav:
  - GitHub: https://github.com/eliestroun14/R-Type
  - Report Bug: https://github.com/eliestroun14/R-Type/issues
```

**Result**: Clickable links that open in a new tab.

---

### Complete Navigation Example

```yaml
nav:
  # Homepage
  - Home: index.md
  
  # Simple section
  - Contributing: contributing.md
  
  # Section with index and sub-pages
  - Getting Started:
    - getting-started/index.md
    - Installation: getting-started/install.md
    - Configuration: getting-started/config.md
  
  # Section with sub-sections
  - Architecture:
    - architecture/index.md
    - Client:
      - Overview: architecture/client/overview.md
      - Rendering: architecture/client/rendering.md
    - Server:
      - Overview: architecture/server/overview.md
      - Networking: architecture/server/networking.md
    - Protocol: architecture/protocol.md
  
  # Simple section with multiple pages
  - API Reference:
    - api/index.md
    - Classes: api/classes.md
    - Functions: api/functions.md
  
  # External links
  - GitHub: https://github.com/eliestroun14/R-Type
```

---

### Routing Best Practices

#### ✅ DO

1. **Organize logically**: Group pages by theme
2. **Folder structure = navigation structure**:

   ```
   docs/
   ├── getting-started/
   │   ├── index.md
   │   └── install.md
   └── architecture/
       ├── index.md
       └── protocol.md
   ```

3. **Use index.md for important sections**:

   ```yaml
   - Architecture:
     - architecture/index.md  # Overview
     - Protocol: architecture/protocol.md
   ```

4. **Clear and descriptive names**:

   ```yaml
   - Network Protocol: architecture/protocol.md  # ✅ Clear
   ```

#### ❌ DON'T

1. **Paths too deep**: Maximum 3-4 levels
2. **Redundant names**:

   ```yaml
   - Architecture:
     - Architecture Overview: architecture/overview.md  # ❌ "Architecture" repeated
   ```

3. **Orphan pages**: Every important page should be in the `nav`

---

## See Also

- [Creating a New System](creating-systems.md)
- [Available Components](components-reference.md)

```

---

## Support

If you have questions about documentation:

1. **Open an issue** on GitHub with the `documentation` label
2. **Ask in the channel** `#documentation` on Discord
3. **Check examples** in existing pages

Happy documenting! 📚
