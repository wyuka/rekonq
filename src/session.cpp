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
#include "sessiontabdata.h"
#include "tabbar.h"
#include "websnap.h"
#include "webtab.h"

// KDE Includes
#include <KStandardDirs>

// Qt Includes
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>

Session::Session(QObject* parent)
        : QObject(parent)
        , m_window(0)
        , m_currentTabData(0)
{
    m_active = false;
    m_window = 0;
    m_webTabMap.clear();
    m_tabDataList.clear();
}


TabDataList Session::tabDataList()
{
    return m_tabDataList;
}


void Session::addTabData(WebTab* webTab)
{
    if (m_webTabMap.find(webTab) != m_webTabMap.end())
    {
        changeTabData(webTab);
        return;
    }
    SessionTabData* tabData = new SessionTabData(this);
    tabData->setTitle(webTab->view()->title());
    tabData->setUrl(webTab->url());
    m_tabDataList << tabData;
    m_webTabMap[webTab] = tabData;
    emit changesMade();
}


void Session::changeTabData(WebTab* webTab)
{
    if (m_webTabMap.find(webTab) == m_webTabMap.end())
    {
        addTabData(webTab);
        return;
    }
    SessionTabData* tabData = m_webTabMap[webTab];
    tabData->setTitle(webTab->view()->title());
    tabData->setUrl(webTab->url());
    tabData->setThumbnail(WebSnap::renderTabPreview(*webTab->page(), 200 , 150));
    emit changesMade();
}


void Session::removeTabData(WebTab* webTab)
{
    if (m_webTabMap.find(webTab) == m_webTabMap.end())
    {
        return;
    }
    SessionTabData* tabData = m_webTabMap[webTab];
    m_tabDataList.removeOne(tabData);
    m_webTabMap.remove(webTab);
    tabData->deleteLater();
    emit changesMade();
}


void Session::setCurrentTabData(WebTab* webTab)
{
    if (m_webTabMap.find(webTab) == m_webTabMap.end())
    {
        addTabData(webTab);
    }
    m_currentTabData = m_webTabMap[webTab];
    changesMade();
}


QDomElement Session::getXml(QDomDocument& document)
{
    QDomElement sessionDom = document.createElement("session");
    SessionTabData* tabData;
    QString thumbnailPath;
    foreach(tabData, m_tabDataList)
    {
        QDomElement tab = document.createElement("tab");
        tab.setAttribute("url",QString(tabData->url().toEncoded()));
        tab.setAttribute("title",tabData->title());
        if (m_active)
        {
            tabData->saveThumbnail();
        }
        if (m_currentTabData == tabData)
        {
            tab.setAttribute("current","true");
        }
        sessionDom.appendChild(tab);
    }
    if (m_active)
    {
        sessionDom.setAttribute("active","true");
    }
    return sessionDom;
}


void Session::setXml(QDomElement sessionDom)
{
    clearSession();
    for (int tabNo = 0; tabNo < sessionDom.elementsByTagName("tab").count(); ++tabNo)
    {
        KUrl url(sessionDom.elementsByTagName("tab").at(tabNo).toElement().attribute("url"));
        QString title(sessionDom.elementsByTagName("tab").at(tabNo).toElement().attribute("title"));
        SessionTabData *tabData = new SessionTabData(this);
        tabData->setUrl(url);
        tabData->setTitle(title);
        tabData->loadThumbnail();
        if (sessionDom.elementsByTagName("tab").at(tabNo).toElement().hasAttribute("current"))
        {
            m_currentTabData = tabData;
        }

        m_tabDataList << tabData;
    }
}


bool Session::load()
{
    if (m_active)
    {
        if (m_window)
        {
            bool firstTab = true;
            SessionTabData* tabData;
            foreach(tabData, m_tabDataList)
            {
                if (firstTab)
                {
                    rApp->loadUrl(tabData->url(), Rekonq::CurrentTabLastWindow);
                    firstTab = false;
                }
                else if (tabData != m_currentTabData)
                {
                    rApp->loadUrl(tabData->url(), Rekonq::NewBackTabLastWindow);
                }
                else
                {
                    rApp->loadUrl(tabData->url(), Rekonq::NewFocusedTabLastWindow);
                }
                m_tabDataList.removeOne(tabData);
                tabData->deleteLater();
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}


void Session::setWindow(MainWindow* w)
{
    if (!m_active)
    {
        m_active = true;
    }
    m_window = w;
    connect(m_window,SIGNAL(tabAdded(WebTab*)),this,SLOT(addTabData(WebTab*)));
    connect(m_window,SIGNAL(tabClosed(WebTab*)),this,SLOT(removeTabData(WebTab*)));
    connect(m_window,SIGNAL(tabChanged(WebTab*)),this,SLOT(changeTabData(WebTab*)));
    connect(m_window,SIGNAL(currentTabChanged(WebTab*)),this,SLOT(setCurrentTabData(WebTab*)));
}


MainWindow* Session::window()
{
    return m_window;
}


void Session::deactivate()
{
    if (m_active)
    {
        m_webTabMap.clear();
        m_window->disconnect(this);
        m_window = 0;
        m_active = false;
        
        emit changesMade();
    }
}


void Session::clearSession()
{
    SessionTabData* tabData;
    foreach(tabData, m_tabDataList)
    {
        tabData->deleteLater();
    }
    m_tabDataList.clear();
    m_webTabMap.clear();
}
