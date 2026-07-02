# BareBrain OLED Face Plugin

`tool-oled-face` depends on `tool-oled` and renders animated faces on the
SSD1306-compatible OLED.

When enabled, it takes over the OLED from the base clock screen. The registered
tool `oled_face` can still switch expressions manually, and firmware feedback
events can also drive the face automatically.

Supported emotions:

- `idle`
- `happy`
- `online`
- `thinking`
- `error`
- `speaking`

Automatic feedback in v0.1.2:

- WiFi connecting: `thinking`
- WiFi connected: temporary `happy`
- WiFi setup/error: `error`
- LLM processing: `thinking`
- LLM answer ready: temporary `online`
- LLM failure: temporary `error`
- WonderEcho `tts_speak`: temporary `speaking`

Example tool call:

```json
{
  "emotion": "online"
}
```

This plugin does not require extra GPIO configuration. It reuses the SCL/SDA
configuration from `tool-oled`.
