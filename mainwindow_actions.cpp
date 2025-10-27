#include "mainwindow.h"

#include <QtWidgets/QDialog>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineCore/QWebEnginePage>

void MainWindow::updateWindowTitle(bool ok) {
    if (ok) {
        setWindowTitle(webView->title() + " - Browser");
    } else {
        setWindowTitle("Loading failed - Browser");
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
    backButton->setEnabled(historyIndex > 0);
    forwardButton->setEnabled(historyIndex < history.size() - 1);
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