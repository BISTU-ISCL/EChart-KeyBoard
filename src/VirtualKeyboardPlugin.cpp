#include "VirtualKeyboardWidget.h"

#include <QtUiPlugin/QDesignerCustomWidgetInterface>

class VirtualKeyboardPlugin : public QObject, public QDesignerCustomWidgetInterface {
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")

public:
    // 简单的设计器插件，暴露 VirtualKeyboardWidget 供拖拽使用
    VirtualKeyboardPlugin(QObject *parent = nullptr) : QObject(parent) {}

    bool isContainer() const override { return false; }
    bool isInitialized() const override { return m_initialized; }
    QIcon icon() const override { return QIcon(); }
    QString domXml() const override {
        return R"(<widget class="VirtualKeyboardWidget" name="virtualKeyboard">
                <property name="geometry">
                    <rect>
                        <x>0</x>
                        <y>0</y>
                        <width>800</width>
                        <height>300</height>
                    </rect>
                </property>
            </widget>)";
    }
    QString group() const override { return "Custom Widgets"; }
    QString includeFile() const override { return "VirtualKeyboardWidget.h"; }
    QString name() const override { return "VirtualKeyboardWidget"; }
    // 提示文案使用英文，以便设计器一致性
    QString toolTip() const override { return "Full keyboard widget with heat map statistics"; }
    QString whatsThis() const override { return "Visual keyboard that reacts to physical keystrokes and exposes heat map statistics."; }
    QWidget *createWidget(QWidget *parent) override { return new VirtualKeyboardWidget(parent); }
    void initialize(QDesignerFormEditorInterface *core) override {
        Q_UNUSED(core);
        m_initialized = true;
    }

private:
    // 初始化标记
    bool m_initialized {false};
};

#include "VirtualKeyboardPlugin.moc"
