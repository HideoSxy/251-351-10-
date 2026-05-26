#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    // Запрещаем копирование и присваивание
    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;

    // Глобальная точка доступа к синглтону
    static NetworkClient& instance();

    // Публичные методы
    void connectToServer(const QString &host = "127.0.0.1", quint16 port = 33333);
    void sendCommand(const QString &command);
    bool isConnected() const;
    void disconnect();

    // Зашифрованные методы для авторизации и регистрации
    void sendEncryptedAuth(const QString &login, const QString &password);
    void sendEncryptedRegister(const QString &login, const QString &password);

signals:
    // Сигналы для оповещения GUI
    void connected();
    void disconnected();
    void errorOccurred(const QString &errorString);
    void messageReceived(const QString &response);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    QTcpSocket *m_socket;
    QByteArray m_buffer;
};

#endif // NETWORK_CLIENT_H