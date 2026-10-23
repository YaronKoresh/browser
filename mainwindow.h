#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtGui/QCloseEvent>

class QLineEdit;
class QPushButton;
class QProgressBar;
class QAction;
class QCompleter;
class QStringListModel;
class QWebEngineDownloadRequest;
class QTextEdit;
class QDialog;
class QListView;

class TabManager;
class BrowserTab;
class DownloadManager;
class BookmarkManager;
class FindBar;
class QVBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void navigateFromAddressBar();
    void onBackClicked();
    void onForwardClicked();

    void onCurrentTabChanged(BrowserTab *tab);
    void onTabTitleChanged(const QString &title);
    void onTabUrlChanged(const QUrl &url);
    void onTabLoadStarted();
    void onTabLoadProgress(int progress);
    void onTabLoadFinished(bool ok);
    void onLastTabClosed();

    void onDownloadRequested(QWebEngineDownloadRequest *download);
    void onDownloadCompleted(int id, const QString &savePath);
    void onDownloadFailed(int id, const QString &error);

    void createNewTab();
    void createPrivateTab();
    void closeCurrentTab();
    void showDownloadManager();
    void refreshPage();
    void viewSource();
    void handleSourceCode(const QString &source);
    void showAboutDialog();
    void showPreferences();

    void toggleBookmark();
    void showBookmarkManager();
    void openBookmark(const QUrl &url);
    void onBookmarksChanged();

    void toggleFindBar();
    void onFindRequested(const QString &text, bool forward);
    void onFindCleared();

    void zoomIn();
    void zoomOut();
    void zoomReset();

    void toggleFullscreen();

    void printPage();

private:
    void setupCentralUI();
    void setupToolBar();
    void setupStatusBar();
    void setupDownloadManagerDialog();
    void setupSourceDialog();
    void setupAboutDialog();
    void applyAboutDialogBackground();
    void setupShortcuts();
    void setupAddressBarCompleter();
    void connectGlobalSignals();
    void restoreLastSession();

    void updateNavigationButtons();
    void updateCompleterModel();
    void updateBookmarkStar();
    void disconnectCurrentTab();
    void connectCurrentTab(BrowserTab *tab);
    void navigateToInput(const QString &input);

    QLineEdit         *m_addressBar         = nullptr;
    QPushButton       *m_goButton           = nullptr;
    QPushButton       *m_backButton         = nullptr;
    QPushButton       *m_forwardButton      = nullptr;
    QPushButton       *m_bookmarkButton     = nullptr;
    QProgressBar      *m_progressBar        = nullptr;
    FindBar           *m_findBar            = nullptr;
    QVBoxLayout       *m_downloadNotices    = nullptr;
    QCompleter        *m_completer          = nullptr;
    QStringListModel  *m_completerModel     = nullptr;

    TabManager        *m_tabManager         = nullptr;
    DownloadManager   *m_downloadManager    = nullptr;
    BookmarkManager   *m_bookmarkManager    = nullptr;

    QDialog           *m_downloadDialog     = nullptr;
    QListView         *m_downloadListView   = nullptr;
    QDialog           *m_sourceDialog       = nullptr;
    QTextEdit         *m_sourceView         = nullptr;
    QDialog           *m_aboutDialog        = nullptr;

    bool               m_isClosing          = false;
    BrowserTab        *m_connectedTab       = nullptr;
};

#endif
