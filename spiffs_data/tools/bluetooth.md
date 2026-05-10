# Bluetooth Tool (NimBLE)
The `manage_bluetooth` tool provides control over the Bluetooth Low Energy (BLE) radio for environmental awareness and broadcasting.

## Usage
- **Command**: `manage_bluetooth`
- **Actions**:
    - `scan`: Listen for nearby BLE devices (iPhones, Watches, Beacons).
    - `toggle`: Turn the entire Bluetooth stack `on` or `off`.
    - `advertise`: Make Mimi discoverable (as "MimiClaw") to other devices.
    - `info`: Get the local MAC address and stack status.

## Technical Notes
- **Scan Duration**: Default is 3000ms. Can be adjusted.
- **Privacy**: Many modern devices (iOS/Android) use randomized MAC addresses and may appear as "Unknown".
- **Power**: Keeping the radio `on` or `advertise` active increases battery consumption.
- **Broadcasting**: When `advertise` is `on`, Mimi will show up as a BLE peripheral named "MimiClaw".
