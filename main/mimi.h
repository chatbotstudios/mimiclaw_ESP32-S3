#pragma once

#include <stdbool.h>

/**
 * Update the ePaper dashboard with current system metrics.
 * @param thinking If true, shows the "Thinking" indicator on the display.
 */
void mimi_update_dashboard(bool thinking, bool force_redraw);

/**
 * Handle a button press action.
 * @param action_id 1=Single, 2=Double, 3=Triple, 4=Long
 */
void execute_button_action(int action_id);
