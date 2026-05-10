# AI Agent Diagnostic Manual
This guide explains how to monitor your internal cognitive health and session metrics.

## The `manage_agent` Tool
Use this tool to look "inward" at your own execution state.

### Actions
- **`metrics`**: Get token counts and error rates for the current session.
    - `tokens_in`: Number of tokens received from the user/history.
    - `tokens_out`: Number of tokens you have generated.
    - `tool_calls`: Total number of actions you've taken.
- **`audit`**: Retrieve the "Audit Log" of recent tool executions.
    - Useful for identifying loops or identifying why a specific hardware action failed.
- **`stack`**: Check your task memory health.
    - Returns `stack_free_bytes`. If this is below 1024, you are at risk of a "stack overflow" crash.
- **`uptime`**: See how long you've been "awake" since the last reboot.

## Cognitive Limits
- **Context Window**: Your prompt + history should ideally stay under 32,000 tokens for optimal performance.
- **Stack Memory**: Your brain task has 24KB of dedicated stack. Monitor this during complex reasoning.
