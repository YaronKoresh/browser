#include <QtTest/QTest>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEngineSettings>
#include <QtWebEngineCore/QWebEnginePage>
#include <QtWebEngineCore/QWebEngineProfile>
#include <QtTest/QSignalSpy>
#include "browsertab.h"
#include "tabmanager.h"
#include "settingsmanager.h"
#include "navigationmanager.h"

class TestBrowserTab : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_configDir;

private slots:
    void initTestCase()
    {
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_configDir.path());
    }

    void javascript_disabledSettingIsAppliedToNewTab()
    {
        auto &s = SettingsManager::instance();
        s.setJavascriptEnabled(false);

        BrowserTab tab;
        QVERIFY(!tab.webView()->settings()->testAttribute(
            QWebEngineSettings::JavascriptEnabled));

        s.setJavascriptEnabled(true);
    }

    void javascript_settingChangeReachesOpenTab()
    {
        auto &s = SettingsManager::instance();
        s.setJavascriptEnabled(true);

        BrowserTab tab;
        QVERIFY(tab.webView()->settings()->testAttribute(
            QWebEngineSettings::JavascriptEnabled));

        s.setJavascriptEnabled(false);
        QVERIFY(!tab.webView()->settings()->testAttribute(
            QWebEngineSettings::JavascriptEnabled));

        s.setJavascriptEnabled(true);
        QVERIFY(tab.webView()->settings()->testAttribute(
            QWebEngineSettings::JavascriptEnabled));
    }

    void javascript_disabledSettingIsAppliedToReplacedPage()
    {
        auto &s = SettingsManager::instance();
        s.setJavascriptEnabled(false);

        BrowserTab tab;
        auto *profile = new QWebEngineProfile(&tab);
        tab.setPage(new QWebEnginePage(profile, tab.webView()));
        QVERIFY(!tab.webView()->settings()->testAttribute(
            QWebEngineSettings::JavascriptEnabled));

        s.setJavascriptEnabled(true);
        QVERIFY(tab.webView()->settings()->testAttribute(
            QWebEngineSettings::JavascriptEnabled));
    }

    void sessionUrls_skipPrivateAndBlankTabs()
    {
        TabManager tabs;
        const QUrl page("data:text/html,<p>session</p>");

        auto *normal = tabs.createTab();
        QSignalSpy normalLoaded(normal, &BrowserTab::loadFinished);
        normal->loadUrl(page);
        QTRY_VERIFY_WITH_TIMEOUT(normalLoaded.count() >= 1, 10000);

        auto *priv = tabs.createTab();
        priv->setPrivate(true);
        QVERIFY(priv->isPrivate());
        QSignalSpy privLoaded(priv, &BrowserTab::loadFinished);
        priv->loadUrl(QUrl("data:text/html,<p>secret</p>"));
        QTRY_VERIFY_WITH_TIMEOUT(privLoaded.count() >= 1, 10000);

        tabs.createTab();

        QCOMPARE(tabs.sessionUrls(), QStringList{page.toString()});
    }

    void zoom_savedFactorIsAppliedToNewTab()
    {
        auto &s = SettingsManager::instance();
        s.setZoomFactor(1.5);

        BrowserTab tab;
        QVERIFY(qFuzzyCompare(tab.zoomFactor(), 1.5));

        s.setZoomFactor(AppConstants::ZOOM_DEFAULT);
    }

    void zoom_settingChangeReachesOpenTab()
    {
        auto &s = SettingsManager::instance();
        s.setZoomFactor(AppConstants::ZOOM_DEFAULT);

        BrowserTab tab;
        s.setZoomFactor(2.0);
        QVERIFY(qFuzzyCompare(tab.zoomFactor(), 2.0));

        s.setZoomFactor(AppConstants::ZOOM_DEFAULT);
        QVERIFY(qFuzzyCompare(tab.zoomFactor(), AppConstants::ZOOM_DEFAULT));
    }

    void zoom_savedFactorIsAppliedToReplacedPage()
    {
        auto &s = SettingsManager::instance();
        s.setZoomFactor(1.25);

        BrowserTab tab;
        auto *profile = new QWebEngineProfile(&tab);
        tab.setPage(new QWebEnginePage(profile, tab.webView()));
        QVERIFY(qFuzzyCompare(tab.zoomFactor(), 1.25));

        s.setZoomFactor(AppConstants::ZOOM_DEFAULT);
    }

    void newWindow_scriptOpenedWindowBecomesTab()
    {
        TabManager tabs;
        auto *opener = tabs.createTab();
        QSignalSpy loaded(opener, &BrowserTab::loadFinished);
        opener->loadUrl(QUrl("data:text/html,<p>opener</p>"));
        QTRY_VERIFY_WITH_TIMEOUT(loaded.count() >= 1, 10000);
        QCOMPARE(tabs.count(), 1);

        opener->webView()->page()->runJavaScript("window.open('about:blank#popup');");

        QTRY_COMPARE_WITH_TIMEOUT(tabs.count(), 2, 10000);
        QVERIFY(!tabs.tabAt(1)->isPrivate());
        QCOMPARE(tabs.currentTab(), tabs.tabAt(1));
    }

    void newWindow_privateTabOpensPrivatePopupInSameProfile()
    {
        TabManager tabs;
        auto *opener = tabs.createTab();
        auto *profile = new QWebEngineProfile(opener);
        opener->setPage(new QWebEnginePage(profile, opener->webView()));
        opener->setPrivate(true);
        QSignalSpy loaded(opener, &BrowserTab::loadFinished);
        opener->loadUrl(QUrl("data:text/html,<p>secret</p>"));
        QTRY_VERIFY_WITH_TIMEOUT(loaded.count() >= 1, 10000);

        opener->webView()->page()->runJavaScript("window.open('about:blank#popup');");

        QTRY_COMPARE_WITH_TIMEOUT(tabs.count(), 2, 10000);
        QVERIFY(tabs.tabAt(1)->isPrivate());
        QCOMPARE(tabs.tabAt(1)->webView()->page()->profile(), profile);
        QVERIFY(profile->isOffTheRecord());
    }

    void history_limitFromSettingsReachesNavigation()
    {
        auto &s = SettingsManager::instance();
        s.setMaxHistorySize(10);

        BrowserTab tab;
        QCOMPARE(tab.navigation()->maxHistorySize(), 10);

        s.setMaxHistorySize(50);
        QCOMPARE(tab.navigation()->maxHistorySize(), 50);

        s.setMaxHistorySize(AppConstants::MAX_HISTORY_SIZE);
    }
};

QTEST_MAIN(TestBrowserTab)
#include "test_browsertab.moc"
