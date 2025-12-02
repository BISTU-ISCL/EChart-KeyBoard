#pragma once

#include "KeyButton.h"

#include <QEvent>
#include <QGridLayout>
#include <QHash>
#include <QPointer>
#include <QWidget>

struct KeySpec {
    QString label;
    int qtKey {0};
    int columnSpan {1};
    int rowSpan {1};
};

class VirtualKeyboardWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool trackPhysicalKeyboard READ trackPhysicalKeyboard WRITE setTrackPhysicalKeyboard)
    Q_PROPERTY(bool heatMapEnabled READ heatMapEnabled WRITE setHeatMapEnabled)
    Q_PROPERTY(QColor coldColor READ coldColor WRITE setColdColor)
    Q_PROPERTY(QColor hotColor READ hotColor WRITE setHotColor)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor)
public:
    explicit VirtualKeyboardWidget(QWidget *parent = nullptr);
    ~VirtualKeyboardWidget() override;

    QSize sizeHint() const override;

    bool trackPhysicalKeyboard() const { return m_trackPhysicalKeyboard; }
    void setTrackPhysicalKeyboard(bool enabled);

    bool heatMapEnabled() const { return m_heatMapEnabled; }
    void setHeatMapEnabled(bool enabled);

    QColor coldColor() const { return m_coldColor; }
    void setColdColor(const QColor &color);

    QColor hotColor() const { return m_hotColor; }
    void setHotColor(const QColor &color);

    QColor highlightColor() const { return m_highlightColor; }
    void setHighlightColor(const QColor &color);

    void setKeyFont(const QFont &font);

public slots:
    void recordKey(int qtKey);
    void setHeatSamples(const QHash<int, int> &samples);
    void clearStatistics();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void addKey(int row, int column, const KeySpec &spec);
    void refreshHeatMap();

    QGridLayout *m_layout {nullptr};
    QHash<int, QPointer<KeyButton>> m_keyButtons;
    QHash<int, int> m_heatCounter;
    bool m_trackPhysicalKeyboard {true};
    bool m_heatMapEnabled {true};
    QColor m_coldColor {QColor(18, 26, 38)};
    QColor m_hotColor {QColor(126, 192, 255)};
    QColor m_highlightColor {QColor(255, 65, 130)};
    QFont m_keyFont;
};
