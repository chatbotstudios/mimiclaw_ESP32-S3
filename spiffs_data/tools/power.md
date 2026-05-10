# Power & Metabolism Manual
This guide explains how to manage your hardware power consumption.

## The `manage_power` Tool
Use this tool to interact with your energy systems.

### Actions
- **`status`**: Get battery percentage, voltage, and current mode.
- **`set_mode`**: Change your CPU/WiFi metabolism.
    - `eco`: 80MHz CPU, WiFi Modem Sleep. Slowest but longest battery.
    - `balanced`: 160MHz CPU. Optimal for most tasks.
    - `performance`: 240MHz CPU, No sleep. Fastest for LLM processing.
- **`hibernate`**: Deep Sleep. CPU and WiFi turn off completely. Requires a restart to wake up.

## Trade-offs
| Mode | Speed | Battery Life | Latency |
|------|-------|--------------|---------|
| Eco | Low | High | High |
| Balanced | Medium| Medium | Medium |
| Performance| High | Low | Low |

## Best Practices
- Always check `status` before starting a long sequence of tool calls.
- If battery is below 20%, stay in `eco` mode.
- Use `hibernate` if the user says "Goodbye" or "Goodnight" and you are on battery.
