TEMPLATE = app
TARGET = browser

QT += widgets network webenginewidgets

HEADERS += \
    mainwindow.h \
    downloaddialog.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mainwindow_navigation.cpp \
    mainwindow_downloads.cpp \
    mainwindow_actions.cpp \
    downloaddialog.cpp

CONFIG += c++17

DESTDIR = ./build