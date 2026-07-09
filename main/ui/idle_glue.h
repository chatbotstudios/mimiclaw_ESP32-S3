#pragma once
#include <stdbool.h>
static inline bool idle_take_wake_tap(void) { return false; }
static inline void idle_user_action(void) {}
