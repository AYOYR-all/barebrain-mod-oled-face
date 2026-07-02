#include "tool_oled_face.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "esp_check.h"
#include "esp_log.h"
#include "feedback/face_state.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "tool_oled.h"

static const char *TAG = "tool_oled_face";
static SemaphoreHandle_t s_lock;
static char s_emotion[16] = "idle";
static TickType_t s_until_tick;

static bool is_supported_emotion(const char *emotion)
{
    return emotion &&
           (strcmp(emotion, "idle") == 0 ||
            strcmp(emotion, "happy") == 0 ||
            strcmp(emotion, "online") == 0 ||
            strcmp(emotion, "thinking") == 0 ||
            strcmp(emotion, "error") == 0 ||
            strcmp(emotion, "speaking") == 0);
}

static void face_task(void *arg)
{
    (void)arg;
    unsigned int frame = 0;
    char emotion[sizeof(s_emotion)];

    while (true) {
        if (xSemaphoreTake(s_lock, pdMS_TO_TICKS(200)) == pdTRUE) {
            if (s_until_tick != 0 && xTaskGetTickCount() >= s_until_tick) {
                strlcpy(s_emotion, "idle", sizeof(s_emotion));
                s_until_tick = 0;
            }
            strlcpy(emotion, s_emotion, sizeof(emotion));
            xSemaphoreGive(s_lock);
        } else {
            strlcpy(emotion, "idle", sizeof(emotion));
        }

        esp_err_t err = tool_oled_draw_face(emotion, frame++);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "draw face failed: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

esp_err_t tool_oled_face_init(void)
{
    s_lock = xSemaphoreCreateMutex();
    if (!s_lock) {
        return ESP_ERR_NO_MEM;
    }

    ESP_RETURN_ON_ERROR(tool_oled_set_clock_enabled(false), TAG, "disable OLED clock");
    ESP_RETURN_ON_ERROR(tool_oled_draw_face("idle", 0), TAG, "draw initial face");

    BaseType_t created = xTaskCreate(face_task, "tool_oled_face", 3072, NULL, 3, NULL);
    return created == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t tool_oled_face_set(const char *emotion, uint32_t duration_ms)
{
    if (!is_supported_emotion(emotion)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_lock || xSemaphoreTake(s_lock, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return ESP_ERR_INVALID_STATE;
    }
    strlcpy(s_emotion, emotion, sizeof(s_emotion));
    s_until_tick = duration_ms > 0 ? xTaskGetTickCount() + pdMS_TO_TICKS(duration_ms) : 0;
    xSemaphoreGive(s_lock);
    return ESP_OK;
}

esp_err_t brn_face_set(const char *emotion, uint32_t duration_ms)
{
    return tool_oled_face_set(emotion, duration_ms);
}

esp_err_t tool_oled_face_execute(const char *input_json, char *output, size_t output_size)
{
    if (!input_json || !output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }

    const char *emotion = cJSON_GetStringValue(cJSON_GetObjectItem(root, "emotion"));
    if (!is_supported_emotion(emotion)) {
        cJSON_Delete(root);
        snprintf(output, output_size, "Error: emotion must be idle, happy, online, thinking, error or speaking");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = tool_oled_face_set(emotion, 0);
    cJSON_Delete(root);
    if (err != ESP_OK) {
        snprintf(output, output_size, "Error: OLED face state failed (%s)", esp_err_to_name(err));
        return err;
    }

    snprintf(output, output_size, "OLED face switched to %s", emotion);
    return ESP_OK;
}
