#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include "backend_server/func/stego.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
    , isAuthenticated(false)
{
    ui->setupUi(this);

    // Показываем страницу входа
    showLoginPage();

    // Подключаем сигналы сокета
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);

    // Подключаемся к серверу
    connectToServer();
}

MainWindow::~MainWindow()
{
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
    delete ui;
}

void MainWindow::connectToServer()
{
    appendToLog("Подключение к серверу 127.0.0.1:33333...");
    socket->connectToHost("127.0.0.1", 33333);
}

void MainWindow::sendCommand(const QString &command)
{
    if (socket->state() != QTcpSocket::ConnectedState) {
        appendToOutput("Ошибка: Нет подключения к серверу");
        return;
    }
    QString cmd = command.trimmed();
    if (!cmd.endsWith('\n')) {
        cmd += '\n';
    }
    qDebug() << "[sendCommand] Sending:" << cmd;
    socket->write(cmd.toUtf8());
    socket->flush();
}

void MainWindow::appendToLog(const QString &text)
{
    ui->textEditLog->append(text);
}

void MainWindow::appendToRegLog(const QString &text)
{
    ui->textEditRegLog->append(text);
}

void MainWindow::appendToOutput(const QString &text)
{
    ui->textEditOutput->append(text);
}

void MainWindow::showLoginPage()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEditLogin->clear();
    ui->lineEditPassword->clear();
    ui->textEditLog->clear();
}

void MainWindow::showRegisterPage()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->lineEditRegLogin->clear();
    ui->lineEditRegPassword->clear();
    ui->lineEditRegConfirm->clear();
    ui->textEditRegLog->clear();
}

void MainWindow::showMainPage()
{
    ui->stackedWidget->setCurrentIndex(2);
    updateUserInfo();
}

void MainWindow::updateUserInfo()
{
    ui->labelCurrentUser->setText(QString("%1 (%2)").arg(currentUser, currentRole));

    // Админ-панель доступна только админам
    if (currentRole == "admin") {
        ui->tabAdmin->setEnabled(true);
    } else {
        ui->tabAdmin->setEnabled(false);
    }
}

// ==================== СЛОТЫ СОКЕТА ====================

void MainWindow::onConnected()
{
    appendToLog("Подключено к серверу!");
    ui->labelConnectionStatus->setText("Подключено к серверу");
    ui->labelConnectionStatus->setStyleSheet("color: green; font-size: 10pt;");
}

void MainWindow::onDisconnected()
{
    isAuthenticated = false;
    appendToLog("Отключено от сервера");
    ui->labelConnectionStatus->setText("Отключено от сервера");
    ui->labelConnectionStatus->setStyleSheet("color: #666666; font-size: 10pt;");
    showLoginPage();
}

void MainWindow::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    appendToLog("Ошибка: " + socket->errorString());
}

void MainWindow::onReadyRead()
{
    buffer.append(socket->readAll());

    while (buffer.contains('\n')) {
        int idx = buffer.indexOf('\n');
        QByteArray line = buffer.left(idx + 1);
        buffer.remove(0, idx + 1);

        QString response = QString::fromUtf8(line).trimmed();

        qDebug() << "[onReadyRead] Response:" << response;

        // Обработка ответов сервера
        if (response.startsWith("CONNECTED:")) {
            appendToLog(response);
        }
        else if (response.startsWith("REGISTER_OK:")) {
            appendToRegLog(response);
            QMessageBox::information(this, "Регистрация", "Регистрация прошла успешно! Теперь вы можете войти.");
            showLoginPage();
        }
        else if (response.startsWith("REGISTER_ERR:")) {
            appendToRegLog(response);
            QMessageBox::warning(this, "Ошибка регистрации", response);
        }
        else if (response.startsWith("AUTH_OK:")) {
            isAuthenticated = true;

            // Извлекаем логин и роль
            QString authResponse = response.mid(8).trimmed();
            int loginEnd = authResponse.indexOf(" logged in successfully");
            if (loginEnd != -1) {
                currentUser = authResponse.left(loginEnd);
            }

            // Извлекаем роль
            int roleStart = authResponse.lastIndexOf('(');
            int roleEnd = authResponse.lastIndexOf(')');
            if (roleStart != -1 && roleEnd != -1) {
                currentRole = authResponse.mid(roleStart + 1, roleEnd - roleStart - 1);
            }

            appendToLog(response);
            appendToOutput("Авторизация успешна. Роль: " + currentRole);
            showMainPage();
        }
        else if (response.startsWith("AUTH_ERR:")) {
            appendToLog(response);
            QMessageBox::warning(this, "Ошибка авторизации", response);
        }
        else if (response.startsWith("SHA384_OK:")) {
            QString hash = response.mid(10).trimmed();
            ui->textEditShaOutput->setText(hash);
            appendToOutput(response);
        }
        else if (response.startsWith("SHA384_ERR:")) {
            appendToOutput(response);
            QMessageBox::warning(this, "Ошибка SHA-384", response);
        }
        else if (response.startsWith("EMBED_OK:")) {
            appendToOutput(response);
            QMessageBox::information(this, "Стеганография", "Сообщение успешно встроено в изображение!");
        }
        else if (response.startsWith("EMBED_ERR:")) {
            appendToOutput(response);
            QMessageBox::warning(this, "Ошибка встраивания", response);
        }
        else if (response.startsWith("EXTRACT_OK:")) {
            QString message = response.mid(11).trimmed();
            ui->textEditExtracted->setText(message);
            appendToOutput("Извлечено сообщение: " + message);
            QMessageBox::information(this, "Стеганография", "Сообщение успешно извлечено!");
        }
        else if (response.startsWith("EXTRACT_ERR:")) {
            appendToOutput(response);
            QMessageBox::warning(this, "Ошибка извлечения", response);
        }
        else if (response.startsWith("SORT_OK:")) {
            QString users = response.mid(8).trimmed();
            QStringList userList = users.split(", ");
            ui->textEditUsersList->setText(userList.join("\n"));
            appendToOutput(response);
        }
        else if (response.startsWith("SORT_ERR:")) {
            appendToOutput(response);
            QMessageBox::warning(this, "Ошибка", response);
        }
        else if (response.startsWith("ERR:")) {
            appendToOutput(response);
        }
        else {
            appendToOutput(response);
        }
    }
}

// ==================== КНОПКИ ОКНА ВХОДА ====================

void MainWindow::on_btnLogin_clicked()
{
    QString login = ui->lineEditLogin->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль");
        return;
    }

    sendCommand(QString("auth&%1,%2").arg(login, password));
}

void MainWindow::on_btnRegister_clicked()
{
    showRegisterPage();
}

void MainWindow::on_btnBackToLogin_clicked()
{
    showLoginPage();
}

void MainWindow::on_btnSubmitRegister_clicked()
{
    QString login = ui->lineEditRegLogin->text().trimmed();
    QString password = ui->lineEditRegPassword->text().trimmed();
    QString confirm = ui->lineEditRegConfirm->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    if (password != confirm) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    if (password.length() < 4) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен быть не менее 4 символов");
        return;
    }

    sendCommand(QString("reg&%1,%2").arg(login, password));
}

// ==================== КНОПКИ ГЛАВНОГО ОКНА ====================

void MainWindow::on_btnSha384_clicked()
{
    if (!isAuthenticated) {
        QMessageBox::warning(this, "Ошибка", "Сначала авторизуйтесь");
        showLoginPage();
        return;
    }

    QString text = ui->textEditShaInput->toPlainText().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для хэширования");
        return;
    }

    sendCommand(QString("sha384&%1").arg(text));
}

void MainWindow::on_btnEmbed_clicked()
{
    if (!isAuthenticated) {
        QMessageBox::warning(this, "Ошибка", "Сначала авторизуйтесь");
        showLoginPage();
        return;
    }

    QString imageIn = ui->lineEditImageIn->text().trimmed();
    QString imageOut = ui->lineEditImageOut->text().trimmed();
    QString message = ui->textEditMessage->toPlainText().trimmed();

    qDebug() << "[Embed] Image in:" << imageIn;
    qDebug() << "[Embed] Image out:" << imageOut;
    qDebug() << "[Embed] Message:" << message;

    // Проверяем существование входного файла
    QFileInfo checkFile(imageIn);
    if (!checkFile.exists()) {
        QMessageBox::warning(this, "Ошибка", "Входной файл не существует: " + imageIn);
        return;
    }

    if (imageIn.isEmpty() || imageOut.isEmpty() || message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    // Отправляем команду на сервер
    sendCommand(QString("embed&%1,%2,%3").arg(imageIn, imageOut, message));
    appendToOutput("Отправлен запрос на встраивание сообщения...");
}

void MainWindow::on_btnExtract_clicked()
{
    if (!isAuthenticated) {
        QMessageBox::warning(this, "Ошибка", "Сначала авторизуйтесь");
        showLoginPage();
        return;
    }

    QString imagePath = ui->lineEditExtractImage->text().trimmed();

    qDebug() << "[Extract] Image path:" << imagePath;

    if (imagePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите PNG файл");
        return;
    }

    // Проверяем существование файла
    QFileInfo checkFile(imagePath);
    if (!checkFile.exists()) {
        QMessageBox::warning(this, "Ошибка", "Файл не существует: " + imagePath);
        return;
    }



    QString message = fn_extract(imagePath);  // Вызываем функцию из stego.cpp

    if (message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось извлечь сообщение или сообщение не найдено");
        appendToOutput("Ошибка извлечения: сообщение не найдено");
    } else {
        ui->textEditExtracted->setText(message);
        appendToOutput("Извлечено сообщение: " + message);
        QMessageBox::information(this, "Стеганография", "Сообщение успешно извлечено!");
    }
}

void MainWindow::on_btnListUsers_clicked()
{
    if (!isAuthenticated) {
        QMessageBox::warning(this, "Ошибка", "Сначала авторизуйтесь");
        showLoginPage();
        return;
    }

    if (currentRole != "admin") {
        QMessageBox::warning(this, "Ошибка", "Доступно только для администраторов");
        return;
    }

    QString sortBy = ui->comboSortBy->currentText();
    sendCommand(QString("sort&%1").arg(sortBy));
}

void MainWindow::on_btnLogout_clicked()
{
    isAuthenticated = false;
    currentUser.clear();
    currentRole.clear();
    showLoginPage();
    appendToOutput("Вы вышли из системы");
}

// ==================== ОБЗОР ФАЙЛОВ ====================

void MainWindow::on_btnBrowseIn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите PNG файл", "", "PNG Images (*.png)");
    if (!fileName.isEmpty()) {
        ui->lineEditImageIn->setText(fileName);
        qDebug() << "[BrowseIn] Selected file:" << fileName;
    }
}

void MainWindow::on_btnBrowseOut_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить PNG как", "", "PNG Images (*.png)");
    if (!fileName.isEmpty()) {
        ui->lineEditImageOut->setText(fileName);
        qDebug() << "[BrowseOut] Selected file:" << fileName;
    }
}

void MainWindow::on_btnBrowseExtract_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите PNG файл с сообщением", "", "PNG Images (*.png)");
    if (!fileName.isEmpty()) {
        ui->lineEditExtractImage->setText(fileName);
        qDebug() << "[BrowseExtract] Selected file:" << fileName;
    }
}