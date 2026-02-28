#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QString>

class QApplication;

class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum class Theme { System, Light, Dark };

    static ThemeManager &instance();

    void setTheme(Theme theme);
    void setTheme(const QString &name);

    [[nodiscard]] Theme currentTheme() const;
    [[nodiscard]] bool isDark() const;

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager();
    ~ThemeManager() override = default;

    void applyLightPalette();
    void applyDarkPalette();
    void applySystemPalette();

    Theme m_current = Theme::System;
};

#endif // THEMEMANAGER_H
