# Justification for Selecting SFML as the Graphics Library

## Context
The project mandates a lightweight 2D client, UDP networking, a multithreaded authoritative server, a minimal engine architecture, cross-platform compatibility (Linux/Windows), CMake integration, package-manager availability, and full delivery within seven weeks by a small team. The priority is not graphical innovation but structural correctness: engine design, networking reliability, maintainability, and documentation quality.

## Why ?
SFML provides a narrow, targeted feature set that matches the project’s requirements without introducing unnecessary abstraction layers. It delivers window creation, an implicit OpenGL context, straightforward 2D rendering primitives, sprites, textures, fonts, audio, input handling, and timing utilities. All components are native C++, portable, and packaged cleanly through vcpkg or Conan with full MSVC compatibility.

SFML does not impose architectural constraints. Rendering stays isolated from logic and networking, allowing the construction of a clean separation of concerns. Performance is sufficient for a 2D shooter.

**Moreover all the team is already used to the SFML.**
