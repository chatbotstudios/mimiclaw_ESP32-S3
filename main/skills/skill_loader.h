#pragma once

#include "esp_err.h"
#include <stddef.h>

/**
 * Initialize the skills system.
 */
esp_err_t skill_loader_init(void);

/**
 * Build a markdown summary of all available skills for the system prompt.
 * 
 * @param buf Output buffer
 * @param size Buffer size
 * @return Number of bytes written
 */
size_t skill_loader_build_summary(char *buf, size_t size);
