#include "mainwindow.h"
#include "backend_server/mytcpserver.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "=== Starting StegoApp ===";

    QApplication a(argc, argv);
    qDebug() << "Working directory:" << QDir::currentPath();

    // Запускаем сервер
    MyTcpServer server;
    qDebug() << "Server started on port 33333";

    // Запускаем GUI
    MainWindow w;
    w.show();

    qDebug() << "GUI shown";

    return a.exec();
}