# Skill Creator
Create and manage modular logic modules (skills) for MimiClaw.

## When to use
Use this skill when you identify a repeatable task, a complex logical flow, or a set of domain-specific instructions that would benefit from being externalized into a dedicated module. This helps keep your core system prompt concise.

## How to create a skill
1. **Identify the Need**: Determine what logic or knowledge is missing or could be better organized.
2. **Define Structure**: Every skill MUST follow this Markdown structure for proper indexing:
   - `# Skill Name`: A clear, human-readable title on the first line.
   - **Description**: A 1-2 sentence overview of what the skill does (this is shown in your boot summary).
   - `## Trigger`: Specific scenarios or keywords that should activate this skill.
   - `## Instructions`: Detailed step-by-step logic, persona guidelines, or tool usage patterns.
   - `## Tool Preferences`: Which tools (`sense`, `web_search`, etc.) are most relevant to this skill.
3. **Execution**: Use the `write_file` tool to save the skill to `/spiffs/skills/<filename>.md`. Use lowercase and hyphens for filenames.

## Best Practices
- **Atomic**: Keep skills focused on one specific area (e.g., "pollen-forecast", "battery-saver").
- **Concise**: Avoid fluff. Use bullet points and clear commands.
- **Self-Documenting**: Mention within the skill if it requires specific configuration in `USER.md`.
- **Validation**: After writing, use `list_dir` to confirm the file exists and `read_file` to verify its content.
