#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include "preferencesdialog.h"
#include "settingsmanager.h"

class TestPreferences : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_configDir;

    static QComboBox *themeCombo(PreferencesDialog &dialog)
    {
        for (auto *combo : dialog.findChildren<QComboBox *>()) {
            if (combo->findData("dark") >= 0)
                return combo;
        }
        return nullptr;
    }

private slots:
    void initTestCase()
    {
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_configDir.path());
    }

    void theme_savedChoiceIsShown()
    {
        SettingsManager::instance().setTheme("dark");
        PreferencesDialog dialog;
        auto *combo = themeCombo(dialog);
        QVERIFY(combo);
        QCOMPARE(combo->currentData().toString(), QString("dark"));
        SettingsManager::instance().setTheme("system");
    }

    void theme_choiceIsSavedOnOk()
    {
        auto &s = SettingsManager::instance();
        s.setTheme("system");
        PreferencesDialog dialog;
        auto *combo = themeCombo(dialog);
        QVERIFY(combo);
        combo->setCurrentIndex(combo->findData("light"));
        auto *box = dialog.findChild<QDialogButtonBox *>();
        QVERIFY(box);
        box->button(QDialogButtonBox::Ok)->click();
        QCOMPARE(s.theme(), QString("light"));
        s.setTheme("system");
    }
};

QTEST_MAIN(TestPreferences)
#include "test_preferences.moc"
