# MimiClaw Gemini: The Ultimate S3-ePaper AI Agent

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-ESP--IDF--v6.0-green.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)

**MimiClaw Gemini** is a high-performance, modular AI agent firmware for the ESP32-S3. It transforms a $10 ePaper dev board into a fully autonomous, self-documenting personal assistant that lives in your pocket.

---

## 🚀 Key Enhancements (Gemini Edition)

- **Modular Skills System**: Agent logic is now decoupled into Markdown files stored in `/spiffs/skills/`. Mimi can "learn" new behaviors just by reading a file.
- **Integrated Audio Engine**: Supports the **ES8311 I2S Codec** for voice and sonic feedback. Features a background DMA audio service with `play_audio` AI tooling.
- **Dual-LED Signaling**: Multi-color status system (Red on GPIO 3, Green on GPIO 1) for distinct "Processing" vs "System Health" heartbeat signals.
- **Enhanced Display Clarity**: Optimized ePaper driver with 2MHz SPI stabilization and an automated "Full Refresh" anti-ghosting cycle every 10 updates.
- **High-Density E-Ink UI**: A custom 16x16 pixel icon rendering engine with dynamically updating state icons (e.g. progressive battery fills, precise temperature/humidity iconography, and multi-channel messaging status) for a premium, visually dense dashboard.
- **Autonomous CLI Bridge**: Mimi can now execute her own firmware console commands (`i2c_scan`, `heap_info`, etc.) autonomously via the `run_cli` tool, giving her root-level diagnostic power.
- **Unified Tool Registry**: A dynamic C-based registry for hardware tools (`sense`, `display_control`, `led_control`, `play_audio`) and cloud tools (`web_search`, `time`).
- **Hardware Native**: First-class support for **1.54" ePaper displays** and **SHTC3 temperature/humidity sensors**.
- **Self-Documenting**: Mimi now has a `/spiffs/tools/` directory containing technical manuals for her own hardware, allowing her to research how to control herself.

---

## 🛠 Hardware Specs & Compatibility

Designed specifically for the **ESP32-S3-ePaper-1.54** board (and similar LilyGo/Waveshare variants):
- **MCU**: ESP32-S3 (Dual-core, 240MHz)
- **Memory**: 8MB PSRAM / 16MB Flash
- **Display**: 1.54" GDEH0154D67 ePaper (200x200 pixels)
- **Audio**: ES8311 I2S DAC + Mono Speaker Amplifier
- **Sensors**: SHTC3 (Temp/Hum), Integrated Battery Voltage Sensing (GPIO 4 with GPIO 17 Control)
- **I/O**: Native USB-C (JTAG/Serial), MicroSD Slot, Dual Status LEDs (Red/Green), User Buttons (GPIO 0).

---

## ⚡️ Advanced Features

- **Dual-Core Processing**: Network I/O and SSL/TLS run on Core 0, while the AI Agent and tool execution run on Core 1 for maximum responsiveness.
- **Smart Power Management**: Performance vs. Balanced modes (240MHz vs 80MHz) and deep hibernation support.
- **WebSocket Gateway**: Open port `18789` for local network integration and remote dashboarding.
- **OTA Updates**: Seamless over-the-air firmware updates via the `/ota` endpoint.
- **SOCKS5/HTTP Proxy**: Built-in support for CONNECT tunnels to bypass regional network restrictions.

---

## 🧩 The "Mimi" Ecosystem

### 📁 Filesystem Layout (SPIFFS / SSD)
Mimi uses a structured flat filesystem to organize her brain:
- `/spiffs/config/`: `SOUL.md` (Personality), `USER.md` (Owner data).
- `/spiffs/skills/`: Markdown-based logic modules (Weather, Daily Briefing, Skill Creator).
- `/spiffs/audio/`: System sounds and startup chimes (`boot.raw`, `siren.raw`).
- `/spiffs/tools/`: Technical manuals for hardware tools.
- `/spiffs/memory/`: Persistent long-term memory (`MEMORY.md`) and daily logs.
- `/spiffs/sessions/`: Encrypted JSONL chat histories.

### 🧰 Integrated Tool Registry (v1.2)
Mimi has direct access to the physical world and the cloud through these specialized C-drivers:

| Tool | Capability | Physical Target |
|------|----------|--------|
| **`sense`** | Fetches high-precision temperature and humidity data. | SHTC3 I2C Sensor |
| **`display_control`** | Manages UI drawing, text scaling, 16x16 icon rendering, and anti-ghosting "Full Refreshes." | 1.54" ePaper |
| **`led_control`** | Independent control of the **Red** (Processing) and **Green** (Health) LEDs. | GPIO 3 / GPIO 1 |
| **`play_audio`** | Streams raw 16kHz PCM audio from storage to the speaker. | ES8311 I2S DAC |
| **`run_cli`** | Bridges the AI to the firmware's root terminal for advanced diagnostics. | Internal Serial Console |
| **`manage_power`** | Monitors battery voltage (via GPIO 17 MOSFET) and toggles 240MHz/80MHz modes. | PMU / ADC1_CH3 |
| **`manage_network`** | Performs WiFi pings, network scans, and SNTP time synchronization. | ESP-WiFi / LWIP |
| **`manage_agent`** | Audits tool execution logs, token usage, and real-time PSRAM heap health. | System Metrics |
| **`manage_bluetooth`** | Scans for BLE beacons and manages local Bluetooth advertising state. | NimBLE Stack |
| **`web_search`** | Real-time internet access for world news and technical data. | Tavily Search API |
| **`filesystem`** | Atomic `list_dir`, `read_file`, and `write_file` operations for persistent storage. | SPIFFS (SSD) |

---

### 📁 Dual-Storage Architecture
Mimi treats her internal and external memory as two distinct "Drives," allowing for high-speed system execution and massive data storage:

- **Internal SSD (SPIFFS)**: 
    - **Mount Point**: `/spiffs`
    - **Role**: Mimi's "Core Brain." Stores her Soul, Skills, System Config, and Audio Alerts. 
    - **Capacity**: High-speed, persistent internal flash memory.
- **External Media (SD Card)**:
    - **Mount Point**: `/sdcard`
    - **Role**: Long-term "Vault" and "Archive." Ideal for large datasets, conversation logs, and massive audio libraries.
    - **Capacity**: Supports up to 128GB MicroSD cards (FAT32).

---

## 📜 Skills Registry (Modular Brain)
Skills are Markdown-based logic modules that Mimi uses to define her high-level behaviors. She can read, modify, and even *create* these files at runtime.

- **`weather.md`**: Interprets raw `sense` data and `web_search` results to provide human-centric atmospheric advice.
- **`epaper.md`**: Contains the design system for the Mimi Dashboard, including grid constraints and font usage.
- **`skill-creator.md`**: Mimi's "Self-Evolution" module. It allows her to identify gaps in her knowledge and write new `.md` skills to her own memory.
- **`memory-manager.md`**: A background process that distills long chat histories into concise entries in `MEMORY.md` to save context tokens.
- **`daily-briefing.md`**: Mimi’s morning routine module. It synthesizes weather, system health, and news into a concise "Good Morning" greeting.

---

## 💻 Enhanced Serial CLI
Mimi includes a powerful developer console. Connect via USB at **115200 baud**.

### Essential Commands
- `help`: List all available commands.
- `i2c_scan`: Scan the bus (find SHTC3 at 0x70, ES8311 at 0x18).
- `ls_ssd`: **Tree view** of the Internal SSD (Mimi's Brain).
- `ls_sd`: **Tree view** of the External SD Card (Archive).
- `ls_r`: Recursive "God View" starting from root (`/`).
- `df`: Disk usage statistics for the internal SSD.
- `sd_info`: Capacity and health diagnostics for the SD Card.
- `read /path/to/file`: View file contents.
- `wifi_set SSID PASS`: Configure network.
- `bt_toggle on`: Enable Bluetooth radio.
- `bt_advertise on`: Start broadcasting as "MimiClaw".
- `bt_scan`: Scan for nearby BLE devices.
- `set_provider gemini`: Switch AI backends.
- `pwr_perf`: Force the CPU to 240MHz Performance Mode.

---

## 🛠️ Tools & Skills
Mimi is equipped with a library of **Tools** (low-level hardware drivers) and **Skills** (high-level behavioral modules) stored in her internal SSD.

### 🧰 Built-in Tools (`/spiffs/tools/`)
| Tool | Purpose |
| :--- | :--- |
| `agent` | Core LLM reasoning loop and task management. |
| `bluetooth` | Control BLE stack, scanning, and advertising. |
| `cli` | Execute raw firmware commands via the AI bridge. |
| `display` | Manipulate the ePaper UI and draw custom text. |
| `network` | Manage WiFi connections and signal monitoring. |
| `sense` | Read temperature and humidity from the SHTC3. |
| `storage` | CRUD operations on Internal SSD and SD Card. |
| `time` | Synchronize and retrieve high-precision system time. |
| `web_search` | External knowledge retrieval via HTTP proxy. |

### 🧠 Installed Skills (`/spiffs/skills/`)
| Skill | Description |
| :--- | :--- |
| `bluetooth-navigator` | Detects people and devices nearby using BLE. |
| `daily-briefing` | Summarizes weather, time, and system health. |
| `led-indicator` | Communicates status via RGB light patterns. |
| `memory-manager` | Long-term memory storage and retrieval. |
| `network-wizard` | Auto-detects and maintains network stability. |
| `power-manager` | Optimizes battery life vs performance. |
| `sd-card` | Handles archival storage and filesystem safety. |
| `self-diagnostics` | Monitors hardware health and brownout alerts. |
| `skill-creator` | Allows Mimi to write her own .md skill files. |

---

## 🛠 Build & Deployment

### Requirements
- **ESP-IDF v6.0** (Recommended)
- **Python 3.12+**

### Flashing
1. **Clone & Setup**:
   ```bash
   git clone https://github.com/memovai/mimiclaw_gemini.git
   cd mimiclaw_gemini
   cp main/mimi_secrets.h.example main/mimi_secrets.h
   ```
2. **Build & Flash**:
   ```bash
   idf.py build flash monitor
   ```
3. **Sync Internal SSD (SPIFFS)**:
   ```bash
   # This uploads the UI, Skills, and Audio assets to /spiffs
   python -m esptool --chip esp32s3 -p /dev/cu.usbmodem101 -b 460800 write-flash 0x420000 build/spiffs.bin
   ```
4. **External SD Management**: 
   The SD card is managed via FAT32. To batch-upload files, insert the card into your Mac and copy files to the root or specialized folders like `/resources` or `/vault`.


---

## 🛡 License
MIT License. Created by the MimiClaw Community.
Inspired by pure C and the dream of an autonomous agent in every pocket.