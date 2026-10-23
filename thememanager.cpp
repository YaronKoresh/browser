#include "thememanager.h"
#include "settingsmanager.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleFactory>
#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtGui/QStyleHints>

namespace {

// Native styles (windowsvista, windows11, macOS) draw buttons, tabs, line edits
// and frames with platform colours and ignore most palette roles, so a custom
// palette on top of them gives light-on-light text. Fusion honours every role.
constexpr const char *PALETTE_STYLE = "Fusion";

} // namespace

ThemeManager &ThemeManager::instance()
{
    static ThemeManager s_instance;
    return s_instance;
}

ThemeManager::ThemeManager()
    : QObject(nullptr)
    , m_systemStyleName(QApplication::style()->name())
{
    connect(&SettingsManager::instance(), &SettingsManager::themeChanged,
            this, qOverload<const QString &>(&ThemeManager::setTheme));
}

void ThemeManager::setTheme(Theme theme)
{
    m_current = theme;

    switch (theme) {
    case Theme::Light:  applyLightPalette(); break;
    case Theme::Dark:   applyDarkPalette();  break;
    case Theme::System: applySystemPalette(); break;
    }

    emit themeChanged(theme);
}

void ThemeManager::setTheme(const QString &name)
{
    if (name == "dark")
        setTheme(Theme::Dark);
    else if (name == "light")
        setTheme(Theme::Light);
    else
        setTheme(Theme::System);
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_current;
}

bool ThemeManager::isDark() const
{
    const QPalette pal = QApplication::palette();
    return pal.color(QPalette::Window).lightness() < 128;
}

void ThemeManager::useStyle(const QString &name)
{
    if (QApplication::style()->name().compare(name, Qt::CaseInsensitive) == 0)
        return;
    if (QStyle *style = QStyleFactory::create(name))
        QApplication::setStyle(style);
}

void ThemeManager::applyLightPalette()
{
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
    useStyle(PALETTE_STYLE);

    QPalette pal;

    pal.setColor(QPalette::Window,          QColor(245, 245, 245));
    pal.setColor(QPalette::WindowText,      QColor(30, 30, 30));
    pal.setColor(QPalette::Base,            QColor(255, 255, 255));
    pal.setColor(QPalette::AlternateBase,   QColor(240, 240, 240));
    pal.setColor(QPalette::ToolTipBase,     QColor(255, 255, 220));
    pal.setColor(QPalette::ToolTipText,     QColor(0, 0, 0));
    pal.setColor(QPalette::PlaceholderText, QColor(110, 110, 110));
    pal.setColor(QPalette::Text,            QColor(30, 30, 30));
    pal.setColor(QPalette::Button,          QColor(235, 235, 235));
    pal.setColor(QPalette::ButtonText,      QColor(30, 30, 30));
    pal.setColor(QPalette::BrightText,      QColor(255, 0, 0));
    pal.setColor(QPalette::Light,           QColor(255, 255, 255));
    pal.setColor(QPalette::Midlight,        QColor(245, 245, 245));
    pal.setColor(QPalette::Mid,             QColor(190, 190, 190));
    pal.setColor(QPalette::Dark,            QColor(160, 160, 160));
    pal.setColor(QPalette::Shadow,          QColor(105, 105, 105));
    pal.setColor(QPalette::Link,            QColor(0, 100, 200));
    pal.setColor(QPalette::LinkVisited,     QColor(110, 60, 170));
    pal.setColor(QPalette::Highlight,       QColor(0, 102, 204));
    pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    pal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(130, 130, 130));
    pal.setColor(QPalette::Disabled, QPalette::Text,         QColor(130, 130, 130));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,   QColor(130, 130, 130));
    pal.setColor(QPalette::Disabled, QPalette::Base,         QColor(240, 240, 240));
    pal.setColor(QPalette::Disabled, QPalette::Highlight,    QColor(190, 190, 190));

    QApplication::setPalette(pal);
}

void ThemeManager::applyDarkPalette()
{
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    useStyle(PALETTE_STYLE);

    QPalette pal;

    pal.setColor(QPalette::Window,          QColor(45, 45, 48));
    pal.setColor(QPalette::WindowText,      QColor(212, 212, 212));
    pal.setColor(QPalette::Base,            QColor(30, 30, 30));
    pal.setColor(QPalette::AlternateBase,   QColor(50, 50, 53));
    pal.setColor(QPalette::ToolTipBase,     QColor(60, 60, 60));
    pal.setColor(QPalette::ToolTipText,     QColor(212, 212, 212));
    pal.setColor(QPalette::PlaceholderText, QColor(140, 140, 140));
    pal.setColor(QPalette::Text,            QColor(212, 212, 212));
    pal.setColor(QPalette::Button,          QColor(60, 60, 63));
    pal.setColor(QPalette::ButtonText,      QColor(212, 212, 212));
    pal.setColor(QPalette::BrightText,      QColor(255, 50, 50));
    pal.setColor(QPalette::Light,           QColor(80, 80, 84));
    pal.setColor(QPalette::Midlight,        QColor(70, 70, 74));
    pal.setColor(QPalette::Mid,             QColor(40, 40, 42));
    pal.setColor(QPalette::Dark,            QColor(30, 30, 32));
    pal.setColor(QPalette::Shadow,          QColor(15, 15, 15));
    pal.setColor(QPalette::Link,            QColor(86, 156, 214));
    pal.setColor(QPalette::LinkVisited,     QColor(180, 140, 230));
    pal.setColor(QPalette::Highlight,       QColor(0, 102, 204));
    pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    pal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(150, 150, 150));
    pal.setColor(QPalette::Disabled, QPalette::Text,         QColor(150, 150, 150));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,   QColor(150, 150, 150));
    pal.setColor(QPalette::Disabled, QPalette::Base,         QColor(40, 40, 42));
    pal.setColor(QPalette::Disabled, QPalette::Highlight,    QColor(80, 80, 80));

    QApplication::setPalette(pal);
}

void ThemeManager::applySystemPalette()
{
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Unknown);
    useStyle(m_systemStyleName);
    QApplication::setPalette(QApplication::style()->standardPalette());
}
