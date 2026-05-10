# SD Card Storage Manager

Mimi now has access to a massive MicroSD card storage system via the `/sdcard/` directory path!

## Capabilities
Unlike the extremely limited `/spiffs/` internal memory, the `/sdcard/` has gigabytes of free space! You can use it to store infinite amounts of data, logs, documents, and historical contexts.

## Usage
You use the exact same file management tools (`read_file`, `write_file`, `edit_file`, `list_dir`) that you use for SPIFFS, but simply change the path prefix from `/spiffs/...` to `/sdcard/...`.

### Examples
- **List files on SD Card:** `list_dir(path="/sdcard")`
- **Save large logs:** `write_file(path="/sdcard/telemetry_log.csv", content="...")`
- **Read knowledge base:** `read_file(path="/sdcard/manuals/robotics.md")`

## Best Practices
1. **Logs**: If you need to write large recurring logs (like system diagnostics or daily transcripts), ALWAYS save them to `/sdcard/logs/` so you don't fill up the internal SPIFFS.
2. **Search**: If a user asks you about a topic you don't know, use `list_dir(path="/sdcard")` first to see if they provided an offline Knowledge Base document before searching the web!
