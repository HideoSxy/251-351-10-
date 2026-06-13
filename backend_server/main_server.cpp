/**
 * @file main_server.cpp
 * @brief Точка входа: БД + TCP-сервер на порту 33333.
 */

#include <QCoreApplication>
#include "mytcpserver.h"
#include "databasemanager.h"

/**
 * @brief Запуск StegoApp server.
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (!DatabaseManager::instance().open()) {
        return -1;
    }

    MyTcpServer myserv;

    return a.exec();
}