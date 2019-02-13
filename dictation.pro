!equals(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 11) {
    error("Qt version 5.11 or later is required")
}

QT += widgets multimedia
CONFIG += c++11
INCLUDEPATH += $$PWD

UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
RCC_DIR = rccs

win32 {
    # Enabled by default on Windows
    CONFIG -= debug_and_release debug_and_release_target
    # Stop console from popping up on Windows if QT += testlib is used
    QT.testlib.CONFIG -= console

    RC_ICONS = images/app-icon.ico

    # NOTE: A static version of Qt is required to build a standalone application
    CONFIG += static
    static {
        DEFINES += STANDALONE
        HEADERS += standalone.h

        QTDIR = $$(QTDIR)
        isEmpty(QTDIR)): error("Environment variable `QTDIR` is required and not set.")
        QT -= multimedia
        MM = $$QTDIR/../qtmultimedia
        INCLUDEPATH += $$MM/include/QtMultimedia $$MM/include
        LIBS += -L$$MM/lib -lQt5Multimedia
        LIBS += -L$$MM/plugins/mediaservice -ldsengine -lwmfengine
    }
}

# CONFIG += debug
CONFIG(debug, debug|release) {
} else {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

TEMPLATE = app
RESOURCES += dictation.qrc
HEADERS += \
    dictation.h \
    utils/debug.h \
    utils/color.h \
    utils/md5.h \
    customwidgets/button.h \
    customwidgets/tooltippedslider.h \
    audioplayer/audioplayer.h \
    speechsegmenter/speechsegmenter.h \
    controlpanel/controlpanel.h \
    controlpanel/trackinfo.h \
    controlpanel/controlbuttonset.h \
    controlpanel/progressslider.h \
    controlpanel/volumecontrol.h \
    segmentpanel/subrip.h \
    segmentpanel/textedit.h \
    segmentpanel/segmentedit.h \
    segmentpanel/segmentpanel.h

SOURCES += \
    main.cpp \
    dictation.cpp \
    utils/color.cpp \
    utils/md5.cpp \
    customwidgets/button.cpp \
    customwidgets/tooltippedslider.cpp \
    audioplayer/audioplayer.cpp \
    speechsegmenter/speechsegmenter.cpp \
    controlpanel/controlpanel.cpp \
    controlpanel/trackinfo.cpp \
    controlpanel/controlbuttonset.cpp \
    controlpanel/progressslider.cpp \
    controlpanel/volumecontrol.cpp \
    segmentpanel/subrip.cpp \
    segmentpanel/textedit.cpp \
    segmentpanel/segmentedit.cpp \
    segmentpanel/segmentpanel.cpp

TARGET = Dictation
QMAKE_CLEAN += $${TARGET}

MAJOR = 1
MINOR = 0
PATCH = 0
BUILD = 0
DEFINES += __APP_VERSION_MAJOR__=$$MAJOR
DEFINES += __APP_VERSION_MINOR__=$$MINOR
win32: VERSION = $$sprintf("%1.%2.%3.%4", $$MAJOR, $$MINOR, $$PATCH, $$BUILD)

DEFINES += AUDIOPLAYER_DEBUG
DEFINES += SPEECHSEGMENTER_DEBUG
DEFINES += DICTATION_DEBUG
DEFINES += CONTROLPANEL_DEBUG
DEFINES += CONTROLBUTTONSET_DEBUG
DEFINES += SEGMENTPANEL_DEBUG
DEFINES += SEGMENTEDIT_DEBUG
DEFINES += SUBRIP_DEBUG
DEFINES += VOLUMECONTROL_DEBUG
DEFINES += PROGRESSSLIDER_DEBUG
DEFINES += TRACKINFO_DEBUG
DEFINES += TOOLTIPPEDSLIDER_DEBUG
DEFINES += BUTTON_DEBUG
DEFINES += ROUNDBUTTON_DEBUG
DEFINES += MAIN_DEBUG
