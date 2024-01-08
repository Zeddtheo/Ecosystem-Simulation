QT       += core gui openglwidgets
QT += opengl

CONFIG += c++14
LIBS += -lopengl32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Ecosystem
TEMPLATE = app

SOURCES += main.cpp\
        animal.cpp \
        grid.cpp \
        ground.cpp \
        plant.cpp \
        princ.cpp \
        glarea.cpp

HEADERS  += princ.h \
        animal.h \
        glarea.h \
        grid.h \
        ground.h \
        plant.h

FORMS    += princ.ui

RESOURCES += \
    ecosystem.qrc
