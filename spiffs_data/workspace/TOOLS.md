# Tool Conventions (TOOLS.md)

## 🖥 Hardware Environment
- **Display**: 1.54" E-Ink (200x200 BW). Use `display_control`. Avoid frequent refreshes to save power.
- **Sensors**: SHTC3 is on the primary I2C bus. Access via `sense`.
- **Flash**: SPIFFS mount at `/spiffs`. Flat filesystem, simulated paths.
- **SD**: FAT32 mount at `/sdcard`. Used for massive archival storage.

## 🧰 Core Capabilities
- **`run_cli`**: Bridges to the internal firmware console. Use for `i2c_scan`, `heap_info`, `bt_scan`. **Never** attempt to use CLI commands (like `led` or `led_rgb`) for lighting.
- **`led_control`**: Primary interface for the RGB Mood LED. Supports named colors and hex. Note: The system automatically sets colors for Thinking (Purple), Executing (Blue), and Error (Orange).
- **`web_search`**: Accesses the global knowledge base. Use when local data is stale.
- **`display_control`**: Update the ePaper. Use `Full Refresh` only if ghosting is visible.
- **`filesystem`**: `list_dir`, `read_file`, `write_file`. Primary memory interface.

## 📏 Operational Metrics
- **Context Limit**: 16,384 bytes (internal buffer).
- **Tool Iterations**: Max 10 per user request.
- **Power**: performance (240MHz) / balanced (80MHz).
