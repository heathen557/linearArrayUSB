#-------------------------------------------------
#
# Project created by QtCreator 2019-08-06T14:34:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = linearArrayUSB
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    helper.cpp \
    dealusb_msg.cpp \
    receusb_msg.cpp \
    savepcdthread.cpp \
    filesave.cpp \
    showsettingdia.cpp \
    statisticsdialog.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    helper.h \
    dealusb_msg.h \
    receusb_msg.h \
    savepcdthread.h \
    filesave.h \
    showsettingdia.h \
    statisticsdialog.h

FORMS    += mainwindow.ui \
    filesave.ui \
    showsettingdia.ui \
    statisticsdialog.ui


INCLUDEPATH += "D:/programInstall/libsub"
#INCLUDEPATH += -lD:/programInstall/libsub

LIBS += "D:/programInstall/libsub/libusb.lib"
