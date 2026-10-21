#include "bookmarkmanager.h"

#include <QtCore/QFile>
#include <QtCore/QSaveFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(lcBookmarks, "browser.bookmarks")

BookmarkManager::BookmarkManager(QObject *parent)
    : QObject(parent)
{
    load();
}

void BookmarkManager::load()
{
    const QString path = bookmarkFilePath();
    QFile file(path);
    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(lcBookmarks) << "Could not open bookmarks file:" << path;
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        qCWarning(lcBookmarks) << "Invalid bookmarks JSON format";
        return;
    }

    m_bookmarks.clear();
    const QJsonArray arr = doc.array();
    for (const auto &val : arr) {
        const QJsonObject obj = val.toObject();
        Bookmark bm;
        bm.title  = obj["title"].toString();
        bm.url    = QUrl(obj["url"].toString());
        bm.folder = obj["folder"].toString();
        if (bm.url.isValid())
            m_bookmarks.append(bm);
    }

    qCInfo(lcBookmarks) << "Loaded" << m_bookmarks.size() << "bookmarks from" << path;
}

void BookmarkManager::save() const
{
    const QString path = bookmarkFilePath();

    QDir().mkpath(QFileInfo(path).path());

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(lcBookmarks) << "Could not save bookmarks to:" << path;
        return;
    }

    QJsonArray arr;
    for (const auto &bm : m_bookmarks) {
        QJsonObject obj;
        obj["title"]  = bm.title;
        obj["url"]    = bm.url.toString();
        obj["folder"] = bm.folder;
        arr.append(obj);
    }

    const QByteArray json = QJsonDocument(arr).toJson(QJsonDocument::Indented);
    if (file.write(json) != json.size() || !file.flush() || !file.commit()) {
        qCWarning(lcBookmarks) << "Could not save bookmarks to:" << path << file.errorString();
        file.cancelWriting();
        return;
    }
    qCInfo(lcBookmarks) << "Saved" << m_bookmarks.size() << "bookmarks";
}

bool BookmarkManager::addBookmark(const QString &title, const QUrl &url,
                                   const QString &folder)
{
    if (url.isEmpty() || !url.isValid() || isBookmarked(url))
        return false;

    Bookmark bm;
    bm.title  = title.trimmed().isEmpty() ? url.toString() : title;
    bm.url    = url;
    bm.folder = folder;
    m_bookmarks.append(bm);

    save();
    emit bookmarkAdded(bm);
    emit bookmarksChanged();
    return true;
}

bool BookmarkManager::removeBookmark(const QUrl &url)
{
    for (int i = 0; i < m_bookmarks.size(); ++i) {
        if (m_bookmarks[i].url == url) {
            m_bookmarks.removeAt(i);
            save();
            emit bookmarkRemoved(url);
            emit bookmarksChanged();
            return true;
        }
    }
    return false;
}

bool BookmarkManager::isBookmarked(const QUrl &url) const
{
    for (const auto &bm : m_bookmarks) {
        if (bm.url == url)
            return true;
    }
    return false;
}

QVector<Bookmark> BookmarkManager::bookmarks(const QString &folder) const
{
    if (folder.isEmpty())
        return m_bookmarks;

    QVector<Bookmark> filtered;
    for (const auto &bm : m_bookmarks) {
        if (bm.folder == folder)
            filtered.append(bm);
    }
    return filtered;
}

QStringList BookmarkManager::folders() const
{
    QStringList result;
    for (const auto &bm : m_bookmarks) {
        if (!bm.folder.isEmpty() && !result.contains(bm.folder))
            result.append(bm.folder);
    }
    return result;
}

QStringList BookmarkManager::titles() const
{
    QStringList result;
    for (const auto &bm : m_bookmarks)
        result.append(bm.title);
    return result;
}

QStringList BookmarkManager::urls() const
{
    QStringList result;
    for (const auto &bm : m_bookmarks)
        result.append(bm.url.toString());
    return result;
}

int BookmarkManager::count() const
{
    return m_bookmarks.size();
}

QString BookmarkManager::bookmarkFilePath() const
{
    const QString dataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return dataDir + "/bookmarks.json";
}
