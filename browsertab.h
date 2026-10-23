#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>
#include <QtWebEngineCore/QWebEngineNewWindowRequest>
#include <memory>

class QWebEngineView;
class QWebEnginePage;
class QVBoxLayout;
class NavigationManager;

class BrowserTab : public QWidget {
    Q_OBJECT

public:
    explicit BrowserTab(QWidget *parent = nullptr);
    ~BrowserTab() override;

    [[nodiscard]] QWebEngineView *webView() const;
    [[nodiscard]] NavigationManager *navigation() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QUrl url() const;
    [[nodiscard]] double zoomFactor() const;
    [[nodiscard]] bool isLoading() const;
    [[nodiscard]] int loadingProgress() const;

    void setPage(QWebEnginePage *page);

    [[nodiscard]] bool isPrivate() const;
    void setPrivate(bool isPrivate);

public slots:
    void loadUrl(const QUrl &url);
    void reload();
    void stop();
    void setZoomFactor(double factor);

signals:
    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void loadStarted();
    void loadProgress(int progress);
    void loadFinished(bool ok);
    void iconChanged(const QIcon &icon);
    void newTabRequested(QWebEngineNewWindowRequest &request);

private slots:
    void applyJavascriptSetting(bool enabled);
    void applyTheme();
    void onNavigationRequested(const QUrl &url);
    void onUrlChanged(const QUrl &url);
    void onNewWindowRequested(QWebEngineNewWindowRequest &request);

private:
    void watchPage(QWebEnginePage *page);

    QVBoxLayout        *m_layout     = nullptr;
    QWebEngineView     *m_webView    = nullptr;
    NavigationManager  *m_navigation = nullptr;
    bool                m_private    = false;
    bool                m_loading    = false;
    int                 m_loadingProgress = 0;
};

#endif // BROWSERTAB_H
