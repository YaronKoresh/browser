#include "mainwindow.h"
#include "constants.h"
#include "utils.h"
#include "settingsmanager.h"
#include "thememanager.h"
#include "tabmanager.h"
#include "browsertab.h"
#include "navigationmanager.h"
#include "downloadmanager.h"
#include "bookmarkmanager.h"
#include "bookmarksdialog.h"
#include "downloaddialog.h"
#include "preferencesdialog.h"
#include "findbar.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QDialog>
#include <QtWidgets/QCompleter>
#include <QtGui/QShortcut>
#include <QtGui/QAction>
#include <QtGui/QFont>
#include <QtGui/QKeySequence>
#include <QtGui/QPalette>
#include <QtGui/QLinearGradient>
#include <QtGui/QDesktopServices>
#include <QtCore/QStringListModel>
#include <QtWebEngineCore/QWebEngineProfile>
#include <QtWebEngineCore/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEngineFindTextResult>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>

Q_LOGGING_CATEGORY(lcMain, "browser.main")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_downloadManager(new DownloadManager(this))
    , m_bookmarkManager(new BookmarkManager(this))
{
    setupCentralUI();
    setupToolBar();
    setupStatusBar();
    setupDownloadManagerDialog();
    setupSourceDialog();
    setupAboutDialog();
    setupAddressBarCompleter();
    setupShortcuts();
    connectGlobalSignals();

    m_downloadManager->setMaxConcurrentDownloads(
        SettingsManager::instance().maxConcurrentDownloads());

    auto &settings = SettingsManager::instance();
    ThemeManager::instance().setTheme(settings.theme());

    const QByteArray savedGeometry = settings.windowGeometry();
    if (!savedGeometry.isEmpty()) {
        restoreGeometry(savedGeometry);
    } else {
        resize(settings.windowSize());
    }

    setWindowTitle(AppConstants::APP_NAME);

    createNewTab();
}

void MainWindow::setupCentralUI()
{
    auto *centralWidget = new QWidget(this);
    auto *mainLayout    = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *urlLayout = new QHBoxLayout;
    urlLayout->setContentsMargins(4, 4, 4, 4);

    m_backButton    = new QPushButton(AppConstants::LABEL_BACK, this);
    m_forwardButton = new QPushButton(AppConstants::LABEL_FORWARD, this);
    m_addressBar    = new QLineEdit(this);
    m_goButton      = new QPushButton(AppConstants::LABEL_GO, this);
    m_bookmarkButton = new QPushButton(QString::fromUtf8("\xe2\x98\x86"), this);
    m_bookmarkButton->setToolTip(tr("Toggle Bookmark"));
    m_bookmarkButton->setFixedWidth(30);

    m_backButton->setEnabled(false);
    m_forwardButton->setEnabled(false);

    m_addressBar->setPlaceholderText(tr("Enter URL or search..."));

    urlLayout->addWidget(m_backButton);
    urlLayout->addWidget(m_forwardButton);
    urlLayout->addWidget(m_addressBar);
    urlLayout->addWidget(m_bookmarkButton);
    urlLayout->addWidget(m_goButton);

    mainLayout->addLayout(urlLayout);

    m_tabManager = new TabManager(this);
    mainLayout->addWidget(m_tabManager);

    m_findBar = new FindBar(this);
    mainLayout->addWidget(m_findBar);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::setupToolBar()
{
    auto *toolbar = new QToolBar(this);
    addToolBar(toolbar);

    auto *newTabAction = new QAction(AppConstants::LABEL_NEW_TAB, this);
    toolbar->addAction(newTabAction);
    connect(newTabAction, &QAction::triggered, this, &MainWindow::createNewTab);

    auto *privateTabAction = new QAction(tr("Private Tab"), this);
    toolbar->addAction(privateTabAction);
    connect(privateTabAction, &QAction::triggered, this, &MainWindow::createPrivateTab);

    toolbar->addSeparator();

    auto *bookmarksAction = new QAction(AppConstants::LABEL_BOOKMARKS, this);
    toolbar->addAction(bookmarksAction);
    connect(bookmarksAction, &QAction::triggered, this, &MainWindow::showBookmarkManager);

    auto *downloadAction = new QAction(AppConstants::LABEL_DOWNLOADS, this);
    toolbar->addAction(downloadAction);
    connect(downloadAction, &QAction::triggered, this, &MainWindow::showDownloadManager);

    auto *refreshAction = new QAction(AppConstants::LABEL_REFRESH, this);
    toolbar->addAction(refreshAction);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshPage);

    toolbar->addSeparator();

    auto *zoomInAction = new QAction(AppConstants::LABEL_ZOOM_IN, this);
    toolbar->addAction(zoomInAction);
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    auto *zoomOutAction = new QAction(AppConstants::LABEL_ZOOM_OUT, this);
    toolbar->addAction(zoomOutAction);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    auto *zoomResetAction = new QAction(AppConstants::LABEL_ZOOM_RESET, this);
    toolbar->addAction(zoomResetAction);
    connect(zoomResetAction, &QAction::triggered, this, &MainWindow::zoomReset);

    toolbar->addSeparator();

    auto *viewSourceAction = new QAction(AppConstants::LABEL_VIEW_SOURCE, this);
    toolbar->addAction(viewSourceAction);
    connect(viewSourceAction, &QAction::triggered, this, &MainWindow::viewSource);

    auto *fullscreenAction = new QAction(AppConstants::LABEL_FULLSCREEN, this);
    toolbar->addAction(fullscreenAction);
    connect(fullscreenAction, &QAction::triggered, this, &MainWindow::toggleFullscreen);

    auto *printAction = new QAction(AppConstants::LABEL_PRINT, this);
    toolbar->addAction(printAction);
    connect(printAction, &QAction::triggered, this, &MainWindow::printPage);

    toolbar->addSeparator();

    auto *settingsAction = new QAction(AppConstants::LABEL_SETTINGS, this);
    toolbar->addAction(settingsAction);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::showPreferences);

    auto *aboutAction = new QAction(AppConstants::LABEL_ABOUT, this);
    toolbar->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::setupStatusBar()
{
    auto *sbar = new QStatusBar(this);
    setStatusBar(sbar);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    sbar->addPermanentWidget(m_progressBar);
}

void MainWindow::setupDownloadManagerDialog()
{
    m_downloadDialog = new QDialog(this);
    m_downloadDialog->setWindowTitle(AppConstants::LABEL_DOWNLOADS);
    m_downloadDialog->setMinimumSize(AppConstants::DOWNLOAD_DIALOG_WIDTH,
                                      AppConstants::DOWNLOAD_DIALOG_HEIGHT);

    auto *dialogLayout = new QVBoxLayout;

    m_downloadListView = new QListView(m_downloadDialog);
    m_downloadListView->setModel(m_downloadManager->model());
    dialogLayout->addWidget(m_downloadListView);

    auto *buttonLayout = new QHBoxLayout;
    auto *cancelBtn    = new QPushButton(AppConstants::LABEL_CANCEL, m_downloadDialog);
    auto *retryBtn     = new QPushButton(AppConstants::LABEL_RETRY, m_downloadDialog);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(retryBtn);
    buttonLayout->addStretch();
    dialogLayout->addLayout(buttonLayout);
    m_downloadDialog->setLayout(dialogLayout);

    connect(cancelBtn, &QPushButton::clicked, this, [this]() {
        const QModelIndex idx = m_downloadListView->currentIndex();
        if (idx.isValid()) {
            const int id = idx.data(DownloadModel::IdRole).toInt();
            m_downloadManager->cancelDownload(id);
        }
    });

    connect(retryBtn, &QPushButton::clicked, this, [this]() {
        const QModelIndex idx = m_downloadListView->currentIndex();
        if (idx.isValid()) {
            const int id = idx.data(DownloadModel::IdRole).toInt();
            m_downloadManager->retryDownload(id);
        }
    });

    m_downloadDialog->hide();
}

void MainWindow::setupSourceDialog()
{
    m_sourceDialog = new QDialog(this);
    m_sourceView   = new QTextEdit(m_sourceDialog);
    m_sourceView->setReadOnly(true);

    auto *layout = new QVBoxLayout;
    layout->addWidget(m_sourceView);
    m_sourceDialog->setLayout(layout);
    m_sourceDialog->setMinimumSize(AppConstants::SOURCE_DIALOG_WIDTH,
                                    AppConstants::SOURCE_DIALOG_HEIGHT);
    m_sourceDialog->setWindowTitle(tr("Source Code"));
}

void MainWindow::setupAboutDialog()
{
    m_aboutDialog = new QDialog(this);
    m_aboutDialog->setWindowTitle(tr("About %1").arg(AppConstants::APP_NAME));

    auto *layout = new QVBoxLayout;

    auto *nameLabel = new QLabel(AppConstants::APP_DISPLAY_NAME);
    QFont nameFont("Arial", 20, QFont::Bold);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignCenter);

    auto *versionLabel = new QLabel(tr("Version %1").arg(AppConstants::APP_VERSION));
    versionLabel->setAlignment(Qt::AlignCenter);

    auto *creatorLabel = new QLabel(tr("Created by: %1").arg(AppConstants::APP_ORGANIZATION));
    QFont creatorFont("Arial", 12);
    creatorLabel->setFont(creatorFont);
    creatorLabel->setAlignment(Qt::AlignCenter);

    auto *descLabel = new QLabel(AppConstants::APP_DESCRIPTION);
    descLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(nameLabel);
    layout->addWidget(versionLabel);
    layout->addWidget(creatorLabel);
    layout->addWidget(descLabel);
    m_aboutDialog->setLayout(layout);

    QPalette palette;
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(240, 255, 240));
    gradient.setColorAt(1, QColor(220, 220, 220));
    palette.setBrush(QPalette::Window, gradient);
    m_aboutDialog->setPalette(palette);
    m_aboutDialog->setAutoFillBackground(true);
}

void MainWindow::setupAddressBarCompleter()
{
    m_completerModel = new QStringListModel(this);
    m_completer = new QCompleter(m_completerModel, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);
    m_completer->setMaxVisibleItems(10);
    m_addressBar->setCompleter(m_completer);

    updateCompleterModel();
}

void MainWindow::updateCompleterModel()
{
    QStringList suggestions;

    suggestions.append(m_bookmarkManager->urls());
    suggestions.append(m_bookmarkManager->titles());

    for (int i = 0; i < m_tabManager->count(); ++i) {
        if (auto *tab = m_tabManager->tabAt(i)) {
            const QString urlStr = tab->url().toString();
            if (!urlStr.isEmpty() && !suggestions.contains(urlStr))
                suggestions.append(urlStr);
        }
    }

    suggestions.removeDuplicates();
    m_completerModel->setStringList(suggestions);
}

void MainWindow::setupShortcuts()
{
    new QShortcut(QKeySequence("Alt+Left"),  this, SLOT(onBackClicked()));
    new QShortcut(QKeySequence("Alt+Right"), this, SLOT(onForwardClicked()));
    new QShortcut(QKeySequence::Refresh,     this, SLOT(refreshPage()));
    new QShortcut(QKeySequence("F5"),        this, SLOT(refreshPage()));

    new QShortcut(QKeySequence("Ctrl+L"), this, [this]() {
        m_addressBar->setFocus();
        m_addressBar->selectAll();
    });

    new QShortcut(QKeySequence::AddTab,   this, SLOT(createNewTab()));
    new QShortcut(QKeySequence("Ctrl+W"),  this, SLOT(closeCurrentTab()));
    new QShortcut(QKeySequence("Ctrl+Shift+N"), this, SLOT(createPrivateTab()));

    new QShortcut(QKeySequence::Find, this, SLOT(toggleFindBar()));

    new QShortcut(QKeySequence("Ctrl+D"), this, SLOT(toggleBookmark()));
    new QShortcut(QKeySequence("Ctrl+Shift+B"), this, SLOT(showBookmarkManager()));

    new QShortcut(QKeySequence::ZoomIn,  this, SLOT(zoomIn()));
    new QShortcut(QKeySequence::ZoomOut, this, SLOT(zoomOut()));
    new QShortcut(QKeySequence("Ctrl+0"), this, SLOT(zoomReset()));

    new QShortcut(QKeySequence("F11"), this, SLOT(toggleFullscreen()));

    new QShortcut(QKeySequence::Print, this, SLOT(printPage()));

    new QShortcut(QKeySequence("Ctrl+,"), this, SLOT(showPreferences()));
}

void MainWindow::connectGlobalSignals()
{
    connect(m_goButton, &QPushButton::clicked,
            this, &MainWindow::navigateFromAddressBar);
    connect(m_addressBar, &QLineEdit::returnPressed,
            this, &MainWindow::navigateFromAddressBar);

    connect(m_bookmarkButton, &QPushButton::clicked,
            this, &MainWindow::toggleBookmark);

    connect(m_backButton, &QPushButton::clicked,
            this, &MainWindow::onBackClicked);
    connect(m_forwardButton, &QPushButton::clicked,
            this, &MainWindow::onForwardClicked);

    connect(m_tabManager, &TabManager::currentTabChanged,
            this, &MainWindow::onCurrentTabChanged);
    connect(m_tabManager, &TabManager::lastTabClosed,
            this, &MainWindow::onLastTabClosed);

    connect(m_downloadManager, &DownloadManager::downloadCompleted,
            this, &MainWindow::onDownloadCompleted);
    connect(m_downloadManager, &DownloadManager::downloadFailed,
            this, &MainWindow::onDownloadFailed);

    connect(m_bookmarkManager, &BookmarkManager::bookmarksChanged,
            this, &MainWindow::onBookmarksChanged);

    connect(m_findBar, &FindBar::searchRequested,
            this, &MainWindow::onFindRequested);
    connect(m_findBar, &FindBar::searchCleared,
            this, &MainWindow::onFindCleared);

    connect(QWebEngineProfile::defaultProfile(),
            &QWebEngineProfile::downloadRequested,
            this, &MainWindow::onDownloadRequested);

    connect(&SettingsManager::instance(), &SettingsManager::maxConcurrentDownloadsChanged,
            m_downloadManager, &DownloadManager::setMaxConcurrentDownloads);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_isClosing) {
        if (m_downloadManager->hasActiveDownloads()) {
            auto reply = QMessageBox::question(
                this, tr("Active Downloads"),
                tr("Downloads are still in progress. Close anyway?"),
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                event->ignore();
                return;
            }
        }

        m_isClosing = true;
        auto reply = QMessageBox::question(
            this, tr("Close %1").arg(AppConstants::APP_NAME),
            tr("Are you sure you want to close the browser?"),
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            auto &settings = SettingsManager::instance();
            settings.setWindowGeometry(saveGeometry());

            settings.setLastSessionUrls(m_tabManager->sessionUrls());

            m_downloadManager->cancelAll();
            event->accept();
        } else {
            event->ignore();
            m_isClosing = false;
        }
    } else {
        event->accept();
    }
}

void MainWindow::navigateFromAddressBar()
{
    navigateToInput(m_addressBar->text().trimmed());
}

void MainWindow::navigateToInput(const QString &input)
{
    if (input.isEmpty())
        return;

    auto *tab = m_tabManager->currentTab();
    if (!tab)
        tab = m_tabManager->createTab();

    QUrl url;
    if (Utils::looksLikeUrl(input)) {
        url = QUrl::fromUserInput(input);
    } else {
        const QString searchTemplate = SettingsManager::instance().defaultSearchEngine();
        url = QUrl(searchTemplate.arg(QString(QUrl::toPercentEncoding(input))));
    }

    if (!url.isValid()) {
        QMessageBox::warning(this, tr("Invalid URL"),
                             tr("The entered URL is not valid: %1").arg(input));
        return;
    }

    tab->loadUrl(url);
}

void MainWindow::onBackClicked()
{
    if (auto *tab = m_tabManager->currentTab())
        tab->navigation()->goBack();
}

void MainWindow::onForwardClicked()
{
    if (auto *tab = m_tabManager->currentTab())
        tab->navigation()->goForward();
}

void MainWindow::onCurrentTabChanged(BrowserTab *tab)
{
    disconnectCurrentTab();
    connectCurrentTab(tab);

    if (tab) {
        m_addressBar->setText(tab->url().toString());
        const QString title = tab->title();
        setWindowTitle(title.isEmpty()
                           ? AppConstants::APP_NAME
                           : QString("%1 - %2").arg(title, AppConstants::APP_NAME));
        updateNavigationButtons();

        const bool bookmarked = m_bookmarkManager->isBookmarked(tab->url());
        m_bookmarkButton->setText(bookmarked
            ? QString::fromUtf8("\xe2\x98\x85")
            : QString::fromUtf8("\xe2\x98\x86"));
    } else {
        m_addressBar->clear();
        setWindowTitle(AppConstants::APP_NAME);
        m_backButton->setEnabled(false);
        m_forwardButton->setEnabled(false);
        m_bookmarkButton->setText(QString::fromUtf8("\xe2\x98\x86"));
    }
}

void MainWindow::disconnectCurrentTab()
{
    if (!m_connectedTab)
        return;

    disconnect(m_connectedTab, &BrowserTab::urlChanged,
               this, &MainWindow::onTabUrlChanged);
    disconnect(m_connectedTab, &BrowserTab::titleChanged,
               this, &MainWindow::onTabTitleChanged);
    disconnect(m_connectedTab, &BrowserTab::loadStarted,
               this, &MainWindow::onTabLoadStarted);
    disconnect(m_connectedTab, &BrowserTab::loadProgress,
               this, &MainWindow::onTabLoadProgress);
    disconnect(m_connectedTab, &BrowserTab::loadFinished,
               this, &MainWindow::onTabLoadFinished);

    auto *nav = m_connectedTab->navigation();
    disconnect(nav, &NavigationManager::canGoBackChanged,
               m_backButton, &QPushButton::setEnabled);
    disconnect(nav, &NavigationManager::canGoForwardChanged,
               m_forwardButton, &QPushButton::setEnabled);

    m_connectedTab = nullptr;
}

void MainWindow::connectCurrentTab(BrowserTab *tab)
{
    if (!tab)
        return;

    m_connectedTab = tab;

    connect(tab, &BrowserTab::urlChanged,
            this, &MainWindow::onTabUrlChanged);
    connect(tab, &BrowserTab::titleChanged,
            this, &MainWindow::onTabTitleChanged);
    connect(tab, &BrowserTab::loadStarted,
            this, &MainWindow::onTabLoadStarted);
    connect(tab, &BrowserTab::loadProgress,
            this, &MainWindow::onTabLoadProgress);
    connect(tab, &BrowserTab::loadFinished,
            this, &MainWindow::onTabLoadFinished);

    auto *nav = tab->navigation();
    connect(nav, &NavigationManager::canGoBackChanged,
            m_backButton, &QPushButton::setEnabled);
    connect(nav, &NavigationManager::canGoForwardChanged,
            m_forwardButton, &QPushButton::setEnabled);
}

void MainWindow::onTabTitleChanged(const QString &title)
{
    setWindowTitle(title.isEmpty()
                       ? AppConstants::APP_NAME
                       : QString("%1 - %2").arg(title, AppConstants::APP_NAME));
}

void MainWindow::onTabUrlChanged(const QUrl &url)
{
    m_addressBar->setText(url.toString());
    statusBar()->showMessage(tr("URL: %1").arg(url.toString()),
                             AppConstants::STATUS_TIMEOUT_SHORT);

    const bool bookmarked = m_bookmarkManager->isBookmarked(url);
    m_bookmarkButton->setText(bookmarked
        ? QString::fromUtf8("\xe2\x98\x85")
        : QString::fromUtf8("\xe2\x98\x86"));

    updateCompleterModel();
}

void MainWindow::onTabLoadStarted()
{
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    statusBar()->showMessage(tr("Loading..."), 0);
}

void MainWindow::onTabLoadProgress(int progress)
{
    m_progressBar->setValue(progress);
}

void MainWindow::onTabLoadFinished(bool ok)
{
    m_progressBar->setVisible(false);
    statusBar()->showMessage(
        ok ? tr("Page loaded successfully.") : tr("Page load failed."),
        AppConstants::STATUS_TIMEOUT_SHORT);
    updateNavigationButtons();
}

void MainWindow::onLastTabClosed()
{
    createNewTab();
}

void MainWindow::updateNavigationButtons()
{
    if (auto *tab = m_tabManager->currentTab()) {
        m_backButton->setEnabled(tab->navigation()->canGoBack());
        m_forwardButton->setEnabled(tab->navigation()->canGoForward());
    }
}

void MainWindow::onDownloadRequested(QWebEngineDownloadRequest *download)
{
    if (download->state() != QWebEngineDownloadRequest::DownloadRequested)
        return;

    const QString fileName = download->suggestedFileName();

    DownloadDialog dialog(this);
    dialog.setFileName(fileName);

    if (dialog.exec() == QDialog::Accepted) {
        const QString savePath = dialog.getSavePath();

        if (QFileInfo::exists(savePath)) {
            auto reply = QMessageBox::question(
                this, tr("File Exists"),
                tr("'%1' already exists. Overwrite?").arg(savePath),
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                download->cancel();
                return;
            }
        }

        m_downloadManager->enqueueDownload(
            download->url(), savePath,
            QFileInfo(savePath).fileName(),
            download->totalBytes());

        download->cancel();
        m_downloadDialog->show();
    } else {
        download->cancel();
    }
}

void MainWindow::onDownloadCompleted(int id, const QString &savePath)
{
    const auto item = m_downloadManager->model()->findById(id);
    statusBar()->showMessage(
        tr("Downloaded %1 to %2").arg(item.fileName, savePath),
        AppConstants::STATUS_TIMEOUT_LONG);

    QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));
}

void MainWindow::onDownloadFailed(int id, const QString &error)
{
    const auto item = m_downloadManager->model()->findById(id);
    QMessageBox::critical(this, tr("Download Error"),
                          tr("Failed to download %1:\n%2").arg(item.fileName, error));
}

void MainWindow::createNewTab()
{
    const QUrl home = SettingsManager::instance().homePage();
    m_tabManager->createTab(home);
}

void MainWindow::createPrivateTab()
{
    auto *tab = m_tabManager->createTab();
    if (tab) {
        auto *offTheRecordProfile = new QWebEngineProfile(tab);
        auto *page = new QWebEnginePage(offTheRecordProfile, tab->webView());
        tab->setPage(page);
        tab->setPrivate(true);

        connect(offTheRecordProfile, &QWebEngineProfile::downloadRequested,
                this, &MainWindow::onDownloadRequested);

        tab->loadUrl(SettingsManager::instance().homePage());

        const int idx = m_tabManager->indexOf(tab);
        if (idx >= 0)
            m_tabManager->setTabText(idx, tr("(Private) New Tab"));

        statusBar()->showMessage(tr("Private browsing tab opened — no history or cookies saved."),
                                 AppConstants::STATUS_TIMEOUT_LONG);
    }
}

void MainWindow::closeCurrentTab()
{
    m_tabManager->closeCurrentTab();
}

void MainWindow::showDownloadManager()
{
    m_downloadDialog->show();
    m_downloadDialog->raise();
    m_downloadDialog->activateWindow();
}

void MainWindow::refreshPage()
{
    if (auto *tab = m_tabManager->currentTab())
        tab->reload();
}

void MainWindow::viewSource()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    tab->webView()->page()->toHtml([this](const QString &source) {
        handleSourceCode(source);
    });
}

void MainWindow::handleSourceCode(const QString &source)
{
    m_sourceView->clear();
    m_sourceView->setPlainText(source);
    m_sourceDialog->exec();
}

void MainWindow::showAboutDialog()
{
    m_aboutDialog->exec();
}

void MainWindow::showPreferences()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

void MainWindow::toggleBookmark()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    const QUrl url = tab->url();
    if (url.isEmpty() || !url.isValid())
        return;

    if (m_bookmarkManager->isBookmarked(url)) {
        m_bookmarkManager->removeBookmark(url);
        m_bookmarkButton->setText(QString::fromUtf8("\xe2\x98\x86"));
        statusBar()->showMessage(tr("Bookmark removed."),
                                 AppConstants::STATUS_TIMEOUT_SHORT);
    } else {
        m_bookmarkManager->addBookmark(tab->title(), url);
        m_bookmarkButton->setText(QString::fromUtf8("\xe2\x98\x85"));
        statusBar()->showMessage(tr("Bookmark added."),
                                 AppConstants::STATUS_TIMEOUT_SHORT);
    }
}

void MainWindow::showBookmarkManager()
{
    BookmarksDialog dialog(m_bookmarkManager, this);
    connect(&dialog, &BookmarksDialog::openRequested, this,
            [this](const QUrl &url) { navigateToInput(url.toString()); });
    dialog.exec();
}

void MainWindow::onBookmarksChanged()
{
    updateCompleterModel();
}

void MainWindow::toggleFindBar()
{
    if (m_findBar->isVisible())
        m_findBar->deactivate();
    else
        m_findBar->activate();
}

void MainWindow::onFindRequested(const QString &text, bool forward)
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    QWebEnginePage::FindFlags flags;
    if (!forward)
        flags |= QWebEnginePage::FindBackward;

    tab->webView()->page()->findText(text, flags, [this](const QWebEngineFindTextResult &result) {
        m_findBar->setMatchInfo(result.activeMatch(), result.numberOfMatches());
    });
}

void MainWindow::onFindCleared()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    tab->webView()->page()->findText(QString());
}

void MainWindow::zoomIn()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    double factor = tab->zoomFactor() + AppConstants::ZOOM_STEP;
    factor = qMin(factor, AppConstants::ZOOM_MAX);
    tab->setZoomFactor(factor);
    statusBar()->showMessage(tr("Zoom: %1%").arg(qRound(factor * 100)),
                             AppConstants::STATUS_TIMEOUT_SHORT);
}

void MainWindow::zoomOut()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    double factor = tab->zoomFactor() - AppConstants::ZOOM_STEP;
    factor = qMax(factor, AppConstants::ZOOM_MIN);
    tab->setZoomFactor(factor);
    statusBar()->showMessage(tr("Zoom: %1%").arg(qRound(factor * 100)),
                             AppConstants::STATUS_TIMEOUT_SHORT);
}

void MainWindow::zoomReset()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab)
        return;

    tab->setZoomFactor(AppConstants::ZOOM_DEFAULT);
    statusBar()->showMessage(tr("Zoom: 100%"), AppConstants::STATUS_TIMEOUT_SHORT);
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::printPage()
{
    auto *tab = m_tabManager->currentTab();
    if (!tab || !tab->webView())
        return;

    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        tab->webView()->print(&printer);
    }
}
