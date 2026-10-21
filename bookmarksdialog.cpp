#include "bookmarksdialog.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtCore/QStringListModel>

BookmarksDialog::BookmarksDialog(BookmarkManager *manager, QWidget *parent)
    : QDialog(parent)
    , m_manager(manager)
{
    setWindowTitle(tr("Bookmarks"));
    setMinimumSize(400, 350);

    auto *layout = new QVBoxLayout(this);
    m_listView = new QListView(this);
    m_model = new QStringListModel(this);
    m_listView->setModel(m_model);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_listView);

    auto *buttonLayout = new QHBoxLayout;
    m_openBtn = new QPushButton(tr("Open"), this);
    m_deleteBtn = new QPushButton(tr("Delete"), this);
    m_closeBtn = new QPushButton(tr("Close"), this);
    buttonLayout->addWidget(m_openBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeBtn);
    layout->addLayout(buttonLayout);

    connect(m_openBtn, &QPushButton::clicked, this, &BookmarksDialog::onOpenClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &BookmarksDialog::onDeleteClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    if (m_manager)
        connect(m_manager, &BookmarkManager::bookmarksChanged, this, &BookmarksDialog::refresh);

    refresh();
}

void BookmarksDialog::refresh()
{
    const int previousRow = m_listView->currentIndex().row();

    m_shown = m_manager ? m_manager->bookmarks() : QVector<Bookmark>();

    QStringList items;
    items.reserve(m_shown.size());
    for (const auto &bm : std::as_const(m_shown))
        items.append(QString("%1 — %2").arg(bm.title, bm.url.toString()));
    m_model->setStringList(items);

    if (!m_shown.isEmpty() && previousRow >= 0)
        m_listView->setCurrentIndex(m_model->index(qMin(previousRow, m_shown.size() - 1), 0));
}

void BookmarksDialog::onOpenClicked()
{
    const int row = m_listView->currentIndex().row();
    if (row < 0 || row >= m_shown.size())
        return;
    emit openRequested(m_shown[row].url);
    accept();
}

void BookmarksDialog::onDeleteClicked()
{
    const int row = m_listView->currentIndex().row();
    if (!m_manager || row < 0 || row >= m_shown.size())
        return;
    m_manager->removeBookmark(m_shown[row].url);
}
