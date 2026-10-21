#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include "bookmarkmanager.h"

#ifdef Q_OS_UNIX
#include <signal.h>
#include <sys/resource.h>
#endif

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

#ifdef Q_OS_UNIX
    void save_failedWriteKeepsPreviousFile()
    {
        {
            BookmarkManager mgr;
            QVERIFY(mgr.addBookmark("One", QUrl("https://one.example/")));
            QVERIFY(mgr.addBookmark("Two", QUrl("https://two.example/")));
        }
        QFile before(filePath());
        QVERIFY(before.open(QIODevice::ReadOnly));
        const QByteArray original = before.readAll();
        before.close();

        auto *oldHandler = signal(SIGXFSZ, SIG_IGN);
        struct rlimit old{};
        getrlimit(RLIMIT_FSIZE, &old);
        struct rlimit limit = old;
        limit.rlim_cur = 64;
        if (setrlimit(RLIMIT_FSIZE, &limit) != 0) {
            signal(SIGXFSZ, oldHandler);
            QSKIP("cannot limit file size");
        }
        {
            BookmarkManager mgr;
            QCOMPARE(mgr.count(), 2);
            mgr.addBookmark("Three", QUrl("https://three.example/"));
        }
        setrlimit(RLIMIT_FSIZE, &old);
        signal(SIGXFSZ, oldHandler);

        QFile after(filePath());
        QVERIFY(after.open(QIODevice::ReadOnly));
        QCOMPARE(after.readAll(), original);
        BookmarkManager reloaded;
        QCOMPARE(reloaded.count(), 2);
    }
#endif

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
