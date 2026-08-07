# C++ Advanced GUI Template

This project provides a lightweight modern **C++ GUI template** for quickly setting up a cross-platform development environment with a graphical user interface - powered by Vulkan.
It uses only free and open-source software, tools and libraries under permissive licenses, including **VSCodium**, **Clang**, **CMake**, **Ninja**, **GLFW**, **ImGui** and **Git**.
It is designed as a ready-to-use starting point for developing or experimenting with modern C++ and Vulkan on **Windows**, **macOS**, and **Linux**.

The project is intended as a starting point, not a complete rendering framework or prescribed architecture. The Vulkan implementation intentionally avoids introducing wrapper classes, rendering APIs or unnecessary abstractions. Instead, related operations are grouped together by responsibility, such as window creation, shader and geometry management, and rendering. This keeps the examples easy to understand while also providing a practical foundation for future development. These sections can be extracted almost directly into your own window, shader, mesh or renderer classes without first having to reorganize the project.

## Features

- Ready-to-use modern C++ development environment out-of-the-box.
- Lightweight empty project template with sensible defaults and configurations.
- Cross-platform support (Windows, macOS, Linux).
- Preconfigured C++20 project using CMake and Ninja.
- Preconfigured Vulkan/GUI support via GLFW and ImGui.
- Automated GLSL shader compilation to SPIR-V via glslc.
- Integrated Clang compiler and clangd language server support.
- Automated code formatting with clang-format.
- Static analysis with clang-tidy.
- Integrated LLDB debugging support.
- Git-ready project structure with sensible exclusions.
- MIT licensed for personal, educational, and commercial use.

## Prerequisites

Install the following prerequisites for this project to function as intended.

*Note: If you prefer, you can use VS Code instead of VSCodium, and Microsoft Visual Studio Build Tools instead of LLVM-MinGW on Windows. The project will function identically. VSCodium and LLVM-MinGW are simply recommended here to keep the toolchain entirely open-source on all platforms.*

### 1) IDE

**VSCodium**<br>
*Open source VSCode binary.*<br>
https://github.com/VSCodium/vscodium/releases

- Download the binary installer for your platform and install. On Linux, avoid the Flatpak version as sandbox restrictions can cause permission and filesystem access issues with development tools. Use the `.deb` package instead.

#### Extensions

Open VSCodium and go the extensions tab *(Ctrl + Shift + X)* and install the following:
- **clangd**<br>
*Code completion & diagnostics.*<br>
https://open-vsx.org/vscode/item?itemName=llvm-vs-code-extensions.vscode-clangd

- **CMake Tools**<br>
*Build integration.*<br>
https://open-vsx.org/vscode/item?itemName=ms-vscode.cmake-tools

- **CodeLLDB**<br>
*Debugger.*<br>
https://open-vsx.org/vscode/item?itemName=vadimcn.vscode-lldb

- **Hex Editor (Optional)**<br>
*Memory viewer.*<br>
https://open-vsx.org/vscode/item?itemName=ms-vscode.hexeditor

### 2) C++ Toolchain

*Standard library, system headers, compiler & linker.*

- ***Windows***<br>
    https://github.com/mstorsjo/llvm-mingw/releases
    - Download **LLVM-MinGW** for your platform via the link above (e.g. `llvm-mingw-xxxxxx-ucrt-x86_64.zip`) and install.
    - Extract the contents to a folder on your drive (e.g. `C:\llvm-mingw`).
    - Add the **bin** folder path (e.g. `C:\llvm-mingw\bin`) to your system environment **PATH** variable.
        - Go to Start and type `env` and select *'Edit the system environment variables'*.
        - Click *'Environment Variables...'* and under *'System variables'* select *'Path'* and click *'Edit'*.
        - Click *'New'* and then paste the bin folder path (e.g. `C:\llvm-mingw\bin`) and click *'OK'*.

- ***macOS***<br>
    https://apps.apple.com/us/app/xcode/id497799835
    - Download Xcode via the App Store *(includes Git)*.
    - Launch Xcode once to complete its initial setup, then close it.

- ***Linux***
    - Use the terminal command: `sudo apt install -y libc++-dev libc++abi-dev clang lld xorg-dev libx11-dev libwayland-dev wayland-protocols libxkbcommon-dev extra-cmake-modules`.

*(Optional)* Verify via terminal `clang --version` - should display clang version number.

### 3) Vulkan SDK
*Vulkan headers, loader and shader compiler.*<br>
https://vulkan.lunarg.com/sdk/home

- **Windows**
    - Download the binary installer for your platform and install.
    - Add the SDK folder path (e.g. `C:\VulkanSDK\`) to your system environment **PATH** variable.
        - Go to Start and type `env` and select *'Edit the system environment variables'*.
        - Click *'Environment Variables...'* and under *'System variables'* if `VULKAN_SDK` does not already exist, select *'Path'* and click *'Edit'*.
        - Click *'New'* and then enter `%VULKAN_SDK%\Bin` and click *'OK'*.

- **macOS**
    - Download the binary installer for your platform and install *(includes MoltenVK to automatically enable Vulkan support on macOS via Metal)*.
    - Open a terminal and enter `nano ~/.zshrc` and add to the bottom of the file `source "$HOME/VulkanSDK/1.4.xxx.x/setup-env.sh"` *(replacing 1.4.xxx.x with the version number)*.
    - Save and exit nano (*'Ctrl + O' then 'Ctrl + X'*). 
    - Enter `source ~/.zshrc`.

- **Linux**
    - Use the terminal command: `sudo apt install -y libvulkan-dev vulkan-tools vulkan-validationlayers glslc spirv-tools mesa-vulkan-drivers`.

(Optional) Verify via terminal `glslc --version` - should display the glslc version number.

### 4) Build Tools

**CMake**<br>
*Build configuration.*<br>
https://cmake.org/download/

- ***Windows***
    - Download the binary installer for your platform and install.
    - Make sure **'Add CMake to the PATH environment variable'** is checked.

- ***macOS***
    - Download the binary installer for your platform and install.
    - Open terminal and enter `sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install=/usr/local/bin` to add CMake to your PATH environment variable.

- ***Linux***
    - Use the terminal command: `sudo apt install -y cmake`.

*(Optional)* Verify via terminal `cmake --version` - should display cmake version number.

**Ninja**<br>
*Build system.*<br>
https://github.com/ninja-build/ninja/releases

- ***Windows***
    - Download the binary zip for your platform and extract.
    - Move the binary file to your **LLVM-MinGW** bin folder (e.g. `C:\llvm-mingw\bin`).

- ***macOS***
    - Download the binary zip for your platform and extract.
    - Move the binary file to `/usr/local/bin`.
    - Launch the binary file. If it fails the security check, go to *'System Settings -> Privacy & Security'* and allow Ninja.

- ***Linux***
    - Use the terminal command: `sudo apt install -y ninja-build`.

*(Optional)* Verify via terminal `ninja --version` - should display ninja version number.

### 5) Version Control

**Git**<br>
*Open source version control.*<br>
https://git-scm.com/install/

- ***Windows***
    - Download the binary installer for your platform and install.

- ***macOS***
    - Installed automatically as part of **Xcode** *(see above)*.

- ***Linux***
    - Use the terminal command: `sudo apt install -y git`.

*(Optional)* Verify via terminal `git --version` - should display git version number.
    
Use the terminal command: `git config --global user.name "Your Name"`.<br>
Use the terminal command: `git config --global user.email youremail@example.com`.

## Getting Started

1) Open VSCodium.
2) Press *Ctrl + Shift + P* to open the command palette and search for **'Git: Clone'** and select it.
3) Enter this repository URL: `https://github.com/PeterHoghtonDigital/cpp-advanced-gui-template.git`
4) Choose which folder you want to download the project to and wait for it to download.
- Alternatively, if you prefer to use the terminal, you can clone via `git clone https://github.com/PeterHoghtonDigital/cpp-advanced-gui-template.git /path/to/folder`.
5) Select *'Open'* to open the folder as the current workspace.
6) Wait for CMake to automatically fetch and download the required third-party libraries.
7) The first time you open a C++ file it will prompt you to install the latest clangd language server, select **'Install'**.
8) The Vulkan implementation can be found in `Source/vulkan.h`.
9) The project includes simple Vulkan mesh, shader and ImGui examples intended to demonstrate the basic rendering pipeline without introducing unnecessary abstractions, making them easy to understand, modify and expand. These can be found in `Source/examples.h` and `Data/Shaders/`.

### Project Structure

```
.github/
.vscode/
├─ launch.json
├─ settings.json
└─ tasks.json
Build/
Data/
└─ Shaders/
   ├─ example.vert
   └─ example.frag
External/
Source/
├─ examples.h
├─ main.cpp
└─ vulkan.h
.clang-format
.clang-tidy
.clangd
.gitignore
CMakeLists.txt
CMakePresets.json
LICENSE.md
README.md
```

The project is structured so that all necessary source files are automatically included in the build, while generated and other irrelevant files are excluded from Git.

- **Build** - Generated binaries, compiled shaders and intermediate build files.
- **Data** - Place local data and large binary assets here.
- **Data/Shaders** - Place your GLSL shaders here.
- **External** - Place third-party libraries here.
- **Source** - Place your C++ source and header files here.

### Build Instructions

Once setup is complete, the project can be built, run and debugged out of the box. First select the *'Build Config'* button in the status bar and select one of the following options:

- ***Windows***
    - **Windows Debug** - No optimization, full debug symbols, outputs to *'Build/Windows/Debug'*.
    - **Windows Release (w/ Debug)** - Full optimization, partial debug symbols, outputs to *'Build/Windows/ReleaseWithDebug'*.
    - **Windows Release** - Full optimization, no debug symbols, outputs to *'Build/Windows/Release'*.

- ***macOS***
    - **macOS Debug** - No optimization, full debug symbols, outputs to *'Build/Mac/Debug'*.
    - **macOS Release (w/ Debug)** - Full optimization, partial debug symbols, outputs to *'Build/Mac/ReleaseWithDebug'*.
    - **macOS Release** - Full optimization, no debug symbols, outputs to *'Build/Mac/Release'*.

- ***Linux***
    - **Linux Debug** - No optimization, full debug symbols, outputs to *'Build/Linux/Debug'*.
    - **Linux Release (w/ Debug)** - Full optimization, partial debug symbols, outputs to *'Build/Linux/ReleaseWithDebug'*.
    - **Linux Release** - Full optimization, no debug symbols, outputs to *'Build/Linux/Release'*.

You can then use the build and run buttons in the status bar on the selected configuration to build and run your application.

You can also manually run build tasks via **'Ctrl + Shift + P' -> 'Tasks: Run Task'**, choosing from:
- **Build** - Builds the project (**'Ctrl + Shift + B'**).
- **Run** - Runs the executable as a standalone application *(no debugger)*.
- **Build & Run** - Builds the project, then runs the executable as a standalone application *(no debugger)*.
- **Rebuild** - Cleans and builds the project.
- **Clean** - Cleans the active configuration output directory *(deletes existing binary and intermediate files)*.

The project comes with several predefined macros that you can use for platform-dependent compilation:

- `WINDOWS`
- `MAC`
- `LINUX`

### Debugging

You can use the debug button in the status bar on the selected configuration to start debugging your application.

You can also manually run debugging tasks via **'Ctrl + Shift + D'** to open the *'Run & Debug'* panel and choose:
- **Debug** - Runs the application and attaches the debugger.
- **Build & Debug** - Builds the project, then runs the application and attaches the debugger.

While debugging you can open the **Memory View** *(requires Hex Editor extension)* by selecting the *'View Binary Data'* icon next to any variable in the *'Run & Debug'* panel.

The project also defines configuration-dependent macros:

- `DEBUG`
- `RELEASE`

`RELEASE` is defined for both `Release` and `RelWithDebInfo` configurations.

## Additional Information

This project is primarily developed and tested on **Linux**.
Windows and macOS are supported but testing on these platforms is more limited.

If you encounter problems or would like to suggest an improvement, please submit an [issue](https://github.com/PeterHoghtonDigital/cpp-advanced-gui-template/issues) on GitHub and I will investigate when possible.