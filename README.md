<p align="center">
  <img src="icon.svg" alt="Godot Logo" width="128" height="128" />
</p>

<h1 align="center">Godot Script Switcher</h1>

<p align="center">
  <b>Quick switching of open scripts in the ScriptEditor with Ctrl + Tab.</b>
</p>

<p align="center">
  <!-- <a href="https://godotengine.org/asset-library/asset/xxxx">
    <img src="https://img.shields.io/badge/Godot%20Asset%20Lib-4.6%2B-478cbf?logo=godot-engine" alt="Godot Asset Library">
  </a> -->
  <img src="https://img.shields.io/github/license/travlee/godot-script-switcher" alt="License">
</p>

This is a plugin for the [Godot Game Engine](https://godotengine.org/) created with [godot-cpp](https://github.com/godotengine/godot-cpp) that provides quick switching between recently opened scripts using a *currently* hardcoded keyboard shortcut: `Ctrl + Tab`. This is a recreation of VSCode's Quick Open for MRU (Most Recently Used) files, improving workflow efficiency for developers who are familiar with this ... workflow. Mainly just a pain point for **me** working in Godot - so here we are.

---

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Features](#features)
- [Usage](#usage)
- [Installation](#installation)
  - [~~Godot Asset Library (recommended)~~](#godot-asset-library-recommended)
  - [~~Releases (also easy)~~](#releases-also-easy)
  - [Manual (fun)](#manual-fun)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)

## Features

*   **Most Recently Used (MRU) Script History**: Keeps a list of the scripts you have open.
*   **Quick Switch Popup**: A compact popup window with recent scripts by their file names.
*   **Keyboard-driven Navigation**: Navigate and select scripts within the popup using familiar keyboard shortcuts.
*   **Contextual Activation**: The popup only appears when the Script Editor is the active tab, as that makes sense for a script switcher.
*   **Full Path Tooltips**: Each script in the list displays its full path as a tooltip.

## Usage

Once the plugin is enabled, you can use the following keyboard shortcuts:

*   **Trigger the popup**:
    *   Press `Ctrl + Tab` and hold `Ctrl`.
    *   A popup will appear, displaying a list of your open scripts, sorted by how recently they were used.
    *   *Note: the second item in the list (the previously active script) will be selected, allowing for quick toggling between your two most recent scripts.*

*   **Navigate the List**:
    *   While still holding `Ctrl`, press `Tab`, `Down Arrow`, or `Up Arrow` to select a script.

*   **Select a script**:
    *   Release the `Ctrl` key.
    *   The script currently highlighted in the popup will be opened in the Script Editor.

## Installation

### ~~Godot Asset Library (recommended)~~

> Note: Not uploaded as of yet.

1. Open Godot → **AssetLib** tab
2. Search **"[Godot Script Switcher](https://godotengine.org/asset-library/asset/xxxx)"** → **Download**
3. In the install dialog, make sure **"Ignore asset root"** is **checked**, then click **Install**
4. **Project → Project Settings** → **Plugins** → Enable **Godot Script Switcher**
5. **Restart the Editor**

### ~~Releases (also easy)~~

> Note: No releases as of yet.

Head to releases and download whatever version you want. I'll probably have files for Windows/Linux/Mac available there.

1. Download a release zip.
   * [Latest release](https://github.com/Travlee/godot-script-switcher/releases/latest)
   * [All releases](https://github.com/Travlee/godot-script-switcher/releases)
2. Extract to your project's addons dir: `../addons/godot-script-switcher`.
3. Enable the plugin:
   1. **Project** → **Project Settings** → **Plugins**
   2.   Click to enable **Godot Script Switcher**

### Manual (fun)
You'll have to compile it on your target platform and then enable it in your Godot project. This project requries CMake & GCC.

1.  **Clone the Repository**.
    ```bash
    cd godot-scipt-switcher/
    git clone https://github.com/travlee/godot-script-switcher.git .
    ```

2.  **Compiling**:
    This plugin is written in C++ using `godot-cpp`. You must compile the plugin for your specific OS and architecture using CMake.
    *   From the root of the project, configure the CMake build:
        ```bash
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
        ```
    *   Build the plugin:
        ```bash
        cmake --build build --config Release
        ```
    *   After building, simply copy the `./godot/addons/godot_script_switcher/` dir to your project's `./addons/` dir.

3.  **Enable the Plugin in Godot**:
    *   **Project** → **Project Settings** → **Plugins**
    *   Click to enable **Godot Script Switcher**


## Technical Details

This plugin is implemented in C++, using Godot's **GDExtension** with [godot-cpp](https://github.com/godotengine/godot-cpp) bindings for blazingly fast performance and deep integration with the Godot editor. Also I wanted to.

The **ScriptSwitcher** class tracks script changes by binding to the `ScriptEditor::editor_script_changed` signal, updating an MRU history variable stored as a **vector** type. It overrides the global `_inputs` method to listen for `Ctrl + Tab` combinations and only displays the quick-open popup if the script editor is active. The popup UI is designed in Godot and saved to a `.tscn` file and dynamically loaded in when the plugin is enabled.

## Contributing

If you encounter any bugs, have suggestions for new features, or would like to improve the existing code, please feel free to:

*  **Open an Issue**: Describe the bug or feature request in detail on the GitHub issue tracker.
*   **Submit a Pull Request**: Fork the repo, make some changes, and submit a PR.

## License

[MIT](LICENSE)
