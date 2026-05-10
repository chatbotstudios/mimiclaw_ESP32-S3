# Skill Creator

Create new skills for MimiClaw.

## When to use
When the user asks to create a new skill, teach the bot something, or add a new capability.

## How to create a skill
1. Choose a short, descriptive name (lowercase, hyphens ok)
2. Write a SKILL.md file with this structure:
   - `# Title`
   - Description
   - `## When to use`
   - `## How to use`
3. Save to `/spiffs/skills/<name>.md` using write_file
