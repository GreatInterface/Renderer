# Repository Guidelines

## Project Structure & Module Organization
- `Include/`: Public headers (math, renderer helpers, application interface).
- `Src/`: C++ sources (SDL app loop, drawing, entry point).
- `assets/`: Runtime assets copied into the build output.
- `CMakeLists.txt` and `CMakePresets.json`: CMake configuration and a vcpkg preset.
- Build output is typically under `build/` (preset) or `cmake-build-debug/` (IDE).

## Build, Test, and Development Commands
This repo uses CMake + vcpkg (SDL2, imgui, glm).
- Configure with preset:
  - `cmake --preset vcpkg`
  - Generates build files in `build/` with the vcpkg toolchain.
- Build:
  - `cmake --build build`
- Run (example):
  - `build/Renderer.exe`

If you are not using presets, ensure `VCPKG_ROOT` is set and pass
`-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake`.

## Coding Style & Naming Conventions
- C++23, 4-space indentation, braces on new lines (follow existing files).
- Types/classes use `PascalCase` (e.g., `Application`, `Matrix44`).
- Functions and methods use `PascalCase` (e.g., `DrawLine`, `ComputeBarycentric2D`).
- Variables use `camelCase` (e.g., `screenTexture`, `framebuffer`).
- Keep headers lightweight; prefer inline helpers in `Include/` as shown in `Renderer.h`.

## Testing Guidelines
No test framework is currently configured. If you add tests, document how to
build and run them and keep them under a dedicated folder such as `Tests/`.

## Commit & Pull Request Guidelines
This directory is not a git repository, so there is no commit history to infer
conventions. If you introduce version control:
- Use clear, imperative commit messages (e.g., “Fix matrix multiply order”).
- In PRs, include a short summary, build/run notes, and screenshots for visual changes.

## Configuration Tips
- Ensure SDL2 is available via vcpkg.
- Window size is dynamic; use `Application::GetWidth()`/`GetHeight()` in render code.
