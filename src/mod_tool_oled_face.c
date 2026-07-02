#include "core/mod/brn_mod.h"
#include "tool_oled_face.h"
#include "tools/tool_registry.h"

static const char *const tool_oled_face_deps[] = {
    "tool-oled",
    "core.tool_registry",
    NULL,
};

static const brn_tool_t oled_face_tool = {
    .name = "oled_face",
    .description = "Switch OLED face expression: idle, happy, thinking, error, speaking.",
    .input_schema_json =
        "{\"type\":\"object\","
        "\"properties\":{\"emotion\":{\"type\":\"string\",\"enum\":[\"idle\",\"happy\",\"thinking\",\"error\",\"speaking\"]}},"
        "\"required\":[\"emotion\"]}",
    .execute = tool_oled_face_execute,
};

static esp_err_t tool_oled_face_mod_init(void)
{
    esp_err_t err = tool_oled_face_init();
    if (err != ESP_OK) {
        return err;
    }
    return brn_tool_register(&oled_face_tool);
}

const brn_mod_t brn_mod_tool_oled_face = {
    .id = "tool-oled-face",
    .name = "OLED Face Tool",
    .version = "0.1.1",
    .deps = tool_oled_face_deps,
    .init = tool_oled_face_mod_init,
};
