#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QtWidgets/QTabWidget>
#include <QtCore/QUrl>

class BrowserTab;
class QWebEngineNewWindowRequest;

class TabManager : public QTabWidget {
    Q_OBJECT

public:
    explicit TabManager(QWidget *parent = nullptr);

    BrowserTab *createTab(const QUrl &url = QUrl());
    [[nodiscard]] BrowserTab *currentTab() const;
    [[nodiscard]] BrowserTab *tabAt(int index) const;
    void closeTab(int index);
    void closeCurrentTab();

    [[nodiscard]] QStringList sessionUrls() const;

signals:
    void currentTabChanged(BrowserTab *tab);
    void tabCreated(BrowserTab *tab);
    void tabClosed(int index);
    void lastTabClosed();

private slots:
    void onCurrentChanged(int index);
    void onTabCloseRequested(int index);
    void onTabTitleChanged(const QString &title);
    void onNewTabRequested(QWebEngineNewWindowRequest &request);

private:
    void connectTabSignals(BrowserTab *tab);
};

#endif // TABMANAGER_H
