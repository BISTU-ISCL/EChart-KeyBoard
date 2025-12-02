#pragma once

#include <QPushButton>
#include <QTimer>

class KeyButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal glowLevel READ glowLevel WRITE setGlowLevel NOTIFY glowLevelChanged)
public:
    explicit KeyButton(const QString &text = QString(), QWidget *parent = nullptr);

    void triggerGlow(int durationMs = 900);
    void setHeat(int count, int maxCount);
    void setHeatColors(const QColor &cold, const QColor &hot);
    void setHighlightColor(const QColor &color);
    void setBaseTextColor(const QColor &color);

    qreal glowLevel() const { return m_glowLevel; }
    void setGlowLevel(qreal level);

signals:
    void glowLevelChanged(qreal level);

private slots:
    void onFadeStep();

private:
    void updateVisualState();
    QColor mixColor(const QColor &a, const QColor &b, qreal factor) const;

    QTimer m_glowTimer;
    QColor m_coldColor {QColor(30, 35, 45)};
    QColor m_hotColor {QColor(102, 170, 255)};
    QColor m_highlightColor {QColor(255, 51, 102)};
    QColor m_textColor {Qt::white};
    int m_heat {0};
    int m_heatMax {1};
    qreal m_glowLevel {0.0};
};
