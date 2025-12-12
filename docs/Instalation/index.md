# Installation Guide

This guide provides detailed instructions for installing all the necessary dependencies and building the R-Type project on both Linux and Windows platforms.

## Table of Contents

- [System Requirements](#system-requirements)
- [Linux Installation](#linux-installation)
  - [Ubuntu/Debian](#ubuntudebian)
  - [Fedora/RHEL](#fedorarhel)
  - [Arch Linux](#arch-linux)
- [Windows Installation](#windows-installation)
  - [Using Visual Studio](#using-visual-studio)
  - [Using MinGW-w64](#using-mingw-w64)
- [Building the Project](#building-the-project)
  - [Linux Build](#linux-build)
  - [Windows Build](#windows-build)
- [Running the Project](#running-the-project)
- [Verifying the Installation](#verifying-the-installation)
- [Troubleshooting](#troubleshooting)

---

## System Requirements

### Minimum Requirements

- **Operating System**: Linux (Ubuntu 20.04+, Debian 11+, Fedora 35+, Arch Linux) or Windows 10/11
- **RAM**: 4 GB minimum, 8 GB recommended
- **Disk Space**: 2 GB free space for dependencies and build artifacts
- **C++ Compiler**: 
  - Linux: GCC 7+ or Clang 6+
  - Windows: Visual Studio 2019+ (with C++ Desktop Development) or MinGW-w64
- **CMake**: Version 3.21 or higher
- **Git**: For cloning the repository

### Dependencies

All third-party libraries are automatically downloaded and built by CMake using CPM (C++ Package Manager):

- **SFML 2.6.1** - Graphics and multimedia library
- **Asio 1.28.0** - Asynchronous I/O library for networking
- **spdlog 1.12.0** - Fast C++ logging library
- **Google Test 1.14.0** - C++ testing framework

> **Note**: You don't need to manually install these libraries. CMake will automatically download and build them during the configuration step.

---

## Linux Installation

### 1. Install System Dependencies

#### Ubuntu/Debian

```bash
# Update package lists
sudo apt update

# Install build essentials
sudo apt install -y build-essential

# Install CMake (if not already installed or version < 3.21)
sudo apt install -y cmake

# Check CMake version
cmake --version

# If CMake version is < 3.21, install from Kitware repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null
sudo apt update
sudo apt install -y cmake

# Install Git
sudo apt install -y git

# Install SFML system dependencies (required for SFML compilation)
sudo apt install -y \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libflac-dev \
    libogg-dev \
    libvorbis-dev \
    libvorbisenc2 \
    libvorbisfile3 \
    libopenal-dev \
    libfreetype6-dev
```

#### Fedora/RHEL

```bash
# Update system
sudo dnf update -y

# Install development tools
sudo dnf groupinstall -y "Development Tools"

# Install CMake
sudo dnf install -y cmake

# Check CMake version
cmake --version

# If CMake version is < 3.21, install from source or use pip
# sudo pip3 install cmake --upgrade

# Install Git
sudo dnf install -y git

# Install SFML system dependencies
sudo dnf install -y \
    libX11-devel \
    libXrandr-devel \
    libXcursor-devel \
    libXi-devel \
    systemd-devel \
    mesa-libGL-devel \
    flac-devel \
    libogg-devel \
    libvorbis-devel \
    openal-soft-devel \
    freetype-devel
```

#### Arch Linux

```bash
# Update system
sudo pacman -Syu

# Install base development tools
sudo pacman -S --needed base-devel

# Install CMake
sudo pacman -S cmake

# Install Git
sudo pacman -S git

# Install SFML system dependencies
sudo pacman -S \
    libx11 \
    libxrandr \
    libxcursor \
    libxi \
    systemd \
    mesa \
    flac \
    libogg \
    libvorbis \
    openal \
    freetype2
```

### 2. Clone the Repository

```bash
# Navigate to your desired directory
cd ~

# Clone the R-Type repository
git clone https://github.com/eliestroun14/R-Type.git

# Navigate into the project directory
cd R-Type
```

---

## Windows Installation

### Using Visual Studio

#### 1. Install Visual Studio

1. Download **Visual Studio 2019** or **Visual Studio 2022** from [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/)
2. Run the installer
3. Select **"Desktop development with C++"** workload
4. Ensure the following components are selected:
   - MSVC v142 or v143 - VS 2019/2022 C++ x64/x86 build tools
   - Windows 10/11 SDK
   - C++ CMake tools for Windows
   - C++ Clang tools for Windows (optional)
5. Click **Install** and wait for the installation to complete

#### 2. Install CMake

**Option A: Through Visual Studio Installer**
- CMake is included with Visual Studio's C++ CMake tools

**Option B: Standalone Installation**
1. Download CMake from [https://cmake.org/download/](https://cmake.org/download/)
2. Run the installer and select **"Add CMake to the system PATH for all users"**
3. Complete the installation

#### 3. Install Git

1. Download Git for Windows from [https://git-scm.com/download/win](https://git-scm.com/download/win)
2. Run the installer with default settings
3. Complete the installation

#### 4. Verify Installation

Open **PowerShell** or **Command Prompt** and verify:

```powershell
# Check CMake version
cmake --version

# Check Git version
git --version

# Check compiler (Visual Studio)
cl
```

#### 5. Clone the Repository

```powershell
# Navigate to your desired directory
cd C:\Users\YourUsername\Documents

# Clone the R-Type repository
git clone https://github.com/eliestroun14/R-Type.git

# Navigate into the project directory
cd R-Type
```

### Using MinGW-w64

#### 1. Install MSYS2

1. Download MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
2. Run the installer and follow the installation wizard
3. Complete the installation to `C:\msys64`

#### 2. Install Development Tools

Open **MSYS2 MinGW 64-bit** terminal and run:

```bash
# Update package database
pacman -Syu

# Close and reopen the terminal if prompted, then run:
pacman -Su

# Install development tools
pacman -S --needed base-devel mingw-w64-x86_64-toolchain

# Install CMake
pacman -S mingw-w64-x86_64-cmake

# Install Git
pacman -S git

# Install SFML system dependencies
pacman -S \
    mingw-w64-x86_64-libvorbis \
    mingw-w64-x86_64-flac \
    mingw-w64-x86_64-freetype \
    mingw-w64-x86_64-openal
```

#### 3. Add MinGW to System PATH

1. Open **System Environment Variables**
2. Edit the **Path** variable
3. Add `C:\msys64\mingw64\bin` to the path
4. Click **OK** to save

#### 4. Clone the Repository

Open **MSYS2 MinGW 64-bit** terminal:

```bash
# Navigate to your desired directory
cd /c/Users/YourUsername/Documents

# Clone the R-Type repository
git clone https://github.com/eliestroun14/R-Type.git

# Navigate into the project directory
cd R-Type
```

---

## Building the Project

### Linux Build

```bash
# Navigate to the project directory
cd R-Type

# Create a build directory
mkdir -p build
cd build

# Configure the project with CMake
cmake ..

# Build the project (use -j flag for parallel compilation)
cmake --build . -j$(nproc)

# Alternatively, you can use make directly
# make -j$(nproc)
```

### Windows Build

#### Using Visual Studio

**Option 1: Using Command Line**

```powershell
# Navigate to the project directory
cd R-Type

# Create a build directory
mkdir build
cd build

# Configure the project with CMake for Visual Studio
cmake ..

# Build the project in Release mode
cmake --build . --config Release

# Or build in Debug mode
cmake --build . --config Debug
```

**Option 2: Using Visual Studio IDE**

1. Open Visual Studio
2. Select **"Open a local folder"**
3. Navigate to and select the `R-Type` directory
4. Visual Studio will automatically detect the CMakeLists.txt and configure the project
5. Select the build configuration (Debug/Release) from the toolbar
6. Press **F7** or select **Build > Build All** to build the project

#### Using MinGW-w64

Open **MSYS2 MinGW 64-bit** terminal:

```bash
# Navigate to the project directory
cd R-Type

# Create a build directory
mkdir -p build
cd build

# Configure the project with CMake for MinGW
cmake .. -G "MinGW Makefiles"

# Build the project
cmake --build . -j$(nproc)
```

---

## Running the Project

### Linux

```bash
# From the build directory
# Run the server
./bin/r-type_server

# In another terminal, run the client
./bin/r-type_client
```

### Windows

#### Using Visual Studio Build

```powershell
# From the build directory
# Run the server
.\bin\Release\r-type_server.exe

# In another PowerShell window, run the client
.\bin\Release\r-type_client.exe
```

#### Using MinGW Build

```bash
# From the build directory (in MSYS2 terminal)
# Run the server
./bin/r-type_server.exe

# In another MSYS2 terminal, run the client
./bin/r-type_client.exe
```

---

## Verifying the Installation

### Run Tests

To verify that everything is installed and built correctly, run the test suite:

#### Linux

```bash
# From the build directory
ctest --output-on-failure

# Or run tests with more verbosity
ctest -V
```

#### Windows (Visual Studio)

```powershell
# From the build directory
ctest -C Release --output-on-failure

# Or for Debug build
ctest -C Debug --output-on-failure
```

#### Windows (MinGW)

```bash
# From the build directory (in MSYS2 terminal)
ctest --output-on-failure
```

### Check Build Artifacts

Verify that the executables were created:

#### Linux

```bash
ls -la bin/
# You should see: r-type_server, r-type_client
```

#### Windows

```powershell
dir bin\Release\
# You should see: r-type_server.exe, r-type_client.exe
```

---

## Troubleshooting

### Common Issues

#### CMake Version Too Old

**Symptom**: Error message about CMake version requirement

**Solution**:
- **Linux**: Follow the Kitware repository installation steps above for your distribution
- **Windows**: Download the latest CMake from [cmake.org](https://cmake.org/download/)

#### Missing SFML Dependencies (Linux)

**Symptom**: Compilation errors related to X11, OpenGL, or audio libraries

**Solution**: Make sure you've installed all the SFML system dependencies listed in the installation section for your distribution

#### Visual Studio Not Found (Windows)

**Symptom**: CMake cannot find the Visual Studio compiler

**Solution**:
1. Open **Visual Studio Installer**
2. Modify your installation
3. Ensure **"Desktop development with C++"** is installed
4. Run CMake from **"Developer Command Prompt for VS"** or **"Developer PowerShell for VS"**

#### CMake Cannot Find Compiler (Windows MinGW)

**Symptom**: CMake error about missing compiler

**Solution**:
1. Verify MinGW bin directory is in PATH: `echo $env:Path` (PowerShell) or `echo $PATH` (MSYS2)
2. Use MSYS2 MinGW 64-bit terminal instead of regular Command Prompt
3. Specify the generator explicitly: `cmake .. -G "MinGW Makefiles"`

#### Network Issues During Build

**Symptom**: CPM fails to download dependencies

**Solution**:
1. Check your internet connection
2. If behind a proxy, configure Git proxy:
   ```bash
   git config --global http.proxy http://proxy.example.com:8080
   ```
3. Try building again - CMake will resume from where it stopped

#### Permission Denied (Linux)

**Symptom**: Permission errors when installing packages

**Solution**: Ensure you're using `sudo` for system package installations

#### Out of Memory During Compilation

**Symptom**: Compiler crashes or system freezes during build

**Solution**:
- Reduce parallel jobs: Use `-j2` or `-j4` instead of `-j$(nproc)`
- Close other applications to free up RAM
- Consider building in Release mode which uses less memory than Debug

### Getting Help

If you encounter issues not covered here:

1. Check the [GitHub Issues](https://github.com/eliestroun14/R-Type/issues) page
2. Review the project's [Contributing Guide](../CONTRIBUTING.md)
3. Open a new issue with:
   - Your operating system and version
   - CMake version (`cmake --version`)
   - Compiler version
   - Complete error message
   - Steps to reproduce the problem

---

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [SFML Documentation](https://www.sfml-dev.org/documentation/)
- [Asio Documentation](https://think-async.com/Asio/)
- [Project README](../../README.md)