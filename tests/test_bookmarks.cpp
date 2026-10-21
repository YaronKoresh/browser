#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include "bookmarkmanager.h"

class TestBookmarks : public QObject {
    Q_OBJECT

private:
    static QString filePath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
               + "/bookmarks.json";
    }

    static void writeFile(const QByteArray &data)
    {
        QDir().mkpath(QFileInfo(filePath()).path());
        QFile f(filePath());
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(data);
    }

private slots:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void init()
    {
        QFile::remove(filePath());
    }

    void add_emptyUrlIsRejected()
    {
        BookmarkManager mgr;
        QSignalSpy added(&mgr, &BookmarkManager::bookmarkAdded);
        QVERIFY(!mgr.addBookmark("Blank", QUrl()));
        QCOMPARE(mgr.count(), 0);
        QCOMPARE(added.count(), 0);
    }

    void add_invalidUrlIsRejected()
    {
        BookmarkManager mgr;
        QVERIFY(!mgr.addBookmark("Bad", QUrl("http://exa mple.com")));
        QCOMPARE(mgr.count(), 0);
    }

    void add_blankTitleFallsBackToUrl()
    {
        BookmarkManager mgr;
        QVERIFY(mgr.addBookmark("   ", QUrl("https://example.com/")));
        QCOMPARE(mgr.bookmarks().first().title, QString("https://example.com/"));
    }

    void add_andReload()
    {
        {
            BookmarkManager mgr;
            QVERIFY(mgr.addBookmark("Example", QUrl("https://example.com/"), "Work"));
            QVERIFY(!mgr.addBookmark("Again", QUrl("https://example.com/")));
        }
        BookmarkManager reloaded;
        QCOMPARE(reloaded.count(), 1);
        QCOMPARE(reloaded.bookmarks().first().title, QString("Example"));
        QCOMPARE(reloaded.folders(), QStringList{"Work"});
    }

    void load_skipsEntriesWithoutUsableUrl()
    {
        writeFile("[\n"
                  "  {\"title\": \"No url\"},\n"
                  "  {\"title\": \"Empty\", \"url\": \"\"},\n"
                  "  {\"title\": \"Good\", \"url\": \"https://example.com/\"},\n"
                  "  \"not an object\"\n"
                  "]");
        BookmarkManager mgr;
        QCOMPARE(mgr.count(), 1);
        QCOMPARE(mgr.bookmarks().first().title, QString("Good"));
    }

    void load_invalidJsonKeepsFileUntouched()
    {
        writeFile("{ broken");
        BookmarkManager mgr;
        QCOMPARE(mgr.count(), 0);
        QFile f(filePath());
        QVERIFY(f.open(QIODevice::ReadOnly));
        QCOMPARE(f.readAll(), QByteArray("{ broken"));
    }
};

QTEST_MAIN(TestBookmarks)
#include "test_bookmarks.moc"
