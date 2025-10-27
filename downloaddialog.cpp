#include "downloaddialog.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

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