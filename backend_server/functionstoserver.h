#ifndef FUNCTIONS_TO_SERVER_H
#define FUNCTIONS_TO_SERVER_H

#include <QString>

// Регистрация. payload: "login,password"
QString fn_register(const QString &payload);

// Авторизация. payload: "login,password"
QString fn_auth(const QString &payload);

// SHA-384. payload: произвольный текст
QString fn_sha384(const QString &payload);

// Метод хорд
QString fn_chord(const QString &payload);

QString fn_list_users_sorted(const QString &payload);

#endif