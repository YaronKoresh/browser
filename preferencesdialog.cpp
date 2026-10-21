#include "preferencesdialog.h"
#include "settingsmanager.h"
#include "constants.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QDir>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    setMinimumSize(480, 400);
    setupUI();
    loadSettings();
}

void PreferencesDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *tabs = new QTabWidget(this);

    auto *generalPage = new QWidget;
    auto *generalLayout = new QFormLayout(generalPage);

    m_homePageEdit = new QLineEdit;
    generalLayout->addRow(tr("Home Page:"), m_homePageEdit);

    m_themeCombo = new QComboBox;
    m_themeCombo->addItem(tr("System Default"), "system");
    m_themeCombo->addItem(tr("Light"), "light");
    m_themeCombo->addItem(tr("Dark"), "dark");
    generalLayout->addRow(tr("Theme:"), m_themeCombo);

    tabs->addTab(generalPage, tr("General"));

    auto *browsingPage = new QWidget;
    auto *browsingLayout = new QFormLayout(browsingPage);

    m_jsEnabledCheck = new QCheckBox(tr("Enable JavaScript"));
    browsingLayout->addRow(m_jsEnabledCheck);

    m_searchEngineCombo = new QComboBox;
    m_searchEngineCombo->setEditable(true);
    m_searchEngineCombo->addItem(tr("Google"), "https://www.google.com/search?q=%1");
    m_searchEngineCombo->addItem(tr("DuckDuckGo"), "https://duckduckgo.com/?q=%1");
    m_searchEngineCombo->addItem(tr("Bing"), "https://www.bing.com/search?q=%1");
    browsingLayout->addRow(tr("Search Engine:"), m_searchEngineCombo);

    m_maxHistorySpin = new QSpinBox;
    m_maxHistorySpin->setRange(10, 100000);
    m_maxHistorySpin->setSingleStep(100);
    browsingLayout->addRow(tr("Max History Size:"), m_maxHistorySpin);

    tabs->addTab(browsingPage, tr("Browsing"));

    auto *downloadPage = new QWidget;
    auto *downloadLayout = new QFormLayout(downloadPage);

    auto *dirRow = new QHBoxLayout;
    m_downloadDirEdit = new QLineEdit;
    auto *browseBtn = new QPushButton(tr("Browse..."));
    dirRow->addWidget(m_downloadDirEdit);
    dirRow->addWidget(browseBtn);
    downloadLayout->addRow(tr("Download Folder:"), dirRow);

    connect(browseBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(
            this, tr("Choose Download Folder"), m_downloadDirEdit->text());
        if (!dir.isEmpty())
            m_downloadDirEdit->setText(dir);
    });

    m_maxConcurrentSpin = new QSpinBox;
    m_maxConcurrentSpin->setRange(1, 10);
    downloadLayout->addRow(tr("Max Concurrent Downloads:"), m_maxConcurrentSpin);

    tabs->addTab(downloadPage, tr("Downloads"));

    mainLayout->addWidget(tabs);

    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &PreferencesDialog::applySettings);
}

void PreferencesDialog::loadSettings()
{
    auto &s = SettingsManager::instance();

    m_homePageEdit->setText(s.homePage().toString());
    m_downloadDirEdit->setText(s.defaultDownloadDirectory());
    m_maxConcurrentSpin->setValue(s.maxConcurrentDownloads());
    m_jsEnabledCheck->setChecked(s.javascriptEnabled());
    const int themeIdx = m_themeCombo->findData(s.theme());
    m_themeCombo->setCurrentIndex(themeIdx >= 0 ? themeIdx : 0);
    m_maxHistorySpin->setValue(s.maxHistorySize());

    const QString currentEngine = s.defaultSearchEngine();
    int idx = m_searchEngineCombo->findData(currentEngine);
    if (idx >= 0) {
        m_searchEngineCombo->setCurrentIndex(idx);
    } else {
        m_searchEngineCombo->setEditText(currentEngine);
    }
}

void PreferencesDialog::applySettings()
{
    auto &s = SettingsManager::instance();

    s.setHomePage(QUrl::fromUserInput(m_homePageEdit->text().trimmed()));
    s.setDefaultDownloadDirectory(m_downloadDirEdit->text().trimmed());
    s.setMaxConcurrentDownloads(m_maxConcurrentSpin->value());
    s.setJavascriptEnabled(m_jsEnabledCheck->isChecked());
    s.setTheme(m_themeCombo->currentData().toString());
    s.setMaxHistorySize(m_maxHistorySpin->value());

    const QString typed = m_searchEngineCombo->currentText().trimmed();
    const int idx = m_searchEngineCombo->findText(typed);
    const QString engine = idx >= 0 ? m_searchEngineCombo->itemData(idx).toString() : QString();
    s.setDefaultSearchEngine(engine.isEmpty() ? typed : engine);
}

void PreferencesDialog::onAccepted()
{
    applySettings();
    accept();
}
