#include "functionforserver.h"

#include <QStringList>
#include <QCryptographicHash>
#include <cmath>

/*
    ВНЕДРЕНИЕ СООБЩЕНИЯ В ИЗОБРАЖЕНИЕ

    payload формат:
    image_path|message

    пример:
    cat.png|secret text

    Пока это заглушка.
    Реальную стеганографию добавим позже.
*/
QString FunctionForServer::embedMessage(QString payload)
{
    qDebug() << "Embed message request:" << payload;

    QStringList data = payload.split("|");

    if(data.size() != 2)
    {
        return "ERROR_PAYLOAD";
    }

    QString imagePath = data[0];
    QString message = data[1];

    qDebug() << "Image:" << imagePath;
    qDebug() << "Message:" << message;

    // здесь позже будет алгоритм внедрения сообщения

    return "EMBED_NOT_IMPLEMENTED";
}

