#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>

#include "VirtualKeyboardWidget.h"

// 简单演示窗口，展示虚拟键盘的配置、手动记录与清空统计
class DemoWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit DemoWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // 中心部件与水平布局
        auto *central = new QWidget(this);
        auto *layout = new QHBoxLayout(central);
        layout->setContentsMargins(12, 12, 12, 12);
        setCentralWidget(central);

        // 左侧放置虚拟键盘控件
        m_keyboard = new VirtualKeyboardWidget(this);
        m_keyboard->setTrackPhysicalKeyboard(true);   // 监听物理键盘高亮
        m_keyboard->setHeatMapEnabled(true);          // 开启热力图
        m_keyboard->setHighlightColor(QColor(255, 128, 188));
        m_keyboard->setColdColor(QColor(26, 32, 44));
        m_keyboard->setHotColor(QColor(92, 178, 255));
        m_keyboard->setKeyFont(QFont("Noto Sans", 10));
        layout->addWidget(m_keyboard, 2);

        // 右侧放置操作按钮与提示
        auto *rightPanel = new QWidget(this);
        auto *rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setSpacing(12);

        auto *info = new QLabel(tr("使用说明:\n- 在真实键盘上按键可看到高亮渐隐\n- 按下“模拟按键”按钮可快速累积热力图\n- 点击“清空统计”会重置热力图"), rightPanel);
        info->setWordWrap(true);
        rightLayout->addWidget(info);

        auto *simulate = new QPushButton(tr("模拟按键"), rightPanel);
        connect(simulate, &QPushButton::clicked, this, [this]() {
            // 通过 recordKey 手动注入多个按键，观察热力分布
            const QList<int> keys = {Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_Space};
            for (int key : keys) {
                m_keyboard->recordKey(key);
            }
        });
        rightLayout->addWidget(simulate);

        auto *clear = new QPushButton(tr("清空统计"), rightPanel);
        connect(clear, &QPushButton::clicked, m_keyboard, &VirtualKeyboardWidget::clearStatistics);
        rightLayout->addWidget(clear);

        rightLayout->addStretch();
        layout->addWidget(rightPanel, 1);

        setWindowTitle(tr("虚拟键盘控件示例"));
        resize(1100, 420);

        // 可选: 定时记录空格键，让热力图快速可见
        auto *timer = new QTimer(this);
        timer->setInterval(1500);
        connect(timer, &QTimer::timeout, this, [this]() { m_keyboard->recordKey(Qt::Key_Space); });
        timer->start();
    }

private:
    VirtualKeyboardWidget *m_keyboard {nullptr};
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    DemoWindow window;
    window.show();
    return app.exec();
}

#include "demo_main.moc"
