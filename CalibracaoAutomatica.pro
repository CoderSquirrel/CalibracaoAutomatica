#-------------------------------------------------
#
# Project created by QtCreator 2016-03-08T07:34:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CalibracaoAutomatica
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc


SOURCES += main.cpp \
    contornoescolha.cpp \
    corcalibrada.cpp

HEADERS  += \
    contornoescolha.h \
    corcalibrada.h

FORMS    +=    contornoescolha.ui

DISTFILES += \
    CalibracaoAutomatica.pro.user
