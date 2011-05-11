/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009-2011 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2009 by Yoram Bar-Haim <<yoram.b at zend dot com>
* Copyright (C) 2009-2011 by Lionel Chauvin <megabigbug@yahoo.fr>
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
#include "sessionmanager.h"
#include "sessionmanager.moc"

// Local Includes
#include "application.h"
#include "mainview.h"
#include "mainwindow.h"
#include "tabbar.h"
#include "webtab.h"

// KDE Includes
#include <KStandardDirs>

// Qt Includes
#include <QtCore/QFile>


SessionManager::SessionManager(QObject *parent)
        : QObject(parent)
        , m_safe(false)
{
    m_sessionFilePath = KStandardDirs::locateLocal("appdata" , "session");
}


void SessionManager::saveSession()
{
    if (!m_safe || QWebSettings::globalSettings()->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return;
    m_safe = false;


    QFile sessionFile(m_sessionFilePath);
    if (!sessionFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        kDebug() << "Unable to open session file" << sessionFile.fileName();
        return;
    }
    QFile mysessionFile("/home/tirtha/rekonq/sessionFile");
    if (!mysessionFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        kDebug() << "Unable to open session file" << mysessionFile.fileName();
        return;
    }
    QTextStream out(&sessionFile);
    MainWindowList wl = rApp->mainWindowList();
    QDomDocument document("session");
    QDomElement session = document.createElement("session");
    document.appendChild(session);

    Q_FOREACH(const QWeakPointer<MainWindow> &w, wl)
    {
        out << "window\n";
        QDomElement window = document.createElement("window");
        MainView *mv = w.data()->mainView();
        for (int i = 0 ; i < mv->count() ; i++)
        {
            out << mv->webTab(i)->url().toEncoded() << "\n";
            QDomElement tab = document.createElement("tab");
            tab.setAttribute("title", mv->webTab(i)->view()->title());
            tab.setAttribute("url", mv->webTab(i)->view()->url().toString());
            window.appendChild(tab);
        }

        // Current Tab for window
        out << "currenttab\n";
        out << mv->tabBar()->currentIndex() << "\n";
        window.setAttribute("currentTab",mv->tabBar()->currentIndex());
        session.appendChild(window);
    }
    QTextStream myout(&mysessionFile);
    document.save(myout,2);
    sessionFile.close();
    mysessionFile.close();
    m_safe = true;
    return;
}


bool SessionManager::restoreSession()
{
    QFile sessionFile(m_sessionFilePath);
    if (!sessionFile.exists())
        return false;
    if (!sessionFile.open(QFile::ReadOnly))
    {
        kDebug() << "Unable to open session file" << sessionFile.fileName();
        return false;
    }
    
    QFile mysessionFile("/home/tirtha/rekonq/sessionFile");
    if (!mysessionFile.exists())
        return false;
    if (!mysessionFile.open(QFile::ReadOnly))
    {
        kDebug() << "Unable to open session file" << mysessionFile.fileName();
        return false;
    }

    QTextStream in(&sessionFile);
    QString line;
    MainWindowList mywl;
    bool windowAlreadyOpen = rApp->mainWindowList().count();
    QDomDocument document("session");
    if (!document.setContent(&mysessionFile, false))
    {
        kDebug() << "Unable to parse session file" << mysessionFile.fileName();
        return false;
    }

    QDomElement session = document.elementsByTagName("session").at(0).toElement();
    for (uint winNo = 0; winNo < session.elementsByTagName("window").length(); winNo++ )
    {
        QDomElement window = session.elementsByTagName("window").at(winNo).toElement();
        int currentTab = window.attribute("currentTab").toInt();
        
        if (windowAlreadyOpen)
        {
            windowAlreadyOpen = false;
        }
        else
        {
            rApp->newMainWindow(false);
        }

        mywl = rApp->mainWindowList();
        if (mywl.count() > 0)
        {
            MainView *mv = mywl[0].data()->mainView();
            
            for (uint tabNo = 0; tabNo < window.elementsByTagName("tab").length(); tabNo++)
            {
                QDomElement tab = window.elementsByTagName("tab").at(tabNo).toElement();
                rApp->loadUrl(KUrl(tab.attribute("url")), Rekonq::NewFocusedTab);
            }
            mv->setCurrentIndex(currentTab);
        }

        
    }
    /*do
    {
        line = in.readLine();
        if (line == QL1S("window"))
        {
            line = in.readLine();
            if (windowAlreadyOpen)
            {
                rApp->loadUrl(KUrl(line), Rekonq::CurrentTab);
                windowAlreadyOpen = false;
            }
            else
            {
                rApp->loadUrl(KUrl(line), Rekonq::NewWindow);
            }
        }
        else
        {
            if (line == QL1S("currenttab"))
            {
                line = in.readLine();
                bool ok;
                int idx = line.toInt(&ok);
                if (ok)
                {
                    // Get last mainwindow created which will be first one in mainwindow list
                    MainWindowList wl = rApp->mainWindowList();
                    if (wl.count() > 0)
                    {
                        MainView *mv = wl[0].data()->mainView();
                        emit mv->tabBar()->setCurrentIndex(idx);
                    }
                }
            }
            else
            {
                rApp->loadUrl(KUrl(line), Rekonq::NewFocusedTab);
            }
        }
    }
    while (!line.isEmpty());*/

    return true;
}


QStringList SessionManager::closedSites()
{
    QStringList list;

    QFile sessionFile(m_sessionFilePath);
    if (!sessionFile.exists())
        return list;
    if (!sessionFile.open(QFile::ReadOnly))
    {
        kDebug() << "Unable to open session file" << sessionFile.fileName();
        return list;
    }

    QTextStream in(&sessionFile);
    QString line;
    do
    {
        line = in.readLine();
        if (line != QL1S("window"))
        {
            if (line == QL1S("currenttab"))
            {
                in.readLine();  // drop out the next field, containing the index of the current tab..
            }
            else
            {
                list << QString(line);
            }
        }
    }
    while (!line.isEmpty());

    return list;
}
