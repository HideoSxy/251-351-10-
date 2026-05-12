#include "requesthandler.h"
#include "functionstoserver.h"
#include "databasemanager.h"
#include "stego.h"

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

    // =========================
    // ДОСТУПНО ВСЕМ
    // =========================
    if (action == "reg") {
        return fn_register(payload);
    }

    if (action == "auth") {
        QString response = fn_auth(payload);

        // если успешная авторизация — выставляем роль
        if (response.startsWith("AUTH_OK:")) {
            role = response.mid(8).trimmed(); // после AUTH_OK:
        }

        return response;
    }



    // =========================
    // ТОЛЬКО ДЛЯ АВТОРИЗОВАННЫХ
    // =========================
    if (role.isEmpty()) {
        return "ERR: Not authenticated\r\n";
    }

    if (action == "sha384") return fn_sha384(payload);
    if (action == "rsa_gen") return fn_rsa_gen();
    if (action == "rsa_enc") return fn_rsa_encrypt(payload);
    if (action == "rsa_dec") return fn_rsa_decrypt(payload);
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

    // =========================
    // ТОЛЬКО ДЛЯ ADMIN
    // =========================
    if (action == "admin_test") {
        if (role != "admin")
            return "ERR: Access denied. Admin only.\r\n";

        return "ADMIN_OK\r\n";
    }

    // =========================
    // НЕИЗВЕСТНАЯ КОМАНДА
    // =========================
    return "ERR: Unknown command '" + action + "'\r\n";
}