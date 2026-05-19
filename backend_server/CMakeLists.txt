cmake_minimum_required(VERSION 3.16)
project(StegoApp LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем Widgets для GUI
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Sql Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql Widgets)
find_package(PNG REQUIRED)

# Путь к заголовкам (если нужно)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Файлы сервера
set(SERVER_SOURCES
    mytcpserver.cpp
    requesthandler.cpp
    functionstoserver.cpp
    databasemanager.cpp
    func/sha384.cpp
    func/stego.cpp
)

set(SERVER_HEADERS
    mytcpserver.h
    requesthandler.h
    functionstoserver.h
    databasemanager.h
    func/sha384.h
    func/stego.h
)

# Файлы GUI клиента (на уровень выше)
set(GUI_SOURCES
    ../main.cpp
    ../mainwindow.cpp
)

set(GUI_HEADERS
    ../mainwindow.h
)

set(GUI_FORMS
    ../mainwindow.ui
)

# Создаём исполняемый файл с GUI
add_executable(StegoApp
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${GUI_FORMS}
)

# Линкуем библиотеки
target_link_libraries(StegoApp
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
    PNG::PNG
)

# Для Linux/WSL: X11 поддержка для GUI
if(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(StegoApp ${X11_LIBRARIES})
endif()

# Для Windows: копируем DLL
if(WIN32)
    add_custom_command(TARGET StegoApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/msys64/mingw64/bin/libpng16-16.dll"
            $<TARGET_FILE_DIR:StegoApp>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/msys64/mingw64/bin/zlib1.dll"
            $<TARGET_FILE_DIR:StegoApp>
    )
endif()

# Выводим информацию о сборке
message(STATUS "Building StegoApp with Qt${QT_VERSION_MAJOR}")
message(STATUS "Qt path: ${Qt${QT_VERSION_MAJOR}_DIR}")