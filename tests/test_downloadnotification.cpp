#include <QtTest/QTest>
#include <QtCore/QPointer>
#include <QtCore/QTemporaryDir>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include "downloadnotification.h"

class UrlSink : public QObject {
    Q_OBJECT

public:
    QList<QUrl> urls;

public slots:
    void handle(const QUrl &url) { urls.append(url); }
};

class TestDownloadNotification : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_dir;
    UrlSink       m_sink;

    static QPushButton *button(QWidget *parent, const QString &text)
    {
        for (auto *b : parent->findChildren<QPushButton *>()) {
            if (b->text() == text)
                return b;
        }
        return nullptr;
    }

private slots:
    void init()
    {
        m_sink.urls.clear();
        QDesktopServices::setUrlHandler("file", &m_sink, "handle");
    }

    void cleanup()
    {
        QDesktopServices::unsetUrlHandler("file");
    }

    void construction_opensNothing()
    {
        DownloadNotification notice(m_dir.path() + "/setup.exe");
        DownloadNotification other(m_dir.path() + "/notes.txt");
        QVERIFY(m_sink.urls.isEmpty());
    }

    void document_opensWithoutConfirmation()
    {
        const QString path = m_dir.path() + "/notes.txt";
        DownloadNotification notice(path);
        QVERIFY(!notice.isLaunchable());
        QVERIFY(!notice.findChild<QCheckBox *>());

        auto *openFile = button(&notice, "Open File");
        QVERIFY(openFile);
        QVERIFY(openFile->isEnabled());
        openFile->click();
        QCOMPARE(m_sink.urls, QList<QUrl>{QUrl::fromLocalFile(path)});
    }

    void folderButton_opensTheContainingFolder()
    {
        DownloadNotification program(m_dir.path() + "/setup.exe");
        DownloadNotification document(m_dir.path() + "/notes.txt");

        button(&program, "Open Folder")->click();
        button(&document, "Open Folder")->click();

        const QUrl folder = QUrl::fromLocalFile(m_dir.path());
        QCOMPARE(m_sink.urls, (QList<QUrl>{folder, folder}));
    }

    void program_needsTheConfirmationTick()
    {
        const QString path = m_dir.path() + "/setup.exe";
        DownloadNotification notice(path);
        QVERIFY(notice.isLaunchable());

        auto *confirm = notice.findChild<QCheckBox *>();
        auto *openFile = button(&notice, "Open File");
        QVERIFY(confirm);
        QVERIFY(openFile);
        QVERIFY(!confirm->isChecked());
        QVERIFY(!openFile->isEnabled());

        openFile->click();
        QVERIFY(m_sink.urls.isEmpty());
        QVERIFY(QMetaObject::invokeMethod(&notice, "openFile"));
        QVERIFY(m_sink.urls.isEmpty());

        confirm->setChecked(true);
        QVERIFY(openFile->isEnabled());
        openFile->click();
        QCOMPARE(m_sink.urls, QList<QUrl>{QUrl::fromLocalFile(path)});

        confirm->setChecked(false);
        QVERIFY(!openFile->isEnabled());
    }

    void program_withTrailingDotOrSpaceNeedsTheConfirmationTick()
    {
        for (const QString &name : {QString("setup.exe."), QString("setup.exe "),
                                    QString("run.sh. .")}) {
            DownloadNotification notice(m_dir.path() + "/" + name);
            QVERIFY2(notice.isLaunchable(), qPrintable(name));
            QVERIFY2(notice.findChild<QCheckBox *>(), qPrintable(name));
            QVERIFY2(!button(&notice, "Open File")->isEnabled(), qPrintable(name));
        }
    }

    void closeButton_dismissesTheNotice()
    {
        QWidget host;
        QPointer<DownloadNotification> notice(
            new DownloadNotification(m_dir.path() + "/notes.txt", &host));
        auto *close = button(notice, "X");
        QVERIFY(close);
        close->click();
        QTRY_VERIFY(notice.isNull());
        QVERIFY(m_sink.urls.isEmpty());
    }
};

QTEST_MAIN(TestDownloadNotification)
#include "test_downloadnotification.moc"
