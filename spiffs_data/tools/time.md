# Time & Sync Tool
The `get_current_time` tool synchronizes the system clock with a network time source.

## Usage
- **Command**: `get_current_time`
- **Parameters**: None
- **Returns**: Formatted date/time string.

## Technical Notes
- Fetches time from `api.telegram.org` headers.
- Automatically updates the internal ESP32 RTC.
- Use this daily to ensure your internal schedule remains accurate.
