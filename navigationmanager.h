#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include "constants.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QList>

class NavigationManager : public QObject {
    Q_OBJECT

public:
    explicit NavigationManager(QObject *parent = nullptr);

    [[nodiscard]] bool canGoBack() const;
    [[nodiscard]] bool canGoForward() const;
    [[nodiscard]] QUrl currentUrl() const;
    [[nodiscard]] int historyCount() const;
    [[nodiscard]] const QList<QUrl> &history() const;
    [[nodiscard]] int currentIndex() const;
    [[nodiscard]] int maxHistorySize() const;

public slots:
    void navigateTo(const QUrl &url);
    void goBack();
    void goForward();
    void recordExternalNavigation(const QUrl &url);
    void setMaxHistorySize(int size);

signals:
    void navigationRequested(const QUrl &url);
    void canGoBackChanged(bool canGoBack);
    void canGoForwardChanged(bool canGoForward);
    void currentUrlChanged(const QUrl &url);

private:
    void pushUrl(const QUrl &url);
    void trimHistory();
    void emitStateChanges();

    QList<QUrl> m_history;
    int         m_historyIndex = -1;
    int         m_maxHistorySize = AppConstants::MAX_HISTORY_SIZE;

    bool m_lastCanGoBack    = false;
    bool m_lastCanGoForward = false;
};

#endif // NAVIGATIONMANAGER_H
