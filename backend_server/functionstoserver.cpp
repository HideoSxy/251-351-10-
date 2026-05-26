#include "functionstoserver.h"
#include "databasemanager.h"
#include "func/sha384.h"
#include <QStringList>
#include <QDebug>
#include "func/rsa_wrapper.h"

static RSAWrapper s_rsa;
static bool s_rsaInitialized = false;

// Регистрация
QString fn_register(const QString &payload) {
    QStringList parts = payload.split(',', Qt::KeepEmptyParts);
    if (parts.size() != 2)
        return "REGISTER_ERR: Invalid format. Use: reg&login,password\r\n";

    QString login = parts[0].trimmed();
    QString encryptedPass = parts[1].trimmed();

    if (login.isEmpty() || encryptedPass.isEmpty())
        return "REGISTER_ERR: Login or password cannot be empty\r\n";

    if (!s_rsaInitialized) {
        s_rsa.loadFixedKeys();
        s_rsaInitialized = true;
    }

    QString pass = s_rsa.decrypt(encryptedPass);
    if (pass.isEmpty()) {
        return "REGISTER_ERR: Failed to decrypt password\r\n";
    }

    QString hash = func_sha384(pass);

    DatabaseManager &db = DatabaseManager::instance();

    if (!db.open()) {
        return "REGISTER_ERR: Cannot open database\r\n";
    }

    if (db.userExists(login)) {
        return "REGISTER_ERR: User already exists\r\n";
    }

    QString role = login.toLower().contains("admin") ? "admin" : "user";
    if (!db.addUser(login, hash, role)) {
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
    QString encryptedPass = parts[1].trimmed();

    if (login.isEmpty() || encryptedPass.isEmpty())
        return "AUTH_ERR: Login or password cannot be empty\r\n";

    if (!s_rsaInitialized) {
        s_rsa.loadFixedKeys();
        s_rsaInitialized = true;
    }

    QString pass = s_rsa.decrypt(encryptedPass);
    if (pass.isEmpty()) {
        return "AUTH_ERR: Failed to decrypt password\r\n";
    }

    QString hash = func_sha384(pass);

    DatabaseManager &db = DatabaseManager::instance();

    if (!db.open()) {
        return "AUTH_ERR: Cannot open database\r\n";
    }

    if (!db.checkUser(login, hash)) {
        return "AUTH_ERR: Invalid login or password\r\n";
    }

    QString role = db.getUserRole(login);

    return "AUTH_OK: " + login + " logged in successfully (" + role + ")\r\n";
}

// SHA-384
QString fn_sha384(const QString &payload) {
    if (payload.trimmed().isEmpty())
        return "SHA384_ERR: No input text\r\n";

    return "SHA384_OK: " + func_sha384(payload) + "\r\n";
}

QString fn_chord(const QString &payload)
{
    qDebug() << "[fn_chord] STUB called";
    return "CHORD_ERR: Not implemented yet\r\n";
}

QString fn_list_users_sorted(const QString &payload) {
    QString sortBy = payload.trimmed().toLower();
    if (sortBy != "login" && sortBy != "role")
        return "SORT_ERR: Invalid sort parameter. Use: sort&login or sort&role\r\n";
    QString result = DatabaseManager::instance().getSortedUsers(sortBy);
    if (result.isEmpty())
        return "SORT_ERR: No users found or database error\r\n";
    return "SORT_OK: " + result + "\r\n";
}