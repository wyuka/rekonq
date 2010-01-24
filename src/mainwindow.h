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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Local Includes
#include "application.h"

// KDE Includes
#include <KMainWindow>
#include <KActionCollection>
#include <KToolBar>
#include <KUrl>

// Forward Declarations
class QWebFrame;

class KAction;
class KPassivePopup;

class FindBar;
class HistoryPanel;
class BookmarksPanel;
class WebInspectorPanel;
class WebTab;
class MainView;


/**
 * This class serves as the main window for rekonq.
 * It handles the menus, toolbars, and status bars.
 *
 */
class MainWindow : public KMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    MainView *mainView() const;
    WebTab *currentTab() const;
    QAction *actionByName(const QString name);
    virtual QSize sizeHint() const;
    virtual KActionCollection *actionCollection () const;
    void setWidgetsVisible(bool makeFullScreen);
   
private:
    void setupActions();
    void setupTools();
    void setupToolbars();
    void setupPanels();
    
public slots:
    void updateBrowser();
    void homePage();

    /**
     * Notifies a message in a popup
     *
     * @param msg The message to notify
     *
     * @param status The status message
     *
     */
    void notifyMessage(const QString &msg, Rekonq::Notify status = Rekonq::Info);

    void printRequested(QWebFrame *frame = 0);
    
signals:
    // switching tabs
    void ctrlTabPressed();
    void shiftCtrlTabPressed();

protected:
    /**
    * Filters (SHIFT + ) CTRL + TAB events and emit (shift)ctrlTabPressed()
    * to make switch tab
    * Filters out ESC key to show/hide the search bar
    */
    void keyPressEvent(QKeyEvent *event);

private slots:
    void postLaunch();
    void updateConfiguration();
    void browserLoading(bool);
    void updateActions();
    void updateWindowTitle(const QString &title = QString());

    // history related
    void openPrevious();
    void openNext();

    // Find Action slots
    void find(const QString &);
    void findNext();
    void findPrevious();

    // Zoom slots
    void viewTextBigger();
    void viewTextNormal();
    void viewTextSmaller();

    // File Menu slots
    void openLocation();
    void fileOpen();
    void fileSaveAs();

    void viewPageSource();
    void viewFullScreen(bool enable);

    // Tools Menu slots
    void privateBrowsing(bool enable);

    // Settings Menu slot
    void preferences();

    // clear private data
    void clearPrivateData();

    void aboutToShowBackMenu();
    void openActionUrl(QAction *action);

private:
    MainView *m_view;
    FindBar *m_findBar;
    
    HistoryPanel *m_historyPanel;
    BookmarksPanel *m_bookmarksPanel;
    WebInspectorPanel *m_webInspectorPanel;

    KAction *m_stopReloadAction;
    KMenu *m_historyBackMenu;

    KToolBar *m_mainBar;
    KToolBar *m_bmBar;

    QString m_lastSearch;

    KPassivePopup *m_popup;
    QTimer *m_hidePopup;

    KActionCollection *m_ac;
};

#endif // MAINWINDOW_H
