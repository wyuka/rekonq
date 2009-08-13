/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2008-2009 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2009 by Paweł Prażak <pawelprazak at gmail dot com>
* Copyright (C) 2009 by Lionel Chauvin <megabigbug@yahoo.fr>
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


// Self Includes
#include "bookmarks.h"
#include "bookmarks.moc"

// Local Includes
#include "mainwindow.h"
#include "webview.h"
#include "application.h"

// KDE Includes
#include <KActionCollection>
#include <KBookmark>
#include <KBookmarkAction>
#include <KBookmarkGroup>
#include <KBookmarkMenu>
#include <KToolBar>
#include <KDebug>
#include <KMenu>
#include <KStandardDirs>
#include <KUrl>

// Qt Includes
#include <QtCore/QFile>
#include <QtGui/QActionGroup>



BookmarkOwner::BookmarkOwner(QObject *parent)
        : QObject(parent)
        , KBookmarkOwner()
{
}


void BookmarkOwner::openBookmark(const KBookmark & bookmark,
                                 Qt::MouseButtons mouseButtons,
                                 Qt::KeyboardModifiers keyboardModifiers)
{
    if (keyboardModifiers & Qt::ControlModifier || mouseButtons == Qt::MidButton)
        emit openUrl(bookmark.url(), Rekonq::SettingOpenTab);
    else
        emit openUrl(bookmark.url(), Rekonq::CurrentTab);
}


bool BookmarkOwner::supportsTabs() const
{
    return true;
}


QString BookmarkOwner::currentUrl() const
{
    return Application::instance()->mainWindow()->currentTab()->url().url();
}


QString BookmarkOwner::currentTitle() const
{
    return Application::instance()->mainWindow()->currentTab()->title();
}


void BookmarkOwner::openFolderinTabs(const KBookmarkGroup &bm)
{
    QList<KUrl> urlList = bm.groupUrlList();
    QList<KUrl>::iterator url;
    for (url = urlList.begin(); url != urlList.end(); ++url)
    {
        Application::instance()->loadUrl(*url, Rekonq::NewCurrentTab);
    }
}


// ------------------------------------------------------------------------------------------------------


BookmarkMenu::BookmarkMenu(KBookmarkManager *manager,
                           KBookmarkOwner *owner,
                           KMenu *menu,
                           KActionCollection* actionCollection)
        : KBookmarkMenu(manager, owner, menu, actionCollection)

{
    KAction *a = KStandardAction::addBookmark(this, SLOT(slotAddBookmark()), this);
    a->setText(i18n("Add Bookmark"));
    actionCollection->addAction(QLatin1String("rekonq_add_bookmark"),a);
}

BookmarkMenu::~BookmarkMenu()
{
}


KMenu *BookmarkMenu::viewContextMenu(QAction *action)
{
    // contextMenu() returns an invalid  KMenu (seg fault) for the folders in the toolbar
    KMenu *menu = contextMenu(action);
    if(menu)
        return menu;

    return 0;   // new KMenu();
}


void BookmarkMenu::slotAddBookmark()
{
    KAction *action = qobject_cast<KAction *>(sender());
    if (action && !action->data().isNull())
    {
        KBookmarkGroup parentBookmark = manager()->findByAddress(parentAddress()).toGroup();
        /// TODO Add bookmark Icon
        parentBookmark.addBookmark(owner()->currentTitle(), action->data().toUrl());
        manager()->emitChanged();
        return;
    }

    KBookmarkMenu::slotAddBookmark();
}


// ------------------------------------------------------------------------------------------------------


BookmarkProvider::BookmarkProvider(QWidget *parent)
        : QObject(parent)
        , m_parent(parent)
        , m_manager(0)
        , m_owner(0)
        , m_menu(new KMenu(m_parent))
        , m_actionCollection(new KActionCollection(this))
        , m_bookmarkMenu(0)
        , m_bookmarkToolBar(0)
{
    KUrl bookfile = KUrl("~/.kde/share/apps/konqueror/bookmarks.xml");  // share konqueror bookmarks

    if (!QFile::exists(bookfile.path()))
    {
        bookfile = KUrl("~/.kde4/share/apps/konqueror/bookmarks.xml");
        if (!QFile::exists(bookfile.path()))
        {
            QString bookmarksDefaultPath = KStandardDirs::locate("appdata" , "defaultbookmarks.xbel");
            kWarning() << bookmarksDefaultPath;
            QFile bkms(bookmarksDefaultPath);
            QString bookmarksPath = KStandardDirs::locateLocal("appdata", "bookmarks.xml", true);
            bookmarksPath.replace("rekonq", "konqueror");
            bkms.copy(bookmarksPath);

            bookfile = KUrl(bookmarksPath);
        }
    }
    m_manager = KBookmarkManager::managerForExternalFile(bookfile.path());
    connect(m_manager, SIGNAL(changed(const QString &, const QString &)),
            this, SLOT(slotBookmarksChanged(const QString &, const QString &)));

    // setup menu
    m_owner = new BookmarkOwner(this);
    connect(m_owner, SIGNAL(openUrl(const KUrl&, const Rekonq::OpenType &)), this, SIGNAL(openUrl(const KUrl&, const Rekonq::OpenType &)));
    m_bookmarkMenu = new BookmarkMenu(m_manager, m_owner, m_menu, m_actionCollection);

    // setup toolbar
    setupToolBar();
}


BookmarkProvider::~BookmarkProvider()
{
    delete m_bookmarkToolBar;
    delete m_bookmarkMenu;
    delete m_actionCollection;
    delete m_menu;
    delete m_owner;
    delete m_manager;
}


void BookmarkProvider::setupToolBar()
{
    m_bookmarkToolBar = new KToolBar(m_parent);
    m_bookmarkToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_bookmarkToolBar->setIconDimensions(16);
    m_bookmarkToolBar->setAcceptDrops(true);
    m_bookmarkToolBar->setContentsMargins(0, 0, 0, 0);
    m_bookmarkToolBar->setMinimumHeight(16);
    m_bookmarkToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_bookmarkToolBar, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenu(const QPoint &)));

    slotBookmarksChanged("", "");
}


void BookmarkProvider::slotBookmarksChanged(const QString &group, const QString &caller)
{
    Q_UNUSED(group)
    Q_UNUSED(caller)

    if (!m_bookmarkToolBar)
    {
        kWarning() << "There is no bookmark toolbar";
        return;
    }

    KBookmarkGroup toolBarGroup = m_manager->toolbar();
    if (toolBarGroup.isNull())
        return;

    m_bookmarkToolBar->clear();

    KBookmark bookmark = toolBarGroup.first();
    while (!bookmark.isNull())
    {
        m_bookmarkToolBar->addAction(fillBookmarkBar(bookmark));
        bookmark = toolBarGroup.next(bookmark);
    }
}


QAction *BookmarkProvider::actionByName(const QString &name)
{
    QAction *action = m_actionCollection->action(name);
    if (action)
        return action;
    /* else */
    kWarning() << "Action named: " << name << " not found, returning empty action.";
    return new QAction(this);  // return empty object instead of NULL pointer
}


void BookmarkProvider::contextMenu(const QPoint &point)
{
    KAction* action = dynamic_cast<KAction*>(m_bookmarkToolBar->actionAt(point));
    if (!action)
        return;
    KMenu *menu = m_bookmarkMenu->viewContextMenu(action);
    menu->popup(m_bookmarkToolBar->mapToGlobal(point));
}


KActionMenu* BookmarkProvider::bookmarkActionMenu()
{
    KActionMenu *bookmarkActionMenu = new KActionMenu(this);
    bookmarkActionMenu->setMenu(m_menu);
    bookmarkActionMenu->setText(i18n("&Bookmarks"));
    return bookmarkActionMenu;
}


KAction* BookmarkProvider::bookmarkToolBarAction()
{
    KAction *bookmarkToolBarAction = new KAction(this);
    bookmarkToolBarAction->setDefaultWidget(m_bookmarkToolBar);  // The ownership is transferred to action
    bookmarkToolBarAction->setText(i18n("Bookmarks Bar"));
    bookmarkToolBarAction->setShortcutConfigurable(false);
    return bookmarkToolBarAction;
}


KAction *BookmarkProvider::fillBookmarkBar(const KBookmark &bookmark)
{
    if (bookmark.isGroup())
    {
        KBookmarkGroup group = bookmark.toGroup();
        KBookmark bm = group.first();
        KActionMenu *menuAction = new KActionMenu(KIcon(bookmark.icon()), bookmark.text(), this);
        menuAction->setDelayed(false);
        while (!bm.isNull())
        {
            menuAction->addAction(fillBookmarkBar(bm));
            bm = group.next(bm);
        }
        return menuAction;
    }
 
    if(bookmark.isSeparator())
    {
        KAction *a = new KAction(this);
        a->setSeparator(true);
        return a;
    }
    else
    {
        return new KBookmarkAction(bookmark, m_owner, this);
    }
}
