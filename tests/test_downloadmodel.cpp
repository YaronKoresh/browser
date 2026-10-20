#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include "downloadmodel.h"
#include "downloaditem.h"

class TestDownloadModel : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        m_model = new DownloadModel(this);
    }

    void cleanup()
    {
        delete m_model;
        m_model = nullptr;
    }

    void emptyModel_rowCount()
    {
        QCOMPARE(m_model->rowCount(), 0);
    }

    void addDownload_incrementsRowCount()
    {
        DownloadItem item;
        item.fileName = "test.txt";
        item.url = QUrl("https://example.com/test.txt");
        item.savePath = "/tmp/test.txt";

        int id = m_model->addDownload(item);

        QVERIFY(id > 0);
        QCOMPARE(m_model->rowCount(), 1);
    }

    void addDownload_uniqueIds()
    {
        DownloadItem a, b;
        a.fileName = "a.txt";
        b.fileName = "b.txt";

        int idA = m_model->addDownload(a);
        int idB = m_model->addDownload(b);

        QVERIFY(idA != idB);
        QCOMPARE(m_model->rowCount(), 2);
    }

    void addDownload_emitsRowsInserted()
    {
        QSignalSpy spy(m_model, &QAbstractListModel::rowsInserted);

        DownloadItem item;
        item.fileName = "test.txt";
        m_model->addDownload(item);

        QCOMPARE(spy.count(), 1);
    }

    void data_displayRole_queued()
    {
        DownloadItem item;
        item.fileName = "file.zip";
        item.state = DownloadState::Queued;
        m_model->addDownload(item);

        QModelIndex idx = m_model->index(0);
        QString display = m_model->data(idx, Qt::DisplayRole).toString();
        QVERIFY(display.contains("Queued"));
        QVERIFY(display.contains("file.zip"));
    }

    void data_fileNameRole()
    {
        DownloadItem item;
        item.fileName = "file.zip";
        m_model->addDownload(item);

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::FileNameRole).toString(), "file.zip");
    }

    void data_stateRole()
    {
        DownloadItem item;
        item.state = DownloadState::Active;
        m_model->addDownload(item);

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::StateRole).toInt(),
                 static_cast<int>(DownloadState::Active));
    }

    void data_progressRole_zero()
    {
        DownloadItem item;
        item.bytesReceived = 0;
        item.bytesTotal = 0;
        m_model->addDownload(item);

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::ProgressRole).toDouble(), 0.0);
    }

    void data_progressRole_half()
    {
        DownloadItem item;
        item.bytesReceived = 50;
        item.bytesTotal = 100;
        m_model->addDownload(item);

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::ProgressRole).toDouble(), 0.5);
    }

    void data_invalidIndex()
    {
        QModelIndex idx = m_model->index(999);
        QVERIFY(!m_model->data(idx, Qt::DisplayRole).isValid());
    }

    void updateProgress_updatesModel()
    {
        DownloadItem item;
        item.fileName = "big.zip";
        int id = m_model->addDownload(item);

        m_model->updateProgress(id, 1024, 4096);

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::BytesReceivedRole).toLongLong(), 1024);
        QCOMPARE(m_model->data(idx, DownloadModel::BytesTotalRole).toLongLong(), 4096);
    }

    void updateProgress_emitsDataChanged()
    {
        DownloadItem item;
        int id = m_model->addDownload(item);

        QSignalSpy spy(m_model, &QAbstractListModel::dataChanged);
        m_model->updateProgress(id, 500, 1000);

        QCOMPARE(spy.count(), 1);
    }

    void updateState_changesState()
    {
        DownloadItem item;
        int id = m_model->addDownload(item);

        m_model->updateState(id, DownloadState::Completed);

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::StateRole).toInt(),
                 static_cast<int>(DownloadState::Completed));
    }

    void updateState_withError()
    {
        DownloadItem item;
        int id = m_model->addDownload(item);

        m_model->updateState(id, DownloadState::Failed, "Network timeout");

        QModelIndex idx = m_model->index(0);
        QCOMPARE(m_model->data(idx, DownloadModel::ErrorRole).toString(), "Network timeout");
    }

    void findRowById_found()
    {
        DownloadItem a, b;
        a.fileName = "a.txt";
        b.fileName = "b.txt";
        m_model->addDownload(a);
        int idB = m_model->addDownload(b);

        QCOMPARE(m_model->findRowById(idB), 1);
    }

    void findRowById_notFound()
    {
        QCOMPARE(m_model->findRowById(999), -1);
    }

    void findById_returnsCorrectItem()
    {
        DownloadItem item;
        item.fileName = "target.zip";
        int id = m_model->addDownload(item);

        DownloadItem found = m_model->findById(id);
        QCOMPARE(found.fileName, "target.zip");
        QCOMPARE(found.id, id);
    }

    void findById_notFound_returnsDefault()
    {
        DownloadItem found = m_model->findById(999);
        QCOMPARE(found.id, 0);
    }

    void downloadAt_valid()
    {
        DownloadItem item;
        item.fileName = "file.txt";
        m_model->addDownload(item);

        DownloadItem result = m_model->downloadAt(0);
        QCOMPARE(result.fileName, "file.txt");
    }

    void downloadAt_invalid()
    {
        DownloadItem result = m_model->downloadAt(-1);
        QCOMPARE(result.id, 0);
    }

private:
    DownloadModel *m_model = nullptr;
};

QTEST_MAIN(TestDownloadModel)
#include "test_downloadmodel.moc"
