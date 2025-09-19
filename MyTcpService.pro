QT       += core gui sql  network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customwidget.cpp \
    databaseutil.cpp \
    datareceiverwidget.cpp \
    enterinterface.cpp \
    main.cpp \
    maininterface.cpp \
    musicwidget.cpp \
    networkswitchwidget.cpp \
    networktool.cpp \
    timewidget.cpp \
    videowidget.cpp \
    weatherwidget.cpp \
    widget.cpp

HEADERS += \
    customwidget.h \
    databaseutil.h \
    datareceiverwidget.h \
    enterinterface.h \
    maininterface.h \
    musicwidget.h \
    networkswitchwidget.h \
    networktool.h \
    timewidget.h \
    videowidget.h \
    weatherwidget.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
