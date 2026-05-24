QT += core gui
QT += network
QT += sql

INCLUDEPATH += C:/msys64/mingw64/include

LIBS += -LC:/msys64/mingw64/lib \
        -lpng16 \
        -lz

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    backend_server/databasemanager.cpp \
    backend_server/func/chord.cpp \
    backend_server/func/sha384.cpp \
    backend_server/func/stego.cpp \
    backend_server/functionstoserver.cpp \
    backend_server/main_server.cpp \
    backend_server/mytcpserver.cpp \
    backend_server/requesthandler.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    backend_server/databasemanager.h \
    backend_server/func/chord.h \
    backend_server/func/sha384.h \
    backend_server/func/stego.h \
    backend_server/functionstoserver.h \
    backend_server/mytcpserver.h \
    backend_server/requesthandler.h \
    database.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    backend_server/.gitignore \
    backend_server/CMakeLists.txt \
    backend_server/Dockerfile
