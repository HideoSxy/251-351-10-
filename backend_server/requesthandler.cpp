#include "requesthandler.h"
#include "database.h"

#include <QCryptographicHash>
#include <QStringList>
#include <QDebug>
#include <cmath>

// !!!!!!!!!!
// NO rsa YET
// !!!!!!!!!!

QString RequestHandler::handle(const QString& rawCommand) {
    QString cmd = rawCommand.trimmed();
    if(cmd.isEmpty()) return "ERR: Empty command\r\n";

    QStringList parts = cmd.split('&', Qt::KeepEmptyParts);
    QString action  = parts[0].toLower().trimmed();
    QString payload = (parts.size() > 1) ? parts.mid(1).join('&') : "";

    if      (action == "reg")      return handleRegister(payload);
    else if (action == "auth")     return handleAuth(payload);
    else if (action == "sha384")   return handleSha384(payload);
    else if (action == "rsa_gen")  return handleRsaGen();
    else if (action == "rsa_enc")  return handleRsaEncrypt(payload);
    else if (action == "rsa_dec")  return handleRsaDecrypt(payload);
    else if (action == "chord")    return handleChord(payload);
    else
        return "ERR: Unknown command '" + action + "'\r\n";
}

QString RequestHandler::sha384Hex(const QString& text) {
    QByteArray hash = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha384);
    return QString(hash.toHex());
}

QString RequestHandler::handleRegister(const QString& payload) {
    QStringList parts = payload.split(',', Qt::KeepEmptyParts);
    if(parts.size() != 2)
        return "REGISTER_ERR: Invalid format. Expected: reg&login,password\r\n";

    QString login    = parts[0].trimmed();
    QString password = parts[1].trimmed();
    if (login.isEmpty() || password.isEmpty())
        return "REGISTER_ERR: Login or password is empty\r\n";

    QString passHash = sha384Hex(password);
    // NEXT: APPEND DB
}

QString RequestHandler::handleAuth(const QString& payload) {
    QStringList parts = payload.split(',', Qt::KeepEmptyParts);
    if(parts.size() != 2)
        return "AUTH_ERR: Invalid format. Expected: auth&login,password\r\n";

    QString login    = parts[0].trimmed();
    QString password = parts[1].trimmed();
    if (login.isEmpty() || password.isEmpty())
        return "AUTH_ERR: Login or password is empty\r\n";

    QString passHash = sha384Hex(password);
    // NEXT: APPEND DB
}

QString RequestHandler::handleSha384(const QString& payload) {
    if(payload.isEmpty()) return "SHA384_ERR: No input text\r\n";
    QString hash = sha384Hex(payload);
    return QString("SHA384_OK: %1\r\n").arg(hash);
}

// payload = "a,b,epsilon"
// NEEDS TO BE REWORKED
QString RequestHandler::handleChord(const QString& payload) {
    QStringList parts = payload.split(",", Qt::KeepEmptyParts);
    if (parts.size() != 3)
        return "CHORD_ERR: Invalid format. Expected: chord&a,b,epsilon\r\n";
    bool ok1, ok2, ok3;
    double a   = parts[0].trimmed().toDouble(&ok1);
    double b   = parts[1].trimmed().toDouble(&ok2);
    double eps = parts[2].trimmed().toDouble(&ok3);

    if (!ok1 || !ok2 || !ok3)
        return "CHORD_ERR: a, b, epsilon must be numbers\r\n";
    if (eps <= 0)
        return "CHORD_ERR: epsilon must be > 0\r\n";

    auto f = [](double x) { return x*x*x - x - 2.0; };

    double x1 = a;
    double x2 = b;

    double y1 = f(x1);
    double y2 = f(x2);

    double x0 = x1;
    double y0 = f(x0);

    if(y1 * y2 > 0)
        return "CHORD_ERR: no root on interval\r\n";

    const int MAX_ITER = 10000;
    int iter = 0;

    while(std::abs(y0) > eps && iter < MAX_ITER) {
        y1 = f(x1);
        y2 = f(x2);

        x0 = x1 - y1 * (x2 - x1) / (y2 - y1);
        y0 = f(x0);
        if(y0 > 0)
            x1 = x0;
        else
            x2 = x0;
        iter++;
    }

    if(iter >= MAX_ITER)
        return "CHORD_ERR: exceeding MAX_ITER\r\n";
    return QString::number(x0, 'f', 10);
}
