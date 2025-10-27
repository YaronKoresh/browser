#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QtWidgets/QDialog>
#include <QtCore/QString>

class QLineEdit;
class QPushButton;

class DownloadDialog : public QDialog {
    Q_OBJECT
public:
    DownloadDialog(QWidget *parent = nullptr);
    ~DownloadDialog() = default;
    QString getSavePath() const { return savePath; }
    void setFileName(const QString &fileName);

private slots:
    void browseButtonClicked();
    void acceptButtonClicked();

private:
    QLineEdit *fileNameLineEdit;
    QPushButton *browseButton;
    QPushButton *acceptButton;
    QPushButton *cancelButton;
    QString savePath;
    QString suggestedFileName;
};

#endif // DOWNLOADDIALOG_H