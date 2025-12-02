#include "KeyButton.h"

#include <QColor>
#include <QStyle>

KeyButton::KeyButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setCheckable(false);
    setMinimumSize(32, 32);
    m_glowTimer.setInterval(30);
    connect(&m_glowTimer, &QTimer::timeout, this, &KeyButton::onFadeStep);
    updateVisualState();
}

void KeyButton::triggerGlow(int durationMs) {
    Q_UNUSED(durationMs);
    m_glowLevel = 1.0;
    m_glowTimer.start();
    updateVisualState();
}

void KeyButton::setHeat(int count, int maxCount) {
    m_heat = count;
    m_heatMax = qMax(1, maxCount);
    updateVisualState();
}

void KeyButton::setHeatColors(const QColor &cold, const QColor &hot) {
    m_coldColor = cold;
    m_hotColor = hot;
    updateVisualState();
}

void KeyButton::setHighlightColor(const QColor &color) {
    m_highlightColor = color;
    updateVisualState();
}

void KeyButton::setBaseTextColor(const QColor &color) {
    m_textColor = color;
    updateVisualState();
}

void KeyButton::setGlowLevel(qreal level) {
    if (qFuzzyCompare(level, m_glowLevel)) {
        return;
    }
    m_glowLevel = qBound<qreal>(0.0, level, 1.0);
    updateVisualState();
    emit glowLevelChanged(m_glowLevel);
}

void KeyButton::onFadeStep() {
    const qreal step = 0.04;
    if (m_glowLevel <= step) {
        m_glowLevel = 0.0;
        m_glowTimer.stop();
    } else {
        m_glowLevel -= step;
    }
    updateVisualState();
}

void KeyButton::updateVisualState() {
    const qreal heatFactor = qBound<qreal>(0.0, static_cast<qreal>(m_heat) / static_cast<qreal>(m_heatMax), 1.0);
    QColor background = mixColor(m_coldColor, m_hotColor, heatFactor);

    QColor glow = m_highlightColor;
    glow.setAlphaF(qBound<qreal>(0.0, m_glowLevel, 1.0));

    const QString style = QString(
        "QPushButton {"
        "  background-color: rgba(%1, %2, %3, 210);"
        "  color: %7;"
        "  border: 1px solid rgba(%4, %5, %6, 200);"
        "  border-radius: 4px;"
        "  padding: 6px 8px;"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(%8, %9, %10, 255);"
        "}"
    )
                            .arg(background.red())
                            .arg(background.green())
                            .arg(background.blue())
                            .arg(glow.red())
                            .arg(glow.green())
                            .arg(glow.blue())
                            .arg(m_textColor.name())
                            .arg(qMin(255, background.red() + 25))
                            .arg(qMin(255, background.green() + 25))
                            .arg(qMin(255, background.blue() + 25));

    setStyleSheet(style);
    update();
}

QColor KeyButton::mixColor(const QColor &a, const QColor &b, qreal factor) const {
    factor = qBound<qreal>(0.0, factor, 1.0);
    return QColor(
        static_cast<int>(a.red() + (b.red() - a.red()) * factor),
        static_cast<int>(a.green() + (b.green() - a.green()) * factor),
        static_cast<int>(a.blue() + (b.blue() - a.blue()) * factor)
    );
}
