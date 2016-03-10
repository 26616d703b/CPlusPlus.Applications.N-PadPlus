QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = N-Pad+
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
		textarea.cpp

HEADERS  += mainwindow.h\
        textarea.h

FORMS    += mainwindow.ui

RC_ICONS = Resources/app.ico
