add_library(window STATIC
    mainwindow.h mainwindow.cpp mainwindow.ui
    filewindow.h filewindow.cpp filewindow.ui
    transferwindow.h transferwindow.cpp transferwindow.ui
    playerwindow.cpp playerwindow.h playerwindow.ui
)

target_link_libraries(window PRIVATE
    widgets
    network
    media
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_link_directories(window
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
