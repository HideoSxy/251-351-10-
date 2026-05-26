#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDateTime>

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

    // Слоты для сигналов NetworkClient
    void onClientConnected();
    void onClientDisconnected();
    void onClientError(const QString &errorString);
    void onClientMessage(const QString &response);

private:
    Ui::MainWindow *ui;
    QString currentUser;
    QString currentRole;
    QString m_lastEncrypted;
    bool isAuthenticated;

    void appendToLog(const QString &text);
    void appendToRegLog(const QString &text);
    void appendToOutput(const QString &text);
    void showLoginPage();
    void showRegisterPage();
    void showMainPage();
    void updateUserInfo();
    // Таблица истории операций
    QTableWidget *m_historyTable;

    // Метод для добавления записи в историю
    void addToHistory(const QString &action, const QString &result);

    // Метод для получения текущего времени
    QString getCurrentTimestamp();
};

#endif // MAINWINDOW_H