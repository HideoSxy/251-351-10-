#ifndef STEGO_H
#define STEGO_H

#include <QString>

// Встраивает сообщение msg в PNG-файл image_path.
// Сохраняет результат в output_path (может совпадать с image_path).
// Возвращает true при успехе.
bool fn_embed(const QString& image_path,
              const QString& output_path,
              const QString& msg);

// Извлекает сообщение из PNG-файла image_path.
// Возвращает пустую строку при ошибке или если сообщения нет.
QString fn_extract(const QString& image_path);

#endif

