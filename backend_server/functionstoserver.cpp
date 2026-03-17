#include "functionstoserver.h"

#include <QTcpSocket>          // связь с БД
#include <QCryptographicHash>  // SHA-384
#include <QStringList>
#include <QDebug>
#include <cmath>

// TODO:
// class DbClient {
    // Синглтон
    // SQL query
// };

// SHA-384 строки (to hex)
static QString sha384Hex(const QString& text) {
    QByteArray hash = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha384);
    return QString(hash.toHex());
}

// Регистрация
QString fn_register(const QString &payload) {
    QStringList parts = payload.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2)
        return "REGISTER_ERR: Invalid format. Use: reg&login,password\r\n";

    QString login = parts[0].trimmed();
    QString pass = parts[1].trimmed();

    if (login.isEmpty() || pass.isEmpty())
        return "REGISTER_ERR: Login or password cannot be empty\r\n";

    QString hash = sha384Hex(pass); // Хэшируем пароль алгоритмом SHA-384

    return "REGISTER_WITHOUT_DB: " + login + " successfully registered\r\n";

    // qDebug() << "[fn_register] STUB called with:" << payload;
    // return "REGISTER_ERR: Not implemented yet (stub)\r\n";

    // TODO: Проверка существует ли пользователь в БД
    // TODO: Добавляем пользователя в БД
}

// Авторизация
QString fn_auth(const QString &payload) {
    QStringList parts = payload.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2)
        return "AUTH_ERR: Invalid format. Use: auth&login,password\r\n";

    QString login = parts[0].trimmed();
    QString pass = parts[1].trimmed();

    if (login.isEmpty() || pass.isEmpty())
        return "AUTH_ERR: Login or password cannot be empty\r\n";

    QString hash = sha384Hex(pass);

    return "AUTH_WITHOUT_DB: " + login + " logged in successfully\r\n";

    // qDebug() << "[fn_auth] STUB called with:" << payload;
    // return "AUTH_ERR: Not implemented yet (stub)\r\n";

    // TODO: Запрос пользователя с БД с такими данными (проверка)
}

// SHA-384
QString fn_sha384(const QString &payload) {
    if (payload.trimmed().isEmpty())
        return "SHA384_ERR: No input text\r\n";

    return "SHA384_OK: " + sha384Hex(payload) + "\r\n";
}

// Проверка RSA на малых числах (?)
QString fn_rsa_gen() {
    // TODO: Учебный RSA
    // payload = "d,n,числа через пробел"
    qDebug() << "[fn_rsa_gen] STUB called";
    return "RSA_DEC_ERR: Not implemented yet (stub)\r\n";
}

QString fn_rsa_encrypt(const QString &payload) {
    // TODO: Шифрование rsa
    // payload = "e,n,текст"
    qDebug() << "[fn_rsa_encrypt] STUB called with:" << payload;
    return "RSA_ENC_ERR: Not implemented yet (stub)\r\n";
}

QString fn_rsa_decrypt(const QString &payload) {
    // TODO: Дешифрование rsa
    // payload = "d,n,числа через пробел"
    qDebug() << "[fn_rsa_decrypt] STUB called with:" << payload;
    return "RSA_DEC_ERR: Not implemented yet (stub)\r\n";
}

QString fn_chord(const QString &payload) {
    // TODO: Реализовать метод хорд
    // payload = "a,b,eps"
    qDebug() << "[fn_chord] STUB called with:" << payload;
    return "CHORD_ERR: Not implemented yet (stub)\r\n";
}

QString fn_embed(const QString &payload) {
    // TODO: Реализовать LSB-стеганографию
    // payload = "login,image_path,message"
    qDebug() << "[fn_embed] STUB called with:" << payload;
    return "EMBED_ERR: Not implemented yet (stub)\r\n";
}

QString fn_extract(const QString &payload) {
    // TODO: Реализовать извлечение LSB-стеганографии
    // payload = "image_path"
    qDebug() << "[fn_extract] STUB called with:" << payload;
    return "EXTRACT_ERR: Not implemented yet (stub)\r\n";
}
