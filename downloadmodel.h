#ifndef DOWNLOADMODEL_H
#define DOWNLOADMODEL_H

#include "downloaditem.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QVector>

class DownloadModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        FileNameRole      = Qt::UserRole + 1,
        StateRole,
        BytesReceivedRole,
        BytesTotalRole,
        ProgressRole,
        ErrorRole,
        UrlRole,
        SavePathRole,
        IdRole
    };

    explicit DownloadModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int addDownload(const DownloadItem &item);
    void updateProgress(int id, qint64 bytesReceived, qint64 bytesTotal);
    void updateState(int id, DownloadState state, const QString &error = {});
    [[nodiscard]] DownloadItem downloadAt(int row) const;
    [[nodiscard]] int findRowById(int id) const;
    [[nodiscard]] DownloadItem findById(int id) const;

private:
    QVector<DownloadItem> m_downloads;
    int m_nextId = 1;
};

#endif // DOWNLOADMODEL_H
