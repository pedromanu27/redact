#-------------------------------------------------
#
# Project created by QtCreator 2018-04-07T18:45:47
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = redact
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    finddialog.cpp

HEADERS  += mainwindow.h \
    finddialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    redact.rc

RC_FILE = redact.rc
