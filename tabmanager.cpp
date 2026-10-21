#include "tabmanager.h"
#include "browsertab.h"

#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(lcTabs, "browser.tabs")

TabManager::TabManager(QWidget *parent)
    : QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);
    setDocumentMode(true);

    connect(this, &QTabWidget::currentChanged,
            this, &TabManager::onCurrentChanged);
    connect(this, &QTabWidget::tabCloseRequested,
            this, &TabManager::onTabCloseRequested);
}

BrowserTab *TabManager::createTab(const QUrl &url)
{
    auto *tab = new BrowserTab(this);
    connectTabSignals(tab);

    const int idx = addTab(tab, tr("New Tab"));
    setCurrentIndex(idx);

    if (url.isValid() && !url.isEmpty())
        tab->loadUrl(url);

    qCInfo(lcTabs) << "Created tab" << idx << url.toString();
    emit tabCreated(tab);
    return tab;
}

BrowserTab *TabManager::currentTab() const
{
    return qobject_cast<BrowserTab *>(currentWidget());
}

BrowserTab *TabManager::tabAt(int index) const
{
    return qobject_cast<BrowserTab *>(widget(index));
}

void TabManager::closeTab(int index)
{
    if (index < 0 || index >= count())
        return;

    QWidget *w = widget(index);
    removeTab(index);
    w->deleteLater();

    qCInfo(lcTabs) << "Closed tab" << index << "remaining:" << count();
    emit tabClosed(index);

    if (count() == 0)
        emit lastTabClosed();
}

QStringList TabManager::sessionUrls() const
{
    QStringList urls;
    for (int i = 0; i < count(); ++i) {
        const auto *tab = tabAt(i);
        if (!tab || tab->isPrivate())
            continue;
        const QString url = tab->url().toString();
        if (!url.isEmpty())
            urls.append(url);
    }
    return urls;
}

void TabManager::closeCurrentTab()
{
    closeTab(currentIndex());
}

void TabManager::onCurrentChanged(int index)
{
    auto *tab = tabAt(index);
    emit currentTabChanged(tab);
}

void TabManager::onTabCloseRequested(int index)
{
    closeTab(index);
}

void TabManager::onTabTitleChanged(const QString &title)
{
    auto *tab = qobject_cast<BrowserTab *>(sender());
    if (!tab)
        return;

    const int idx = indexOf(tab);
    if (idx >= 0) {
        QString display = title.isEmpty() ? tr("New Tab") : title;
        if (tab->isPrivate())
            display = tr("(Private) %1").arg(display);
        setTabText(idx, display);
        setTabToolTip(idx, title);
    }
}

void TabManager::connectTabSignals(BrowserTab *tab)
{
    connect(tab, &BrowserTab::titleChanged,
            this, &TabManager::onTabTitleChanged);
}
