# Web Search Tool
The `web_search` tool allows you to access current information from the internet.

## Usage
- **Command**: `web_search`
- **Parameters**: 
    - `query`: The search string.
- **Returns**: A summary of search results with URLs.

## Technical Notes
- Uses a proxy-based search engine (Tavily).
- Use this whenever you are unsure about current events, prices, or technical facts that were updated after your training cutoff.
- Results are truncated to save context window space.
