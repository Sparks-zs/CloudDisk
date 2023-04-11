add_library(widgets STATIC
    filewidget.cpp
    filewidget.ui
    flowlayout.cpp
    filewidget.h
    flowlayout.h
    networkcontrollerwidget.cpp
    networkcontrollerwidget.h
)

target_link_libraries(widgets PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    network
)


target_link_directories(widgets
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
