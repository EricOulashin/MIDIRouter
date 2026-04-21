# Visual Studio 2022

This project builds with **CMake**. The canonical project file is the **`CMakeLists.txt`** in the repository root.

## Option A — CMake integration (recommended)

1. Install [Visual Studio 2022](https://visualstudio.microsoft.com/) with the **Desktop development with C++** workload and **CMake tools for Windows**.
2. Install [Qt 6](https://www.qt.io/download) (or build from source) and set **CMAKE_PREFIX_PATH** to the Qt installation (e.g. `C:/Qt/6.x.x/msvc2019_64`).
3. In Visual Studio: **File → Open → CMake…** and select the root **`CMakeLists.txt`** of this repo.
4. Choose **x64** and build the `midirouter` target.

## Option B — Generate a Visual Studio solution from the command line

From a **x64 Native Tools Command Prompt for VS 2022** (after Qt is on `CMAKE_PREFIX_PATH`):

```bat
cd path\to\MIDIRouter
cmake -S . -B vs\build-vs2022-x64 -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

Open the generated **`vs\build-vs2022-x64\MIDIRouter.sln`** (or **`midirouter.sln`**, depending on CMake version).

The same layout is defined in **`CMakePresets.json`** at the repo root (`vs2022-x64` preset, `binaryDir` under `vs/build-vs2022-x64`).

## Dependencies

- **Qt 6** (Core, Gui, Widgets, Network)
- **RtMidi** (headers + library)

On Windows, **vcpkg**, **Qt Online Installer**, or **MSYS2** (`mingw-w64-x86_64-qt6-base`, `mingw-w64-x86_64-rtmidi`) are common ways to satisfy these.
