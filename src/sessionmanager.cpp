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

    MainWindowList wl = rApp->mainWindowList();
    QDomDocument document("sessionFile");
    QDomElement session = document.createElement("sessionFile");
    document.appendChild(session);

    Q_FOREACH(const QWeakPointer<MainWindow> &w, wl)
    {
        QDomElement window = document.createElement("session");
        MainView *mv = w.data()->mainView();
        for (int i = 0 ; i < mv->count() ; i++)
        {
            QDomElement tab = document.createElement("tab");
            tab.setAttribute("title", mv->webTab(i)->view()->title());
            tab.setAttribute("url", mv->webTab(i)->view()->url().toString());
            window.appendChild(tab);
        }
        window.setAttribute("currentTab",mv->tabBar()->currentIndex());
        session.appendChild(window);
    }
    QTextStream out(&sessionFile);
    document.save(out,2);
    sessionFile.close();
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

    MainWindowList wl;
    bool windowAlreadyOpen = rApp->mainWindowList().count();
    QDomDocument document("sessionFile");
    if (!document.setContent(&sessionFile, false))
    {
        kDebug() << "Unable to parse session file" << sessionFile.fileName();
        return false;
    }

    QDomElement session = document.elementsByTagName("sessionFile").at(0).toElement();
    for (uint winNo = 0; winNo < session.elementsByTagName("session").length(); winNo++)
    {
        QDomElement window = session.elementsByTagName("session").at(winNo).toElement();
        int currentTab = window.attribute("currentTab").toInt();

        QDomElement firstTab = window.elementsByTagName("tab").at(0).toElement();
        if (windowAlreadyOpen)
        {
            windowAlreadyOpen = false;
            rApp->loadUrl(KUrl(firstTab.attribute("url")), Rekonq::CurrentTab);
        }
        else
        {
            rApp->loadUrl(KUrl(firstTab.attribute("url")), Rekonq::NewWindow);
        }

        wl = rApp->mainWindowList();
        if (wl.count() > 0)
        {   
            for (uint tabNo = 1; tabNo < window.elementsByTagName("tab").length(); tabNo++)
            {
                QDomElement tab = window.elementsByTagName("tab").at(tabNo).toElement();
                rApp->loadUrl(KUrl(tab.attribute("url")), Rekonq::NewFocusedTab);
            }
            MainView *mv = wl[0].data()->mainView();
            mv->setCurrentIndex(currentTab);
        }   
    }
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
    
    QDomDocument document("sessionFile");
    if (!document.setContent(&sessionFile, false))
    {
        kDebug() << "Unable to parse session file" << sessionFile.fileName();
        return list;
    }
    
    QDomNodeList l = document.elementsByTagName("tab");
    for (int i = 0; i < l.count(); ++i)
    {
        list << l.at(i).toElement().attribute("url");
    }
    return list;
}