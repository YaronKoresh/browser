#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QtCore/QString>
#include <QtCore/QUrl>

enum class DownloadState {
    Queued,
    Active,
    Paused,
    Completed,
    Failed,
    Cancelled
};

struct DownloadItem {
    int id               = 0;
    QUrl url;
    QString fileName;
    QString savePath;
    DownloadState state  = DownloadState::Queued;
    qint64 bytesReceived = 0;
    qint64 bytesTotal    = 0;
    QString errorMessage;
};

#endif // DOWNLOADITEM_H
