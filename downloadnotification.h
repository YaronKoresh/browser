#ifndef DOWNLOADNOTIFICATION_H
#define DOWNLOADNOTIFICATION_H

#include <QtWidgets/QFrame>

class QLabel;
class QPushButton;
class QCheckBox;

class DownloadNotification : public QFrame {
    Q_OBJECT

public:
    explicit DownloadNotification(const QString &savePath, QWidget *parent = nullptr);

    [[nodiscard]] QString savePath() const;
    [[nodiscard]] bool isLaunchable() const;

private slots:
    void openFile();
    void openFolder();

private:
    QString      m_savePath;
    bool         m_launchable       = false;
    QLabel      *m_label            = nullptr;
    QCheckBox   *m_confirmCheckBox  = nullptr;
    QPushButton *m_openFileButton   = nullptr;
    QPushButton *m_openFolderButton = nullptr;
    QPushButton *m_closeButton      = nullptr;
};

#endif // DOWNLOADNOTIFICATION_H
