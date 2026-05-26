#include "network_client.h"
#include "func/rsa_wrapper.h"
#include <QDebug>

static RSAWrapper s_clientRsa;

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &NetworkClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkClient::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);

    // Инициализируем RSA
    s_clientRsa.loadFixedKeys();
    qDebug() << "[NetworkClient] RSA initialized";
}

NetworkClient::~NetworkClient()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

NetworkClient& NetworkClient::instance()
{
    static NetworkClient instance;
    return instance;
}

void NetworkClient::connectToServer(const QString &host, quint16 port)
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        qDebug() << "[NetworkClient] Already connected";
        return;
    }
    qDebug() << "[NetworkClient] Connecting to" << host << ":" << port;
    m_socket->connectToHost(host, port);
}

void NetworkClient::sendCommand(const QString &command)
{
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        emit messageReceived("ERR: Нет подключения к серверу");
        return;
    }

    QString cmd = command.trimmed();
    if (!cmd.endsWith('\n')) {
        cmd += '\n';
    }
    qDebug() << "[NetworkClient] Sending:" << cmd;
    m_socket->write(cmd.toUtf8());
    m_socket->flush();
}

bool NetworkClient::isConnected() const
{
    return m_socket->state() == QTcpSocket::ConnectedState;
}

void NetworkClient::disconnect()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

void NetworkClient::onConnected()
{
    qDebug() << "[NetworkClient] Connected to server";
    emit connected();
}

void NetworkClient::onDisconnected()
{
    qDebug() << "[NetworkClient] Disconnected from server";
    emit disconnected();
}

void NetworkClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString err = m_socket->errorString();
    qDebug() << "[NetworkClient] Error:" << err;
    emit errorOccurred(err);
}

void NetworkClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx + 1);
        m_buffer.remove(0, idx + 1);

        QString response = QString::fromUtf8(line).trimmed();
        qDebug() << "[NetworkClient] Received:" << response;

        emit messageReceived(response);
    }
}

// ========== ЗАШИФРОВАННЫЕ МЕТОДЫ ДЛЯ АВТОРИЗАЦИИ И РЕГИСТРАЦИИ ==========

void NetworkClient::sendEncryptedAuth(const QString &login, const QString &password)
{
    QString encryptedPassword = s_clientRsa.encrypt(password);
//    qDebug() << "[NetworkClient] Original password:" << password;
//    qDebug() << "[NetworkClient] Encrypted password:" << encryptedPassword;
    sendCommand(QString("auth&%1,%2").arg(login, encryptedPassword));
}

void NetworkClient::sendEncryptedRegister(const QString &login, const QString &password)
{
    QString encryptedPassword = s_clientRsa.encrypt(password);
//    qDebug() << "[NetworkClient] Original password:" << password;
//    qDebug() << "[NetworkClient] Encrypted password:" << encryptedPassword;
    sendCommand(QString("reg&%1,%2").arg(login, encryptedPassword));
}