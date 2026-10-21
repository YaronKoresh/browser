#include "settingsmanager.h"

#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

SettingsManager &SettingsManager::instance()
{
    static SettingsManager s_instance;
    return s_instance;
}

SettingsManager::SettingsManager()
    : QObject(nullptr)
    , m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                               AppConstants::APP_ORGANIZATION,
                               AppConstants::APP_NAME, this))
{
}

QUrl SettingsManager::homePage() const
{
    return QUrl(m_settings->value(AppConstants::KEY_HOME_PAGE,
                                  AppConstants::DEFAULT_HOME_URL).toString());
}

void SettingsManager::setHomePage(const QUrl &url)
{
    if (homePage() == url)
        return;
    m_settings->setValue(AppConstants::KEY_HOME_PAGE, url.toString());
    emit homePageChanged(url);
    emit settingChanged(AppConstants::KEY_HOME_PAGE);
}

QSize SettingsManager::windowSize() const
{
    return m_settings->value(AppConstants::KEY_WINDOW_SIZE,
                              QSize(AppConstants::DEFAULT_WINDOW_WIDTH,
                                    AppConstants::DEFAULT_WINDOW_HEIGHT)).toSize();
}

void SettingsManager::setWindowSize(const QSize &size)
{
    if (windowSize() == size)
        return;
    m_settings->setValue(AppConstants::KEY_WINDOW_SIZE, size);
    emit windowSizeChanged(size);
    emit settingChanged(AppConstants::KEY_WINDOW_SIZE);
}

QByteArray SettingsManager::windowGeometry() const
{
    return m_settings->value(AppConstants::KEY_WINDOW_GEOMETRY).toByteArray();
}

void SettingsManager::setWindowGeometry(const QByteArray &geometry)
{
    m_settings->setValue(AppConstants::KEY_WINDOW_GEOMETRY, geometry);
}

QByteArray SettingsManager::windowState() const
{
    return m_settings->value(AppConstants::KEY_WINDOW_STATE).toByteArray();
}

void SettingsManager::setWindowState(const QByteArray &state)
{
    m_settings->setValue(AppConstants::KEY_WINDOW_STATE, state);
}

QString SettingsManager::defaultDownloadDirectory() const
{
    const QString fallback =
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    return m_settings->value(AppConstants::KEY_DOWNLOAD_DIR, fallback).toString();
}

void SettingsManager::setDefaultDownloadDirectory(const QString &path)
{
    if (defaultDownloadDirectory() == path)
        return;
    m_settings->setValue(AppConstants::KEY_DOWNLOAD_DIR, path);
    emit downloadDirectoryChanged(path);
    emit settingChanged(AppConstants::KEY_DOWNLOAD_DIR);
}

int SettingsManager::maxConcurrentDownloads() const
{
    return m_settings->value(AppConstants::KEY_MAX_CONCURRENT_DL,
                              AppConstants::MAX_CONCURRENT_DOWNLOADS).toInt();
}

void SettingsManager::setMaxConcurrentDownloads(int count)
{
    count = qMax(1, count);
    if (maxConcurrentDownloads() == count)
        return;
    m_settings->setValue(AppConstants::KEY_MAX_CONCURRENT_DL, count);
    emit maxConcurrentDownloadsChanged(count);
    emit settingChanged(AppConstants::KEY_MAX_CONCURRENT_DL);
}

int SettingsManager::maxHistorySize() const
{
    return m_settings->value(AppConstants::KEY_MAX_HISTORY,
                              AppConstants::MAX_HISTORY_SIZE).toInt();
}

void SettingsManager::setMaxHistorySize(int size)
{
    size = qMax(10, size);
    if (maxHistorySize() == size)
        return;
    m_settings->setValue(AppConstants::KEY_MAX_HISTORY, size);
    emit maxHistorySizeChanged(size);
    emit settingChanged(AppConstants::KEY_MAX_HISTORY);
}

bool SettingsManager::javascriptEnabled() const
{
    return m_settings->value(AppConstants::KEY_JS_ENABLED, true).toBool();
}

void SettingsManager::setJavascriptEnabled(bool enabled)
{
    if (javascriptEnabled() == enabled)
        return;
    m_settings->setValue(AppConstants::KEY_JS_ENABLED, enabled);
    emit javascriptEnabledChanged(enabled);
    emit settingChanged(AppConstants::KEY_JS_ENABLED);
}

QString SettingsManager::defaultSearchEngine() const
{
    return m_settings->value(AppConstants::KEY_SEARCH_ENGINE,
                              AppConstants::DEFAULT_SEARCH_URL).toString();
}

void SettingsManager::setDefaultSearchEngine(const QString &engineUrl)
{
    const QUrl parsed(QString(engineUrl).replace("%1", "query"), QUrl::StrictMode);
    const bool webScheme = parsed.scheme() == "http" || parsed.scheme() == "https";
    if (!engineUrl.contains("%1") || !parsed.isValid() || !webScheme || parsed.host().isEmpty())
        return;
    if (defaultSearchEngine() == engineUrl)
        return;
    m_settings->setValue(AppConstants::KEY_SEARCH_ENGINE, engineUrl);
    emit searchEngineChanged(engineUrl);
    emit settingChanged(AppConstants::KEY_SEARCH_ENGINE);
}

double SettingsManager::zoomFactor() const
{
    return m_settings->value(AppConstants::KEY_ZOOM_FACTOR,
                              AppConstants::ZOOM_DEFAULT).toDouble();
}

void SettingsManager::setZoomFactor(double factor)
{
    factor = qBound(AppConstants::ZOOM_MIN, factor, AppConstants::ZOOM_MAX);
    if (qFuzzyCompare(zoomFactor(), factor))
        return;
    m_settings->setValue(AppConstants::KEY_ZOOM_FACTOR, factor);
    emit zoomFactorChanged(factor);
    emit settingChanged(AppConstants::KEY_ZOOM_FACTOR);
}

QStringList SettingsManager::lastSessionUrls() const
{
    return m_settings->value(AppConstants::KEY_LAST_SESSION_URLS).toStringList();
}

void SettingsManager::setLastSessionUrls(const QStringList &urls)
{
    m_settings->setValue(AppConstants::KEY_LAST_SESSION_URLS, urls);
}
