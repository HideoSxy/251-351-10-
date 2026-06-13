/**
 * @file databasemanager.h
 * @brief SQLite users.db — логины, хэши паролей, роли.
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QString>

/**
 * @brief Singleton для таблицы users (login, hash, role).
 */
class DatabaseManager {
public:
    /** @return единственный экземпляр */
    static DatabaseManager& instance();

    /** @brief Открывает users.db, создаёт таблицу при первом запуске. */
    bool open();

    /** @param login логин для проверки */
    bool userExists(const QString &login);

    /**
     * @brief Добавляет пользователя.
     * @param login уникальный логин
     * @param hash SHA-384 пароля
     * @param role user или admin
     */
    bool addUser(const QString &login, const QString &hash, const QString &role = "user");

    /** @brief Сверяет хэш с записью в БД. */
    bool checkUser(const QString &login, const QString &hash);

    /**
     * @param login логин
     * @return роль или user по умолчанию
     */
    QString getUserRole(const QString &login);

    /**
     * @brief Список пользователей для sort&...
     * @param sortBy login или role
     * @return строки login:role через запятую
     */
    QString getSortedUsers(const QString &sortBy);

    bool removeUser(const QString &login);

private:
    DatabaseManager();
    ~DatabaseManager();

    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
