#include "downloadnotification.h"
#include "constants.h"
#include "utils.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QDesktopServices>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QUrl>

DownloadNotification::DownloadNotification(const QString &savePath, QWidget *parent)
    : QFrame(parent)
    , m_savePath(savePath)
    , m_label(new QLabel(this))
    , m_openFileButton(new QPushButton(AppConstants::LABEL_OPEN_FILE, this))
    , m_openFolderButton(new QPushButton(AppConstants::LABEL_OPEN_FOLDER, this))
    , m_closeButton(new QPushButton("X", this))
{
    const QFileInfo info(savePath);
    m_launchable = Utils::isLaunchableProgram(info.fileName());

    setFrameShape(QFrame::StyledPanel);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);

    m_label->setText(tr("Download complete: %1").arg(info.fileName()));
    m_label->setToolTip(QDir::toNativeSeparators(savePath));
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    m_closeButton->setFixedWidth(28);
    m_closeButton->setToolTip(tr("Dismiss"));
    m_openFileButton->setToolTip(QDir::toNativeSeparators(savePath));
    m_openFolderButton->setToolTip(QDir::toNativeSeparators(info.absolutePath()));

    layout->addWidget(m_label, 1);

    if (m_launchable) {
        m_confirmCheckBox = new QCheckBox(AppConstants::LABEL_CONFIRM_RUN_PROGRAM, this);
        m_confirmCheckBox->setToolTip(
            tr("This file is a program or script. Opening it runs it on your computer."));
        m_openFileButton->setEnabled(false);
        layout->addWidget(m_confirmCheckBox);
        connect(m_confirmCheckBox, &QCheckBox::toggled,
                m_openFileButton, &QPushButton::setEnabled);
    }

    layout->addWidget(m_openFileButton);
    layout->addWidget(m_openFolderButton);
    layout->addWidget(m_closeButton);

    connect(m_openFileButton, &QPushButton::clicked,
            this, &DownloadNotification::openFile);
    connect(m_openFolderButton, &QPushButton::clicked,
            this, &DownloadNotification::openFolder);
    connect(m_closeButton, &QPushButton::clicked,
            this, &QObject::deleteLater);
}

QString DownloadNotification::savePath() const
{
    return m_savePath;
}

bool DownloadNotification::isLaunchable() const
{
    return m_launchable;
}

void DownloadNotification::openFile()
{
    if (m_launchable && !(m_confirmCheckBox && m_confirmCheckBox->isChecked()))
        return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_savePath));
}

void DownloadNotification::openFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_savePath).absolutePath()));
}
