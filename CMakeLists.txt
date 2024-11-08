cmake_minimum_required(VERSION 3.16)
project(war1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(war1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(war1 SHARED ${PROJECT_SOURCES})
    else()
        add_executable(war1 ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(war1 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

set_target_properties(war1 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Добавляем цели установки
include(GNUInstallDirs)

install(TARGETS war1
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Устанавливаем desktop файл
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ru.metnerium.war1.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/ru.metnerium.war1.desktop
    @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ru.metnerium.war1.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(war1)
endif()
