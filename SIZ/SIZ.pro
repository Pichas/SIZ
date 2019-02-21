#-------------------------------------------------
#
# Project created by QtCreator 2018-12-26T12:58:56
#
#-------------------------------------------------

QT       += core gui sql axcontainer network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SIZ
TEMPLATE = app

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += ALL


SOURCES += \
    report/inmemdb.cpp \
    report/mybuffer.cpp \
    report/mycell.cpp \
    report/myrow.cpp \
    report/myrowlarge.cpp \
    report/myrowonce.cpp \
    report/myrowperiodic.cpp \
    report/myrowsmall.cpp \
    report/mytable.cpp \
    report/result.cpp \
    main.cpp \
    mainwindow.cpp \
    myset.cpp \
    wintables.cpp \
    winsetprice.cpp \
    winnorma.cpp \
    winstock.cpp \
    modelnames.cpp \
    winstafflist.cpp \
    winnames.cpp \
    modelstafflist.cpp \
    winreport.cpp \
    winemplcard.cpp \
    windblist.cpp \
    update/appupdate.cpp

HEADERS += \
    report/inmemdb.h \
    report/mybuffer.h \
    report/mycell.h \
    report/myrow.h \
    report/myrowlarge.h \
    report/myrowonce.h \
    report/myrowperiodic.h \
    report/myrowsmall.h \
    report/mytable.h \
    report/result.h \
    mainwindow.h \
    myset.h \
    wintables.h \
    winsetprice.h \
    winnorma.h \
    winstock.h \
    modelnames.h \
    modelstafflist.h \
    winstafflist.h \
    winnames.h \
    winreport.h \
    winemplcard.h \
    windblist.h \
    update/appupdate.h

FORMS += \
        mainwindow.ui \
    wintables.ui \
    winsetprice.ui \
    winnorma.ui \
    winstock.ui \
    winstafflist.ui \
    winnames.ui \
    winreport.ui \
    winemplcard.ui \
    windblist.ui

RESOURCES += \
    src.qrc


win32 {
    RC_FILE += file.rc
    OTHER_FILES += file.rc version.h
}
