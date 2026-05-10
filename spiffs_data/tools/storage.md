# Filesystem Tool (SPIFFS)
You have access to a persistent storage partition (/spiffs/).

## Usage
- **list_dir**: List files. Use `prefix` to filter.
- **read_file**: Read content of a file.
- **write_file**: Save or overwrite a file.
- **edit_file**: Simple string replacement in a file.

## Technical Notes
- Storage is limited to ~1MB.
- SPIFFS does not support nested directories, but uses `/` in filenames to simulate them.
- Use `/spiffs/memory/` for long-term data.
- Use `/spiffs/config/` for your personality and user info.
- Use `/spiffs/tools/` for technical manuals.
