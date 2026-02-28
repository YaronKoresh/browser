#ifndef FINDBAR_H
#define FINDBAR_H

#include <QtWidgets/QWidget>

class QLineEdit;
class QPushButton;
class QLabel;
class QHBoxLayout;

class FindBar : public QWidget {
    Q_OBJECT

public:
    explicit FindBar(QWidget *parent = nullptr);

    void activate();
    void deactivate();
    void setMatchInfo(int current, int total);

signals:
    void searchRequested(const QString &text, bool forward);
    void searchCleared();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTextChanged(const QString &text);
    void findNext();
    void findPrevious();

private:
    QHBoxLayout *m_layout      = nullptr;
    QLineEdit   *m_searchInput = nullptr;
    QPushButton *m_nextButton  = nullptr;
    QPushButton *m_prevButton  = nullptr;
    QPushButton *m_closeButton = nullptr;
    QLabel      *m_matchLabel  = nullptr;
};

#endif // FINDBAR_H
