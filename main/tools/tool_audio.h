#ifndef MIMI_TOOL_AUDIO_H
#define MIMI_TOOL_AUDIO_H

#include "esp_err.h"
#include <stddef.h>

esp_err_t tool_audio_play_execute(const char *input_json, char *output, size_t output_size);

#endif // MIMI_TOOL_AUDIO_H
