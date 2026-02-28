#include "downloadmanager.h"
#include "utils.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(lcDownload, "browser.download")

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_model(new DownloadModel(this))
{
    m_progressTimer.setInterval(AppConstants::PROGRESS_UI_THROTTLE_MS);
    connect(&m_progressTimer, &QTimer::timeout, this, &DownloadManager::flushThrottledProgress);
}

DownloadManager::~DownloadManager()
{
    cancelAll();
}

DownloadModel *DownloadManager::model() const
{
    return m_model;
}

int DownloadManager::enqueueDownload(const QUrl &url, const QString &savePath,
                                     const QString &fileName, qint64 totalBytes)
{
    DownloadItem item;
    item.url           = url;
    item.savePath      = savePath;
    item.fileName      = fileName;
    item.bytesTotal    = totalBytes;
    item.state         = DownloadState::Queued;

    const int id = m_model->addDownload(item);

    qCInfo(lcDownload) << "Enqueued download" << id << fileName << url.toString();

    m_queue.append(id);
    processQueue();

    return id;
}

void DownloadManager::cancelDownload(int id)
{
    m_queue.removeAll(id);

    if (auto *dl = findById(id)) {
        if (dl->reply) {
            dl->reply->abort();
        }
    } else {
        m_model->updateState(id, DownloadState::Cancelled);
        emit downloadCancelled(id);
    }
}

void DownloadManager::retryDownload(int id)
{
    const DownloadItem item = m_model->findById(id);
    if (item.id == 0)
        return;

    if (item.state != DownloadState::Failed && item.state != DownloadState::Cancelled)
        return;

    qCInfo(lcDownload) << "Retrying download" << id << item.fileName;

    m_model->updateState(id, DownloadState::Queued);
    m_model->updateProgress(id, 0, item.bytesTotal);

    m_queue.append(id);
    processQueue();
}

void DownloadManager::cancelAll()
{
    m_queue.clear();

    const auto ids = m_active.keys();
    for (int id : ids)
        cancelDownload(id);
}

void DownloadManager::setMaxConcurrentDownloads(int n)
{
    m_maxConcurrent = qMax(1, n);
    processQueue();
}

int DownloadManager::activeDownloadCount() const
{
    return m_active.size();
}

bool DownloadManager::hasActiveDownloads() const
{
    return !m_active.isEmpty();
}

void DownloadManager::processQueue()
{
    while (m_active.size() < m_maxConcurrent && !m_queue.isEmpty()) {
        const int id = m_queue.takeFirst();
        startDownload(id);
    }

    if (m_active.isEmpty()) {
        m_progressTimer.stop();
    } else if (!m_progressTimer.isActive()) {
        m_progressTimer.start();
    }

    emit activeDownloadCountChanged(m_active.size());
}

void DownloadManager::onReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    auto *dl = findByReply(reply);
    if (!dl) {
        reply->deleteLater();
        return;
    }

    const int id = dl->id;
    const QNetworkReply::NetworkError error = reply->error();

    if (error == QNetworkReply::NoError) {
        const QByteArray remaining = reply->readAll();
        if (!remaining.isEmpty() && dl->file)
            dl->file->write(remaining);

        if (dl->file)
            dl->file->close();

        m_model->updateState(id, DownloadState::Completed);
        qCInfo(lcDownload) << "Download completed:" << dl->fileName << "->" << dl->savePath;
        emit downloadCompleted(id, dl->savePath);

    } else if (error == QNetworkReply::OperationCanceledError) {
        if (dl->file) {
            dl->file->close();
            dl->file->remove();
        }
        m_model->updateState(id, DownloadState::Cancelled);
        qCInfo(lcDownload) << "Download cancelled:" << dl->fileName;
        emit downloadCancelled(id);

    } else {
        if (dl->file) {
            dl->file->close();
            dl->file->remove();
        }
        const QString errStr = reply->errorString();
        m_model->updateState(id, DownloadState::Failed, errStr);
        qCWarning(lcDownload) << "Download failed:" << dl->fileName << errStr;
        emit downloadFailed(id, errStr);
    }

    reply->deleteLater();
    m_active.remove(id);
    processQueue();
}

void DownloadManager::onReplyProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    auto *dl = findByReply(reply);
    if (!dl)
        return;

    if (dl->file) {
        const QByteArray data = reply->readAll();
        if (!data.isEmpty())
            dl->file->write(data);
    }

    dl->latestBytesRecv  = bytesReceived;
    dl->latestBytesTotal = bytesTotal;
    dl->dirty            = true;
}

void DownloadManager::flushThrottledProgress()
{
    for (auto &dl : m_active) {
        if (!dl.dirty)
            continue;

        dl.dirty = false;
        dl.lastReportedBytes = dl.latestBytesRecv;

        m_model->updateProgress(dl.id, dl.latestBytesRecv, dl.latestBytesTotal);
        emit progressUpdated(dl.id, dl.latestBytesRecv, dl.latestBytesTotal);
    }
}

void DownloadManager::startDownload(int id)
{
    DownloadItem item = m_model->findById(id);
    if (item.id == 0) {
        qCWarning(lcDownload) << "Cannot start download — id not found:" << id;
        return;
    }

    auto file = QSharedPointer<QFile>::create(item.savePath);
    if (!file->open(QIODevice::WriteOnly)) {
        const QString err = QString("Could not open file for writing: %1").arg(item.savePath);
        m_model->updateState(id, DownloadState::Failed, err);
        qCWarning(lcDownload) << err;
        emit downloadFailed(id, err);
        processQueue();
        return;
    }

    QNetworkRequest request(item.url);
    QNetworkReply *reply = m_networkManager->get(request);

    ActiveDownload dl;
    dl.id       = id;
    dl.reply    = reply;
    dl.file     = file;
    dl.savePath = item.savePath;
    dl.fileName = item.fileName;
    dl.url      = item.url;

    connect(reply, &QNetworkReply::finished,         this, &DownloadManager::onReplyFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::onReplyProgress);

    m_active.insert(id, std::move(dl));
    m_model->updateState(id, DownloadState::Active);

    qCInfo(lcDownload) << "Started download" << id << item.fileName;
    emit downloadStarted(id);
}

DownloadManager::ActiveDownload *DownloadManager::findByReply(QNetworkReply *reply)
{
    for (auto it = m_active.begin(); it != m_active.end(); ++it) {
        if (it->reply == reply)
            return &(*it);
    }
    return nullptr;
}

DownloadManager::ActiveDownload *DownloadManager::findById(int id)
{
    auto it = m_active.find(id);
    return (it != m_active.end()) ? &(*it) : nullptr;
}
