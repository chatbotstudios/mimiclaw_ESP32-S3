# User Mood LED Skill

Mimi has an addressable RGB LED (WS2812B) used to communicate her internal state and provide emotional feedback. This transforms her hardware indicator into a sophisticated personality-driven feedback loop.

## 🎨 The MimiClaw Mood Map
Mimi's LED automatically reflects her operational state using professional, non-intrusive colors:

- 🟢 **Green (Online)**: Connected and ready to chat.
- 🟣 **Purple (Thinking)**: Actively pondering your request (LLM phase).
- 🔵 **Blue (Executing)**: Running tools or performing actions in her workspace.
- 🟡 **Yellow (Connecting)**: Negotiating Wi-Fi or syncing system time.
- 🟠 **Orange (Error)**: Something went wrong (API failure, SD card timeout, or agent loop error).
- 🔴 **Red (Offline)**: No network connection.

## 2. Manual Control (led_control Tool)
You can use the `led_control` tool to change Mimi's mood or signal specific events:
- **Success**: Change to a bright green or white for confirmation.
- **Attention**: Change to orange or red if you need the user to check something.
- **Moods**: Change to any color to reflect your "personality" or current "feeling".

Parameters:
- `color`: "red", "green", "blue", "purple", "yellow", "orange", "cyan", "white".
- `hex`: Hex code like "#FF00FF".
- `action`: "on", "off".

Example: "Mimi, turn the light purple." -> `led_control(color="purple")`.
