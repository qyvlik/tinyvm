TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tinyvm.cpp

HEADERS += \
    tinyvm.h \
    tinyvm_test.h
