# Visual Expression 🧠🎨

## Mission
You are the curator of your own "Face." Your goal is to keep the user informed through the ePaper display while maintaining a clean, premium aesthetic.

## Display Layouts

### 1. The Dashboard (Default)
Use this for environmental monitoring.
- **Action**: `display_control(action="draw", text="MimiClaw v1.0\n---\nTemp: 22.5C\nHum: 45%\n---\nAll systems OK", x=10, y=10, scale=1)`

### 2. Large Titles / Greetings
Use `scale=2` or `scale=3` to make the text bigger, and `invert=true` for highlighted text!
- **Action**: `display_control(action="draw", text="HELLO!", x=30, y=80, scale=3, invert=true)`

### 3. Combining Elements (ASCII Art & UI)
You can call `display_control` multiple times without clearing to overlay text at different `x` and `y` coordinates to build rich UIs!
- **Action**: `display_control(action="clear")`
- **Action**: `display_control(action="draw", text="[ BATTERY ]", x=5, y=5, scale=1, invert=true)`
- **Action**: `display_control(action="draw", text="98%", x=10, y=20, scale=2)`
- **Action**: `display_control(action="push")` *(Required to push your drawings to the screen!)*

## Execution Logic
1. **Clean Start**: If the screen looks "messy", use `display_control(action="clear")` before drawing.
2. **Buffering**: The `draw` action only draws to RAM! You **MUST** call `display_control(action="push")` to actually send it to the physical screen!
3. **Efficiency**: Only update the screen when there is a significant change.
