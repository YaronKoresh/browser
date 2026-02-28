#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QJsonArray>
#include <QtCore/QVector>

struct Bookmark {
    QString title;
    QUrl url;
    QString folder;
};

class BookmarkManager : public QObject {
    Q_OBJECT

public:
    explicit BookmarkManager(QObject *parent = nullptr);

    void load();
    void save() const;

    bool addBookmark(const QString &title, const QUrl &url,
                     const QString &folder = {});
    bool removeBookmark(const QUrl &url);

    [[nodiscard]] bool isBookmarked(const QUrl &url) const;
    [[nodiscard]] QVector<Bookmark> bookmarks(const QString &folder = {}) const;
    [[nodiscard]] QStringList folders() const;
    [[nodiscard]] QStringList titles() const;
    [[nodiscard]] QStringList urls() const;
    [[nodiscard]] int count() const;

signals:
    void bookmarkAdded(const Bookmark &bookmark);
    void bookmarkRemoved(const QUrl &url);
    void bookmarksChanged();

private:
    [[nodiscard]] QString bookmarkFilePath() const;

    QVector<Bookmark> m_bookmarks;
};

#endif // BOOKMARKMANAGER_H
