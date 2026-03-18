#include <QCoreApplication>
#include "mytcpserver.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (!DatabaseManager::instance().open()) {
        return -1;
    }

    MyTcpServer myserv;

    return a.exec();
}