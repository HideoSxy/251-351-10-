cmake_minimum_required(VERSION 3.16)
project(backend_server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Sql Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Sql Test)
find_package(PNG REQUIRED)

add_executable(main_server
    main_server.cpp
    mytcpserver.h           mytcpserver.cpp
    requesthandler.h        requesthandler.cpp
    functionstoserver.h     functionstoserver.cpp
    databasemanager.h       databasemanager.cpp
    func/sha384.h           func/sha384.cpp
    func/stego.h            func/stego.cpp
    func/chord.h            func/chord.cpp
    func/genetic_path.h     func/genetic_path.cpp
)

target_link_libraries(main_server
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    PNG::PNG  # подключаем libpng
)

add_executable(unit_tests
    tests/tests_simple.cpp
    functionstoserver.cpp
    databasemanager.cpp
    func/sha384.cpp
    func/chord.cpp
    func/genetic_path.cpp
)

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(unit_tests
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Test
)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)

