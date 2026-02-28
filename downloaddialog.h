#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QtWidgets/QDialog>
#include <QtCore/QString>

class QLineEdit;
class QPushButton;

class DownloadDialog : public QDialog {
    Q_OBJECT

public:
    explicit DownloadDialog(QWidget *parent = nullptr);
    ~DownloadDialog() override = default;

    [[nodiscard]] QString getSavePath() const;
    void setFileName(const QString &fileName);

private slots:
    void onBrowseClicked();
    void onAcceptClicked();

private:
    QLineEdit   *m_fileNameInput     = nullptr;
    QPushButton *m_browseButton      = nullptr;
    QPushButton *m_acceptButton      = nullptr;
    QPushButton *m_cancelButton      = nullptr;
    QString      m_savePath;
    QString      m_suggestedFileName;
};

#endif // DOWNLOADDIALOG_H
