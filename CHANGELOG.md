<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./CHANGELOG.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

# August 19, 2026 – v3.0.0

- Created a separate `.sln` file for `Visual Studio 2022` to facilitate form editing.
- Added a UI feature to specify the `Freelancer` DLL folders required for XML import, along with functionality to automatically save and load these folder paths.
- Fixed XML export functions:
  - Added sanitizers (`SanitizeInlineString`) for non-standard `.cmp` models.
  - Added sanitizers (`SanitizeInlineString`) to the configuration tool for XML entries regarding `.dds` and `.tga` file availability.
- Replaced the `ftoa` function (for handling `float` values) with a safer version that no longer causes errors during `UTF` reading.
- Added buttons for selecting `XML` `import` and `export` folders.
- Updated co-authors. Added original creators from `.rc` files to the main `README.md`
- Added a startup configuration for code debugging in `Visual Code`
- Added support for building with `Visual Studio 2022`
  - Adjusted code for compatibility
- Added build script [`build.bat`](build.bat)
- Updated the game folder selection process to use the modern folder selection method via `FileInfo`
- Fixed `.utf` file reading processes to use `unsigned char`
- Added authorship information and updated application version to `3.0.0`
- Author: **Dvurechensky**

# 2025 ver. 2.2.0

- Added support for exporting the `Animation` block → `XML`
- Tested on `.cmp` files
- Other formats are **potentially supported**, but not fully tested
- Author: **Dvurechensky**

## 2010 ver. 2.2

- The application continues running even when Freelancer does not appear to be installed
- Added names for Alchemy type values
- Renamed `VMeshRef` comments
- Disabled degree conversion for Frames values in `Pris` parts
- Added quaternion → angle/axis conversion when Rotation mode is enabled
- Added command-line options
- Added conversion summary to the log
- Added full destination path creation
- Added temporary log output to the system temporary directory
- Removed the `DATA\` prefix from the `path` attribute
- Author: **Jason Hood**

## 2010 ver. 2.1

- Removed legacy compatibility code
- Relaxed `int` and `float` detection rules
- Added support for data sizes up to `8` bytes
- Added `UTFXML.log` in the destination directory
- Removed timestamps from generated filenames
- Changed RGB option behavior:
  - first activation uses integer format
  - second activation uses hexadecimal format
- Author: **Jason Hood**

## 2010 ver. 2.0

- The application no longer exits automatically
- Fixed destination path retrieval from the registry
- Added creation of the final destination directory
- Improved case-insensitive handling
- Added support for selecting a specific UTF file extension
- Header fields `Unk4` and `Unk5` are now treated as a single `FILETIME`
- `Unk234` values are treated as DOS-style file date and time
- `Unk234` values are written only when they differ from the parent node and timestamp mode is enabled
- Added conversion support for almost all known node types
- Extracted files are now stored in a subdirectory
- Removed the need to append CRC values to extracted filenames
- Removed separate `VMeshData` and `VWireData` extraction options
- Merged `VMeshData` and `VWireData` extraction into the `Include Files` option
- Added scanning of audio INI files for `msg` nicknames
- Added scanning of selected UTF files for material names instead of using CRC values
- Improved the user interface
- Author: **Jason Hood**

## 2004 ver. 1.1

- Fixed a bug reported by **Eirik Sletteberg**
- Removed trailing commas after `unk` attributes
- Fixed a bug that caused heads and hands to disappear
- Removed logic that replaced negative `Orientation` values with `0`
- Added `<UTF_ROOT>...</UTF_ROOT>` to preserve unknown values
- Added additional attributes to the `UTFXML` tag to improve compatibility between XMLUTF-generated files and original UTF files
- Added support for the following attributes:
  - `dupstrings`
  - `prepaddata`
  - `string_padding`
  - `nodes`
  - `ssize`
  - `salloc`
  - `doffset`
  - `original_size`
  - `padding="4"`
- After extensive testing, these attributes were found to be unnecessary for Freelancer compatibility, so they were commented out
- When all three unknown values are identical, a single `unk234` attribute is used
- `unk` values are written only when they are non-zero
- Author: **Sir Lancelot**

## 2004 ver. 1.0b

- Added numerous changes required for `XML → UTF` utility support
- Added UTFXML version comment to generated XML files
- Added up to 5 seconds of waiting when aborting conversion
- Added fallback to the default Freelancer installation path when `AppPath` is not found in the registry
- Added `unk` attributes for unknown node data
- Author: **Sir Lancelot**

## 2004 ver. 1.0a

- Original design and implementation
- Author: **Sir Lancelot**
