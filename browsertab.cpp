#include "browsertab.h"
#include "navigationmanager.h"
#include "settingsmanager.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEnginePage>

BrowserTab::BrowserTab(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_webView(new QWebEngineView(this))
    , m_navigation(new NavigationManager(this))
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_webView);
    setLayout(m_layout);

    m_navigation->setMaxHistorySize(SettingsManager::instance().maxHistorySize());
    connect(&SettingsManager::instance(), &SettingsManager::maxHistorySizeChanged,
            m_navigation, &NavigationManager::setMaxHistorySize);

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
