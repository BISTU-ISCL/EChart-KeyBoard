cmake_minimum_required(VERSION 3.16)
project(EChartKeyBoard VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

# Designer plugin is optional; if available we expose the widget in Qt Designer.
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
