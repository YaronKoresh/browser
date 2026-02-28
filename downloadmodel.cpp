#include "downloadmodel.h"
#include "utils.h"

DownloadModel::DownloadModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_downloads.size();
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_downloads.size())
        return {};

    const DownloadItem &item = m_downloads[index.row()];

    switch (role) {
    case Qt::DisplayRole: {
        QString prefix;
        switch (item.state) {
        case DownloadState::Queued:     prefix = "Queued";     break;
        case DownloadState::Active:     prefix = "Downloading"; break;
        case DownloadState::Paused:     prefix = "Paused";     break;
        case DownloadState::Completed:  prefix = "Downloaded";  break;
        case DownloadState::Failed:     prefix = "Failed";      break;
        case DownloadState::Cancelled:  prefix = "Cancelled";   break;
        }
        QString text = QString("%1: %2").arg(prefix, item.fileName);
        if (item.state == DownloadState::Active && item.bytesTotal > 0) {
            text += QString(" (%1 / %2)")
                        .arg(Utils::formatBytes(item.bytesReceived),
                             Utils::formatBytes(item.bytesTotal));
        } else if (item.state == DownloadState::Active && item.bytesReceived > 0) {
            text += QString(" (%1)").arg(Utils::formatBytes(item.bytesReceived));
        }
        return text;
    }
    case FileNameRole:      return item.fileName;
    case StateRole:         return static_cast<int>(item.state);
    case BytesReceivedRole: return item.bytesReceived;
    case BytesTotalRole:    return item.bytesTotal;
    case ProgressRole:
        return (item.bytesTotal > 0)
                   ? static_cast<double>(item.bytesReceived) / item.bytesTotal
                   : 0.0;
    case ErrorRole:         return item.errorMessage;
    case UrlRole:           return item.url;
    case SavePathRole:      return item.savePath;
    case IdRole:            return item.id;
    default:                return {};
    }
}

int DownloadModel::addDownload(const DownloadItem &item)
{
    DownloadItem copy = item;
    copy.id = m_nextId++;

    const int row = m_downloads.size();
    beginInsertRows(QModelIndex(), row, row);
    m_downloads.append(copy);
    endInsertRows();

    return copy.id;
}

void DownloadModel::updateProgress(int id, qint64 bytesReceived, qint64 bytesTotal)
{
    const int row = findRowById(id);
    if (row < 0)
        return;

    m_downloads[row].bytesReceived = bytesReceived;
    m_downloads[row].bytesTotal    = bytesTotal;

    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {Qt::DisplayRole, BytesReceivedRole, BytesTotalRole, ProgressRole});
}

void DownloadModel::updateState(int id, DownloadState state, const QString &error)
{
    const int row = findRowById(id);
    if (row < 0)
        return;

    m_downloads[row].state        = state;
    m_downloads[row].errorMessage = error;

    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {Qt::DisplayRole, StateRole, ErrorRole});
}

DownloadItem DownloadModel::downloadAt(int row) const
{
    if (row >= 0 && row < m_downloads.size())
        return m_downloads[row];
    return {};
}

int DownloadModel::findRowById(int id) const
{
    for (int i = 0; i < m_downloads.size(); ++i) {
        if (m_downloads[i].id == id)
            return i;
    }
    return -1;
}

DownloadItem DownloadModel::findById(int id) const
{
    const int row = findRowById(id);
    if (row >= 0)
        return m_downloads[row];
    return {};
}
