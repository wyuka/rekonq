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


void SessionTabData::setTitle(QString title)
{
    m_title = title;
}


void SessionTabData::setUrl(KUrl url)
{
    m_url = url;
}


void SessionTabData::setThumbnail(QPixmap pixmap)
{
    saveThumbnail(pixmap);
}


QString SessionTabData::title()
{
    return m_title;
}


KUrl SessionTabData::url()
{
    return m_url;
}


QPixmap SessionTabData::thumbnail()
{
    QString path = WebSnap::imagePathFromUrl(m_url);
    QPixmap thumbnail;
    if (QFile::exists(path))
    {
        thumbnail.load(path);
    }
    return thumbnail;
}


void SessionTabData::saveThumbnail(QPixmap& pixmap)
{
    QString path = WebSnap::imagePathFromUrl(m_url);
    QFile::remove(path);
    pixmap.save(path);
}