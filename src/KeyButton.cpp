#include "KeyButton.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QStyle>

KeyButton::KeyButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent) {
    // 基础外观与布局设定
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCheckable(false);
    setMinimumSize(32, 32);
    // 设置渐隐计时器频率
    m_glowTimer.setInterval(30);
    connect(&m_glowTimer, &QTimer::timeout, this, &KeyButton::onFadeStep);
    updateVisualState();
}

void KeyButton::triggerGlow(int durationMs) {
    Q_UNUSED(durationMs);
    // 重置高亮强度并开启渐隐计时
    m_glowLevel = 1.0;
    m_glowTimer.start();
    updateVisualState();
}

void KeyButton::setHeat(int count, int maxCount) {
    // 记录当前键计数与全局最大计数
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

void KeyButton::setBackgroundPixmap(const QPixmap &pixmap) {
    if (pixmap.cacheKey() == m_backgroundPixmap.cacheKey()) {
        return;
    }
    // 设置背景图片，同时保持路径字符串一致性
    m_backgroundPixmap = pixmap;
    if (m_backgroundImagePath.isEmpty()) {
        m_backgroundImagePath = QString();
        emit backgroundImagePathChanged(m_backgroundImagePath);
    }
    updateVisualState();
    emit backgroundPixmapChanged(m_backgroundPixmap);
}

void KeyButton::setBackgroundImagePath(const QString &path) {
    if (path == m_backgroundImagePath) {
        return;
    }
    m_backgroundImagePath = path;
    QPixmap pixmap(path);
    if (!pixmap.isNull()) {
        m_backgroundPixmap = pixmap;
        emit backgroundPixmapChanged(m_backgroundPixmap);
    }
    updateVisualState();
    emit backgroundImagePathChanged(m_backgroundImagePath);
}

void KeyButton::setGlowLevel(qreal level) {
    // 仅当变化显著时才更新，避免多余刷新
    if (qFuzzyCompare(level, m_glowLevel)) {
        return;
    }
    m_glowLevel = qBound<qreal>(0.0, level, 1.0);
    updateVisualState();
    emit glowLevelChanged(m_glowLevel);
}

void KeyButton::onFadeStep() {
    const qreal step = 0.04;
    // 每个周期衰减 glowLevel，直至停止计时器
    if (m_glowLevel <= step) {
        m_glowLevel = 0.0;
        m_glowTimer.stop();
    } else {
        m_glowLevel -= step;
    }
    updateVisualState();
}

void KeyButton::updateVisualState() {
    // 仅更新边框、圆角与文字颜色，背景交由 paintEvent 绘制以便叠加图片
    QColor glow = m_highlightColor;
    glow.setAlphaF(qBound<qreal>(0.0, m_glowLevel, 1.0));

    const QString style = QString(
        "QPushButton {"
        "  color: %1;"
        "  border: 1px solid rgba(%2, %3, %4, 200);"
        "  border-radius: 4px;"
        "  padding: 6px 8px;"
        "}"
        "QPushButton:pressed {"
        "  border-color: rgba(%5, %6, %7, 230);"
        "}"
    )
                            .arg(m_textColor.name())
                            .arg(glow.red())
                            .arg(glow.green())
                            .arg(glow.blue())
                            .arg(qMin(255, glow.red() + 40))
                            .arg(qMin(255, glow.green() + 40))
                            .arg(qMin(255, glow.blue() + 40));

    setStyleSheet(style);
    update();
}

QColor KeyButton::mixColor(const QColor &a, const QColor &b, qreal factor) const {
    // 线性插值返回中间色
    factor = qBound<qreal>(0.0, factor, 1.0);
    return QColor(
        static_cast<int>(a.red() + (b.red() - a.red()) * factor),
        static_cast<int>(a.green() + (b.green() - a.green()) * factor),
        static_cast<int>(a.blue() + (b.blue() - a.blue()) * factor)
    );
}

void KeyButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    // 热力图基础颜色
    const qreal heatFactor = qBound<qreal>(0.0, static_cast<qreal>(m_heat) / static_cast<qreal>(m_heatMax), 1.0);
    QColor baseColor = mixColor(m_coldColor, m_hotColor, heatFactor);

    // 高亮叠加
    QColor overlayColor = m_highlightColor;
    overlayColor.setAlphaF(qBound<qreal>(0.0, m_glowLevel, 1.0));
    baseColor = mixColor(baseColor, overlayColor, overlayColor.alphaF());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 圆角矩形区域
    const qreal radius = 6.0;
    QRectF outer = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    QPainterPath path;
    path.addRoundedRect(outer, radius, radius);

    // 若存在背景图则先绘制贴图，再叠加颜色以保留热图与高亮效果
    if (!m_backgroundPixmap.isNull()) {
        QPixmap scaled = m_backgroundPixmap.scaled(outer.size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        // 计算裁剪区域，使图片充满圆角区域
        const int sx = static_cast<int>((scaled.width() - outer.width()) / 2);
        const int sy = static_cast<int>((scaled.height() - outer.height()) / 2);
        const int sw = static_cast<int>(outer.width());
        const int sh = static_cast<int>(outer.height());
        QRect sourceRect(sx, sy, sw, sh);
        painter.setClipPath(path);
        painter.drawPixmap(outer.topLeft(), scaled, sourceRect);
        painter.fillPath(path, QColor(baseColor.red(), baseColor.green(), baseColor.blue(), 140));
    } else {
        painter.fillPath(path, baseColor);
    }

    // 绘制边框
    painter.setPen(QPen(overlayColor.isValid() ? overlayColor : baseColor, 1.2));
    painter.drawPath(path);

    // 显示文本
    painter.setPen(m_textColor);
    painter.setFont(font());
    painter.drawText(outer, Qt::AlignCenter, text());
}
