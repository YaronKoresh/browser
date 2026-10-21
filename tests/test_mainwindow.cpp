#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include "mainwindow.h"
#include "bookmarkmanager.h"
#include "browsertab.h"
#include "navigationmanager.h"
#include "tabmanager.h"

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
};

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
