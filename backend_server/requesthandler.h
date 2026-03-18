#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <QString>

class RequestHandler {
public:
    static QString handle(const QString &rawCommand, QString &role);
};

#endif
