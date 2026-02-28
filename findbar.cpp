#include "findbar.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QKeyEvent>

FindBar::FindBar(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
    , m_searchInput(new QLineEdit(this))
    , m_prevButton(new QPushButton("<", this))
    , m_nextButton(new QPushButton(">", this))
    , m_closeButton(new QPushButton("X", this))
    , m_matchLabel(new QLabel(this))
{
    m_layout->setContentsMargins(4, 2, 4, 2);

    m_searchInput->setPlaceholderText(tr("Find on page..."));
    m_searchInput->setMinimumWidth(200);
    m_prevButton->setFixedWidth(28);
    m_nextButton->setFixedWidth(28);
    m_closeButton->setFixedWidth(28);
    m_prevButton->setToolTip(tr("Previous match (Shift+Enter)"));
    m_nextButton->setToolTip(tr("Next match (Enter)"));
    m_closeButton->setToolTip(tr("Close find bar (Escape)"));
    m_matchLabel->setMinimumWidth(80);

    m_layout->addWidget(m_searchInput);
    m_layout->addWidget(m_prevButton);
    m_layout->addWidget(m_nextButton);
    m_layout->addWidget(m_matchLabel);
    m_layout->addStretch();
    m_layout->addWidget(m_closeButton);

    connect(m_searchInput, &QLineEdit::textChanged,
            this, &FindBar::onTextChanged);
    connect(m_nextButton, &QPushButton::clicked,
            this, &FindBar::findNext);
    connect(m_prevButton, &QPushButton::clicked,
            this, &FindBar::findPrevious);
    connect(m_closeButton, &QPushButton::clicked,
            this, &FindBar::deactivate);

    hide();
}

void FindBar::activate()
{
    show();
    m_searchInput->setFocus();
    m_searchInput->selectAll();
}

void FindBar::deactivate()
{
    hide();
    m_searchInput->clear();
    m_matchLabel->clear();
    emit searchCleared();
}

void FindBar::setMatchInfo(int current, int total)
{
    if (total == 0 && !m_searchInput->text().isEmpty())
        m_matchLabel->setText(tr("No matches"));
    else if (total > 0)
        m_matchLabel->setText(tr("%1 of %2").arg(current).arg(total));
    else
        m_matchLabel->clear();
}

void FindBar::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        deactivate();
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (event->modifiers() & Qt::ShiftModifier)
            findPrevious();
        else
            findNext();
        return;
    }
    QWidget::keyPressEvent(event);
}

void FindBar::onTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        m_matchLabel->clear();
        emit searchCleared();
    } else {
        emit searchRequested(text, true);
    }
}

void FindBar::findNext()
{
    const QString text = m_searchInput->text();
    if (!text.isEmpty())
        emit searchRequested(text, true);
}

void FindBar::findPrevious()
{
    const QString text = m_searchInput->text();
    if (!text.isEmpty())
        emit searchRequested(text, false);
}
