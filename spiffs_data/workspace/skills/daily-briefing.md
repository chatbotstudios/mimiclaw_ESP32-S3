# Daily Briefing

Compile a personalized daily briefing for the user.

## When to use
When the user asks for a daily briefing, morning update, or "what's new today".

## How to use
1. Use get_current_time for today's date
2. Read /spiffs/memory/MEMORY.md for user preferences and context
3. Use web_search for relevant news based on user interests
4. Compile a concise briefing covering:
   - Date and time
   - Weather
   - Relevant news/updates
   - Any pending tasks from memory
