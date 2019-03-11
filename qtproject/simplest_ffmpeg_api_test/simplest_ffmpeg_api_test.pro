QT += core
QT -= gui

CONFIG += c++11

TARGET = simplest_ffmpeg_api_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR = $$PWD/bin

SOURCES += main.cpp

LIBS += $$PWD/../../ffmpge-lib/lib/*.lib

INCLUDEPATH += $$PWD/../../ffmpge-lib/include
