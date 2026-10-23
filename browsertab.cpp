#include "browsertab.h"
#include "navigationmanager.h"
#include "settingsmanager.h"
#include "thememanager.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEnginePage>
#include <QtWebEngineCore/QWebEngineSettings>

BrowserTab::BrowserTab(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_webView(new QWebEngineView(this))
    , m_navigation(new NavigationManager(this))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_webView);
    setLayout(m_layout);

    watchPage(m_webView->page());

    m_navigation->setMaxHistorySize(SettingsManager::instance().maxHistorySize());
    connect(&SettingsManager::instance(), &SettingsManager::maxHistorySizeChanged,
            m_navigation, &NavigationManager::setMaxHistorySize);

    applyJavascriptSetting(SettingsManager::instance().javascriptEnabled());
    connect(&SettingsManager::instance(), &SettingsManager::javascriptEnabledChanged,
            this, &BrowserTab::applyJavascriptSetting);

    applyTheme();
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &BrowserTab::applyTheme);

    setZoomFactor(SettingsManager::instance().zoomFactor());
    connect(&SettingsManager::instance(), &SettingsManager::zoomFactorChanged,
            this, &BrowserTab::setZoomFactor);

    connect(m_navigation, &NavigationManager::navigationRequested,
            this, &BrowserTab::onNavigationRequested);

    connect(m_webView, &QWebEngineView::titleChanged,
            this, &BrowserTab::titleChanged);
    connect(m_webView, &QWebEngineView::iconChanged,
            this, &BrowserTab::iconChanged);
    connect(m_webView, &QWebEngineView::loadStarted,
            this, &BrowserTab::loadStarted);
    connect(m_webView, &QWebEngineView::loadProgress,
            this, &BrowserTab::loadProgress);
    connect(m_webView, &QWebEngineView::loadFinished,
            this, &BrowserTab::loadFinished);

    connect(m_webView, &QWebEngineView::urlChanged,
            this, &BrowserTab::onUrlChanged);

    connect(this, &BrowserTab::loadStarted, this, [this]() {
        m_loading = true;
        m_loadingProgress = 0;
    });
    connect(this, &BrowserTab::loadProgress, this, [this](int progress) {
        m_loadingProgress = progress;
    });
    connect(this, &BrowserTab::loadFinished, this, [this](bool) {
        m_loading = false;
        m_loadingProgress = 0;
    });
}

BrowserTab::~BrowserTab() = default;

QWebEngineView *BrowserTab::webView() const { return m_webView; }
NavigationManager *BrowserTab::navigation() const { return m_navigation; }

QString BrowserTab::title() const
{
    return m_webView->title();
}

QUrl BrowserTab::url() const
{
    return m_webView->url();
}

double BrowserTab::zoomFactor() const
{
    return m_webView->zoomFactor();
}

bool BrowserTab::isLoading() const
{
    return m_loading;
}

int BrowserTab::loadingProgress() const
{
    return m_loadingProgress;
}

void BrowserTab::setPage(QWebEnginePage *page)
{
    m_webView->setPage(page);
    watchPage(page);
    applyJavascriptSetting(SettingsManager::instance().javascriptEnabled());
    applyTheme();
    setZoomFactor(SettingsManager::instance().zoomFactor());
}

bool BrowserTab::isPrivate() const
{
    return m_private;
}

void BrowserTab::setPrivate(bool isPrivate)
{
    m_private = isPrivate;
}

void BrowserTab::loadUrl(const QUrl &url)
{
    m_navigation->navigateTo(url);
}

void BrowserTab::reload()
{
    m_webView->reload();
}

void BrowserTab::stop()
{
    m_webView->stop();
}

void BrowserTab::setZoomFactor(double factor)
{
    m_webView->setZoomFactor(factor);
}

void BrowserTab::applyJavascriptSetting(bool enabled)
{
    m_webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, enabled);
}

void BrowserTab::applyTheme()
{
    // Pages keep their own (usually white) colours unless Chromium is told to
    // darken them, so the web view has to follow the browser theme explicitly.
    m_webView->settings()->setAttribute(QWebEngineSettings::ForceDarkMode,
                                        ThemeManager::instance().isDark());
}

void BrowserTab::onNavigationRequested(const QUrl &url)
{
    m_webView->load(url);
    m_webView->setFocus();
}

void BrowserTab::onUrlChanged(const QUrl &url)
{
    m_navigation->recordExternalNavigation(url);
    emit urlChanged(url);
}

void BrowserTab::watchPage(QWebEnginePage *page)
{
    connect(page, &QWebEnginePage::newWindowRequested,
            this, &BrowserTab::onNewWindowRequested);
}

void BrowserTab::onNewWindowRequested(QWebEngineNewWindowRequest &request)
{
    emit newTabRequested(request);
}
