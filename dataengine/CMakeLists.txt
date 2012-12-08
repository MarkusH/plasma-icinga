cmake_minimum_required(VERSION 2.6)

# A name for the project
project(icinga_dataengine)

# Find the required Libaries
find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)
find_package(QJSON REQUIRED)

include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
    )

# We add our source code here
set(dataengine_SRCS icingadataengine.cpp)

# Now make sure all files get to the right place
kde4_add_plugin(icinga_dataengine ${dataengine_SRCS})
target_link_libraries(icinga_dataengine
    ${KDE4_KDECORE_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${QT_QTNETWORK_LIBRARY}
    qjson
    )

install(TARGETS icinga_dataengine
    DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES icinga-status-monitor-dataengine.desktop
    DESTINATION ${SERVICES_INSTALL_DIR})

