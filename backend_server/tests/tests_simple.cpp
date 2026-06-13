#include <QtTest>

#include "func/sha384.h"
#include "func/chord.h"
#include "func/genetic_path.h"
#include "functionstoserver.h"

class SimpleTests : public QObject
{
    Q_OBJECT

private slots:
    void sha384_returns_96_hex_chars();
    void sha384_abc_known_hash();
    void chord_finds_root();
    void genetic_path_finds_shortest_route();
    void logout_clears_role();
};

void SimpleTests::sha384_returns_96_hex_chars()
{
    const QString hash = func_sha384("hello");
    QCOMPARE(hash.size(), 96);
    for (const QChar c : hash) {
        QVERIFY((c >= QLatin1Char('0') && c <= QLatin1Char('9'))
                || (c >= QLatin1Char('a') && c <= QLatin1Char('f')));
    }
}

void SimpleTests::sha384_abc_known_hash()
{
    // эталон: SHA-384("abc") (совпадает с hashlib / стандартом)
    const QString expected =
        "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed"
        "8086072ba1e7cc2358baeca134c825a7";
    QCOMPARE(func_sha384("abc"), expected);
}

void SimpleTests::chord_finds_root()
{
    const QString result = func_chord("1,3,0.0001");
    QVERIFY(result.startsWith("Root="));
}

void SimpleTests::genetic_path_finds_shortest_route()
{
    // 0 -> 1 (2) -> 3 (1) = 3; альтернатива 0 -> 2 (5) -> 3 (4) = 9
    const QString result = func_genetic_path("0,3,0:1:2,0:2:5,1:3:1,2:3:4");
    QVERIFY(result.startsWith("path="));
    QVERIFY(result.contains("length=3"));
}

void SimpleTests::logout_clears_role()
{
    QString role = "user";
    QCOMPARE(fn_logout(role), QString("LOGOUT_OK\r\n"));
    QVERIFY(role.isEmpty());
}

QTEST_APPLESS_MAIN(SimpleTests)
#include "tests_simple.moc"
