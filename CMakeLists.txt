cmake_minimum_required(VERSION 3.19)

project(browser
    VERSION 2.0.0
    DESCRIPTION "A modern web browser built with Qt"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    WebEngineWidgets
    WebEngineCore
    PrintSupport
)

set(HEADERS
    constants.h
    utils.h
    downloaditem.h
    navigationmanager.h
    downloadmodel.h
    downloadmanager.h
    settingsmanager.h
    bookmarkmanager.h
    thememanager.h
    browsertab.h
    tabmanager.h
    findbar.h
    preferencesdialog.h
    downloaddialog.h
    mainwindow.h
)

set(SOURCES
    main.cpp
    navigationmanager.cpp
    downloadmodel.cpp
    downloadmanager.cpp
    settingsmanager.cpp
    bookmarkmanager.cpp
    thememanager.cpp
    browsertab.cpp
    tabmanager.cpp
    findbar.cpp
    preferencesdialog.cpp
    downloaddialog.cpp
    mainwindow.cpp
)

if(WIN32)
    qt_add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} browser.rc)
else()
    qt_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebEngineWidgets
    Qt6::WebEngineCore
    Qt6::PrintSupport
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_subdirectory(tests)
endif()
