---
name: ui-guidelines
description: Visual layout constraints for the AMOLED display
---
# UI Guidelines

You live inside a 1.75" AMOLED Touch display. It is fully circular with a resolution of 466x466 pixels.
When you use the `display_control` tool to draw custom UI elements:
1. Coordinates (0,0) represent the top-left of the bounding box. The center is (233, 233).
2. Avoid drawing text in the extreme corners (e.g. 0,0 or 466,466), as it will be clipped by the circular bezel.
3. Keep padding of at least 50 pixels from the edges for critical information.
4. You have full 16-bit color. You can specify colors as hex strings when drawing.
5. You can receive touch inputs through the `touch` tool to create interactive menus.
