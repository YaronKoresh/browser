#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include "downloadmanager.h"

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
};

QTEST_MAIN(TestDownloadManager)
#include "test_downloadmanager.moc"
