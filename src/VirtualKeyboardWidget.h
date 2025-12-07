#pragma once

#include "KeyButton.h"

#include <QEvent>
#include <QGridLayout>
#include <QHash>
#include <QPointer>
#include <QPixmap>
#include <QWidget>

// 键位布局描述，用于生成整排键
struct KeySpec {
    QString label;      // 键帽显示文本
    int qtKey {0};      // 对应 Qt::Key
    int columnSpan {1}; // 横向跨列数
    int rowSpan {1};    // 纵向跨行数
};

class VirtualKeyboardWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool trackPhysicalKeyboard READ trackPhysicalKeyboard WRITE setTrackPhysicalKeyboard)
    Q_PROPERTY(bool heatMapEnabled READ heatMapEnabled WRITE setHeatMapEnabled)
    Q_PROPERTY(QColor coldColor READ coldColor WRITE setColdColor)
    Q_PROPERTY(QColor hotColor READ hotColor WRITE setHotColor)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor)
public:
    // 构造与析构
    explicit VirtualKeyboardWidget(QWidget *parent = nullptr);
    ~VirtualKeyboardWidget() override;

    // 建议尺寸，便于在设计器中显示
    QSize sizeHint() const override;

    bool trackPhysicalKeyboard() const { return m_trackPhysicalKeyboard; }
    // 是否监听物理键盘事件
    void setTrackPhysicalKeyboard(bool enabled);

    bool heatMapEnabled() const { return m_heatMapEnabled; }
    // 是否启用热力图着色
    void setHeatMapEnabled(bool enabled);

    QColor coldColor() const { return m_coldColor; }
    // 设置热力图冷色
    void setColdColor(const QColor &color);

    QColor hotColor() const { return m_hotColor; }
    // 设置热力图热色
    void setHotColor(const QColor &color);

    QColor highlightColor() const { return m_highlightColor; }
    // 设置高亮颜色
    void setHighlightColor(const QColor &color);

    // 配置键帽字体
    void setKeyFont(const QFont &font);
    // 为指定按键设置自定义背景图（可用于替换默认热图色块）
    void setKeyBackgroundImage(int qtKey, const QString &imagePath);
    void setKeyBackgroundPixmap(int qtKey, const QPixmap &pixmap);
    // 清除指定按键的自定义背景图
    void clearKeyBackgroundImage(int qtKey);

public slots:
    // 记录一次按键（外部调用或内部点击）
    void recordKey(int qtKey);
    // 批量注入历史统计
    void setHeatSamples(const QHash<int, int> &samples);
    // 清空统计并刷新热力图
    void clearStatistics();

protected:
    // 监听全局按键事件，响应硬件键盘
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    // 创建一个键并放入布局
    void addKey(int row, int column, const KeySpec &spec);
    // 根据计数刷新热力图与按钮状态
    void refreshHeatMap();

    QGridLayout *m_layout {nullptr};
    // Qt::Key -> 对应 KeyButton
    QHash<int, QPointer<KeyButton>> m_keyButtons;
    // 按键计数表
    QHash<int, int> m_heatCounter;
    // 监听物理键盘开关
    bool m_trackPhysicalKeyboard {true};
    // 热力图开关
    bool m_heatMapEnabled {true};
    // 热力图冷/热色
    QColor m_coldColor {QColor(18, 26, 38)};
    QColor m_hotColor {QColor(126, 192, 255)};
    // 高亮颜色
    QColor m_highlightColor {QColor(255, 65, 130)};
    // 键帽字体
    QFont m_keyFont;
    // 每个按键可选的背景贴图
    QHash<int, QPixmap> m_keyBackgrounds;
};
