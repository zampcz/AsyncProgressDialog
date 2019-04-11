#-------------------------------------------------
#
# Project created by QtCreator 2019-03-12T20:44:06
#
#-------------------------------------------------

QT       += core gui concurrent charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsyncProgressDialog
TEMPLATE = app

INCLUDEPATH += $$PWD/include/apd

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    src/AsyncProgressDialog.cpp \
    src/ProgressBar.cpp \
    src/ProgressEstimate.cpp \
    src/ProgressLabel.cpp \
    src/ProgressOutput.cpp \
    src/ProgressVelocityPlot.cpp \
    src/ProgressWidget.cpp \
    src/ProgressWidgetContainer.cpp \
    src/ProgressWidgetFactory.cpp \
    src/TaskThread.cpp \
    src/Documentation.cpp

HEADERS += \
        mainwindow.h \
    include/apd/AsyncProgressDialog.h \
    include/apd/FunctionThread.h \
    include/apd/ProgressBar.h \
    include/apd/ProgressEstimate.h \
    include/apd/ProgressLabel.h \
    include/apd/ProgressOutput.h \
    include/apd/ProgressVelocityPlot.h \
    include/apd/ProgressWidget.h \
    include/apd/ProgressWidgetContainer.h \
    include/apd/ProgressWidgetFactory.h \
    include/apd/TaskThread.h \
    include/apd/TimeStamp.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
