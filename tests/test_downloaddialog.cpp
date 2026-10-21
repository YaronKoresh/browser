#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QLineEdit>
#include "downloaddialog.h"
#include "settingsmanager.h"

class TestDownloadDialog : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_configDir;

private slots:
    void initTestCase()
    {
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_configDir.path());
    }

    void suggestedPath_usesConfiguredDownloadFolder()
    {
        SettingsManager::instance().setDefaultDownloadDirectory("/tmp/custom-downloads");

        DownloadDialog dialog;
        dialog.setFileName("report.pdf");
        auto *edit = dialog.findChild<QLineEdit *>();
        QVERIFY(edit);
        QCOMPARE(edit->text(), QString("/tmp/custom-downloads/report.pdf"));
    }

    void suggestedPath_blankSettingFallsBackToSystemFolder()
    {
        SettingsManager::instance().setDefaultDownloadDirectory("   ");

        DownloadDialog dialog;
        dialog.setFileName("report.pdf");
        auto *edit = dialog.findChild<QLineEdit *>();
        QVERIFY(edit);
        QVERIFY(edit->text().endsWith("/report.pdf"));
        QVERIFY(!edit->text().startsWith("   "));
    }
};

QTEST_MAIN(TestDownloadDialog)
#include "test_downloaddialog.moc"
