#include "mytcpserver.h"
#include "requesthandler.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>

MyTcpServer::~MyTcpServer() {
    mTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent) {
    // NEXT: START DB
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qCritical() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection() {
    while (mTcpServer->hasPendingConnections()) {
        QTcpSocket *socket = mTcpServer->nextPendingConnection();
        mBuffers[socket] = QByteArray();
        connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
        connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

        socket->write("CONNECTED: StegoApp Server ready!\r\n");

        qDebug() << "[Server] New client:"
                 << socket->peerAddress().toString()
                 << ":" << socket->peerPort();
    }

}

void MyTcpServer::slotServerRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    mBuffers[socket] += socket->readAll();

    int newlineIdx;
    while ((newlineIdx = mBuffers[socket].indexOf('\n')) != -1) {
        QByteArray lineBytes = mBuffers[socket].left(newlineIdx);
        mBuffers[socket].remove(0, newlineIdx + 1);

        QString command = QString::fromUtf8(lineBytes).trimmed();
        if (command.isEmpty()) continue;

        qDebug() << "[Server] Got command:" << command;

        QString response = RequestHandler::handle(command);
        socket->write(response.toUtf8());
        socket->flush();
    }

}

void MyTcpServer::slotClientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    qDebug() << "[Server] Client disconnected:"
             << socket->peerAddress().toString();

    mBuffers.remove(socket);
    socket->deleteLater();
}
