#include "mytcpserver.h"
#include "requesthandler.h"
#include "databasemanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>

MyTcpServer::~MyTcpServer() {
    mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent) {
    // TODO: START DB
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qCritical() << "[Server] Failed to start on port 33333";
    } else {
        qDebug() << "[Server] Started on port 33333";
    }
}

void MyTcpServer::slotNewConnection() {
    while (mTcpServer->hasPendingConnections()) {
        QTcpSocket *socket = mTcpServer->nextPendingConnection();

        // Клиент. Пустой буфер и пустая роль
        mBuffers[socket] = QByteArray();
        mRoles[socket]   = "";

        connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
        connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

        socket->write("CONNECTED: StegoApp Server ready. Use reg& or auth& to start.\r\n");

        qDebug() << "[Server] New client:" << socket->peerAddress().toString() << socket->peerPort();
    }

}

void MyTcpServer::slotServerRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    mBuffers[socket] += socket->readAll();

    int idx;
    while ((idx = mBuffers[socket].indexOf('\n')) != -1) {
        QByteArray lineBytes = mBuffers[socket].left(idx);
        mBuffers[socket].remove(0, idx + 1);

        QString command = QString::fromUtf8(lineBytes).trimmed();
        if (command.isEmpty()) continue;

        qDebug() << "[Server] From" << socket->peerPort() << ":" << command.left(80);

        QString response = RequestHandler::handle(command, mRoles[socket]);

        // Запись роли определенного сокета
        if (response.startsWith("AUTH_OK:")) {
            QString role = response.mid(9).trimmed();
            mRoles[socket] = role.split('\r').first().split('\n').first().trimmed();
            qDebug() << "[Server] Client" << socket->peerPort()
                     << "authenticated as:" << mRoles[socket];
        }

        socket->write(response.toUtf8());
        socket->flush();
    }

}

void MyTcpServer::slotClientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    qDebug() << "[Server] Disconnected:" << socket->peerPort()
             << "(was role: " << mRoles[socket] << ")";

    mBuffers.remove(socket);
    mRoles.remove(socket);
    socket->deleteLater();
}
