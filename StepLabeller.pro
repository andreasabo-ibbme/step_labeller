QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    capture_thread.cpp \
    filetable.cpp \
    main.cpp \
    mainwindow.cpp \
    playercontrols.cpp \
    steptable.cpp

HEADERS += \
    capture_thread.h \
    filetable.h \
    mainwindow.h \
    playercontrols.h \
    steptable.h

INCLUDEPATH += c:/opencv_build_release/install/include
LIBS += c:/opencv_build_release/bin/libopencv_world451.dll


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
