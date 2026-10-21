#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include "bookmarksdialog.h"

class TestBookmarksDialog : public QObject {
    Q_OBJECT

private:
    static QString filePath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
               + "/bookmarks.json";
    }

    static QPushButton *button(QWidget *parent, const QString &text)
    {
        for (auto *b : parent->findChildren<QPushButton *>()) {
            if (b->text() == text)
                return b;
        }
        return nullptr;
    }

private slots:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void init()
    {
        QFile::remove(filePath());
    }

    void lists_allBookmarks()
    {
        BookmarkManager mgr;
        mgr.addBookmark("One", QUrl("https://one.example/"));
        mgr.addBookmark("Two", QUrl("https://two.example/"));
        BookmarksDialog dialog(&mgr);
        auto *list = dialog.findChild<QListView *>();
        QVERIFY(list);
        QCOMPARE(list->model()->rowCount(), 2);
    }

    void delete_removesRowFromList()
    {
        BookmarkManager mgr;
        mgr.addBookmark("One", QUrl("https://one.example/"));
        mgr.addBookmark("Two", QUrl("https://two.example/"));
        BookmarksDialog dialog(&mgr);
        auto *list = dialog.findChild<QListView *>();
        list->setCurrentIndex(list->model()->index(0, 0));
        button(&dialog, "Delete")->click();
        QCOMPARE(mgr.count(), 1);
        QCOMPARE(list->model()->rowCount(), 1);
        QVERIFY(list->model()->index(0, 0).data().toString().contains("Two"));
    }

    void delete_thenOpenUsesRemainingBookmark()
    {
        BookmarkManager mgr;
        mgr.addBookmark("One", QUrl("https://one.example/"));
        mgr.addBookmark("Two", QUrl("https://two.example/"));
        BookmarksDialog dialog(&mgr);
        QSignalSpy opened(&dialog, &BookmarksDialog::openRequested);
        auto *list = dialog.findChild<QListView *>();
        list->setCurrentIndex(list->model()->index(0, 0));
        button(&dialog, "Delete")->click();
        QVERIFY(list->currentIndex().isValid());
        button(&dialog, "Open")->click();
        QCOMPARE(opened.count(), 1);
        QCOMPARE(opened.first().first().toUrl(), QUrl("https://two.example/"));
    }

    void delete_withoutSelectionKeepsBookmarks()
    {
        BookmarkManager mgr;
        mgr.addBookmark("One", QUrl("https://one.example/"));
        BookmarksDialog dialog(&mgr);
        button(&dialog, "Delete")->click();
        QCOMPARE(mgr.count(), 1);
    }

    void delete_lastRowSelectsPreviousRow()
    {
        BookmarkManager mgr;
        mgr.addBookmark("One", QUrl("https://one.example/"));
        mgr.addBookmark("Two", QUrl("https://two.example/"));
        BookmarksDialog dialog(&mgr);
        auto *list = dialog.findChild<QListView *>();
        list->setCurrentIndex(list->model()->index(1, 0));
        button(&dialog, "Delete")->click();
        QCOMPARE(list->currentIndex().row(), 0);
    }

    void open_withoutSelectionDoesNothing()
    {
        BookmarkManager mgr;
        mgr.addBookmark("One", QUrl("https://one.example/"));
        BookmarksDialog dialog(&mgr);
        QSignalSpy opened(&dialog, &BookmarksDialog::openRequested);
        button(&dialog, "Open")->click();
        QCOMPARE(opened.count(), 0);
    }
};

QTEST_MAIN(TestBookmarksDialog)
#include "test_bookmarksdialog.moc"
