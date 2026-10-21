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
};

QTEST_MAIN(TestDownloadManager)
#include "test_downloadmanager.moc"
