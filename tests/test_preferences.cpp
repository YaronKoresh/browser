#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
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

    void homePage_bareHostIsSavedAsWebAddress()
    {
        auto &s = SettingsManager::instance();
        const QUrl before = s.homePage();
        PreferencesDialog dialog;
        for (auto *edit : dialog.findChildren<QLineEdit *>()) {
            if (edit->text() == before.toString())
                edit->setText("  example.org/start  ");
        }
        auto *box = dialog.findChild<QDialogButtonBox *>();
        QVERIFY(box);
        box->button(QDialogButtonBox::Ok)->click();
        QCOMPARE(s.homePage(), QUrl("http://example.org/start"));
        s.setHomePage(before);
    }

    void homePage_emptyInputKeepsCurrentHomePage()
    {
        auto &s = SettingsManager::instance();
        s.setHomePage(QUrl("https://kept.example/"));
        PreferencesDialog dialog;
        for (auto *edit : dialog.findChildren<QLineEdit *>()) {
            if (edit->text() == "https://kept.example/")
                edit->setText("   ");
        }
        auto *box = dialog.findChild<QDialogButtonBox *>();
        box->button(QDialogButtonBox::Ok)->click();
        QCOMPARE(s.homePage(), QUrl("https://kept.example/"));
        s.setHomePage(QUrl(AppConstants::DEFAULT_HOME_URL));
    }

    void searchEngine_typedCustomAddressIsSaved()
    {
        auto &s = SettingsManager::instance();
        s.setDefaultSearchEngine("https://www.google.com/search?q=%1");
        PreferencesDialog dialog;
        QComboBox *combo = nullptr;
        for (auto *c : dialog.findChildren<QComboBox *>()) {
            if (c->isEditable())
                combo = c;
        }
        QVERIFY(combo);
        combo->setEditText("https://search.example/find?q=%1");
        auto *box = dialog.findChild<QDialogButtonBox *>();
        QVERIFY(box);
        box->button(QDialogButtonBox::Ok)->click();
        QCOMPARE(s.defaultSearchEngine(), QString("https://search.example/find?q=%1"));
        s.setDefaultSearchEngine("https://www.google.com/search?q=%1");
    }

    void searchEngine_listedNameIsSavedAsItsAddress()
    {
        auto &s = SettingsManager::instance();
        s.setDefaultSearchEngine("https://www.google.com/search?q=%1");
        PreferencesDialog dialog;
        QComboBox *combo = nullptr;
        for (auto *c : dialog.findChildren<QComboBox *>()) {
            if (c->isEditable())
                combo = c;
        }
        QVERIFY(combo);
        combo->setCurrentIndex(combo->findData("https://duckduckgo.com/?q=%1"));
        auto *box = dialog.findChild<QDialogButtonBox *>();
        box->button(QDialogButtonBox::Ok)->click();
        QCOMPARE(s.defaultSearchEngine(), QString("https://duckduckgo.com/?q=%1"));
        s.setDefaultSearchEngine("https://www.google.com/search?q=%1");
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
