# Yaron's Browser

A simple, lightweight web browser built with C++ and the Qt 6 framework, using the Qt WebEngine (Chromium) module. This project started as a single-file experiment and is now a fully structured Qt application.

## Features
* Basic web navigation (Back, Forward, Go, Refresh)
* Address bar with URL loading
* Loading progress bar and status bar messages
* "View Source" functionality
* Download manager with "Save As" dialog

## How to Build
This project is built using `qmake` and the MSVC compiler on Windows.

### Prerequisites
1.  **Qt 6:** Download and install the Qt Online Installer.
    * Select `Qt 6.10.0` (or newer).
    * Ensure you select the `MSVC 2022 64-bit` component.
    * Ensure you select the `Qt WebEngine` component.
2.  **Visual Studio 2022:** Install Visual Studio 2022 Community.
    * Ensure you select the **"Desktop development with C++"** workload.

### Building from Command Line
1.  Open a `cmd` terminal.
2.  Set up the MSVC environment:
    ```
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ```
3.  Navigate to the project's root directory:
    ```
    cd path\to\your\browser-repo
    ```
4.  Run `qmake` to generate the `Makefile`:
    ```
    C:\Qt\6.10.0\msvc2022_64\bin\qmake.exe
    ```
5.  Run `nmake` to compile the project:
    ```
    nmake
    ```
6.  The executable (`browser.exe`) will be in the `build` directory.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.