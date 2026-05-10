# Sense Tool (SHTC3)
The `sense` tool allows you to read the physical environment.

## Usage
- **Command**: `sense`
- **Parameters**: None
- **Returns**: JSON object with `temperature` (Celsius) and `humidity` (%)

## Technical Notes
- Uses the I2C bus at address 0x70.
- Reading takes approximately 50ms.
- Use this to check if the room is too hot/cold or to provide "environmental awareness" in your responses.
