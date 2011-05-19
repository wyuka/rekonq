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
#include <QtCore/QString>
#include <QtCore/QStringList>

Session::Session(QObject* parent)
        : QObject(parent)
{
    m_live = false;
    m_window = 0;
}

void Session::update()
{
    if (!m_live)
    {
        return;
    }
    MainView *mv = m_window->mainView();
    m_urlList.clear();
    for (int i=0; i < mv->count(); ++i)
    {
        QByteArray url = mv->webTab(i)->url().toEncoded();
        m_urlList << url;
    }
    //emit changesMade();
}


QDomElement Session::getXml(QDomDocument& document)
{
    kDebug() << "in here..";
    QDomElement sessionDom = document.createElement("session");
    QString urlString;
    foreach(urlString, m_urlList)
    {
        QDomElement tab = document.createElement("tab");
        tab.setAttribute("url",urlString);
        sessionDom.appendChild(tab);
    }
    if (m_live)
    {
        sessionDom.setAttribute("active","true");
    }
    return sessionDom;
}


void Session::setXml(QDomElement sessionDom)
{
    m_urlList.clear();
    for (int tabNo = 0; tabNo < sessionDom.elementsByTagName("tab").count(); ++tabNo)
    {
        m_urlList << sessionDom.elementsByTagName("tab").at(tabNo).toElement().attribute("url");
    }
}


bool Session::load()
{
    if (m_live)
    {
        if (m_window)
        {
            QString urlString;
            bool firstTab = true;
            foreach (urlString, m_urlList)
            {
                kDebug() << "loading url " << urlString << "while firstTab is " << firstTab;
                if (firstTab)
                {
                    rApp->loadUrl(KUrl(urlString), Rekonq::CurrentTab, m_window);
                    firstTab = false;
                }
                else
                {
                    rApp->loadUrl(KUrl(urlString), Rekonq::NewFocusedTab, m_window);
                }
            }
        }
        else
        {
            return false;
        }
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


void Session::deactivate()
{
    if (m_live)
    {
        m_window = 0;
        m_live = false;
        emit changesMade();
    }
}


void Session::activate(MainWindow* w)
{
    if (!m_live)
    {
        m_window = w;
        m_live = true;
    }
}