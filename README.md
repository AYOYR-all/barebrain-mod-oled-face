# BareBrain OLED 表情插件

`tool-oled-face` 依赖 `tool-oled`，用于在 SSD1306 兼容 OLED 屏幕上显示表情动画。

启用规则：

- 只启用 `tool-oled`：OLED 默认显示时间、日期和星期。
- 同时启用 `tool-oled` 和 `tool-oled-face`：表情插件接管 OLED，时间界面停止刷新。

支持的表情：

- `idle`
- `happy`
- `thinking`
- `error`
- `speaking`

工具调用示例：

```json
{
  "emotion": "happy"
}
```

这个插件不需要额外配置 GPIO，复用 `tool-oled` 的 SCL/SDA 配置。
