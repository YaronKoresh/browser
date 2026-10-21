#include "thememanager.h"
#include "settingsmanager.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtGui/QStyleHints>

ThemeManager &ThemeManager::instance()
{
    static ThemeManager s_instance;
    return s_instance;
}

ThemeManager::ThemeManager()
    : QObject(nullptr)
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

void ThemeManager::applyLightPalette()
{
    QPalette pal;

    pal.setColor(QPalette::Window,          QColor(245, 245, 245));
    pal.setColor(QPalette::WindowText,      QColor(30, 30, 30));
    pal.setColor(QPalette::Base,            QColor(255, 255, 255));
    pal.setColor(QPalette::AlternateBase,   QColor(240, 240, 240));
    pal.setColor(QPalette::ToolTipBase,     QColor(255, 255, 220));
    pal.setColor(QPalette::ToolTipText,     QColor(0, 0, 0));
    pal.setColor(QPalette::Text,            QColor(30, 30, 30));
    pal.setColor(QPalette::Button,          QColor(235, 235, 235));
    pal.setColor(QPalette::ButtonText,      QColor(30, 30, 30));
    pal.setColor(QPalette::BrightText,      QColor(255, 0, 0));
    pal.setColor(QPalette::Link,            QColor(0, 100, 200));
    pal.setColor(QPalette::Highlight,       QColor(0, 120, 215));
    pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    pal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(150, 150, 150));
    pal.setColor(QPalette::Disabled, QPalette::Text,         QColor(150, 150, 150));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,   QColor(150, 150, 150));

    QApplication::setPalette(pal);
}

void ThemeManager::applyDarkPalette()
{
    QPalette pal;

    pal.setColor(QPalette::Window,          QColor(45, 45, 48));
    pal.setColor(QPalette::WindowText,      QColor(212, 212, 212));
    pal.setColor(QPalette::Base,            QColor(30, 30, 30));
    pal.setColor(QPalette::AlternateBase,   QColor(50, 50, 53));
    pal.setColor(QPalette::ToolTipBase,     QColor(60, 60, 60));
    pal.setColor(QPalette::ToolTipText,     QColor(212, 212, 212));
    pal.setColor(QPalette::Text,            QColor(212, 212, 212));
    pal.setColor(QPalette::Button,          QColor(60, 60, 63));
    pal.setColor(QPalette::ButtonText,      QColor(212, 212, 212));
    pal.setColor(QPalette::BrightText,      QColor(255, 50, 50));
    pal.setColor(QPalette::Link,            QColor(86, 156, 214));
    pal.setColor(QPalette::Highlight,       QColor(0, 120, 215));
    pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    pal.setColor(QPalette::Disabled, QPalette::WindowText,  QColor(100, 100, 100));
    pal.setColor(QPalette::Disabled, QPalette::Text,         QColor(100, 100, 100));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,   QColor(100, 100, 100));

    QApplication::setPalette(pal);
}

void ThemeManager::applySystemPalette()
{
    QApplication::setPalette(QApplication::style()->standardPalette());
}
