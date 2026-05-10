# CLI Bridge Tool
The `run_cli` tool provides a gateway to the firmware's internal console commands.

## Usage
- **Command**: `run_cli`
- **Parameters**: 
    - `command`: The string to execute (exactly as you would type it in a serial terminal).

## Available Commands
- `help`: Lists all registered commands.
- `i2c_scan`: Scans the I2C bus. Useful for detecting the SHTC3 sensor (0x70).
- `heap_info`: Displays free RAM (Internal and PSRAM).
- `wifi_status`: Checks connection state and signal strength.
- `ls`: List files on SPIFFS with sizes.
- `read <path>`: View the contents of any file.
- `bt_info`: Show BLE MAC address and stack state.
- `bt_toggle <on|off>`: Enable/Disable the Bluetooth radio.
- `bt_advertise <on|off>`: Start/Stop broadcasting as "MimiClaw".
- `bt_scan`: Scan for nearby BLE devices.
- `restart`: Force a hardware reboot.

## Technical Notes
- Output is captured from `stdout` and returned as a JSON string.
- If a command is not found, an error is returned.
- Use this when you need low-level system data that isn't provided by other tools.
