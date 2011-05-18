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
#include "session.h"
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


void SessionManager::saveSessions()
{
    if (!m_safe || QWebSettings::globalSettings()->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return;
    m_safe = false;

    kDebug() << "in saveSessions()";

    QDomDocument document("sessionFile");
    QDomElement sessionFileDom = document.createElement("sessionFile");

    Session* s;
    foreach (s, m_sessionList)
    {
        QDomElement e = s->getUpdatedXml(document);
        sessionFileDom.appendChild(e);
    }

    document.appendChild(sessionFileDom);
    QFile mysessionFile("/home/tirtha/rekonq/lol");
    if (!mysessionFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        kDebug() << "Unable to open session file" << mysessionFile.fileName();
        return;
    }
    kDebug() << "starting write";

    /*QFile sessionFile(m_sessionFilePath);
    if (!sessionFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        kDebug() << "Unable to open session file" << sessionFile.fileName();
        return;
    }*/
    QTextStream out(&mysessionFile);
    kDebug() << "created textstream";
    //out << document.toString();
    document.save(out,2);
    kDebug() << "wrote out document";
    //sessionFile.close();
    kDebug() << "write finished";
    m_safe = true;
    return;
}

bool SessionManager::restoreSessions()
{
    QFile sessionFile(m_sessionFilePath);
    if (!sessionFile.exists())
        return false;
    if (!sessionFile.open(QFile::ReadOnly))
    {
        kDebug() << "Unable to open session file" << sessionFile.fileName();
        return false;
    }

    Session* alreadyOpenSession = rApp->mainWindowList().count()
                                ? m_sessionList.at(0)
                                : 0;

    QDomDocument document("sessionFile");
    kDebug() << "starting read";
    if (!document.setContent(&sessionFile, false))
    {
        kDebug() << "Unable to parse session file" << sessionFile.fileName();
        return false;
    }
    sessionFile.close();
    kDebug() << "read finished";

    QDomNodeList l = document.elementsByTagName("session");
    if (l.count() < 1)
    {
        return false;
    }

    /*if (l.at(0).toElement().hasAttribute("live") && !windowAlreadyOpen)
    {
        rApp->newMainWindow(true);
    }
    else if(!windowAlreadyOpen)
    {
        newSession(false);
    }

    Session *s = m_sessionList.at(0);
    s->setXml(l.at(0).toElement());
    if (l.at(0).toElement().hasAttribute("live"))
    {
        s->load();
    }*/

    Session* s;
    for (int i = 0; i < l.count(); ++i)
    {
        if (l.at(i).toElement().hasAttribute("live"))
        {
            if (alreadyOpenSession)
            {
                s = alreadyOpenSession;
                alreadyOpenSession = 0;
            }
            else
            {
                rApp->newMainWindow(true);
                s = m_sessionList.at(0);
            }
        }
        else
        {
            newSession(false);
            s = m_sessionList.at(0);
        }
        s->setXml(l.at(i).toElement());
        if (l.at(i).toElement().hasAttribute("live"))
        {
            s->load();
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
    kDebug() << "starting read";
    QDomDocument document("sessionFile");
    if (!document.setContent(&sessionFile, false))
    {
        kDebug() << "Unable to parse session file" << sessionFile.fileName();
        return list;
    }
    sessionFile.close();
    kDebug() << "read finished";
    QDomNodeList l = document.elementsByTagName("tab");
    kDebug() << l.count() << "tabs read";
    for (int i = 0; i < l.count(); ++i)
    {
        list << l.at(i).toElement().attribute("url");
    }
    return list;
}

Session* SessionManager::newSession(bool live, MainWindow *w)
{
    Session* s = new Session(this);
    if (live == true && w != 0)
    {
        s->toLive(w);
    }
    m_sessionList.prepend(s);
    kDebug() << "created new session";
    return s;
}
