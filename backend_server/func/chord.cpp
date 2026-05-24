#include "chord.h"
#include <cmath>
#include <QStringList>

QString func_chord(const QString &payload) {
    // Парсинг параметров a,b,epsilon
    QStringList params = payload.split(',');

    if (params.size() < 3) {
        return "ERR: Expected format: a,b,epsilon";
    }

    bool ok1, ok2, ok3;
    double a = params[0].trimmed().toDouble(&ok1);
    double b = params[1].trimmed().toDouble(&ok2);
    double epsilon = params[2].trimmed().toDouble(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        return "ERR: Invalid numeric parameters";
    }

    if (epsilon <= 0) {
        return "ERR: Epsilon must be positive";
    }

    if (a >= b) {
        return "ERR: a must be less than b";
    }

    // Функция: x^2 - 4 = 0
    auto f = [](double x) { return x * x - 4; };

    double fa = f(a);
    double fb = f(b);

    if (fa * fb > 0) {
        return "ERR: Function has same signs at interval ends";
    }

    // Метод хорд
    double x0 = a;
    double fx0;
    int iterations = 0;
    const int maxIterations = 1000;

    while (iterations < maxIterations) {
        x0 = a - fa * (b - a) / (fb - fa);
        fx0 = f(x0);

        if (std::abs(fx0) < epsilon) {
            return QString("Root=%1,iter=%2,err=%3")
                .arg(x0, 0, 'f', 10)
                .arg(iterations + 1)
                .arg(fx0, 0, 'e', 6);
        }

        if (fa * fx0 < 0) {
            b = x0;
            fb = fx0;
        } else {
            a = x0;
            fa = fx0;
        }

        iterations++;
    }

    return "ERR: Max iterations exceeded";
}