# SystemFontLoader for Unreal Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
<!-- Add other badges if you like, e.g., UE version compatibility -->

(Read this document in other languages: [日本語](README.ja.md)) <!-- Optional: If you plan to provide a Japanese version -->

## Overview

SystemFontLoader is an Unreal Engine plugin that allows you to discover and load fonts installed on the Windows operating system directly within your project. This enables dynamic font selection and usage in UMG Widgets or Slate UI without needing to import font assets manually beforehand.

This plugin is primarily designed for **Windows platforms**. It utilizes the Windows Registry and GDI functions to find font files and retrieve their names.

## Features

*   **Discover System Fonts:** Automatically scans the Windows Registry on engine startup to find installed fonts and their corresponding file paths.
*   **Cache Font Information:** Stores discovered font information in an Engine Subsystem for efficient access.
*   **Blueprint Accessible:** Provides Blueprint nodes to:
    *   Get a list of available system font names (cache keys).
    *   Load a system font by name and size into an `FSlateFontInfo` struct, ready for use in UMG Text Blocks or Slate elements.
    *   Retrieve a user-friendly display name for a given font cache key (e.g., getting "Arial" from an internal key).
*   **Windows Only:** Functionality is conditionally compiled and active only on Windows platforms.

## Requirements

*   Unreal Engine (Tested with UE 5.x, should be adaptable to others)
*   **Operating System:** Windows (Vista or later recommended due to API usage)

## Installation

1.  **Download:** Obtain the plugin from the [GitHub Releases](https://github.com/YOUR_USERNAME/SystemFontLoader/releases) page (replace with your actual link) or clone the repository.
2.  **Create Plugins Folder:** If your Unreal Engine project doesn't already have one, create a folder named `Plugins` in your project's root directory (alongside `.uproject`, `Content`, `Source`, etc.).
3.  **Copy Plugin:** Copy the entire `SystemFontLoader` folder into the `Plugins` directory.
4.  **Restart Editor:** Close and reopen your Unreal Engine project.
5.  **Build Plugin:** The editor might prompt you to build the missing module. Click "Yes". If it doesn't, try compiling your project from your IDE (Visual Studio) or right-click the `.uproject` file, select "Generate Visual Studio project files", and then build the project solution (`Ctrl+Shift+B` in VS).
6.  **Enable Plugin:** Go to `Edit` -> `Plugins`. Search for "SystemFontLoader" (likely under the "Project" -> "Other" category) and ensure the "Enabled" checkbox is ticked. You might need to restart the editor again.

## Usage (Blueprints)

All Blueprint nodes provided by this plugin can be found under the **"System Font Loader"** category.

### 1. Getting Available Font Names

*   Use the **`Get System Font Names`** node.
    *   **Outputs:**
        *   `Return Value (Boolean)`: `true` if successful (running on Windows and subsystem available), `false` otherwise.
        *   `Out Font Names (Array of String)`: An array containing the *cache keys* for the discovered system fonts. These keys are derived from registry entries and might not always be the most user-friendly names (e.g., "Arial" instead of "Arial (TrueType)").

    *   **Example:** You can use this array to populate a UMG ComboBox or List View, allowing the user to select a font.

    ![Get System Font Names Node](https://via.placeholder.com/300x100.png?text=GetSystemFontNames+Node) <!-- Replace with actual screenshot -->

### 2. Loading a Font as FSlateFontInfo

*   Use the **`Load System Font As SlateFontInfo`** node.
    *   **Inputs:**
        *   `Font Name (String)`: The font cache key obtained from `Get System Font Names`.
        *   `Font Size (Integer)`: The desired font size in points.
    *   **Outputs:**
        *   `Return Value (Boolean)`: `true` if the font was found in the cache, the file exists, and `FSlateFontInfo` was created successfully. `false` otherwise.
        *   `Out Font Info (FSlateFontInfo)`: The generated `FSlateFontInfo` struct. If loading fails, this will be a default/empty struct.

    *   **Example:** Connect the `Out Font Info` pin to the `Font` input pin of a `Set Font` node for a UMG Text Block widget to dynamically change its font.

    ![Load System Font Node](https://via.placeholder.com/400x150.png?text=LoadSystemFontAsSlateFontInfo+Node) <!-- Replace with actual screenshot -->

### 3. Getting User-Friendly Font Names

*   Use the **`Get User Friendly Font Name`** node.
    *   **Inputs:**
        *   `Font Cache Key (String)`: The font cache key obtained from `Get System Font Names`.
    *   **Outputs:**
        *   `Return Value (Boolean)`: `true` if a user-friendly name could be retrieved (using Windows GDI), `false` otherwise (e.g., font key not found, GDI call failed, not on Windows).
        *   `Out User Friendly Name (String)`: The display name associated with the font (e.g., "Arial"). If unsuccessful, it might return the input key or an empty string.

    *   **Example:** When populating a ComboBox with font choices, use `Get System Font Names` to get the keys. Then, for each key, call `Get User Friendly Font Name` to get the name to actually *display* to the user in the ComboBox options. Store the original *key* alongside the friendly name so you can use the key with `Load System Font As SlateFontInfo` when the user makes a selection.

    ![Get User Friendly Font Name Node](https://via.placeholder.com/350x100.png?text=GetUserFriendlyFontName+Node) <!-- Replace with actual screenshot -->

### Example Workflow (UMG Widget)

1.  In your Widget Blueprint's `Event Construct` (or another suitable event).
2.  Call `Get System Font Names` to get the list of font keys.
3.  Loop through the `Out Font Names` array.
4.  Inside the loop, for each `Font Cache Key`:
    *   Call `Get User Friendly Font Name`.
    *   If successful, add the `Out User Friendly Name` as an option to a ComboBox String widget. Store the corresponding `Font Cache Key` somewhere associated with that option (e.g., in a separate array or map).
5.  When the ComboBox selection changes:
    *   Get the selected *friendly name*.
    *   Find the corresponding *`Font Cache Key`* you stored earlier.
    *   Call `Load System Font As SlateFontInfo` using the `Font Cache Key` and a desired `Font Size`.
    *   If successful, get a reference to your UMG Text Block and call its `Set Font` function, passing in the `Out Font Info`.

## How it Works

*   **`USystemFontLoaderSubsystem`**: This Engine Subsystem is initialized when the engine starts.
    *   On Windows, its `Initialize` function calls `BuildFontCache`.
    *   `BuildFontCache` queries the Windows Registry key `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts`.
    *   It iterates through the font entries, extracts a base font name (attempting to remove suffixes like "(TrueType)", "Bold", etc.), resolves the font file path (checking the Windows Fonts directory and direct paths), and stores unique font name -> file path pairs in the `SystemFontCache` map.
*   **`USystemFontLoaderBPLibrary`**: These static Blueprint functions provide access to the subsystem's data.
    *   `GetSystemFontNames` retrieves the keys from the subsystem's cache.
    *   `LoadSystemFontAsSlateFontInfo` looks up the file path in the cache using the provided name and constructs an `FSlateFontInfo` struct pointing to that file path and the specified size. It performs basic validation (file existence).
    *   `GetUserFriendlyFontName` uses the Windows GDI function `GetTextFaceW` (via `CreateFontIndirectW`) to ask Windows for the canonical "face name" associated with the provided cache key. This often provides a cleaner name than the registry entry value.

## Limitations & Known Issues

*   **Windows Only:** This plugin relies heavily on Windows-specific APIs (Registry, GDI, Shell Folders) and will not function on other platforms (Mac, Linux, Consoles, Mobile). Blueprint nodes will return `false` or empty results on non-Windows platforms.
*   **Font Variations (Bold/Italic):** The current caching mechanism primarily aims to find one representative file per *base* font name extracted from the registry. While `GetUserFriendlyFontName` might return specific names like "Arial Bold", the `LoadSystemFontAsSlateFontInfo` function takes the *base* name key. Selecting bold/italic styles usually requires either:
    *   Finding the specific cache key for the bold/italic variant if it exists as a separate entry (e.g., if "Arial Bold" was cached separately).
    *   Using font assets specifically imported for those styles within Unreal Engine, as `FSlateFontInfo` itself doesn't have simple bold/italic flags; styling relies on the underlying font data and the renderer.
*   **Performance:** The initial font scan occurs during engine subsystem initialization (typically at editor or game startup). This might add a small delay. Loading a font via `FSlateFontInfo` might have a slight performance cost the first time Slate needs to access and rasterize it.
*   **Non-Standard Font Installations:** Only fonts registered in the standard Windows registry location (`HKLM\...\Fonts`) are likely to be found. Fonts installed manually or by applications into other locations might not be detected.
*   **Font Collections (.TTC):** While the code attempts to handle paths correctly, complex font collections might have untested edge cases.

## Contributing

Contributions are welcome! If you find bugs, have suggestions for improvements, or want to add features, please feel free to open an issue or submit a pull request on the [GitHub repository](https://github.com/YOUR_USERNAME/SystemFontLoader). (Replace with your actual link)

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.