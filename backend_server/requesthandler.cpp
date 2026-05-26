#include "requesthandler.h"
#include "functionstoserver.h"
#include "databasemanager.h"
#include "func/stego.h"

#include <QStringList>
#include <QDebug>

// handle() — парсинг + маршрутизация
QString RequestHandler::handle(const QString &rawCommand, QString &role)
{
    QString cmd = rawCommand.trimmed();
    if (cmd.isEmpty())
        return "ERR: Empty command\r\n";

    QStringList parts = cmd.split('&');
    QString action = parts[0].toLower().trimmed();
    QString payload = (parts.size() > 1) ? parts[1].trimmed() : "";

    // Доступно всем (без авторизации)
    if (action == "reg") {
        return fn_register(payload);
    }

    if (action == "auth") {
        QString response = fn_auth(payload);

        // выставляем роль
        if (response.startsWith("AUTH_OK:")) {
            QString afterTag = response.mid(9).trimmed();
            int start = afterTag.lastIndexOf('(');
            int end   = afterTag.lastIndexOf(')');
            if (start != -1 && end != -1 && end > start)
                role = afterTag.mid(start + 1, end - start - 1).trimmed();
            else
                role = afterTag.split('\r').first().trimmed();
        }

        return response;
    }

    // Только для авторизованных
    if (role.isEmpty()) {
        return "ERR: Not authenticated\r\n";
    }

    if (action == "sha384") return fn_sha384(payload);
    if (action == "chord") return fn_chord(payload);

    if (action == "embed") {
        QStringList args = payload.split(",");

        QString imagein = args[0].trimmed();
        QString imageout = args[1].trimmed();
        QString message = args[2].trimmed();

        QString response = fn_embed(imagein, imageout, message) ? "Success." : "Error.";

        return response;
    }

    if (action == "extract") {
        QString response = fn_extract(payload);

        return response;
    }

    // Только для admin
    if (action == "admin_test") {
        if (role != "admin")
            return "ERR: Access denied. Admin only.\r\n";

        return "ADMIN_OK\r\n";
    }

    if (action == "sort") {
        if (role != "admin")
            return "ERR: Access denied. Admin only.\r\n";
        return fn_list_users_sorted(payload);
    }

    // Неизвестная команда
    return "ERR: Unknown command '" + action + "'\r\n";
}