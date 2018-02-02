#-------------------------------------------------
#
# Project created by QtCreator 2018-02-01T11:40:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = helpTiaoyitiao
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cmdthread.cpp

HEADERS  += mainwindow.h \
    cmdthread.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\opencv\Release\install\include

LIBS += C:\opencv\Release\install\x86\mingw\lib\libopencv_core340.dll.a
LIBS += C:\opencv\Release\install\x86\mingw\lib\libopencv_highgui340.dll.a
LIBS += C:\opencv\Release\install\x86\mingw\lib\libopencv_imgcodecs340.dll.a
LIBS += C:\opencv\Release\install\x86\mingw\lib\libopencv_imgproc340.dll.a
LIBS += C:\opencv\Release\install\x86\mingw\lib\libopencv_features2d340.dll.a
LIBS += C:\opencv\Release\install\x86\mingw\lib\libopencv_calib3d340.dll.a

RC_ICONS = "myapp.ico"

RESOURCES += \
    11.qrc
