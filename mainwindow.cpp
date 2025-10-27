#include "mainwindow.h"
#include "downloaddialog.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QDialog>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtGui/QAction>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <QtGui/QLinearGradient>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEngineProfile>
#include <QtWebEngineCore/QWebEnginePage>
#include <QtNetwork/QNetworkAccessManager>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), historyIndex(-1), currentDownloadFile(nullptr) {
    addressBar = new QLineEdit(this);
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
    addressBar->setText("https://www.google.com/");
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

    backButton->setEnabled(false);
    forwardButton->setEnabled(false);
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