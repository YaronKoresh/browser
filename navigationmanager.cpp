#include "navigationmanager.h"

NavigationManager::NavigationManager(QObject *parent)
    : QObject(parent)
{
}

bool NavigationManager::canGoBack() const
{
    return m_historyIndex > 0;
}

bool NavigationManager::canGoForward() const
{
    return m_historyIndex >= 0 && m_historyIndex < m_history.size() - 1;
}

QUrl NavigationManager::currentUrl() const
{
    if (m_historyIndex >= 0 && m_historyIndex < m_history.size())
        return m_history[m_historyIndex];
    return {};
}

int NavigationManager::historyCount() const
{
    return m_history.size();
}

const QList<QUrl> &NavigationManager::history() const
{
    return m_history;
}

int NavigationManager::currentIndex() const
{
    return m_historyIndex;
}

void NavigationManager::navigateTo(const QUrl &url)
{
    if (!url.isValid() || url.isEmpty())
        return;

    if (m_historyIndex >= 0 && m_historyIndex < m_history.size()
        && m_history[m_historyIndex] == url) {
        return;
    }

    pushUrl(url);
    emit navigationRequested(url);
    emitStateChanges();
}

void NavigationManager::goBack()
{
    if (!canGoBack())
        return;

    --m_historyIndex;
    emit currentUrlChanged(m_history[m_historyIndex]);
    emit navigationRequested(m_history[m_historyIndex]);
    emitStateChanges();
}

void NavigationManager::goForward()
{
    if (!canGoForward())
        return;

    ++m_historyIndex;
    emit currentUrlChanged(m_history[m_historyIndex]);
    emit navigationRequested(m_history[m_historyIndex]);
    emitStateChanges();
}

void NavigationManager::recordExternalNavigation(const QUrl &url)
{
    if (!url.isValid() || url.isEmpty())
        return;

    if (m_historyIndex >= 0 && m_historyIndex < m_history.size()
        && m_history[m_historyIndex] == url) {
        return;
    }

    pushUrl(url);
    emitStateChanges();
}

void NavigationManager::pushUrl(const QUrl &url)
{
    if (m_historyIndex < m_history.size() - 1) {
        m_history.erase(m_history.begin() + m_historyIndex + 1, m_history.end());
    }

    m_history.append(url);
    m_historyIndex = m_history.size() - 1;

    trimHistory();

    emit currentUrlChanged(url);
}

void NavigationManager::trimHistory()
{
    while (m_history.size() > m_maxHistorySize && m_historyIndex > 0) {
        m_history.removeFirst();
        --m_historyIndex;
    }
    while (m_history.size() > m_maxHistorySize)
        m_history.removeLast();
}

int NavigationManager::maxHistorySize() const
{
    return m_maxHistorySize;
}

void NavigationManager::setMaxHistorySize(int size)
{
    m_maxHistorySize = qMax(1, size);
    const bool hadBack = m_lastCanGoBack;
    trimHistory();
    if (hadBack != canGoBack() || m_lastCanGoForward != canGoForward())
        emitStateChanges();
}

void NavigationManager::emitStateChanges()
{
    const bool back = canGoBack();
    const bool fwd  = canGoForward();

    if (back != m_lastCanGoBack) {
        m_lastCanGoBack = back;
        emit canGoBackChanged(back);
    }
    if (fwd != m_lastCanGoForward) {
        m_lastCanGoForward = fwd;
        emit canGoForwardChanged(fwd);
    }
}
