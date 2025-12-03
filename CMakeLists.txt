cmake_minimum_required(VERSION 3.16)
project(EChartKeyBoard VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 是否编译演示程序，便于快速体验控件
option(BUILD_VIRTUAL_KEYBOARD_DEMO "Build demo application for VirtualKeyboardWidget" ON)
# 控件要求可被 Qt Designer 直接加载，因此提供可配置的插件安装目录，方便部署到设计器的组件库
set(QT_DESIGNER_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/plugins/designer" CACHE PATH "Install path for Qt Designer plugins")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

# Designer 插件可选，若找到 Qt Designer 模块则编译并在设计器中暴露控件
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Designer QUIET)

add_library(VirtualKeyboardWidget STATIC
    src/KeyButton.cpp
    src/VirtualKeyboardWidget.cpp
)

target_include_directories(VirtualKeyboardWidget PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(VirtualKeyboardWidget PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

target_compile_definitions(VirtualKeyboardWidget PRIVATE
    QT_DEPRECATED_WARNINGS
)

# 示例程序，展示控件配置与统计效果
if (BUILD_VIRTUAL_KEYBOARD_DEMO)
    add_executable(VirtualKeyboardDemo
        examples/demo_main.cpp
    )
    target_link_libraries(VirtualKeyboardDemo PRIVATE
        VirtualKeyboardWidget
        Qt${QT_VERSION_MAJOR}::Widgets
    )
endif()

if (Qt${QT_VERSION_MAJOR}Designer_FOUND)
    add_library(VirtualKeyboardPlugin MODULE
        src/VirtualKeyboardPlugin.cpp
    )
    target_link_libraries(VirtualKeyboardPlugin PRIVATE
        VirtualKeyboardWidget
        Qt${QT_VERSION_MAJOR}::Designer
    )
    set_target_properties(VirtualKeyboardPlugin PROPERTIES
        OUTPUT_NAME "VirtualKeyboardPlugin"
    )

    # 安装 Designer 插件到指定目录（默认 CMAKE_INSTALL_PREFIX/plugins/designer），便于 Qt Designer 识别并加载控件
    install(TARGETS VirtualKeyboardPlugin
        LIBRARY DESTINATION ${QT_DESIGNER_PLUGIN_PATH}
        RUNTIME DESTINATION ${QT_DESIGNER_PLUGIN_PATH}
    )
endif()

install(TARGETS VirtualKeyboardWidget
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    src/VirtualKeyboardWidget.h
    src/KeyButton.h
    DESTINATION include/EChartKeyBoard
)
