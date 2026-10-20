#include <QtTest/QTest>
#include "utils.h"

class TestUtils : public QObject {
    Q_OBJECT

private slots:

    void formatBytes_zero()
    {
        QCOMPARE(Utils::formatBytes(0), QString("0 B"));
    }

    void formatBytes_bytes()
    {
        QCOMPARE(Utils::formatBytes(512), QString("512 B"));
    }

    void formatBytes_kilobytes()
    {
        const QString result = Utils::formatBytes(1536);
        QVERIFY(result.contains("KB"));
        QVERIFY(result.startsWith("1.50"));
    }

    void formatBytes_megabytes()
    {
        const QString result = Utils::formatBytes(5 * 1024 * 1024);
        QVERIFY(result.contains("MB"));
        QVERIFY(result.startsWith("5.00"));
    }

    void formatBytes_gigabytes()
    {
        const QString result = Utils::formatBytes(2LL * 1024 * 1024 * 1024);
        QVERIFY(result.contains("GB"));
        QVERIFY(result.startsWith("2.00"));
    }

    void formatBytes_justBelowMegabyte_roundsUpToMb()
    {
        QCOMPARE(Utils::formatBytes(1024 * 1024 - 1), QString("1.00 MB"));
    }

    void formatBytes_justBelowGigabyte_roundsUpToGb()
    {
        QCOMPARE(Utils::formatBytes(1024LL * 1024 * 1024 - 1), QString("1.00 GB"));
    }

    void formatBytes_largestKbValue()
    {
        QCOMPARE(Utils::formatBytes(1024 * 1023 + 1000), QString("1023.98 KB"));
    }

    void formatBytes_boundary_kb()
    {
        const QString result = Utils::formatBytes(1024);
        QVERIFY(result.contains("KB"));
    }

    void formatBytes_boundary_mb()
    {
        const QString result = Utils::formatBytes(1024 * 1024);
        QVERIFY(result.contains("MB"));
    }

    void looksLikeUrl_httpPrefix()
    {
        QVERIFY(Utils::looksLikeUrl("http://example.com"));
        QVERIFY(Utils::looksLikeUrl("https://example.com"));
    }

    void looksLikeUrl_filePrefix()
    {
        QVERIFY(Utils::looksLikeUrl("file:///home/user/test.html"));
    }

    void looksLikeUrl_domainWithDot()
    {
        QVERIFY(Utils::looksLikeUrl("example.com"));
        QVERIFY(Utils::looksLikeUrl("www.google.com"));
    }

    void looksLikeUrl_localhost()
    {
        QVERIFY(Utils::looksLikeUrl("localhost"));
        QVERIFY(Utils::looksLikeUrl("localhost:8080"));
    }

    void looksLikeUrl_searchQuery()
    {
        QVERIFY(!Utils::looksLikeUrl("what is the weather"));
        QVERIFY(!Utils::looksLikeUrl("how to cook pasta"));
    }

    void looksLikeUrl_singleWord()
    {
        QVERIFY(!Utils::looksLikeUrl("google"));
    }

    void looksLikeUrl_emptyString()
    {
        QVERIFY(!Utils::looksLikeUrl(""));
    }

    void looksLikeUrl_spacesInText()
    {
        QVERIFY(!Utils::looksLikeUrl("example .com"));
    }
};

QTEST_MAIN(TestUtils)
#include "test_utils.moc"
