#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleFactory>
#include <cmath>
#include "thememanager.h"
#include "settingsmanager.h"

class TestThemeManager : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_configDir;

    static double luminance(const QColor &c)
    {
        auto channel = [](double v) {
            v /= 255.0;
            return v <= 0.03928 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4);
        };
        return 0.2126 * channel(c.red()) + 0.7152 * channel(c.green())
             + 0.0722 * channel(c.blue());
    }

    static double contrast(const QColor &a, const QColor &b)
    {
        const double la = luminance(a);
        const double lb = luminance(b);
        return (std::max(la, lb) + 0.05) / (std::min(la, lb) + 0.05);
    }

    static void verifyReadable(const QPalette &pal)
    {
        const struct { QPalette::ColorRole fg, bg; double min; } pairs[] = {
            { QPalette::WindowText,      QPalette::Window,      4.5 },
            { QPalette::Text,            QPalette::Base,        4.5 },
            { QPalette::ButtonText,      QPalette::Button,      4.5 },
            { QPalette::PlaceholderText, QPalette::Base,        4.5 },
            { QPalette::ToolTipText,     QPalette::ToolTipBase, 4.5 },
            { QPalette::HighlightedText, QPalette::Highlight,   4.5 },
        };
        for (const auto &p : pairs) {
            const double ratio = contrast(pal.color(QPalette::Active, p.fg),
                                          pal.color(QPalette::Active, p.bg));
            QVERIFY2(ratio >= p.min,
                     qPrintable(QString("role %1 on %2: contrast %3")
                                    .arg(p.fg).arg(p.bg).arg(ratio)));
        }
        QVERIFY(contrast(pal.color(QPalette::Disabled, QPalette::Text),
                         pal.color(QPalette::Disabled, QPalette::Base)) >= 3.0);
        QVERIFY(contrast(pal.color(QPalette::Disabled, QPalette::ButtonText),
                         pal.color(QPalette::Disabled, QPalette::Button)) >= 3.0);
    }

private slots:
    void initTestCase()
    {
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_configDir.path());

        // Stand in for a native platform style that ignores palette colours.
        QApplication::setStyle(QStyleFactory::create("Windows"));
        ThemeManager::instance();
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

    void customThemesUseAPaletteDrivenStyle()
    {
        auto &settings = SettingsManager::instance();

        settings.setTheme("dark");
        QCOMPARE(QApplication::style()->name().toLower(), QString("fusion"));

        settings.setTheme("light");
        QCOMPARE(QApplication::style()->name().toLower(), QString("fusion"));

        settings.setTheme("system");
        QCOMPARE(QApplication::style()->name().toLower(), QString("windows"));
    }

    void darkPaletteIsReadable()
    {
        SettingsManager::instance().setTheme("dark");
        verifyReadable(QApplication::palette());
        SettingsManager::instance().setTheme("system");
    }

    void lightPaletteIsReadable()
    {
        SettingsManager::instance().setTheme("light");
        verifyReadable(QApplication::palette());
        SettingsManager::instance().setTheme("system");
    }
};

QTEST_MAIN(TestThemeManager)
#include "test_thememanager.moc"
