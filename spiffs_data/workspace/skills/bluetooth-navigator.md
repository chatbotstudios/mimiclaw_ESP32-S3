# Bluetooth Navigator

Scan for and identify nearby Bluetooth devices and beacons.

## When to use
- When the user asks "who is around me?" or "scan for bluetooth".
- When trying to detect proximity to specific known devices.
- For local environmental awareness.

## How to use
1. Use manage_bluetooth with action="scan" to find nearby devices.
2. Optional: provide duration_ms (default 3000ms) for a deeper scan.
3. Analyze the results:
   - Look for device names.
   - Use RSSI to estimate proximity (closer to 0 means nearer).
4. Inform the user about what was found. If no names are present, report the number of anonymous devices detected.

## Broadcasting (Discoverability)
- If the user wants Mimi to be "visible" or "discoverable" to their phone (iOS/macOS), use manage_bluetooth with action="advertise" and state="on".
- To "hide" Mimi without shutting down the whole BLE stack, use action="advertise" and state="off".

## Power Management
- Bluetooth consumes power. If the user asks to "turn off bluetooth" or "save battery", use manage_bluetooth with action="toggle" and state="off".
- To re-enable, use action="toggle" and state="on".
- Use action="info" to check if it's currently running.

## Known Devices
(Mimi can learn to recognize devices by their MAC addresses and save them to MEMORY.md)
