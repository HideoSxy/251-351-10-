#include "functionstoserver.h"
#include "databasemanager.h"

#include <QStringList>
#include <QCryptographicHash>
#include <QDebug>

// Хэширование SHA-384
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

    QString hash = sha384Hex(pass);

    DatabaseManager &db = DatabaseManager::instance();

    if (!db.open()) {
        return "REGISTER_ERR: Cannot open database\r\n";
    }

    if (db.userExists(login)) {
        return "REGISTER_ERR: User already exists\r\n";
    }

    if (!db.addUser(login, hash)) {
        return "REGISTER_ERR: Failed to insert user into database\r\n";
    }

    return "REGISTER_OK: " + login + " successfully registered\r\n";
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

    DatabaseManager &db = DatabaseManager::instance();

    if (!db.open()) {
        return "AUTH_ERR: Cannot open database\r\n";
    }

    if (!db.checkUser(login, hash)) {
        return "AUTH_ERR: Invalid login or password\r\n";
    }

    QString role = db.getUserRole(login); // например, "user" или "admin"

    return "AUTH_OK: " + login + " logged in successfully (" + role + ")\r\n";
}

// SHA-384
QString fn_sha384(const QString &payload) {
    if (payload.trimmed().isEmpty())
        return "SHA384_ERR: No input text\r\n";

    return "SHA384_OK: " + sha384Hex(payload) + "\r\n";
}

// Заглушки для остальных функций
QString fn_rsa_gen() { qDebug() << "[fn_rsa_gen] STUB called"; return "RSA_DEC_ERR: Not implemented yet\r\n"; }
QString fn_rsa_encrypt(const QString &payload) { qDebug() << "[fn_rsa_encrypt] STUB called"; return "RSA_ENC_ERR: Not implemented yet\r\n"; }
QString fn_rsa_decrypt(const QString &payload) { qDebug() << "[fn_rsa_decrypt] STUB called"; return "RSA_DEC_ERR: Not implemented yet\r\n"; }
QString fn_chord(const QString &payload) { qDebug() << "[fn_chord] STUB called"; return "CHORD_ERR: Not implemented yet\r\n"; }
QString fn_embed(const QString &payload) { qDebug() << "[fn_embed] STUB called"; return "EMBED_ERR: Not implemented yet\r\n"; }
QString fn_extract(const QString &payload) { qDebug() << "[fn_extract] STUB called"; return "EXTRACT_ERR: Not implemented yet\r\n"; }