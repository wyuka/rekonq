/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009 by Nils Weigel <nehlsen at gmail dot com>
* Copyright (C) 2010 by Andrea Diamantini <adjam7 at gmail dot com>
*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* ============================================================ */


#ifndef BOOKMARKSPANEL_H
#define BOOKMARKSPANEL_H


// Local Includes
#include "rekonqprivate_export.h"
#include "application.h"
#include "urltreeview.h"

// Qt Includes
#include <QDockWidget>

// KDE Includes
#include <KBookmark>
#include <KActionCollection>

// Forward Declarations
class KUrl;
class QModelIndex;


class REKONQ_TESTS_EXPORT BookmarksPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit BookmarksPanel(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~BookmarksPanel();

signals:
    void openUrl(const KUrl &, const Rekonq::OpenType &);
    void itemHovered(const QString &);
    void saveOnlyRequested();

private slots:
    void contextMenuBk(const QPoint &pos);
    void contextMenuBkGroup(const QPoint &pos, const bool emptyGroup = false);
    void contextMenuBlank(const QPoint &pos);
    void deleteBookmark();
    void openFolderInTabs();
    void editBookmark();
    void newBookmark();
    void newBookmarkGroup();
    void newSeparator();
    void onCollapse(const QModelIndex &index);
    void onExpand(const QModelIndex &index);
    void bookmarkCurrentPage();
    void loadFoldedState(const QModelIndex &root);
    void loadFoldedState();


private:
    void setup();
    void setupActions();
    void contextMenuSeparator(const QPoint &pos);
    KBookmark bookmarkForIndex(const QModelIndex &index);

    UrlTreeView *m_treeView;
    KActionCollection *m_ac;
    bool m_loadingState;
};

#endif // BOOKMARKSPANEL_H
