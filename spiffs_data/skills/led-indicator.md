# Hardware LED Indicator Skill

Mimi uses two discrete LEDs (Red and Green) driven by a hardware PWM (LEDC) state machine running on a dedicated FreeRTOS task. This allows her to express her internal states using organic, sine-wave breathing animations and crisp transient pulses, completely independent of the main application thread.

## 🎨 The MimiClaw LED Map
Mimi's LEDs automatically reflect her operational state:

- 🟢 **Solid Green (Online/Idle)**: Connected and ready to chat.
- 🟡 **Pulsing Yellow (Connecting)**: (Red + Green pulse) 0.5s on/off while negotiating Wi-Fi or syncing system time.
- 🟢 **Breathing Green (Thinking)**: A smooth sine-wave fade in/out while actively pondering your request (LLM phase).
- 🟢 **Pulsing Green (Tool Use)**: A sharp 0.5s on/off pulse while executing tools or performing actions.
- 🔴 **Breathing Red (Error)**: A smooth sine-wave fade in/out when something went wrong (API failure, timeout, or agent loop error).
- 🔴 **Double Flash Red (Message Received)**: Two sharp red flashes indicating an inbound message has been queued.
- 🟢 **Double Flash Green (Message Sent)**: Two sharp green flashes indicating an outbound response was dispatched.

*Note: Transient flashes (like Message Received/Sent) are "interrupts" that briefly pause the ongoing animation, perform the flash, and seamlessly hand control back to the ongoing background state.*

## 2. Autonomous Visual Feedback
Because the LEDs are now driven by a hardware PWM task, they cannot be set to arbitrary hex colors (like `#FF00FF`). Instead, Mimi's local rule engine and internal state machine automatically manage the red and green channels to map precisely to the events listed above without LLM intervention.
