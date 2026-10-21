#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "constants.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QSize>
#include <QtCore/QString>

class QSettings;

class SettingsManager : public QObject {
    Q_OBJECT

public:
    static SettingsManager &instance();

    [[nodiscard]] QUrl homePage() const;
    void setHomePage(const QUrl &url);

    [[nodiscard]] QSize windowSize() const;
    void setWindowSize(const QSize &size);

    [[nodiscard]] QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray &geometry);

    [[nodiscard]] QByteArray windowState() const;
    void setWindowState(const QByteArray &state);

    [[nodiscard]] QString defaultDownloadDirectory() const;
    void setDefaultDownloadDirectory(const QString &path);

    [[nodiscard]] int maxConcurrentDownloads() const;
    void setMaxConcurrentDownloads(int count);

    [[nodiscard]] int maxHistorySize() const;
    void setMaxHistorySize(int size);

    [[nodiscard]] bool javascriptEnabled() const;
    void setJavascriptEnabled(bool enabled);

    [[nodiscard]] QString defaultSearchEngine() const;
    void setDefaultSearchEngine(const QString &engineUrl);

    [[nodiscard]] double zoomFactor() const;
    void setZoomFactor(double factor);

    [[nodiscard]] QString theme() const;
    void setTheme(const QString &name);

    [[nodiscard]] QStringList lastSessionUrls() const;
    void setLastSessionUrls(const QStringList &urls);

signals:
    void homePageChanged(const QUrl &url);
    void windowSizeChanged(const QSize &size);
    void downloadDirectoryChanged(const QString &path);
    void maxConcurrentDownloadsChanged(int count);
    void maxHistorySizeChanged(int size);
    void javascriptEnabledChanged(bool enabled);
    void searchEngineChanged(const QString &engineUrl);
    void zoomFactorChanged(double factor);
    void themeChanged(const QString &name);
    void settingChanged(const QString &key);

private:
    SettingsManager();
    ~SettingsManager() override = default;
    SettingsManager(const SettingsManager &) = delete;
    SettingsManager &operator=(const SettingsManager &) = delete;

    QSettings *m_settings = nullptr;
};

#endif // SETTINGSMANAGER_H
