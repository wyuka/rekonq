/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009 Nokia Corporation <qt-info@nokia.com>
* Copyright (C) 2009 by Andrea Diamantini <adjam7 at gmail dot com>
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


#include "websnap.h"
#include "websnap.moc"

#include <KDebug>
#include <KStandardDirs>

#include <QSize>
#include <QWebFrame>
#include <QWebSettings>
#include <QPainter>
#include <QTimer>
#include <QFile>


#define WIDTH  200
#define HEIGHT 150


WebSnap::WebSnap(const QString &url, const QString &pos)
    : QObject()
{
    m_url = url;
    m_pos = pos;

    // this to not register websnap history
    m_page.settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(saveResult(bool)));
    QTimer::singleShot(0, this, SLOT(load()));
}


void WebSnap::load()
{
    m_page.mainFrame()->load( QUrl(m_url) );
}


void WebSnap::saveResult(bool ok)
{
    // crude error-checking
    if (!ok) 
    {
        kDebug() << "Error loading site..";
        return;
    }

    // find proper size, we stick to sensible aspect ratio
    QSize size = m_page.mainFrame()->contentsSize();
    size.setHeight(size.width() * HEIGHT / WIDTH );
    
    // create the target surface
    m_image = QPixmap(WIDTH, HEIGHT);
    m_image.fill(Qt::transparent);

    // render and rescale
    QPainter p(&m_image);
    m_page.setViewportSize(m_page.mainFrame()->contentsSize());
    m_page.mainFrame()->render(&p);
    p.end();
    m_image = m_image.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatioByExpanding);

    
    QString path = KStandardDirs::locateLocal("cache", QString("thumbs/rek") + m_pos + ".png", true);
    if( m_image.save(path) )
    {
        kDebug() << "finished";
        emit finished();
    }
}


QPixmap WebSnap::previewImage()
{
    return m_image;
}