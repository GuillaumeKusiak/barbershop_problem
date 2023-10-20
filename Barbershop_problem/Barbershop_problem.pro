TEMPLATE = app
CONFIG += c++20
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

LIBS += -pthread

QT += core

QMAKE_CXXFLAGS += -std=c++2a
