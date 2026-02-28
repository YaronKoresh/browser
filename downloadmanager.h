#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "downloaditem.h"
#include "downloadmodel.h"
#include "constants.h"

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <memory>

class QNetworkAccessManager;
class QNetworkReply;
class QFile;

class DownloadManager : public QObject {
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);
    ~DownloadManager() override;

    [[nodiscard]] DownloadModel *model() const;

    int enqueueDownload(const QUrl &url, const QString &savePath,
                        const QString &fileName, qint64 totalBytes = 0);

    void cancelDownload(int id);
    void retryDownload(int id);
    void cancelAll();
    void setMaxConcurrentDownloads(int n);

    [[nodiscard]] int activeDownloadCount() const;
    [[nodiscard]] bool hasActiveDownloads() const;

signals:
    void downloadStarted(int id);
    void downloadCompleted(int id, const QString &savePath);
    void downloadFailed(int id, const QString &error);
    void downloadCancelled(int id);
    void activeDownloadCountChanged(int count);
    void progressUpdated(int id, qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void processQueue();
    void onReplyFinished();
    void onReplyProgress(qint64 bytesReceived, qint64 bytesTotal);
    void flushThrottledProgress();

private:
    struct ActiveDownload {
        int id                   = 0;
        QNetworkReply *reply     = nullptr;
        std::unique_ptr<QFile> file;
        QString savePath;
        QString fileName;
        QUrl url;
        qint64 lastReportedBytes = 0;
        qint64 latestBytesRecv   = 0;
        qint64 latestBytesTotal  = 0;
        bool dirty               = false;
    };

    void startDownload(int id);
    ActiveDownload *findByReply(QNetworkReply *reply);
    ActiveDownload *findById(int id);

    QNetworkAccessManager *m_networkManager = nullptr;
    DownloadModel         *m_model          = nullptr;
    QMap<int, ActiveDownload> m_active;
    QVector<int>             m_queue;
    QTimer                   m_progressTimer;
    int                      m_maxConcurrent = AppConstants::MAX_CONCURRENT_DOWNLOADS;
};

#endif // DOWNLOADMANAGER_H
