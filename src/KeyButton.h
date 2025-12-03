#pragma once

#include <QPushButton>
#include <QTimer>

// 单个按键按钮，负责热力图着色、高亮渐隐等效果
class KeyButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal glowLevel READ glowLevel WRITE setGlowLevel NOTIFY glowLevelChanged)
public:
    // 构造函数，text 为显示文本
    explicit KeyButton(const QString &text = QString(), QWidget *parent = nullptr);

    // 触发一次高亮动画，可指定持续时间（当前渐隐步长为定值）
    void triggerGlow(int durationMs = 900);
    // 设置该键的统计次数以及全局最大次数，用于计算热力图强度
    void setHeat(int count, int maxCount);
    // 设置冷/热配色
    void setHeatColors(const QColor &cold, const QColor &hot);
    // 设置高亮颜色
    void setHighlightColor(const QColor &color);
    // 设置基础文本颜色（在混合色上保持可读性）
    void setBaseTextColor(const QColor &color);

    qreal glowLevel() const { return m_glowLevel; }
    void setGlowLevel(qreal level);

signals:
    void glowLevelChanged(qreal level);

private slots:
    void onFadeStep();

private:
    // 更新视觉效果（样式表）
    void updateVisualState();
    // 颜色线性插值
    QColor mixColor(const QColor &a, const QColor &b, qreal factor) const;

    // 渐隐计时器，周期性降低 glowLevel
    QTimer m_glowTimer;
    // 热力图冷色
    QColor m_coldColor {QColor(30, 35, 45)};
    // 热力图热色
    QColor m_hotColor {QColor(102, 170, 255)};
    // 高亮颜色
    QColor m_highlightColor {QColor(255, 51, 102)};
    // 文本基础色
    QColor m_textColor {Qt::white};
    // 当前按键计数
    int m_heat {0};
    // 全局最大计数
    int m_heatMax {1};
    // 当前高亮强度（0~1）
    qreal m_glowLevel {0.0};
};
