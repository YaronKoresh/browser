#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include "bookmarkmanager.h"

#include <QtWidgets/QDialog>
#include <QtCore/QUrl>
#include <QtCore/QVector>

class QListView;
class QPushButton;
class QStringListModel;

class BookmarksDialog : public QDialog {
    Q_OBJECT

public:
    explicit BookmarksDialog(BookmarkManager *manager, QWidget *parent = nullptr);
    ~BookmarksDialog() override = default;

signals:
    void openRequested(const QUrl &url);

private slots:
    void onOpenClicked();
    void onDeleteClicked();
    void refresh();

private:
    BookmarkManager  *m_manager   = nullptr;
    QListView        *m_listView  = nullptr;
    QStringListModel *m_model     = nullptr;
    QPushButton      *m_openBtn   = nullptr;
    QPushButton      *m_deleteBtn = nullptr;
    QPushButton      *m_closeBtn  = nullptr;
    QVector<Bookmark> m_shown;
};

#endif // BOOKMARKSDIALOG_H
