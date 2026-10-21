#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include "downloadmanager.h"

#ifdef Q_OS_UNIX
#include <signal.h>
#include <sys/resource.h>
#endif

class TestDownloadManager : public QObject {
    Q_OBJECT

private:
    QString writeSource(const QString &name, const QByteArray &data)
    {
        const QString path = m_dir.filePath(name);
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly))
            return {};
        f.write(data);
        return path;
    }

    int downloadToCompletion(DownloadManager &mgr, const QString &name)
    {
        const QString src = writeSource(name, "payload");
        QSignalSpy done(&mgr, &DownloadManager::downloadCompleted);
        const int id = mgr.enqueueDownload(QUrl::fromLocalFile(src),
                                           m_dir.filePath(name + ".out"), name);
        if (!done.wait(5000))
            return -1;
        return id;
    }

    QTemporaryDir m_dir;

#ifdef Q_OS_UNIX
    struct FileSizeLimit {
        explicit FileSizeLimit(rlim_t bytes)
        {
            m_oldHandler = signal(SIGXFSZ, SIG_IGN);
            getrlimit(RLIMIT_FSIZE, &m_old);
            struct rlimit limit = m_old;
            limit.rlim_cur = bytes;
            m_ok = setrlimit(RLIMIT_FSIZE, &limit) == 0;
        }
        ~FileSizeLimit()
        {
            if (m_ok)
                setrlimit(RLIMIT_FSIZE, &m_old);
            signal(SIGXFSZ, m_oldHandler);
        }
        bool ok() const { return m_ok; }
        struct rlimit m_old{};
        void (*m_oldHandler)(int) = nullptr;
        bool m_ok = false;
    };
#endif

private slots:
    void cancel_completedDownloadStaysCompleted()
    {
        DownloadManager mgr;
        const int id = downloadToCompletion(mgr, "a.txt");
        QVERIFY(id > 0);
        QCOMPARE(mgr.model()->findById(id).state, DownloadState::Completed);

        QSignalSpy cancelled(&mgr, &DownloadManager::downloadCancelled);
        mgr.cancelDownload(id);

        QCOMPARE(mgr.model()->findById(id).state, DownloadState::Completed);
        QCOMPARE(cancelled.count(), 0);
        QVERIFY(QFile::exists(m_dir.filePath("a.txt.out")));
    }

    void completed_modelShowsFullProgress()
    {
        DownloadManager mgr;
        const int id = downloadToCompletion(mgr, "p.txt");
        QVERIFY(id > 0);

        const DownloadItem item = mgr.model()->findById(id);
        QCOMPARE(item.bytesReceived, qint64(7));
        QCOMPARE(item.bytesTotal, qint64(7));
        const QModelIndex idx = mgr.model()->index(0);
        QCOMPARE(idx.data(DownloadModel::ProgressRole).toDouble(), 1.0);
    }

    void cancel_unknownIdIsIgnored()
    {
        DownloadManager mgr;
        QSignalSpy cancelled(&mgr, &DownloadManager::downloadCancelled);
        mgr.cancelDownload(12345);
        QCOMPARE(cancelled.count(), 0);
        QCOMPARE(mgr.model()->rowCount(), 0);
    }

    void cancel_failedDownloadStaysFailed()
    {
        DownloadManager mgr;
        QSignalSpy failed(&mgr, &DownloadManager::downloadFailed);
        const int id = mgr.enqueueDownload(QUrl::fromLocalFile(m_dir.filePath("missing.txt")),
                                           m_dir.filePath("missing.out"), "missing.txt");
        QVERIFY(failed.count() > 0 || failed.wait(5000));
        QCOMPARE(mgr.model()->findById(id).state, DownloadState::Failed);

        mgr.cancelDownload(id);
        QCOMPARE(mgr.model()->findById(id).state, DownloadState::Failed);
    }

    void cancel_queuedDownloadIsCancelled()
    {
        DownloadManager mgr;
        mgr.setMaxConcurrentDownloads(1);
        const QString src = writeSource("q.txt", "payload");
        const int first = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("q1.out"), "q1");
        const int second = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("q2.out"), "q2");
        QCOMPARE(mgr.model()->findById(second).state, DownloadState::Queued);

        QSignalSpy cancelled(&mgr, &DownloadManager::downloadCancelled);
        mgr.cancelDownload(second);

        QCOMPARE(mgr.model()->findById(second).state, DownloadState::Cancelled);
        QCOMPARE(cancelled.count(), 1);
        Q_UNUSED(first);
    }

    void cancel_cancelledDownloadDoesNotEmitAgain()
    {
        DownloadManager mgr;
        mgr.setMaxConcurrentDownloads(1);
        const QString src = writeSource("c.txt", "payload");
        mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("c1.out"), "c1");
        const int second = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("c2.out"), "c2");
        mgr.cancelDownload(second);

        QSignalSpy cancelled(&mgr, &DownloadManager::downloadCancelled);
        mgr.cancelDownload(second);
        QCOMPARE(cancelled.count(), 0);
    }

    void cancelAll_queuedDownloadsAreMarkedCancelled()
    {
        DownloadManager mgr;
        mgr.setMaxConcurrentDownloads(1);
        const QString src = writeSource("all.txt", "payload");
        const int a = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("all1.out"), "all1");
        const int b = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("all2.out"), "all2");
        const int c = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("all3.out"), "all3");
        QCOMPARE(mgr.model()->findById(b).state, DownloadState::Queued);

        QSignalSpy cancelled(&mgr, &DownloadManager::downloadCancelled);
        mgr.cancelAll();

        QCOMPARE(mgr.model()->findById(b).state, DownloadState::Cancelled);
        QCOMPARE(mgr.model()->findById(c).state, DownloadState::Cancelled);
        QVERIFY(cancelled.count() >= 2);
        QTRY_VERIFY(!mgr.hasActiveDownloads());
        Q_UNUSED(a);
    }

    void save_missingFolderIsCreated()
    {
        DownloadManager mgr;
        const QString src = writeSource("nested.src", "payload");
        const QString out = m_dir.filePath("new/deeper/nested.out");
        QSignalSpy completed(&mgr, &DownloadManager::downloadCompleted);
        QSignalSpy failed(&mgr, &DownloadManager::downloadFailed);
        const int id = mgr.enqueueDownload(QUrl::fromLocalFile(src), out, "nested");
        QTRY_VERIFY_WITH_TIMEOUT(completed.count() + failed.count() == 1, 5000);
        QCOMPARE(failed.count(), 0);
        QCOMPARE(mgr.model()->findById(id).state, DownloadState::Completed);
        QFile f(out);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QCOMPARE(f.readAll(), QByteArray("payload"));
    }

    void save_pathThatIsAFolderFailsWithClearMessage()
    {
        DownloadManager mgr;
        const QString src = writeSource("dirtarget.src", "payload");
        const QString dir = m_dir.filePath("existing-folder");
        QVERIFY(QDir().mkpath(dir));
        QSignalSpy failed(&mgr, &DownloadManager::downloadFailed);
        mgr.enqueueDownload(QUrl::fromLocalFile(src), dir, "dirtarget");
        QCOMPARE(failed.count(), 1);
        QVERIFY(failed.first().at(1).toString().contains(dir));
    }

    void samePath_secondDownloadWaitsAndFileHoldsOnlyItsContent()
    {
        DownloadManager mgr;
        const QString first = writeSource("first.bin", QByteArray(3000000, 'a'));
        const QString second = writeSource("second.bin", QByteArray(1000000, 'b'));
        const QString out = m_dir.filePath("shared.out");
        QSignalSpy started(&mgr, &DownloadManager::downloadStarted);
        QSignalSpy completed(&mgr, &DownloadManager::downloadCompleted);
        const int id1 = mgr.enqueueDownload(QUrl::fromLocalFile(first), out, "shared");
        const int id2 = mgr.enqueueDownload(QUrl::fromLocalFile(second), out, "shared");
        QCOMPARE(mgr.activeDownloadCount(), 1);
        QCOMPARE(mgr.model()->findById(id2).state, DownloadState::Queued);
        QTRY_VERIFY_WITH_TIMEOUT(completed.count() == 2, 10000);
        QCOMPARE(mgr.model()->findById(id1).state, DownloadState::Completed);
        QCOMPARE(mgr.model()->findById(id2).state, DownloadState::Completed);
        QFile f(out);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QCOMPARE(f.readAll(), QByteArray(1000000, 'b'));
    }

    void samePath_otherPathsStillRunTogether()
    {
        DownloadManager mgr;
        const QString src = writeSource("par.bin", QByteArray(2000000, 'c'));
        const int id1 = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("p1.out"), "p1");
        const int id2 = mgr.enqueueDownload(QUrl::fromLocalFile(src), m_dir.filePath("p2.out"), "p2");
        QCOMPARE(mgr.activeDownloadCount(), 2);
        QSignalSpy completed(&mgr, &DownloadManager::downloadCompleted);
        QTRY_VERIFY_WITH_TIMEOUT(completed.count() == 2, 10000);
        QCOMPARE(mgr.model()->findById(id1).state, DownloadState::Completed);
        QCOMPARE(mgr.model()->findById(id2).state, DownloadState::Completed);
    }

    void samePath_cancelWaitingDownloadKeepsFirstRunning()
    {
        DownloadManager mgr;
        const QString first = writeSource("c1.bin", QByteArray(2000000, 'a'));
        const QString second = writeSource("c2.bin", "x");
        const QString out = m_dir.filePath("cancel.out");
        const int id1 = mgr.enqueueDownload(QUrl::fromLocalFile(first), out, "cancel");
        const int id2 = mgr.enqueueDownload(QUrl::fromLocalFile(second), out, "cancel");
        mgr.cancelDownload(id2);
        QCOMPARE(mgr.model()->findById(id2).state, DownloadState::Cancelled);
        QSignalSpy completed(&mgr, &DownloadManager::downloadCompleted);
        QTRY_VERIFY_WITH_TIMEOUT(completed.count() == 1, 10000);
        QCOMPARE(mgr.model()->findById(id1).state, DownloadState::Completed);
        QCOMPARE(QFileInfo(out).size(), qint64(2000000));
    }

#ifdef Q_OS_UNIX
    void write_failureMarksDownloadFailed()
    {
        DownloadManager mgr;
        const QString src = writeSource("full.txt", QByteArray(100000, 'x'));
        FileSizeLimit limit(4096);
        if (!limit.ok())
            QSKIP("cannot limit file size");
        QSignalSpy failed(&mgr, &DownloadManager::downloadFailed);
        QSignalSpy completed(&mgr, &DownloadManager::downloadCompleted);
        const int id = mgr.enqueueDownload(QUrl::fromLocalFile(src),
                                           m_dir.filePath("full.out"), "full.txt");
        QTRY_VERIFY_WITH_TIMEOUT(failed.count() == 1, 5000);
        QCOMPARE(completed.count(), 0);
        QCOMPARE(mgr.model()->findById(id).state, DownloadState::Failed);
        QVERIFY(failed.first().at(1).toString().contains("write", Qt::CaseInsensitive));
        QVERIFY(!QFile::exists(m_dir.filePath("full.out")));
        QTRY_VERIFY(!mgr.hasActiveDownloads());
    }

    void write_failureStartsNextQueuedDownload()
    {
        DownloadManager mgr;
        mgr.setMaxConcurrentDownloads(1);
        const QString bad = writeSource("bad.txt", QByteArray(100000, 'x'));
        const QString good = writeSource("good.txt", "payload");
        FileSizeLimit limit(4096);
        if (!limit.ok())
            QSKIP("cannot limit file size");
        QSignalSpy completed(&mgr, &DownloadManager::downloadCompleted);
        const int first = mgr.enqueueDownload(QUrl::fromLocalFile(bad),
                                              m_dir.filePath("bad.out"), "bad");
        const int second = mgr.enqueueDownload(QUrl::fromLocalFile(good),
                                               m_dir.filePath("good.out"), "good");
        QTRY_VERIFY_WITH_TIMEOUT(completed.count() == 1, 5000);
        QCOMPARE(mgr.model()->findById(first).state, DownloadState::Failed);
        QCOMPARE(mgr.model()->findById(second).state, DownloadState::Completed);
    }
#endif
};

QTEST_MAIN(TestDownloadManager)
#include "test_downloadmanager.moc"
