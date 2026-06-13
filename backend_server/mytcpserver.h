/**
 * @file mytcpserver.h
 * @brief TCP-сервер на порту 33333, построчные команды.
 */

#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>

#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QString>

/**
 * @brief Принимает клиентов, буферизует строки до \\n, вызывает RequestHandler.
 */
class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    
    ~MyTcpServer();
public slots:
    /** @brief Новое подключение, приветствие CONNECTED. */
    void slotNewConnection();

    /** @brief Клиент отключился — сброс буфера и роли. */
    void slotClientDisconnected();

    /** @brief Чтение сокета, разбор команд, ответ клиенту. */
    void slotServerRead();

private:
    QTcpServer *mTcpServer;
    QSqlDatabase db;
    QHash<QTcpSocket*, QByteArray> mBuffers;
    QHash<QTcpSocket*, QString> mRoles;
};
#endif
