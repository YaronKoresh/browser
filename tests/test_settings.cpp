#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include "settingsmanager.h"
#include "constants.h"

class TestSettings : public QObject {
    Q_OBJECT

private slots:

    void defaults_homePage()
    {
        auto &s = SettingsManager::instance();
        QCOMPARE(s.homePage(), QUrl(AppConstants::DEFAULT_HOME_URL));
    }

    void defaults_windowSize()
    {
        auto &s = SettingsManager::instance();
        QCOMPARE(s.windowSize(),
                 QSize(AppConstants::DEFAULT_WINDOW_WIDTH,
                       AppConstants::DEFAULT_WINDOW_HEIGHT));
    }

    void defaults_maxConcurrentDownloads()
    {
        auto &s = SettingsManager::instance();
        QCOMPARE(s.maxConcurrentDownloads(), AppConstants::MAX_CONCURRENT_DOWNLOADS);
    }

    void defaults_maxHistorySize()
    {
        auto &s = SettingsManager::instance();
        QCOMPARE(s.maxHistorySize(), AppConstants::MAX_HISTORY_SIZE);
    }

    void defaults_javascriptEnabled()
    {
        auto &s = SettingsManager::instance();
        QVERIFY(s.javascriptEnabled());
    }

    void defaults_zoomFactor()
    {
        auto &s = SettingsManager::instance();
        QCOMPARE(s.zoomFactor(), AppConstants::ZOOM_DEFAULT);
    }

    void defaults_searchEngine()
    {
        auto &s = SettingsManager::instance();
        QCOMPARE(s.defaultSearchEngine(), QString(AppConstants::DEFAULT_SEARCH_URL));
    }

    void setHomePage_emitsSignal()
    {
        auto &s = SettingsManager::instance();
        QSignalSpy spy(&s, &SettingsManager::homePageChanged);

        QUrl testUrl("https://test.example.com");
        s.setHomePage(testUrl);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toUrl(), testUrl);

        s.setHomePage(QUrl(AppConstants::DEFAULT_HOME_URL));
    }

    void setHomePage_noDuplicate()
    {
        auto &s = SettingsManager::instance();
        QUrl current = s.homePage();
        QSignalSpy spy(&s, &SettingsManager::homePageChanged);

        s.setHomePage(current);
        QCOMPARE(spy.count(), 0);
    }

    void setMaxConcurrent_clampedToMin()
    {
        auto &s = SettingsManager::instance();
        s.setMaxConcurrentDownloads(0);
        QCOMPARE(s.maxConcurrentDownloads(), 1);

        s.setMaxConcurrentDownloads(AppConstants::MAX_CONCURRENT_DOWNLOADS);
    }

    void setMaxHistory_clampedToMin()
    {
        auto &s = SettingsManager::instance();
        s.setMaxHistorySize(1);
        QCOMPARE(s.maxHistorySize(), 10);

        s.setMaxHistorySize(AppConstants::MAX_HISTORY_SIZE);
    }

    void setSearchEngine_rejectsTemplatesWithoutQueryPlaceholder()
    {
        auto &s = SettingsManager::instance();
        const QString before = s.defaultSearchEngine();
        QSignalSpy spy(&s, &SettingsManager::searchEngineChanged);

        s.setDefaultSearchEngine("");
        s.setDefaultSearchEngine("https://example.com/search");
        s.setDefaultSearchEngine("ftp://example.com/?q=%1");
        s.setDefaultSearchEngine("not a url %1");

        QCOMPARE(s.defaultSearchEngine(), before);
        QCOMPARE(spy.count(), 0);
    }

    void setSearchEngine_acceptsValidTemplate()
    {
        auto &s = SettingsManager::instance();
        const QString before = s.defaultSearchEngine();

        s.setDefaultSearchEngine("https://duckduckgo.com/?q=%1");
        QCOMPARE(s.defaultSearchEngine(), QString("https://duckduckgo.com/?q=%1"));

        s.setDefaultSearchEngine(before);
    }

    void setZoomFactor_clamped()
    {
        auto &s = SettingsManager::instance();
        s.setZoomFactor(100.0);
        QCOMPARE(s.zoomFactor(), AppConstants::ZOOM_MAX);

        s.setZoomFactor(0.01);
        QCOMPARE(s.zoomFactor(), AppConstants::ZOOM_MIN);

        s.setZoomFactor(AppConstants::ZOOM_DEFAULT);
    }

    void sessionUrls_roundTrip()
    {
        auto &s = SettingsManager::instance();
        QStringList urls = {"https://a.com", "https://b.com", "https://c.com"};
        s.setLastSessionUrls(urls);
        QCOMPARE(s.lastSessionUrls(), urls);

        s.setLastSessionUrls({});
    }

    void settingChanged_emitted()
    {
        auto &s = SettingsManager::instance();
        QSignalSpy spy(&s, &SettingsManager::settingChanged);

        s.setJavascriptEnabled(!s.javascriptEnabled());
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toString(), QString(AppConstants::KEY_JS_ENABLED));

        s.setJavascriptEnabled(true);
    }
};

QTEST_MAIN(TestSettings)
#include "test_settings.moc"
