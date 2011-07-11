/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009-2011 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2009 by Yoram Bar-Haim <<yoram.b at zend dot com>
* Copyright (C) 2009-2011 by Lionel Chauvin <megabigbug@yahoo.fr>
* Copyright (C) 2011 by Tirtha Chatterjee <tirtha.p.chatterjee@gmail.com>
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
#include "panoramascene.h"
#include "session.h"
#include "tabbar.h"
#include "webtab.h"

// KDE Includes
#include <KStandardDirs>

// Qt Includes
#include <QFile>

SessionManager::SessionManager(QObject *parent)
        : QObject(parent)
        , m_safe(false)
{
    m_sessionFilePath = KStandardDirs::locateLocal("appdata" , "session");

    m_scene = new PanoramaScene(this);

    m_timer = new QTimer(this);
    m_timer->setInterval(saveDelay);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(saveSessions()));
    connect(this, SIGNAL(readyForSave()), this, SLOT(doDelayedSave()));
}


SessionManager::~SessionManager()
{
}


SessionList SessionManager::sessionList()
{
    return m_sessionList;
}


void SessionManager::doDelayedSave()
{
    m_timer->start();
}


void SessionManager::saveSessions()
{
    if (!m_safe || QWebSettings::globalSettings()->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return;
    m_safe = false;

    QDomDocument domDocument("sessionFile");
    QDomElement sessionFileDom = domDocument.createElement("sessionFile");

    Session* s;
    foreach (s, m_sessionList)
    {
        QDomElement e = s->getXml(domDocument);
        sessionFileDom.appendChild(e);
    }

    domDocument.appendChild(sessionFileDom);

    QFile sessionFile(m_sessionFilePath);
    if (!sessionFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        kDebug() << "Unable to open session file" << sessionFile.fileName();
        return;
    }
    QTextStream out(&sessionFile);
    domDocument.save(out,2);
    sessionFile.close();
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
                                ? m_sessionList.last()
                                : 0;

    QDomDocument document("sessionFile");
    if (!document.setContent(&sessionFile, false))
    {
        kDebug() << "Unable to parse session file" << sessionFile.fileName();
        return false;
    }
    sessionFile.close();

    QDomNodeList l = document.elementsByTagName("session");
    if (l.count() < 1)
    {
        return false;
    }

    Session* s;
    for (int i = 0; i < l.count(); ++i)
    {
        if (l.at(i).toElement().hasAttribute("active"))
        {
            if (alreadyOpenSession)
            {
                s = alreadyOpenSession;
                alreadyOpenSession = 0;
            }
            else
            {
                rApp->newMainWindow(true);
                s = m_sessionList.last();
            }
            s->setXml(l.at(i).toElement());
            s->load();
        }
        else
        {
            newSession(false);
            s = m_sessionList.last();
            s->setXml(l.at(i).toElement());
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
    sessionFile.close();
    QDomNodeList l = document.elementsByTagName("tab");
    for (int i = 0; i < l.count(); ++i)
    {
        list << l.at(i).toElement().attribute("url");
    }
    return list;
}


Session* SessionManager::newSession(bool active, MainWindow *w)
{
    Session* s = new Session(this);
    if (active == true && w != 0)
    {
        s->setWindow(w);
    }
    emit sessionAdded(s);
    connect(s,SIGNAL(changesMade()),this,SIGNAL(readyForSave()));
    m_sessionList << s;
    return s;
}


void SessionManager::deactivateSession()
{
    Session* s;
    MainWindow *window = qobject_cast<MainWindow *>(sender());
    foreach (s, m_sessionList)
    {
        if (s->window() == window)
        {
            s->deactivate();
            emit sessionDeactivated(s);
            break;
        }
    }
}


void SessionManager::activateSession(Session* session)
{
    if (session->isActive())
    {
        return;
    }
    MainWindow* window = rApp->newMainWindow(true);
    Session* s = m_sessionList.last();
    m_sessionList.removeLast();
    s->deactivate();
    s->deleteLater();
    session->setWindow(window);
    session->load();

    emit sessionActivated(session);
}


void SessionManager::deleteSession(Session* session)
{
    session->deactivate();
    m_sessionList.removeOne(session);

    emit sessionDeleted(session);
    emit readyForSave();

    session->deleteLater();
}
