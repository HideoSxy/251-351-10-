#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Сокет
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

    // Кнопки окна входа
    void on_btnLogin_clicked();
    void on_btnRegister_clicked();
    void on_btnBackToLogin_clicked();
    void on_btnSubmitRegister_clicked();

    // Кнопки главного окна
    void on_btnSha384_clicked();
    void on_btnEmbed_clicked();
    void on_btnExtract_clicked();
    void on_btnListUsers_clicked();
    void on_btnLogout_clicked();

    // Обзор файлов
    void on_btnBrowseIn_clicked();
    void on_btnBrowseOut_clicked();
    void on_btnBrowseExtract_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray buffer;
    QString currentUser;
    QString currentRole;
    bool isAuthenticated;

    void connectToServer();
    void sendCommand(const QString &command);
    void appendToLog(const QString &text);
    void appendToRegLog(const QString &text);
    void appendToOutput(const QString &text);
    void showLoginPage();
    void showRegisterPage();
    void showMainPage();
    void updateUserInfo();
};

#endif