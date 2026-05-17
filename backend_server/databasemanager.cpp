#include "databasemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) db.close();
}

bool DatabaseManager::open() {
    if (!db.open()) {
        qDebug() << "[DatabaseManager] Cannot open database:" << db.lastError().text();
        return false;
    }

    // Создаём таблицу, если нет
    QSqlQuery query;
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS users ("
                         "login TEXT PRIMARY KEY,"
                         "hash TEXT NOT NULL,"
                         "role TEXT DEFAULT 'user');");
    if (!ok) {
        qDebug() << "[DatabaseManager] Failed to create table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::userExists(const QString &login) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
    query.bindValue(":login", login);
    if (!query.exec()) {
        qDebug() << "[DatabaseManager] userExists query failed:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::addUser(const QString &login, const QString &hash, const QString &role) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (login, hash, role) VALUES (:login, :hash, :role)");
    query.bindValue(":login", login);
    query.bindValue(":hash", hash);
    query.bindValue(":role", role);
    if (!query.exec()) {
        qDebug() << "[DatabaseManager] addUser failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::checkUser(const QString &login, const QString &hash) {
    QSqlQuery query;
    query.prepare("SELECT hash FROM users WHERE login = :login");
    query.bindValue(":login", login);
    if (!query.exec()) {
        qDebug() << "[DatabaseManager] checkUser failed:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        return query.value(0).toString() == hash;
    }
    return false;
}

QString DatabaseManager::getUserRole(const QString &login) {
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE login = :login");
    query.bindValue(":login", login);
    if (!query.exec()) {
        qDebug() << "[DatabaseManager] getUserRole failed:" << query.lastError().text();
        return "user";
    }
    if (query.next()) {
        return query.value(0).toString();
    }
    return "user";
}

QString DatabaseManager::getSortedUsers(const QString &sortBy) {
    QSqlQuery query;
    // sortBy = "login" или "role"
    QString order;
    if (sortBy == "role")
        order = "role, login";
    else
        order = "login";
    if (!query.exec("SELECT login, role FROM users ORDER BY " + order)) {
        qDebug() << "[DatabaseManager] getSortedUsers failed:" << query.lastError().text();
        return "";
    }
    QStringList lines;
    while (query.next()) {
        lines << query.value(0).toString() + ":" + query.value(1).toString();
    }
    return lines.join(", ");
}
