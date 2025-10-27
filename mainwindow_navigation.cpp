#include "mainwindow.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtCore/QUrl>

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

    if (historyIndex >= 0 && historyIndex < history.size() && history[historyIndex] == url) {
        webView->reload();
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
    forwardButton->setEnabled(false);
}

void MainWindow::updateAddressBar(const QUrl &url) {
    bool reallyChanged = (historyIndex < 0 || history.isEmpty() || history[historyIndex] != url);

    if (reallyChanged) {
        addressBar->setText(url.toString());
        statusBar->showMessage("URL: " + url.toString(), 2000);

        if (historyIndex < history.size() - 1) {
            history.erase(history.begin() + historyIndex + 1, history.end());
        }
        history.append(url);
        historyIndex = history.size() - 1;

        backButton->setEnabled(historyIndex > 0);
        forwardButton->setEnabled(false);
    }
}

void MainWindow::goBack() {
    if (historyIndex > 0) {
        historyIndex--;
        webView->load(history[historyIndex]);
        addressBar->setText(history[historyIndex].toString());
        backButton->setEnabled(historyIndex > 0);
        forwardButton->setEnabled(true);
    }
}

void MainWindow::goForward() {
    if (historyIndex < history.size() - 1) {
        historyIndex++;
        webView->load(history[historyIndex]);
        addressBar->setText(history[historyIndex].toString());
        backButton->setEnabled(true);
        forwardButton->setEnabled(historyIndex < history.size() - 1);
    }
}