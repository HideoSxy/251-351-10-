QT += core gui network sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Подключение libpng для Windows
win32: {
    # Путь к libpng в MSYS2 (измените если у вас другой путь)
    INCLUDEPATH += C:/msys64/mingw64/include/libpng16
    INCLUDEPATH += C:/msys64/mingw64/include

    LIBS += -LC:/msys64/mingw64/lib -lpng16 -lz
}

# Подключение libpng для Linux
unix: {
    LIBS += -lpng -lz
}

# Подключение libpng для macOS
macx: {
    LIBS += -lpng -lz
}

SOURCES += \
    backend_server/databasemanager.cpp \
    backend_server/func/rsa_wrapper.cpp \
    backend_server/func/sha384.cpp \
    backend_server/func/stego.cpp \
    backend_server/functionstoserver.cpp \
    backend_server/mytcpserver.cpp \
    backend_server/network_client.cpp \
    backend_server/requesthandler.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    backend_server/databasemanager.h \
    backend_server/func/rsa_wrapper.h \
    backend_server/func/sha384.h \
    backend_server/func/stego.h \
    backend_server/functionstoserver.h \
    backend_server/mytcpserver.h \
    backend_server/network_client.h \
    backend_server/requesthandler.h \
    mainwindow.h

FORMS += \
    mainwindow.ui