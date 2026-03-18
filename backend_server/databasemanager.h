#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool open();                      // открыть БД
    bool userExists(const QString &login);
    bool addUser(const QString &login, const QString &hash);
    bool checkUser(const QString &login, const QString &hash);
    QString getUserRole(const QString &login);

private:
    DatabaseManager();                // приватный конструктор
    ~DatabaseManager();

    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H