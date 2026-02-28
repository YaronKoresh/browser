#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <QtWidgets/QWidget>
#include <QtCore/QUrl>
#include <memory>

class QWebEngineView;
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

private slots:
    void onNavigationRequested(const QUrl &url);
    void onUrlChanged(const QUrl &url);

private:
    QVBoxLayout        *m_layout     = nullptr;
    QWebEngineView     *m_webView    = nullptr;
    NavigationManager  *m_navigation = nullptr;
};

#endif // BROWSERTAB_H
