#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QApplication>
#include "thememanager.h"
#include "settingsmanager.h"

class TestThemeManager : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_configDir;

private slots:
    void initTestCase()
    {
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_configDir.path());
    }

    void savedThemeChangeIsApplied()
    {
        auto &theme = ThemeManager::instance();
        auto &settings = SettingsManager::instance();

        settings.setTheme("dark");
        QCOMPARE(theme.currentTheme(), ThemeManager::Theme::Dark);
        QVERIFY(theme.isDark());

        settings.setTheme("light");
        QCOMPARE(theme.currentTheme(), ThemeManager::Theme::Light);
        QVERIFY(!theme.isDark());

        settings.setTheme("system");
        QCOMPARE(theme.currentTheme(), ThemeManager::Theme::System);
    }
};

QTEST_MAIN(TestThemeManager)
#include "test_thememanager.moc"
