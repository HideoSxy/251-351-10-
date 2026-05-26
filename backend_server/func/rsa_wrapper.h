#ifndef RSA_WRAPPER_H
#define RSA_WRAPPER_H

#include <QString>
#include <QPair>

class RSAWrapper
{
public:
    RSAWrapper();

    void loadFixedKeys();
    QString encrypt(const QString &plainText) const;
    QString decrypt(const QString &cipherText) const;
    QPair<QString, QString> getPublicKey() const;

private:
    long long modPow(long long base, long long exp, long long mod) const;

    QString m_n;
    QString m_e;
    QString m_d;
    bool m_ready;
};

#endif // RSA_WRAPPER_H