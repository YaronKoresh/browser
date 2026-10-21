#include "downloaddialog.h"
#include "constants.h"
#include "settingsmanager.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

DownloadDialog::DownloadDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Save File"));

    m_fileNameInput = new QLineEdit(this);
    m_browseButton  = new QPushButton(AppConstants::LABEL_BROWSE, this);
    m_acceptButton  = new QPushButton(AppConstants::LABEL_ACCEPT, this);
    m_cancelButton  = new QPushButton(AppConstants::LABEL_CANCEL, this);

    auto *formLayout = new QFormLayout;
    formLayout->addRow(tr("File Name:"), m_fileNameInput);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_browseButton);
    buttonLayout->addWidget(m_acceptButton);
    buttonLayout->addWidget(m_cancelButton);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(m_browseButton, &QPushButton::clicked,
            this, &DownloadDialog::onBrowseClicked);
    connect(m_acceptButton, &QPushButton::clicked,
            this, &DownloadDialog::onAcceptClicked);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
    connect(m_fileNameInput, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_acceptButton->setEnabled(!text.trimmed().isEmpty());
    });

    m_acceptButton->setEnabled(false);
}

QString DownloadDialog::getSavePath() const
{
    return m_savePath;
}

void DownloadDialog::setFileName(const QString &fileName)
{
    m_suggestedFileName = QFileInfo(fileName).fileName();

    QString downloadDir = SettingsManager::instance().defaultDownloadDirectory().trimmed();
    if (downloadDir.isEmpty())
        downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    const QString fullPath = downloadDir + "/" + m_suggestedFileName;

    m_fileNameInput->setText(fullPath);
    m_acceptButton->setEnabled(!fileName.isEmpty());
}

void DownloadDialog::onBrowseClicked()
{
    const QString startDir = QFileInfo(m_fileNameInput->text()).path();
    const QString directory = QFileDialog::getExistingDirectory(
        this, tr("Choose Directory"),
        startDir.isEmpty() ? QDir::homePath() : startDir);

    if (!directory.isEmpty()) {
        const QString currentName = QFileInfo(m_fileNameInput->text()).fileName();
        const QString name = currentName.isEmpty() ? m_suggestedFileName : currentName;
        m_savePath = directory + "/" + name;
        m_fileNameInput->setText(m_savePath);
    }
}

void DownloadDialog::onAcceptClicked()
{
    m_savePath = m_fileNameInput->text().trimmed();
    if (m_savePath.isEmpty())
        return;

    QFileInfo fileInfo(m_savePath);
    if (fileInfo.fileName().isEmpty()) {
        m_savePath += "/" + m_suggestedFileName;
    }

    accept();
}
