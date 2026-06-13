/**
 * @file genetic_path.h
 * @brief Поиск кратчайшего пути генетическим алгоритмом.
 */

#ifndef GENETIC_PATH_H
#define GENETIC_PATH_H

#include <QString>

/**
 * @brief Ищет кратчайший путь в графе с помощью ГА.
 * @param payload start,end,from:to:weight,from:to:weight,...
 * @return path=...,length=... или строка ERR:...
 */
QString func_genetic_path(const QString &payload);

#endif // GENETIC_PATH_H
