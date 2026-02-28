#include <QtTest/QTest>
#include <QtCore/QSignalSpy>
#include "navigationmanager.h"

class TestNavigation : public QObject {
    Q_OBJECT

private slots:
    void init()
    {
        m_nav = new NavigationManager(this);
    }

    void cleanup()
    {
        delete m_nav;
        m_nav = nullptr;
    }

    void initialState_empty()
    {
        QVERIFY(!m_nav->canGoBack());
        QVERIFY(!m_nav->canGoForward());
        QCOMPARE(m_nav->historyCount(), 0);
        QCOMPARE(m_nav->currentIndex(), -1);
        QVERIFY(m_nav->currentUrl().isEmpty());
    }

    void navigateTo_singleUrl()
    {
        QSignalSpy navSpy(m_nav, &NavigationManager::navigationRequested);

        m_nav->navigateTo(QUrl("https://example.com"));

        QCOMPARE(m_nav->historyCount(), 1);
        QCOMPARE(m_nav->currentIndex(), 0);
        QCOMPARE(m_nav->currentUrl(), QUrl("https://example.com"));
        QCOMPARE(navSpy.count(), 1);
        QVERIFY(!m_nav->canGoBack());
        QVERIFY(!m_nav->canGoForward());
    }

    void navigateTo_multipleUrls()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        m_nav->navigateTo(QUrl("https://b.com"));
        m_nav->navigateTo(QUrl("https://c.com"));

        QCOMPARE(m_nav->historyCount(), 3);
        QCOMPARE(m_nav->currentIndex(), 2);
        QVERIFY(m_nav->canGoBack());
        QVERIFY(!m_nav->canGoForward());
    }

    void navigateTo_duplicateIgnored()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        m_nav->navigateTo(QUrl("https://a.com"));

        QCOMPARE(m_nav->historyCount(), 1);
    }

    void navigateTo_invalidUrlIgnored()
    {
        m_nav->navigateTo(QUrl());
        QCOMPARE(m_nav->historyCount(), 0);
    }

    void goBack_basic()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        m_nav->navigateTo(QUrl("https://b.com"));

        QSignalSpy navSpy(m_nav, &NavigationManager::navigationRequested);
        m_nav->goBack();

        QCOMPARE(m_nav->currentUrl(), QUrl("https://a.com"));
        QCOMPARE(m_nav->currentIndex(), 0);
        QVERIFY(!m_nav->canGoBack());
        QVERIFY(m_nav->canGoForward());
        QCOMPARE(navSpy.count(), 1);
    }

    void goBack_noop_atStart()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        QSignalSpy navSpy(m_nav, &NavigationManager::navigationRequested);
        m_nav->goBack();

        QCOMPARE(navSpy.count(), 0);
        QCOMPARE(m_nav->currentUrl(), QUrl("https://a.com"));
    }

    void goForward_basic()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        m_nav->navigateTo(QUrl("https://b.com"));
        m_nav->goBack();

        QSignalSpy navSpy(m_nav, &NavigationManager::navigationRequested);
        m_nav->goForward();

        QCOMPARE(m_nav->currentUrl(), QUrl("https://b.com"));
        QCOMPARE(navSpy.count(), 1);
    }

    void goForward_noop_atEnd()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        QSignalSpy navSpy(m_nav, &NavigationManager::navigationRequested);
        m_nav->goForward();

        QCOMPARE(navSpy.count(), 0);
    }

    void navigateAfterBack_truncatesForward()
    {
        m_nav->navigateTo(QUrl("https://a.com"));
        m_nav->navigateTo(QUrl("https://b.com"));
        m_nav->navigateTo(QUrl("https://c.com"));
        m_nav->goBack();
        m_nav->goBack();

        m_nav->navigateTo(QUrl("https://d.com"));

        QCOMPARE(m_nav->historyCount(), 2);
        QCOMPARE(m_nav->currentUrl(), QUrl("https://d.com"));
        QVERIFY(!m_nav->canGoForward());
    }

    void history_bounded()
    {
        for (int i = 0; i < AppConstants::MAX_HISTORY_SIZE + 10; ++i) {
            m_nav->navigateTo(QUrl(QString("https://%1.com").arg(i)));
        }

        QCOMPARE(m_nav->historyCount(), AppConstants::MAX_HISTORY_SIZE);
        QCOMPARE(m_nav->currentUrl(),
                 QUrl(QString("https://%1.com")
                          .arg(AppConstants::MAX_HISTORY_SIZE + 9)));
    }

    void recordExternal_doesNotEmitNavRequest()
    {
        QSignalSpy navSpy(m_nav, &NavigationManager::navigationRequested);
        m_nav->recordExternalNavigation(QUrl("https://redirect.com"));

        QCOMPARE(navSpy.count(), 0);
        QCOMPARE(m_nav->historyCount(), 1);
        QCOMPARE(m_nav->currentUrl(), QUrl("https://redirect.com"));
    }

    void signals_canGoBackChanged()
    {
        QSignalSpy backSpy(m_nav, &NavigationManager::canGoBackChanged);

        m_nav->navigateTo(QUrl("https://a.com"));
        QCOMPARE(backSpy.count(), 0);

        m_nav->navigateTo(QUrl("https://b.com"));
        QCOMPARE(backSpy.count(), 1);
        QCOMPARE(backSpy.last().first().toBool(), true);

        m_nav->goBack();
        QCOMPARE(backSpy.count(), 2);
        QCOMPARE(backSpy.last().first().toBool(), false);
    }

private:
    NavigationManager *m_nav = nullptr;
};

QTEST_MAIN(TestNavigation)
#include "test_navigation.moc"
