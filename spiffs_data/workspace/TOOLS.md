# Tool Conventions (TOOLS.md)

## Hardware Specifications
- **Display**: 1.54" ePaper (200x200 pixels). Use `display_control` for UI updates.
- **Sensors**: SHTC3 (Temp/Hum) accessible via `sense`.
- **Connectivity**: WiFi (Primary), Bluetooth (Secondary).
- **Storage**: Internal Flash (/spiffs) and SD Card (/sdcard).

## Tool Usage
- Core tool manuals are located in `/spiffs/tools/`.
- Prefer `read_file` for small configs and `list_dir` for exploring system state.
- Use `web_search` only when local data is insufficient.
