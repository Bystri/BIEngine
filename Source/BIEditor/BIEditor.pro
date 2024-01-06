TEMPLATE = app
TARGET = BIEditor

QT = core gui widgets

INCLUDEPATH += $$PWD/../BIEngine/3rdParty/luaplus51-all/Src
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/glm
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/tinyxml2
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/zlib
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/glfw/include
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/glad/include
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/irrKlang/include
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/libogg-1.3.0/include
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/libvorbis-1.3.2/include
INCLUDEPATH += $$PWD/../BIEngine/3rdParty/freetype-2.10.0/include

LIBS += -L"$$PWD/../Lib/BIEnginex64Debug" -lBIEngine

SOURCES += \
    BIEditor.cpp \
    BIEditorController.cpp \
    EditWidgets/QColorPicker.cpp \
    EditWidgets/QFilePicker.cpp \
    EditWidgets/QRealTimeLineEdit.cpp \
    QActorComponentEditor.cpp \
    QBIEditor.cpp \
    QDisplayWidget.cpp \
    main.cpp

HEADERS += \
    BIEditor.h \
    BIEditorController.h \
    EditWidgets/QColorPicker.h \
    EditWidgets/QFilePicker.h \
    EditWidgets/QRealTimeLineEdit.h \
    QActorComponentEditor.h \
    QBIEditor.h \
    QDisplayWidget.h

DISTFILES +=
