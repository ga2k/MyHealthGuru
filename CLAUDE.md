# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this project is

MyHealthGuru is a C++23 desktop GUI application — a personal health record-keeper — built on the proprietary HoffSoft framework (`Core` + `Gfx`) with wxWidgets for the UI and SQLite (via SOCI) as the database backend.

## Build commands

Build system: CMake 3.28+ with Ninja. Available presets (on Linux x86_64):

```bash
# Configure
cmake --preset "Linux Debug Shared"
cmake --preset "Linux Release Shared"

# Build
cmake --build --preset "Linux Debug Shared"
cmake --build --preset "Linux Release Shared"

# Cross-compile to Windows from Linux
cmake --preset "WinX Debug Shared"
cmake --build --preset "WinX Debug Shared"

# Package (from build dir)
cd build/linux/debug/shared && cpack
```

Build output lands in `build/<platform>/<buildtype>/<linktype>/` (e.g. `build/linux/debug/shared/`). The staged install prefix is `~/dev/stage/linux/debug/shared/`.

There are no automated tests — the project has no test suite.

## Architecture

### Dependency hierarchy

```
MyHealthGuru (executable)
  → HoffSoft::Gfx  (staged shared library — wraps wxWidgets; found via find_package)
  → HoffSoft::Core (staged shared library — utilities, signals, config; found via find_package)
  → SOCI + SQLite3 (fetched by CMake)
  → Plugins: Appearance, Logger, Print (loaded at runtime as shared libraries)
```

`Gfx` embeds and re-exports all wxWidgets symbols. MyHealthGuru links only to `libhoffsoft_gfx.so` — it must **not** also link the static wx archive directly, as that produces duplicate-symbol errors.

### Source layout under `MyHealthGuru/src/`

| Directory | Contents |
|---|---|
| `app/` | Application entry point (`App.cpp`), `Frame`, `Document`, `DBManager`, `View`, settings pages |
| `db/` | `DBase` — app-specific SQLite database class, schema initializer |
| `ctrls/` | Custom wxWidgets controls (combo/choice widgets for email fields, titles) |
| `interface/` | UI panels (`Interface`, `Group`), wizard pages for the first-run workflow |

### Namespaces

- `mc` — all MyHealthGuru application code
- `hs` — HoffSoft framework (Core, Gfx)
- `wx` — wxWidgets wrappers provided by Gfx

### C++23 modules

Module interface files use the `.ixx` extension. Each subsystem exposes a named module (e.g. `MC.Frame`, `MC.DBManager`, `MC.DBase`). The global module fragment (`module;` block at top of `.ixx`) holds `#include` directives; exported API comes after `export module <name>;`. PCH is disabled on `.ixx` files — a Clang 21 / libc++ 21 `abi_tag` workaround; do not re-enable it.

### Code generation

Two Python scripts in `cmake/` generate `.ixx` module files from YAML at configure time:

- `cmake/yaml2rs.py` — recordset modules from `MyHealthGuru/resources/configs/generator-source/rs/*.yaml` (database row types)
- `cmake/yaml2ui.py` — UI group/page modules from `MyHealthGuru/resources/configs/generator-source/ui/*.yaml` and `resources/configs/form-layouts/*.yaml`

Regenerate manually: `python3 cmake/yaml2rs.py <yaml-file>` / `python3 cmake/yaml2ui.py <yaml-file>`. CMake drives this automatically during configure.

### CMake framework (`cmake/` submodule)

`cmake/` is a git submodule containing the shared HoffSoft build framework. Key files:

- `addLibrary.cmake` — `addLibrary()` function; use `USES GUI CORE GFX` to pull in framework libraries
- `fetchContents.cmake` — two-pass dependency resolver (declare → make-available); extends CMake `FetchContent`
- `standardPackageData.cmake` — canonical package registry (cpptrace, magic_enum, eventpp, yaml-cpp, nlohmann_json, SOCI, sqlite_orm, wxWidgets, Core, Gfx)
- `FindCore/init.cmake`, `FindGfx/init.cmake` — `find_package` handlers for the staged HoffSoft libraries
- `project_setup.cmake` — `project_setup()` macro; call once per subproject after `project()`

### App configuration

`AppSpecific.cmake` (at repo root and inside `MyHealthGuru/`) defines `APP_NAME`, `APP_VERSION`, `APP_FEATURES`, `APP_CONSUMES_PLUGINS`, etc. An embedded YAML config (`app.yaml`) is generated at configure time, checksummed, and compiled into the binary via `cmake/EmbedFile.cmake`.

### Database

`mc::DBase` extends the framework's `Database` base class. `InitializeSchema.cpp` defines the schema. The file extension for MyHealthGuru data files is `.hcdb`; the magic number is `0xC0FFEE`.

## Formatting

`.clang-format` is present at the repo root; run `clang-format` before committing changed C++ files.
