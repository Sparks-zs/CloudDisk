add_library(network STATIC
    httpnetwork.cpp
    networksender.cpp
    networkcontroller.cpp
    networkmanager.cpp
    fileinfo.cpp
    fileinfo.h
    httpnetwork.h
    networksender.h
    networkcontroller.h
    networkmanager.h
    networkmedia.cpp
    networkmedia.h
)

find_package(Qt6 REQUIRED COMPONENTS Network)

target_link_libraries(network PUBLIC
    Qt6::Network
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_link_directories(network
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
