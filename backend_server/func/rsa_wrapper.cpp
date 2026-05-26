#include "rsa_wrapper.h"
#include <QDebug>

RSAWrapper::RSAWrapper()
    : m_ready(false)
{
}

long long RSAWrapper::modPow(long long base, long long exp, long long mod) const
{
    long long result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

void RSAWrapper::loadFixedKeys()
{
    m_n = "10403";
    m_e = "17";
    m_d = "5993";
    m_ready = true;
    qDebug() << "[RSA] Keys loaded";
}

QString RSAWrapper::encrypt(const QString &plainText) const
{
    if (!m_ready) return "";

    long long n = m_n.toLongLong();
    long long e = m_e.toLongLong();
    QStringList result;

    for (QChar ch : plainText) {
        long long c = modPow(ch.unicode(), e, n);
        result << QString::number(c);
    }
    return result.join(";");
}

QString RSAWrapper::decrypt(const QString &cipherText) const
{
    if (!m_ready) return "";

    long long n = m_n.toLongLong();
    long long d = m_d.toLongLong();
    QStringList numbers = cipherText.split(";");
    QString result;

    for (const QString &numStr : numbers) {
        long long c = numStr.toLongLong();
        long long m = modPow(c, d, n);
        result.append(QChar::fromUcs2(static_cast<char16_t>(m)));
    }
    return result;
}

QPair<QString, QString> RSAWrapper::getPublicKey() const
{
    return qMakePair(m_n, m_e);
}