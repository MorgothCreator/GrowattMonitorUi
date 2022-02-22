QT	    += core gui charts serialport

unix:!macx {
TARGET = ../../GrowattMonitor/GrowattMonitorUI-Linux/GrowattMonitorUI-Linux
}
macx: {
TARGET = ../GrowattMonitor/GrowattMonitorUI-MacOsX
CONFIG += QMAKE_APPLE_DEVICE_ARCHS="x86_64 x86_64h arm64"
CONFIG += app_bundle
CONFIG += qt
CONFIG += sdk_no_version_check
DEFINES += MY_LIB_PATH=/
MACDEPLOY = `echo $$QMAKE_QMAKE | sed 's/qmake/macdeployqt/g'`
QMAKE_POST_LINK = $$MACDEPLOY $$OUT_PWD/$$TARGET\.app -qmldir=$$PWD -verbose=3
}
win32: {
TARGET = ../../GrowattMonitor/GrowattMonitorUI-Windows/GrowattMonitorUI-Windows
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    main.cpp \
    mainwindow.cpp \
    powerchart.cpp \
    static.cpp

HEADERS += \
    about.h \
    mainwindow.h \
    powerchart.h \
    static.h

FORMS += \
    about.ui \
    mainwindow.ui \
    powerchart.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_ICONS = resources/icon.ico
ICON = icon.icns
