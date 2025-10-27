#include <QtWebEngineCore/QWebEngineDownloadRequest>

#include "mainwindow.h"
#include "downloaddialog.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtGui/QDesktopServices>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

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

    cancelDownloadButton->hide();
    retryDownloadButton->hide();

    downloadManagerDialog->hide();
}

void MainWindow::showDownloadManager() {
    downloadManagerDialog->show();
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

            currentDownloadFile = new QFile(savePath);
            if (!currentDownloadFile->open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this, "Error", "Could not open file for writing: " + savePath);
                delete currentDownloadFile;
                currentDownloadFile = nullptr;
                return;
            }

            QNetworkRequest request(download->url());
            currentDownloadReply = networkManager->get(request);
            totalBytesToDownload = download->totalBytes();
            bytesDownloaded = 0;
            
            QNetworkReply *reply = currentDownloadReply;
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                downloadFinished(reply);
            });

             connect(reply, &QNetworkReply::readyRead, this, [this]() {
                if (currentDownloadReply)
                {
                    bytesDownloaded += currentDownloadReply->bytesAvailable();
                    downloadProgress(bytesDownloaded, totalBytesToDownload);
                }
            });
            connect(reply, &QNetworkReply::downloadProgress, this, &MainWindow::downloadProgress);
            
            QString listItemText = QString("Downloading: %1").arg(currentDownloadFileName);
            if(totalBytesToDownload > 0){
                 listItemText +=  QString(" (%2 / %3)").arg(formatBytes(bytesDownloaded)).arg(formatBytes(totalBytesToDownload));
            }
            QListWidgetItem *item = new QListWidgetItem(listItemText);
            item->setData(Qt::UserRole, QVariant::fromValue(reply));
            downloadList->addItem(item);
            
            cancelDownloadButton->show();
            retryDownloadButton->show();
            retryDownloadButton->setEnabled(false);

            download->accept();
        } else {
            download->cancel();
        }
    }
}

void MainWindow::downloadFinished(QNetworkReply *reply) {
    if (!currentDownloadFile) {
        if (currentDownloadReply == reply) {
            currentDownloadReply = nullptr;
        }
        return;
    }

    QNetworkReply::NetworkError error = reply->error();
    
    if (error == QNetworkReply::NoError) {
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
    } else if (error != QNetworkReply::OperationCanceledError) {
        QMessageBox::critical(this, "Download Error", reply->errorString());
        for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
             if (item->data(Qt::UserRole) == QVariant::fromValue(reply)) {
                item->setText(QString("Download Failed: %1").arg(currentDownloadFileName));
                break;
            }
        }
        retryDownloadButton->setEnabled(true);
    } else {
        for (int i = 0; i < downloadList->count(); ++i) {
            QListWidgetItem *item = downloadList->item(i);
             if (item->data(Qt::UserRole) == QVariant::fromValue(reply)) {
                item->setText(QString("Download Cancelled: %1").arg(currentDownloadFileName));
                break;
            }
        }
    }
    
    delete currentDownloadFile;
    currentDownloadFile = nullptr;
    
    if(currentDownloadReply == reply){
        currentDownloadReply = nullptr;
        cancelDownloadButton->hide();
        if(error == QNetworkReply::NoError || error == QNetworkReply::OperationCanceledError){
             retryDownloadButton->hide();
        }
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
    }
}

void MainWindow::retryDownload() {
    if (currentDownloadReply) {
        cancelDownload();
    }
    QMessageBox::information(this, "Retry", "Retry functionality is not implemented yet.");
}