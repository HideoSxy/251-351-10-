/**
 * @file requesthandler.h
 * @brief Разбор команд action&payload и проверка прав.
 */

#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <QString>

/**
 * @brief Маршрутизатор команд StegoApp.
 */
class RequestHandler {
public:
    /**
     * @brief Выполняет одну команду от клиента.
     * @param rawCommand строка вида reg&login,pass или sha384&text
     * @param role роль сокета; обновляется после успешного auth
     * @return ответ сервера для клиента
     */
    static QString handle(const QString &rawCommand, QString &role);
};

#endif
