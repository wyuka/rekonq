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
#include "session.h"
#include "session.moc"

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

Session::Session(QObject* parent)
        : QObject(parent)
{
    m_live = false;
    m_window = 0;
    
    QDomDocument d("sessionFile");
    m_sessionDom = d.createElement("session");
}

QDomElement Session::getUpdatedXml(QDomDocument& document)
{
    if (m_live)
    {
        QDomElement sessionDom = document.createElement("session");
        MainView *mv = m_window->mainView();
        for (int i=0; i < mv->count(); ++i)
        {
            QByteArray url = mv->webTab(i)->url().toEncoded();
            QString title = mv->webTab(i)->view()->title();
            
            QDomElement tab = document.createElement("tab");
            tab.setAttribute("title",title);
            tab.setAttribute("url",QString(url));
            if (i == mv->currentIndex())
            {
                tab.setAttribute("currentTab","true");
            }
            sessionDom.appendChild(tab);
        }
        if (m_window == rApp->mainWindowList()[0].data())
        {
            sessionDom.setAttribute("currentSession","true");
        }
        sessionDom.setAttribute("live","true");
        m_sessionDom = sessionDom;
    }
    
    return m_sessionDom;
}


void Session::setXml(QDomElement sessionDom)
{
    m_sessionDom = sessionDom;
}


bool Session::load()
{
    if (m_sessionDom.hasAttribute("live"))
    {
        if (m_window)
        {   
            QDomElement firstTab = m_sessionDom.elementsByTagName("tab").at(0).toElement();
            rApp->loadUrl(KUrl(firstTab.attribute("url")), Rekonq::CurrentTab);
            kDebug() << firstTab.attribute("url");

            for (uint tabNo = 1; tabNo < m_sessionDom.elementsByTagName("tab").length(); ++tabNo)
            {
                QDomElement tab = m_sessionDom.elementsByTagName("tab").at(tabNo).toElement();
                rApp->loadUrl(KUrl(tab.attribute("url")), Rekonq::NewFocusedTab);
                kDebug() << tab.attribute("url");
            }
            m_live = true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        m_live = false;
    }

    return true;
}


void Session::setMainWindow(MainWindow* w)
{
    if (w)
    {
        m_window = w;
    }
}


MainWindow* Session::mainWindow()
{
    return m_window;
}


void Session::toDead()
{
    if (m_live)
    {
        m_window = 0;
        m_live = false;

        if (m_sessionDom.hasAttribute("live"))
        {
            m_sessionDom.removeAttribute("live");
        }
    }
}


void Session::toLive(MainWindow* w)
{
    if (!m_live)
    {
        m_window = w;
        m_live = true;
        m_sessionDom.setAttribute("live","true");
    }
}