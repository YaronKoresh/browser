#include <QtTest/QTest>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include "mainwindow.h"
#include "bookmarkmanager.h"
#include "downloadmanager.h"
#include "browsertab.h"
#include "navigationmanager.h"
#include "settingsmanager.h"
#include "tabmanager.h"

class UrlSink : public QObject {
    Q_OBJECT

public:
    QList<QUrl> urls;

public slots:
    void handle(const QUrl &url) { urls.append(url); }
};

class TestMainWindow : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_configDir;

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
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_configDir.path());
    }

    void init()
    {
        QFile::remove(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                      + "/bookmarks.json");
    }

    void address_bookmarkTitleOpensTheBookmark()
    {
        MainWindow window;
        auto *bookmarks = window.findChild<BookmarkManager *>();
        QVERIFY(bookmarks);
        QVERIFY(bookmarks->addBookmark("My Example Site", QUrl("https://example.org/page")));

        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(tabs);
        auto *address = window.findChildren<QLineEdit *>().first();
        address->setText("my example site");
        QVERIFY(button(&window, "Go"));
        button(&window, "Go")->click();

        QCOMPARE(tabs->currentTab()->navigation()->currentUrl(),
                 QUrl("https://example.org/page"));
    }

    void startup_reopensTabsOfTheLastSession()
    {
        SettingsManager::instance().setLastSessionUrls(
            {"https://one.example/", "https://two.example/"});

        MainWindow window;
        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(tabs);
        QCOMPARE(tabs->count(), 2);
        QCOMPARE(tabs->tabAt(0)->navigation()->currentUrl(), QUrl("https://one.example/"));
        QCOMPARE(tabs->tabAt(1)->navigation()->currentUrl(), QUrl("https://two.example/"));
        QCOMPARE(tabs->currentIndex(), 0);

        SettingsManager::instance().setLastSessionUrls({});
    }

    void startup_withoutLastSessionOpensOneHomeTab()
    {
        SettingsManager::instance().setLastSessionUrls({});

        MainWindow window;
        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(tabs);
        QCOMPARE(tabs->count(), 1);
        QCOMPARE(tabs->tabAt(0)->navigation()->currentUrl(),
                 SettingsManager::instance().homePage());
    }

    void startup_ignoresInvalidSessionEntries()
    {
        SettingsManager::instance().setLastSessionUrls({"", "https://ok.example/"});

        MainWindow window;
        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(tabs);
        QCOMPARE(tabs->count(), 1);
        QCOMPARE(tabs->tabAt(0)->navigation()->currentUrl(), QUrl("https://ok.example/"));

        SettingsManager::instance().setLastSessionUrls({});
    }

    void completer_leavesOutUrlsOfPrivateTabs()
    {
        MainWindow window;
        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(tabs);
        QVERIFY(QMetaObject::invokeMethod(&window, "createPrivateTab"));
        QCOMPARE(tabs->count(), 2);
        auto *priv = tabs->currentTab();
        QVERIFY(priv->isPrivate());
        priv->loadUrl(QUrl("https://secret.example/private-page"));
        QTRY_COMPARE_WITH_TIMEOUT(priv->url(), QUrl("https://secret.example/private-page"), 30000);

        QVERIFY(QMetaObject::invokeMethod(&window, "onTabUrlChanged",
                                          Q_ARG(QUrl, priv->url())));

        auto *completer = window.findChild<QCompleter *>();
        QVERIFY(completer);
        QAbstractItemModel *model = completer->model();
        for (int row = 0; row < model->rowCount(); ++row)
            QVERIFY(!model->index(row, 0).data().toString().contains("secret.example"));
    }

    void download_completedProgramIsNotLaunched()
    {
        MainWindow window;
        auto *downloads = window.findChild<DownloadManager *>();
        QVERIFY(downloads);

        UrlSink sink;
        const QList<QUrl> &opened = sink.urls;
        QDesktopServices::setUrlHandler("file", &sink, "handle");

        QTemporaryDir dir;
        auto addItem = [&](const QString &name) {
            DownloadItem item;
            item.fileName = name;
            item.savePath = dir.path() + "/" + name;
            const int id = downloads->model()->addDownload(item);
            emit downloads->downloadCompleted(id, item.savePath);
        };

        addItem("installer.exe");
        addItem("run-me.sh");
        addItem("notes.txt");
        QDesktopServices::unsetUrlHandler("file");

        QCOMPARE(opened.size(), 3);
        QCOMPARE(opened[0], QUrl::fromLocalFile(dir.path()));
        QCOMPARE(opened[1], QUrl::fromLocalFile(dir.path()));
        QCOMPARE(opened[2], QUrl::fromLocalFile(dir.path() + "/notes.txt"));
    }

    void bookmarkStar_followsBookmarkChangesOfTheCurrentPage()
    {
        MainWindow window;
        auto *bookmarks = window.findChild<BookmarkManager *>();
        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(bookmarks);
        QVERIFY(tabs);

        QPushButton *star = nullptr;
        for (auto *b : window.findChildren<QPushButton *>()) {
            if (b->toolTip() == "Toggle Bookmark")
                star = b;
        }
        QVERIFY(star);

        QTRY_VERIFY_WITH_TIMEOUT(!tabs->currentTab()->url().isEmpty(), 30000);
        const QUrl current = tabs->currentTab()->url();
        const QString empty = QString::fromUtf8("\xe2\x98\x86");
        const QString filled = QString::fromUtf8("\xe2\x98\x85");
        QCOMPARE(star->text(), empty);

        QVERIFY(bookmarks->addBookmark("Current page", current));
        QCOMPARE(star->text(), filled);

        QVERIFY(bookmarks->removeBookmark(current));
        QCOMPARE(star->text(), empty);
    }

    void privateTab_titleKeepsThePrivateMarker()
    {
        MainWindow window;
        auto *tabs = window.findChild<TabManager *>();
        QVERIFY(tabs);
        QVERIFY(QMetaObject::invokeMethod(&window, "createPrivateTab"));
        auto *priv = tabs->currentTab();
        QVERIFY(priv->isPrivate());

        emit priv->titleChanged("Example Domain");
        QCOMPARE(tabs->tabText(tabs->indexOf(priv)), QString("(Private) Example Domain"));

        emit priv->titleChanged("");
        QCOMPARE(tabs->tabText(tabs->indexOf(priv)), QString("(Private) New Tab"));

        auto *normal = tabs->tabAt(0);
        emit normal->titleChanged("Example Domain");
        QCOMPARE(tabs->tabText(0), QString("Example Domain"));
    }
};

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
