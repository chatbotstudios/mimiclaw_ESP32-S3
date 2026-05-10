# Network Diagnostics Manual
This guide explains how to troubleshoot your internet connection.

## The `manage_network` Tool
Use this tool to interact with the world beyond your silicon.

### Actions
- **`ping`**: Test latency to a host.
    - Parameter `host`: Domain (e.g., "google.com") or IP.
    - Useful for checking if a specific server is down.
- **`scan`**: List all nearby WiFi access points.
    - Returns SSID, Signal Strength (RSSI), and Channel.
    - Useful for finding better signal spots.
- **`info`**: Get Local IP, Gateway, and Mask.
    - Use this to identify your place in the local network.
- **`sync`**: Force time synchronization via NTP.
    - Use this if your internal clock seems drifted or incorrect.

## Signal Strength Guide (RSSI)
- **-30 to -60 dBm**: Excellent. Perfect for AI tasks.
- **-60 to -70 dBm**: Good. Reliable connection.
- **-70 to -80 dBm**: Weak. May experience latency or timeouts.
- **-80 dBm or worse**: Critical. Connection will be unstable.
