#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <QString>

class RequestHandler {
public:
    static QString handle(const QString& rawCommand);
private:
    // Регистрация: payload = "login,password"
    static QString handleRegister(const QString& payload);

    // Авторизация: payload = "login,password"
    // Ответ содержит роль пользователя: AUTH_OK: admin  / AUTH_OK: user
    static QString handleAuth(const QString& payload);

    // SHA-384: payload = текст
    static QString handleSha384(const QString& payload);

    // RSA генерация ключей: payload - нет
    // Возвращает: "RSA_KEYS: e=<e>|d=<d>|n=<n>"
    static QString handleRsaGen();

    // RSA шифрование: payload = "e,n,открытый текст"
    // Каждый символ шифруется отдельно: cipher[i] = (char^e) mod n
    // Возвращает числа через пробел: "RSA_ENC_OK: 1234 5678 ..."
    static QString handleRsaEncrypt(const QString& payload);

    // RSA дешифрование: payload = "d,n,1234 5678 ..."
    // Возвращает: "RSA_DEC_OK: исходный текст"
    static QString handleRsaDecrypt(const QString& payload);

    // Метод хорд: payload = "a,b,eps"
    // Ищет корень уравнения f(x) = x^3 - x - 2 на отрезке [a, b]
    // с точностью eps. Функцию можно будет менять.
    static QString handleChord(const QString& payload);

    // SHA-384 хэш строки, возвращает hex-строку.
    // Используется и в handleSha384(), и в handleRegister()/handleAuth()
    // (пароли хранятся хэшированными).
    static QString sha384Hex(const QString& text);

    // Быстрое возведение в степень по модулю: (base^exp) % mod
    // Нужно для RSA. Стандартный C++ не имеет встроенного modpow.
    static long long modPow(long long base, long long exp, long long mod);

    // Проверка числа на простоту (детерминированный тест для малых чисел).
    static bool isPrime(long long n);

    // НОД двух чисел (алгоритм Евклида) — нужен для генерации ключей RSA.
    static long long gcd(long long a, long long b);

    // Расширенный алгоритм Евклида: находит x такой что a*x ≡ 1 (mod m).
    // Нужен для нахождения d (приватного ключа RSA).
    static long long modInverse(long long a, long long m);
};

#endif // REQUEST_HANDLER_H
