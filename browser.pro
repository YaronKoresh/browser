TEMPLATE = app
TARGET = browser

QT += widgets network webenginewidgets webchannel printsupport

CONFIG += c++17
CONFIG += object_parallel_to_source

VERSION = 2.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

msvc {
    QMAKE_CXXFLAGS += /W4
    QMAKE_CXXFLAGS_RELEASE += /O2 /Oi
}
gcc|clang {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    QMAKE_CXXFLAGS_RELEASE += -O2
}

HEADERS += \
    constants.h \
    utils.h \
    downloaditem.h \
    navigationmanager.h \
    downloadmodel.h \
    downloadmanager.h \
    settingsmanager.h \
    bookmarkmanager.h \
    thememanager.h \
    browsertab.h \
    tabmanager.h \
    findbar.h \
    preferencesdialog.h \
    downloaddialog.h \
    mainwindow.h

SOURCES += \
    main.cpp \
    navigationmanager.cpp \
    downloadmodel.cpp \
    downloadmanager.cpp \
    settingsmanager.cpp \
    bookmarkmanager.cpp \
    thememanager.cpp \
    browsertab.cpp \
    tabmanager.cpp \
    findbar.cpp \
    preferencesdialog.cpp \
    downloaddialog.cpp \
    mainwindow.cpp

win32:RC_FILE = browser.rc

DESTDIR = ./build
