#include "requesthandler.h"
#include "functionstoserver.h"

#include <QStringList>
#include <QDebug>

// handle() — только парсинг и маршрутизация
QString RequestHandler::handle(const QString &rawCommand, const QString &role) {
    QString cmd = rawCommand.trimmed();
    if (cmd.isEmpty())
        return "ERR: Empty command\r\n";

    QStringList parts = cmd.split('&');
    QString action = parts[0].toLower().trimmed();
    QString payload = (parts.size() > 1) ? parts[1] : "";

    // Команды доступные для всех
    if (action == "reg") return fn_register(payload);
    if (action == "auth") return fn_auth(payload);

    // Команды доступные для авторизованных пользователей
    if (action == "sha384") return fn_sha384(payload);
    if (action == "rsa_gen") return fn_rsa_gen();
    if (action == "rsa_enc") return fn_rsa_encrypt(payload);
    if (action == "rsa_dec") return fn_rsa_decrypt(payload);
    if (action == "chord") return fn_chord(payload);
    if (action == "embed") return fn_embed(payload);
    if (action == "extract") return fn_extract(payload);

    /* TODO: Команды доступные для admin
    if (action == "*" || action == "*" || action == "*") {
        if (role != "admin")
            return "ERR: Access denied. Admin only.\r\n";

        if (action == "*") return fn_();
        if (action == "*") return fn_();
        if (action == "*") return fn_(payload);
    }
    */

    return "ERR: Unknown command '" + action + "'\r\n";
}
