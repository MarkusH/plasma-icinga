cmake_minimum_required(VERSION 2.6)

# A name for the project
project(icinga_plasmoid)

# Find the required Libaries
find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)

include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
    )

# We add our source code here
set(plasmoid_SRCS
    icingaplasmoid.cpp
    extendedstatus.cpp)

# Now make sure all files get to the right place
kde4_add_plugin(icinga_plasmoid ${plasmoid_SRCS})
target_link_libraries(icinga_plasmoid
    ${KDE4_KDECORE_LIBS}
    ${KDE4_PLASMA_LIBS}
#    ${QT_QTNETWORK_LIBRARY}
    )

install(TARGETS icinga_plasmoid
    DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES icinga-status-monitor-plasmoid.desktop
    DESTINATION ${SERVICES_INSTALL_DIR})