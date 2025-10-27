#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QUrl>
#include <QtGui/QCloseEvent>

class QLineEdit;
class QPushButton;
class QWebEngineView;
class QStatusBar;
class QProgressBar;
class QListWidget;
class QDialog;
class QNetworkAccessManager;
class QNetworkReply;
class QFile;
class QWebEngineDownloadRequest;
class QTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void loadUrl();
    void updateAddressBar(const QUrl &url);
    void updateWindowTitle(bool ok);
    void goBack();
    void goForward();
    void handleLoadStarted();
    void handleLoadProgress(int progress);
    void handleLoadFinished(bool ok);
    void handleDownloadRequested(QWebEngineDownloadRequest *download);
    void downloadFinished(QNetworkReply *reply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void cancelDownload();
    void retryDownload();
    void showDownloadManager();
    void refreshPage();
    void viewSource();
    void handleSourceCode(const QString& source);
    void showAboutDialog();

private:
    void setupDownloadManagerDialog();
    QString formatBytes(qint64 bytes);

    QLineEdit *addressBar;
    QPushButton *goButton;
    QPushButton *backButton;
    QPushButton *forwardButton;
    QWebEngineView *webView;
    QList<QUrl> history;
    int historyIndex;
    QStatusBar *statusBar;
    QProgressBar *progressBar;
    QListWidget *downloadList;
    QDialog *downloadManagerDialog;
    QPushButton *cancelDownloadButton;
    QPushButton *retryDownloadButton;
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentDownloadReply;
    QString currentDownloadFileName;
    QString currentDownloadSavePath;
    QFile *currentDownloadFile;
    qint64 totalBytesToDownload;
    qint64 bytesDownloaded;
    bool m_isClosing = false;
    QTextEdit *sourceView;
    QDialog *sourceDialog;
    QDialog *aboutDialog;
};

#endif // MAINWINDOW_H