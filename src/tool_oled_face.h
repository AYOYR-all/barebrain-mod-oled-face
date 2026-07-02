#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

esp_err_t tool_oled_face_init(void);
esp_err_t tool_oled_face_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_oled_face_set(const char *emotion, uint32_t duration_ms);
