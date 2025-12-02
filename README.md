# EChart-KeyBoard

A customizable Qt widget that renders a full keyboard, reacts to physical key presses, and visualizes usage statistics with fading highlights and a heat map. The widget is also exposed as a Qt Designer plugin when the `Designer` module is available.

## Building

This project uses CMake and supports both Qt 5 and Qt 6.

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

The `VirtualKeyboardWidget` static library is always built. If the Qt Designer module is found, a `VirtualKeyboardPlugin` module is also produced so the widget can be dragged from the Designer palette.

## Using the widget

```cpp
#include "VirtualKeyboardWidget.h"

VirtualKeyboardWidget *keyboard = new VirtualKeyboardWidget(parent);
keyboard->setTrackPhysicalKeyboard(true);  // react to hardware key presses
keyboard->setHeatMapEnabled(true);         // colorize keys based on usage
keyboard->setHighlightColor(QColor(255, 102, 170));
keyboard->setColdColor(QColor(18, 26, 38));
keyboard->setHotColor(QColor(120, 190, 255));

// Programmatically record key activity (for remote events or replay)
keyboard->recordKey(Qt::Key_A);

// Provide an entire histogram at once
QHash<int, int> samples;
samples[Qt::Key_Space] = 42;
samples[Qt::Key_Return] = 17;
keyboard->setHeatSamples(samples);

// Reset counts
keyboard->clearStatistics();
```

The widget installs an event filter (when enabled) so any key press that reaches the application will briefly glow the matching key and increment statistics. Heat map colors scale automatically based on the most frequently used key.
