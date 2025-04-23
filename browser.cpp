#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <QtCore/QUrl>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFileDialog>
#include <QtGui/QDesktopServices>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEngineProfile>
#include <QtWebEngineCore/QWebEnginePage>
#include <QtWebEngineCore/QtWebEngineCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QDir>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QToolBar>
#include <QtGui/QAction>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QLabel>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <QtGui/QLinearGradient>
#include <QtWebEngineQuick/QQuickWebEngineDownloadRequest>
#include <QtWebView/QtWebView>

class DownloadDialog : public QDialog {
    Q_OBJECT
public:
    DownloadDialog(QWidget *parent = nullptr);
    ~DownloadDialog() = default;
    QString getSavePath() const { return savePath; }
    void setFileName(const QString &fileName) {
        fileNameLineEdit->setText(fileName);
        QString baseName = QFileInfo(fileName).baseName();
        suggestedFileName = baseName;
    }
private slots:
    void browseButtonClicked();
    void acceptButtonClicked();
private:
    QLineEdit *fileNameLineEdit;
    QPushButton *browseButton;
    QPushButton *acceptButton;
    QPushButton *cancelButton;
    QString savePath;
    QString suggestedFileName;
};
DownloadDialog::DownloadDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Save File");
    fileNameLineEdit = new QLineEdit(this);
    browseButton = new QPushButton("Browse...", this);
    acceptButton = new QPushButton("Accept", this);
    cancelButton = new QPushButton("Cancel", this);
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("File Name:", fileNameLineEdit);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(browseButton);
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    connect(browseButton, &QPushButton::clicked, this, &DownloadDialog::browseButtonClicked);
    connect(acceptButton, &QPushButton::clicked, this, &DownloadDialog::acceptButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(fileNameLineEdit, &QLineEdit::textChanged, this, [this](const QString& text){
        acceptButton->setEnabled(!text.isEmpty());
    });
    acceptButton->setEnabled(false);
}
void DownloadDialog::browseButtonClicked() {
    QString directory = QFileDialog::getExistingDirectory(this, "Choose Directory", QDir::homePath());
    if (!directory.isEmpty()) {
        savePath = directory + "/" + fileNameLineEdit->text();
        fileNameLineEdit->setText(savePath);
    }
}
void DownloadDialog::acceptButtonClicked() {
    savePath = fileNameLineEdit->text();
    if(!savePath.isEmpty()){
       QFileInfo fileInfo(savePath);
        if(fileInfo.fileName().isEmpty()){
             savePath += "/" + suggestedFileName;
        }
        accept();
    }
}
QString formatBytes(qint64 bytes) {
    if (bytes < 1024)
        return QString("%1 B").arg(bytes);
    else if (bytes < 1024 * 1024)
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 2);
    else if (bytes < 1024 * 1024 * 1024)
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 2);
    else
        return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}
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
    void setupDownloadManagerDialog();
    QTextEdit *sourceView;
    QDialog *sourceDialog;
    QDialog *aboutDialog;
};
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), historyIndex(0) {
    addressBar = new QLineEdit(this);
    addressBar->setPlaceholderText("Enter URL (e.g., https://www.google.com)");
    goButton = new QPushButton("Go", this);
    backButton = new QPushButton("Back", this);
    forwardButton = new QPushButton("Forward", this);
    webView = new QWebEngineView(this);
    QHBoxLayout *urlLayout = new QHBoxLayout;
    urlLayout->addWidget(backButton);
    urlLayout->addWidget(forwardButton);
    urlLayout->addWidget(addressBar);
    urlLayout->addWidget(goButton);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(urlLayout);
    mainLayout->addWidget(webView);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setVisible(false);
    statusBar->addPermanentWidget(progressBar);
    networkManager = new QNetworkAccessManager(this);
    setupDownloadManagerDialog();
    QToolBar *toolbar = new QToolBar(this);
    addToolBar(toolbar);
    QAction *downloadManagerAction = new QAction("Downloads", this);
    toolbar->addAction(downloadManagerAction);
    QAction *refreshAction = new QAction("Refresh", this);
    toolbar->addAction(refreshAction);
    QAction *viewSourceAction = new QAction("View Source", this);
    toolbar->addAction(viewSourceAction);
     QAction *aboutAction = new QAction("About", this);
    toolbar->addAction(aboutAction);
    connect(downloadManagerAction, &QAction::triggered, this, &MainWindow::showDownloadManager);
    connect(goButton, &QPushButton::clicked, this, &MainWindow::loadUrl);
    connect(addressBar, &QLineEdit::returnPressed, this, &MainWindow::loadUrl);
    connect(webView, &QWebEngineView::urlChanged, this, &MainWindow::updateAddressBar);
    connect(webView, &QWebEngineView::loadFinished, this, &MainWindow::updateWindowTitle);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(forwardButton, &QPushButton::clicked, this, &MainWindow::goForward);
    connect(webView, &QWebEngineView::loadStarted, this, &MainWindow::handleLoadStarted);
    connect(webView, &QWebEngineView::loadProgress, this, &MainWindow::handleLoadProgress);
    connect(webView, &QWebEngineView::loadFinished, this, &MainWindow::handleLoadFinished);
    connect(webView->page()->profile(), &QWebEngineProfile::downloadRequested, this, &MainWindow::handleDownloadRequested);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshPage);
    connect(viewSourceAction, &QAction::triggered, this, &MainWindow::viewSource);
     connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
    setWindowTitle("Browser");
    addressBar->setText("https://www.google.com");
    loadUrl();
    resize(1024, 768);
    sourceDialog = new QDialog(this);
    sourceView = new QTextEdit(sourceDialog);
    QVBoxLayout *sourceLayout = new QVBoxLayout;
    sourceLayout->addWidget(sourceView);
    sourceDialog->setLayout(sourceLayout);
    sourceDialog->setMinimumSize(800, 600);
    sourceDialog->setWindowTitle("Source Code");
    aboutDialog = new QDialog(this);
    aboutDialog->setWindowTitle("About Browser");
    QVBoxLayout *aboutLayout = new QVBoxLayout;
    QLabel *nameLabel = new QLabel("Browser");
    QFont nameFont("Arial", 20, QFont::Bold);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignCenter);
    QLabel *creatorLabel = new QLabel("Created by: Yaron Koresh");
    QFont creatorFont("Arial", 12);
    creatorLabel->setFont(creatorFont);
    creatorLabel->setAlignment(Qt::AlignCenter);
    QLabel *descriptionLabel = new QLabel("A simple web browser built with Qt.");
    descriptionLabel->setAlignment(Qt::AlignCenter);
    aboutLayout->addWidget(nameLabel);
    aboutLayout->addWidget(creatorLabel);
    aboutLayout->addWidget(descriptionLabel);
    aboutDialog->setLayout(aboutLayout);
    QPalette palette;
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(240, 255, 240));
    gradient.setColorAt(1, QColor(220, 220, 220));
    palette.setBrush(QPalette::Window, gradient);
    aboutDialog->setPalette(palette);
    aboutDialog->setAutoFillBackground(true);
}
void MainWindow::loadUrl() {
    QString urlString = addressBar->text().trimmed();
    if (urlString.isEmpty()) {
        return;
    }
    QUrl url = QUrl::fromUserInput(urlString);
    if (!url.isValid()) {
        QMessageBox::warning(this, "Invalid URL", "The entered URL is not valid: " + urlString);
        return;
    }
    webView->load(url);
    webView->setFocus();
    if (historyIndex < history.size() - 1) {
        history.erase(history.begin() + historyIndex + 1, history.end());
    }
    history.append(url);
    historyIndex = history.size() - 1;
    backButton->setEnabled(historyIndex > 0);
    forwardButton->setEnabled(historyIndex < history.size() - 1);
}
void MainWindow::updateAddressBar(const QUrl &url) {
    if (url.toString() != addressBar->text()) {
        addressBar->setText(url.toString());
    }
    statusBar->showMessage("URL: " + url.toString(), 2000);
}
void MainWindow::updateWindowTitle(bool ok) {
    if (ok) {
        setWindowTitle(webView->title() + " - Browser");
    } else {
        setWindowTitle("Loading failed - Browser");
    }
}
void MainWindow::goBack() {
    if (historyIndex > 0) {
        historyIndex--;
        webView->load(history[historyIndex]);
        addressBar->setText(history[historyIndex].toString());
        backButton->setEnabled(historyIndex > 0);
        forwardButton->setEnabled(historyIndex < history.size() - 1);
    }
}
void MainWindow::goForward() {
    if (historyIndex < history.size() - 1) {
        historyIndex++;
        webView->load(history[historyIndex]);
        addressBar->setText(history[historyIndex].toString());
        backButton->setEnabled(historyIndex > 0);
        forwardButton->setEnabled(historyIndex < history.size() - 1);
    }
}
void MainWindow::handleLoadStarted() {
    progressBar->setVisible(true);
    progressBar->setValue(0);
    statusBar->showMessage("Loading...", 0);
}
void MainWindow::handleLoadProgress(int progress) {
    progressBar->setValue(progress);
}
void MainWindow::handleLoadFinished(bool ok) {
    progressBar->setVisible(false);
    if (ok) {
        statusBar->showMessage("Page loaded successfully.", 2000);
    } else {
        statusBar->showMessage("Page load failed.", 2000);
    }
}
void MainWindow::handleDownloadRequested(QWebEngineDownloadRequest *download) {
    if (download->state() == QWebEngineDownloadRequest::DownloadRequested) {
        QString fileName = download->suggestedFileName();
        DownloadDialog dialog(this);
        dialog.setFileName(fileName);
        if (dialog.exec() == QDialog::Accepted) {
            QString savePath = dialog.getSavePath();
            currentDownloadFileName = QFileInfo(savePath).fileName();
            currentDownloadSavePath = savePath;
            QFile file(savePath);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, "Error", "Could not open file for writing: " + savePath);
                return;
            }
            currentDownloadFile = &file;
            QNetworkRequest request(download->url());
            currentDownloadReply = networkManager->get(request);
            totalBytesToDownload = download->totalBytes();
            bytesDownloaded = 0;
            connect(currentDownloadReply, &QNetworkReply::finished, this, &MainWindow::downloadFinished);
             connect(currentDownloadReply, &QNetworkReply::readyRead, this, [this]() {
                if (currentDownloadReply)
                {
                    bytesDownloaded += currentDownloadReply->bytesAvailable();
                    downloadProgress(bytesDownloaded, totalBytesToDownload);
                }

            });
            connect(currentDownloadReply, &QNetworkReply::downloadProgress, this, &MainWindow::downloadProgress);
            QString listItemText = QString("Downloading: %1").arg(currentDownloadFileName);
            if(totalBytesToDownload > 0){
                 listItemText +=  QString(" (%2 / %3)").arg(formatBytes(bytesDownloaded)).arg(formatBytes(totalBytesToDownload));
            }
            QListWidgetItem *item = new QListWidgetItem(listItemText);
            item->setData(Qt::UserRole, QVariant::fromValue(currentDownloadReply));
            downloadList->addItem(item);
            cancelDownloadButton = new QPushButton("Cancel", downloadManagerDialog);
            retryDownloadButton = new QPushButton("Retry", downloadManagerDialog);
            QHBoxLayout *buttonLayout = new QHBoxLayout;
            buttonLayout->addWidget(cancelDownloadButton);
            buttonLayout->addWidget(retryDownloadButton);
            if (downloadManagerDialog->layout()) {
                QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(downloadManagerDialog->layout());
                if (layout) {
                    layout->addLayout(buttonLayout);
                }
            }
            connect(cancelDownloadButton, &QPushButton::clicked, this, &MainWindow::cancelDownload);
            connect(retryDownloadButton, &QPushButton::clicked, this, &MainWindow::retryDownload);
            download->accept();
        } else {
            download->cancel();
        }
    }
}
void MainWindow::downloadFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        currentDownloadFile->write(data);
        currentDownloadFile->close();
        statusBar->showMessage(QString("Downloaded %1 to %2").arg(currentDownloadFileName, currentDownloadSavePath), 5000);
         for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
            if (item->data(Qt::UserRole) == QVariant::fromValue(reply)) {
                item->setText(QString("Downloaded: %1").arg(currentDownloadFileName));
                break;
            }
        }
       QDesktopServices::openUrl(QUrl::fromLocalFile(currentDownloadSavePath));
    } else {
        QMessageBox::critical(this, "Download Error", reply->errorString());
        for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
             if (item->data(Qt::UserRole) == QVariant::fromValue(reply)) {
                item->setText(QString("Download Failed: %1").arg(currentDownloadFileName));
                break;
            }
        }
    }
    reply->deleteLater();
    currentDownloadReply = nullptr;
    if (cancelDownloadButton) {
        cancelDownloadButton->deleteLater();
        cancelDownloadButton = nullptr;
    }
     if (retryDownloadButton) {
        retryDownloadButton->deleteLater();
        retryDownloadButton = nullptr;
    }
}
void MainWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (bytesTotal > 0) {
        progressBar->setMaximum(bytesTotal);
        progressBar->setValue(bytesReceived);
        statusBar->showMessage(QString("Downloading %1: %2 / %3").arg(currentDownloadFileName).arg(formatBytes(bytesReceived)).arg(formatBytes(bytesTotal)));
         for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
            if (item->data(Qt::UserRole) == QVariant::fromValue(currentDownloadReply)) {
                 QString listItemText = QString("Downloading: %1 (%2 / %3)").arg(currentDownloadFileName).arg(formatBytes(bytesReceived)).arg(formatBytes(bytesTotal));
                item->setText(listItemText);
                break;
            }
        }
    }  else if (bytesReceived > 0) {
         statusBar->showMessage(QString("Downloading %1: %2").arg(currentDownloadFileName).arg(formatBytes(bytesReceived)));
        for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
            if (item->data(Qt::UserRole) == QVariant::fromValue(currentDownloadReply)) {
                 QString listItemText = QString("Downloading: %1 (%2)").arg(currentDownloadFileName).arg(formatBytes(bytesReceived));
                item->setText(listItemText);
                break;
            }
        }
    }
}
void MainWindow::cancelDownload() {
    if (currentDownloadReply) {
        currentDownloadReply->abort();
        currentDownloadReply->deleteLater();
        currentDownloadReply = nullptr;
         for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
            if (item->data(Qt::UserRole) == QVariant::fromValue(currentDownloadReply)) {
                item->setText(QString("Download Cancelled: %1").arg(currentDownloadFileName));
                break;
            }
        }
    }
    if (currentDownloadFile) {
        currentDownloadFile->close();
        delete currentDownloadFile;
        currentDownloadFile = nullptr;
    }
     if (cancelDownloadButton) {
        cancelDownloadButton->deleteLater();
        cancelDownloadButton = nullptr;
    }
     if (retryDownloadButton) {
        retryDownloadButton->deleteLater();
        retryDownloadButton = nullptr;
    }
}
void MainWindow::retryDownload() {
   if (currentDownloadReply) {
        cancelDownload();
    }
    QUrl url = QUrl::fromUserInput(addressBar->text().trimmed());
    if (url.isValid()) {
        QNetworkRequest request(url);
        currentDownloadReply = networkManager->get(request);
    }
}
void MainWindow::setupDownloadManagerDialog() {
    downloadManagerDialog = new QDialog(this);
    downloadManagerDialog->setWindowTitle("Downloads");
    downloadManagerDialog->setMinimumSize(400, 300);
    QVBoxLayout *dialogLayout = new QVBoxLayout;
    downloadList = new QListWidget(downloadManagerDialog);
    dialogLayout->addWidget(downloadList);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    cancelDownloadButton = new QPushButton("Cancel", downloadManagerDialog);
    retryDownloadButton = new QPushButton("Retry", downloadManagerDialog);
    buttonLayout->addWidget(cancelDownloadButton);
    buttonLayout->addWidget(retryDownloadButton);
    buttonLayout->addStretch();
    dialogLayout->addLayout(buttonLayout);
    downloadManagerDialog->setLayout(dialogLayout);
    connect(cancelDownloadButton, &QPushButton::clicked, this, &MainWindow::cancelDownload);
    connect(retryDownloadButton, &QPushButton::clicked, this, &MainWindow::retryDownload);
    downloadManagerDialog->hide();
}
void MainWindow::showDownloadManager() {
    downloadManagerDialog->show();
}
void MainWindow::closeEvent(QCloseEvent *event) {
    if (!m_isClosing) {
        m_isClosing = true;
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Close Browser",
            "Are you sure you want to close the browser?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
            m_isClosing = false;
        }
    } else {
        event->accept();
    }
}
void MainWindow::refreshPage() {
    webView->reload();
}
void MainWindow::viewSource() {
    webView->page()->toHtml([this](const QString& source) {
        handleSourceCode(source);
    });
}
void MainWindow::handleSourceCode(const QString& source) {
    sourceView->clear();
    sourceView->setPlainText(source);
    sourceDialog->exec();
}
void MainWindow::showAboutDialog() {
    aboutDialog->exec();
}
int main(int argc, char *argv[]) {
    QtWebView::initialize();
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Browser");
    QCoreApplication::setOrganizationName("Yaron Koresh");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    MainWindow browserWindow;
    browserWindow.show();
    return app.exec();
}

