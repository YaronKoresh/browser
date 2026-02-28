#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QtWidgets/QTabWidget>
#include <QtCore/QUrl>

class BrowserTab;

class TabManager : public QTabWidget {
    Q_OBJECT

public:
    explicit TabManager(QWidget *parent = nullptr);

    BrowserTab *createTab(const QUrl &url = QUrl());
    [[nodiscard]] BrowserTab *currentTab() const;
    [[nodiscard]] BrowserTab *tabAt(int index) const;
    void closeTab(int index);
    void closeCurrentTab();

signals:
    void currentTabChanged(BrowserTab *tab);
    void tabCreated(BrowserTab *tab);
    void tabClosed(int index);
    void lastTabClosed();

private slots:
    void onCurrentChanged(int index);
    void onTabCloseRequested(int index);
    void onTabTitleChanged(const QString &title);

private:
    void connectTabSignals(BrowserTab *tab);
};

#endif // TABMANAGER_H
