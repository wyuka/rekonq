/* ============================================================
*
* This file is a part of the rekonq project
*
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
#include "sessiontabdata.h"
#include "sessiontabdata.moc"

// Local Includes
#include "application.h"
#include "mainview.h"
#include "mainwindow.h"
#include "session.h"
#include "websnap.h"
#include "webtab.h"

// KDE Includes
#include <KStandardDirs>

// Qt Includes
#include <KUrl>
#include <QFile>
#include <QHash>
#include <QObject>
#include <QString>
#include <QPixmap>


SessionTabData::SessionTabData(QObject* parent)
        : QObject(parent)
{
}


SessionTabData::~SessionTabData()
{
}


SessionTabData::SessionTabData(const SessionTabData& tabData)
        : QObject()
{
    m_title = tabData.m_title;
    m_url = tabData.m_url;
}


void SessionTabData::operator=(const SessionTabData& tabData)
{
    m_title = tabData.m_title;
    m_url = tabData.m_url;
}


void SessionTabData::setWebTab(WebTab* webTab)
{
    m_webTab = webTab;
}


void SessionTabData::setTitle(QString title)
{
    m_title = title;
}


void SessionTabData::setUrl(KUrl url)
{
    m_url = url;
}


QString SessionTabData::title()
{
    return m_title;
}


KUrl SessionTabData::url()
{
    return m_url;
}


QPixmap& SessionTabData::thumbnail()
{
    if (m_thumbnail.isNull())
    {
        QString path = WebSnap::imagePathFromUrl(m_url);
        if (QFile::exists(path))
        {
            m_thumbnail.load(path);
        }
    }
    return m_thumbnail;
}


WebTab* SessionTabData::webTab()
{
    return m_webTab.data();
}


void SessionTabData::updateThumbnail(int width, int height)
{
    static int prevWidth = 200;
    static int prevHeight = 150;

    if (m_webTab.data() == 0)
    {
        return;
    }
    if (width && height)
    {
        prevWidth = width;
        prevHeight = height;
        m_thumbnail = WebSnap::renderTabPreview(*m_webTab.data()->page(), width , height);
    }
    else
    { 
        m_thumbnail = WebSnap::renderTabPreview(*m_webTab.data()->page(), prevWidth , prevHeight);
    }
    QString path = WebSnap::imagePathFromUrl(m_url);
    QFile::remove(path);
    m_thumbnail.save(path);
    kDebug() << "updated and saved";
}