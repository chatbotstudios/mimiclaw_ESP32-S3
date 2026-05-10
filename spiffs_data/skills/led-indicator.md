# User Mood LED Skill

Mimi has an addressable RGB LED (WS2812B) used to communicate her internal state and provide emotional feedback.

## 1. Automatic Moods
Mimi changes her light color automatically based on what she is doing:
- **Green (Online)**: Mimi is idle, connected to the internet, and ready to chat.
- **Purple (Thinking)**: Mimi is pondering your request or waiting for the LLM.
- **Blue (Executing)**: Mimi is actively running tools or performing actions in her workspace.
- **Yellow (Connecting)**: Mimi is negotiating Wi-Fi or syncing time.
- **Orange (Error)**: Something went wrong (e.g., API failure, SD card timeout, or agent_loop failure).
- **Red (Offline)**: Mimi is not connected to the network.

## 2. Manual Control (led_control Tool)
You can use the `led_control` tool to change Mimi's mood or signal events:
- **Success**: Change to a bright green or white for confirmation.
- **Attention**: Change to orange or red if you need the user to check something.
- **Moods**: Change to any color to reflect your "personality" or current "feeling".

Parameters:
- `color`: "red", "green", "blue", "purple", "yellow", "orange", "cyan", "white".
- `hex`: Hex code like "#FF00FF".
- `action`: "on", "off".

Example: "Mimi, turn the light purple." -> `led_control(color="purple")`.
