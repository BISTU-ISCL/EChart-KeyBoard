#include "VirtualKeyboardWidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>

#include <algorithm>

namespace {
QList<KeySpec> makeTopRow() {
    return {
        {"Esc", Qt::Key_Escape},
        {"F1", Qt::Key_F1}, {"F2", Qt::Key_F2}, {"F3", Qt::Key_F3}, {"F4", Qt::Key_F4},
        {"F5", Qt::Key_F5}, {"F6", Qt::Key_F6}, {"F7", Qt::Key_F7}, {"F8", Qt::Key_F8},
        {"F9", Qt::Key_F9}, {"F10", Qt::Key_F10}, {"F11", Qt::Key_F11}, {"F12", Qt::Key_F12},
        {"PrtSc", Qt::Key_Print}, {"ScrLk", Qt::Key_ScrollLock}, {"Pause", Qt::Key_Pause},
        {"Insert", Qt::Key_Insert}, {"Delete", Qt::Key_Delete}
    };
}

QList<KeySpec> makeNumberRow() {
    return {
        {"`", Qt::Key_QuoteLeft},
        {"1", Qt::Key_1}, {"2", Qt::Key_2}, {"3", Qt::Key_3}, {"4", Qt::Key_4}, {"5", Qt::Key_5},
        {"6", Qt::Key_6}, {"7", Qt::Key_7}, {"8", Qt::Key_8}, {"9", Qt::Key_9}, {"0", Qt::Key_0},
        {"-", Qt::Key_Minus}, {"=", Qt::Key_Equal},
        {"Backspace", Qt::Key_Backspace, 2}
    };
}

QList<KeySpec> makeQRow() {
    return {
        {"Tab", Qt::Key_Tab, 2},
        {"Q", Qt::Key_Q}, {"W", Qt::Key_W}, {"E", Qt::Key_E}, {"R", Qt::Key_R}, {"T", Qt::Key_T},
        {"Y", Qt::Key_Y}, {"U", Qt::Key_U}, {"I", Qt::Key_I}, {"O", Qt::Key_O}, {"P", Qt::Key_P},
        {"[", Qt::Key_BracketLeft}, {"]", Qt::Key_BracketRight}, {"\\", Qt::Key_Backslash, 2}
    };
}

QList<KeySpec> makeARow() {
    return {
        {"Caps", Qt::Key_CapsLock, 2},
        {"A", Qt::Key_A}, {"S", Qt::Key_S}, {"D", Qt::Key_D}, {"F", Qt::Key_F}, {"G", Qt::Key_G},
        {"H", Qt::Key_H}, {"J", Qt::Key_J}, {"K", Qt::Key_K}, {"L", Qt::Key_L},
        {";", Qt::Key_Semicolon}, {"'", Qt::Key_Apostrophe},
        {"Enter", Qt::Key_Return, 3}
    };
}

QList<KeySpec> makeZRow() {
    return {
        {"Shift", Qt::Key_Shift, 3},
        {"Z", Qt::Key_Z}, {"X", Qt::Key_X}, {"C", Qt::Key_C}, {"V", Qt::Key_V}, {"B", Qt::Key_B},
        {"N", Qt::Key_N}, {"M", Qt::Key_M}, {",", Qt::Key_Comma}, {".", Qt::Key_Period}, {"/", Qt::Key_Slash},
        {"Shift", Qt::Key_Shift, 3}
    };
}

QList<KeySpec> makeBottomRow() {
    return {
        {"Ctrl", Qt::Key_Control, 2},
        {"Win", Qt::Key_Meta},
        {"Alt", Qt::Key_Alt},
        {"Space", Qt::Key_Space, 6},
        {"Alt", Qt::Key_Alt},
        {"Win", Qt::Key_Meta},
        {"Menu", Qt::Key_Menu},
        {"Ctrl", Qt::Key_Control, 2},
        {"←", Qt::Key_Left}, {"↑", Qt::Key_Up}, {"↓", Qt::Key_Down}, {"→", Qt::Key_Right}
    };
}
} // namespace

VirtualKeyboardWidget::VirtualKeyboardWidget(QWidget *parent)
    : QWidget(parent) {
    m_layout = new QGridLayout(this);
    m_layout->setSpacing(4);
    m_layout->setContentsMargins(6, 6, 6, 6);
    setLayout(m_layout);

    QList<QList<KeySpec>> rows = {makeTopRow(), makeNumberRow(), makeQRow(), makeARow(), makeZRow(), makeBottomRow()};

    for (int row = 0; row < rows.size(); ++row) {
        int column = 0;
        for (const auto &spec : rows[row]) {
            addKey(row, column, spec);
            column += spec.columnSpan;
        }
    }

    setKeyFont(QFont("Inter", 10));
    setMinimumWidth(720);
    setMinimumHeight(260);

    if (m_trackPhysicalKeyboard) {
        qApp->installEventFilter(this);
    }
}

VirtualKeyboardWidget::~VirtualKeyboardWidget() {
    if (m_trackPhysicalKeyboard) {
        qApp->removeEventFilter(this);
    }
}

QSize VirtualKeyboardWidget::sizeHint() const {
    return {980, 300};
}

void VirtualKeyboardWidget::setTrackPhysicalKeyboard(bool enabled) {
    if (m_trackPhysicalKeyboard == enabled) {
        return;
    }
    m_trackPhysicalKeyboard = enabled;
    if (enabled) {
        qApp->installEventFilter(this);
    } else {
        qApp->removeEventFilter(this);
    }
}

void VirtualKeyboardWidget::setHeatMapEnabled(bool enabled) {
    m_heatMapEnabled = enabled;
    refreshHeatMap();
}

void VirtualKeyboardWidget::setColdColor(const QColor &color) {
    m_coldColor = color;
    refreshHeatMap();
}

void VirtualKeyboardWidget::setHotColor(const QColor &color) {
    m_hotColor = color;
    refreshHeatMap();
}

void VirtualKeyboardWidget::setHighlightColor(const QColor &color) {
    m_highlightColor = color;
    for (auto button : m_keyButtons) {
        if (button) {
            button->setHighlightColor(color);
        }
    }
}

void VirtualKeyboardWidget::setKeyFont(const QFont &font) {
    m_keyFont = font;
    for (auto button : m_keyButtons) {
        if (button) {
            button->setFont(font);
        }
    }
}

void VirtualKeyboardWidget::recordKey(int qtKey) {
    if (!m_keyButtons.contains(qtKey)) {
        return;
    }
    m_heatCounter[qtKey] += 1;
    if (auto button = m_keyButtons.value(qtKey)) {
        button->triggerGlow();
    }
    refreshHeatMap();
}

void VirtualKeyboardWidget::setHeatSamples(const QHash<int, int> &samples) {
    m_heatCounter = samples;
    refreshHeatMap();
}

void VirtualKeyboardWidget::clearStatistics() {
    m_heatCounter.clear();
    refreshHeatMap();
}

bool VirtualKeyboardWidget::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched);
    if (!m_trackPhysicalKeyboard) {
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        recordKey(keyEvent->key());
    }
    return QWidget::eventFilter(watched, event);
}

void VirtualKeyboardWidget::addKey(int row, int column, const KeySpec &spec) {
    auto *button = new KeyButton(spec.label, this);
    button->setFont(m_keyFont);
    button->setHeatColors(m_coldColor, m_hotColor);
    button->setHighlightColor(m_highlightColor);
    button->setBaseTextColor(Qt::white);

    m_layout->addWidget(button, row, column, spec.rowSpan, spec.columnSpan);
    m_keyButtons.insertMulti(spec.qtKey, button);

    connect(button, &QPushButton::clicked, this, [this, spec]() {
        recordKey(spec.qtKey);
    });
}

void VirtualKeyboardWidget::refreshHeatMap() {
    int maxCount = 1;
    if (!m_heatCounter.isEmpty()) {
        maxCount = std::max(1, *std::max_element(m_heatCounter.begin(), m_heatCounter.end()));
    }

    for (auto it = m_keyButtons.begin(); it != m_keyButtons.end(); ++it) {
        auto key = it.key();
        auto button = it.value();
        if (!button) {
            continue;
        }

        int count = m_heatMapEnabled ? m_heatCounter.value(key, 0) : 0;
        button->setHeat(count, maxCount);
        button->setHeatColors(m_coldColor, m_hotColor);
    }
}
