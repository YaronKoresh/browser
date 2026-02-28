#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtWidgets/QDialog>

class QLineEdit;
class QSpinBox;
class QCheckBox;
class QComboBox;

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);

private slots:
    void loadSettings();
    void applySettings();
    void onAccepted();

private:
    void setupUI();

    QLineEdit *m_homePageEdit        = nullptr;
    QLineEdit *m_downloadDirEdit     = nullptr;
    QSpinBox  *m_maxConcurrentSpin   = nullptr;
    QCheckBox *m_jsEnabledCheck      = nullptr;
    QComboBox *m_searchEngineCombo   = nullptr;
    QSpinBox  *m_maxHistorySpin      = nullptr;
    QComboBox *m_themeCombo          = nullptr;
};

#endif // PREFERENCESDIALOG_H
