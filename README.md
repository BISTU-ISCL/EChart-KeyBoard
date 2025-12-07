# EChart-KeyBoard

一个可嵌入 Qt 应用及 Qt Designer 的全键盘控件。支持监听真实键盘事件、按键高亮渐隐、热力图统计等能力，并提供丰富的自定义属性，方便在界面库中直接拖拽使用。

## 构建

项目使用 CMake，**限定 Qt 5.15.2** 环境，便于在 Qt 5.15.2 版 Designer 中直接加载插件。

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

默认会生成 `VirtualKeyboardWidget` 静态库；若找到 Qt Designer 模块（Qt 5.15.2），还会同时生成可在设计器调色板中拖拽的 `VirtualKeyboardPlugin`。

若要让 Designer 自动识别该控件，请在安装阶段将插件复制或安装到 Qt 的 `plugins/designer` 目录，可通过变量 `QT_DESIGNER_PLUGIN_PATH` 定制：

```bash
cmake .. -DQT_DESIGNER_PLUGIN_PATH=/path/to/Qt/plugins/designer
cmake --build . --target install
```

## 属性与接口（VirtualKeyboardWidget）

下列属性均可在代码或 Qt Designer 属性面板中配置：

| 属性 | 类型 | 说明 | 默认值 |
| --- | --- | --- | --- |
| `trackPhysicalKeyboard` | `bool` | 是否监听物理键盘事件并同步高亮、计数 | `true` |
| `heatMapEnabled` | `bool` | 是否启用热力图着色 | `true` |
| `coldColor` | `QColor` | 热力图最低频率颜色 | `QColor(18, 26, 38)` |
| `hotColor` | `QColor` | 热力图最高频率颜色 | `QColor(126, 192, 255)` |
| `highlightColor` | `QColor` | 按键被触发时的高亮颜色 | `QColor(255, 65, 130)` |
| `backgroundImagePath`（KeyButton） | `QString` | 单个键帽的背景图片路径，可在 Designer 中指定，用于纹理化热图 | 空 |

常用接口（方法/槽）：

- `void setKeyFont(const QFont &font)`: 设置键帽字体。
- `void setKeyBackgroundImage(int qtKey, const QString &imagePath) / setKeyBackgroundPixmap(int qtKey, const QPixmap &pixmap)`: 为某个 Qt::Key 键设置专属背景贴图，保留热图与高亮混色。
- `void clearKeyBackgroundImage(int qtKey)`: 清除指定键的背景贴图。
- `void recordKey(int qtKey)`: 手动记录一次按键（如远端事件或回放）。
- `void setHeatSamples(const QHash<int, int> &samples)`: 批量设置按键计数，便于恢复或注入统计数据。
- `void clearStatistics()`: 清空所有统计并重置热力图。
- `bool eventFilter(QObject *watched, QEvent *event)`: 内部安装的事件过滤器，开启 `trackPhysicalKeyboard` 后自动响应硬件按键。

## 自定义视觉

- 高亮渐隐：任何一次按键调用 `recordKey` 或硬件键入都会触发对应键帽的光晕动画，亮度随时间衰减。
- 热力图：`heatMapEnabled` 打开时，按键背景会按累计计数在 `coldColor` 与 `hotColor` 之间插值；计数越高颜色越接近 `hotColor`。若为单个键设置背景图片，将在图片上叠加热图和高亮色。
- 字体/文本色：通过 `setKeyFont` 调整键帽字体，颜色会在内部根据热力图和高亮混合，保持可读性。
- 自适应缩放：网格行列均设置拉伸因子，控件缩放时键帽比例保持一致，字体采用固定像素大小以避免随缩放模糊。

## 使用示例

```cpp
#include "VirtualKeyboardWidget.h"

VirtualKeyboardWidget *keyboard = new VirtualKeyboardWidget(parent);
keyboard->setTrackPhysicalKeyboard(true);   // 监听硬件键盘
keyboard->setHeatMapEnabled(true);          // 开启热力图
keyboard->setHighlightColor(QColor(255, 102, 170));
keyboard->setColdColor(QColor(18, 26, 38));
keyboard->setHotColor(QColor(120, 190, 255));
keyboard->setKeyFont(QFont("Noto Sans", 10));
// 为空格键设置背景图，体验与热图叠加效果
keyboard->setKeyBackgroundImage(Qt::Key_Space, "/path/to/space_texture.png");

// 手动记录按键
keyboard->recordKey(Qt::Key_A);

// 批量注入统计
QHash<int, int> samples;
samples[Qt::Key_Space] = 42;
samples[Qt::Key_Return] = 17;
keyboard->setHeatSamples(samples);

// 清空统计
keyboard->clearStatistics();
```

控件启用事件过滤后，任何传递到应用的键盘事件都会在虚拟键盘上高亮对应键位并计数。热力图会根据最大计数自动缩放。

## Demo 运行

项目默认开启 `BUILD_VIRTUAL_KEYBOARD_DEMO=ON`，编译后会生成 `VirtualKeyboardDemo` 可执行文件，包含中文注释的使用示例：

```bash
mkdir -p build && cd build
cmake .. -DBUILD_VIRTUAL_KEYBOARD_DEMO=ON
cmake --build .
./VirtualKeyboardDemo
```

演示窗口支持：

- 监听真实键盘并呈现高亮渐隐。
- “模拟按键”按钮触发 `recordKey`，便于快速观察热力图变化。
- “清空统计”按钮调用 `clearStatistics`，重置计数与热力图。
- 示例中包含切换键帽背景图与热力图叠加效果的小示例，方便测试纹理化热图。
