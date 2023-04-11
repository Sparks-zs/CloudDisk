add_library(media STATIC
    playlistmodel.cpp
    playlistmodel.h
    playercontrols.cpp
    playercontrols.h
    qmediaplaylist.cpp
    qmediaplaylist.h
    qmediaplaylist_p.cpp
    qmediaplaylist_p.h
    qplaylistfileparser.cpp
    qplaylistfileparser.h
    videowidget.cpp
    videowidget.h
)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets Network)

target_link_libraries(media PUBLIC
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Network
)

target_link_directories(media
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
