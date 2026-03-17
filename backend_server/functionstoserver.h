#ifndef FUNCTIONS_TO_SERVER_H
#define FUNCTIONS_TO_SERVER_H

#include <QString>

// Регистрация. payload: "login,password"
QString fn_register(const QString &payload);

// Авторизация. payload: "login,password"
QString fn_auth(const QString &payload);

// TODO: Функции администратора

// SHA-384. payload: произвольный текст
QString fn_sha384(const QString &payload);

// RSA
// Генерация ключевой пары (?).
QString fn_rsa_gen();

// Шифрование. payload: "e,n,текст"
QString fn_rsa_encrypt(const QString &payload);

// Дешифрование. payload: "d,n,1234 5678 ..."
QString fn_rsa_decrypt(const QString &payload);

// Метод хорд. payload: "a,b,eps"
QString fn_chord(const QString &payload);

// Стеганография
// Внедрить сообщение в картинку методом LSB. payload: "login,image_path,message"
QString fn_embed(const QString &payload);

// Извлечь сообщение из картинки. payload: "image_path"
QString fn_extract(const QString &payload);

#endif
