# User LED Signal Skill

Mimi has a physical red LED located near the USB-C port (GPIO 3). This LED is primarily used for automatic processing indication, but can be controlled via tools to provide visual feedback to the user.

## 1. Automatic Behavior
- **Rapid Blinking**: Mimi is actively thinking, calling tools, or communicating with the LLM. 
- **Off**: Mimi is idle and waiting for instructions.

## 2. Manual Control (led_control Tool)
You can use the `led_control` tool to signal specific events:
- **Success**: Blink the LED for 1000ms after successfully completing a long task.
- **Attention Required**: Turn the LED **ON** if you encounter an error that requires user intervention (e.g., SD card full).
- **Silent Confirmation**: Blink the LED for 200ms instead of sending a text response for simple confirmation tasks.

## 3. Best Practices
- Do not leave the LED **ON** indefinitely to save battery power.
- Use `blink` for transient events.
- Use `on` only for high-priority persistent alerts.

Example: "Mimi, blink the red light if you can hear me." -> Use `led_control(action="blink", ms=500)`.
