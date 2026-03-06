#ifndef FUNCTION_FOR_SERVER_H
#define FUNCTION_FOR_SERVER_H

#include <QObject>
#include <QString>
#include <QDebug>

class FunctionForServer : public QObject
{
    Q_OBJECT
public:
    // Внедрение сообщения в изображение
    QString embedMessage(QString payload);

};

#endif // FUNCTION_FOR_SERVER_H
