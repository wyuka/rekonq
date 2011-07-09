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


#ifndef SESSION_TAB_DATA_H
#define SESSION_TAB_DATA_H


// Rekonq Includes
#include "rekonq_defines.h"

// Qt Includes
#include <KUrl>
#include <QPixmap>
#include <QString>
#include <QWeakPointer>

class WebTab;

class SessionTabData : public QObject
{
    Q_OBJECT

public:
    explicit SessionTabData(QObject* parent = 0);
    SessionTabData(const SessionTabData& tabData);
    void operator=(const SessionTabData& tabData);

    void setUrl(KUrl url);
    void setTitle(QString title);
    void updateThumbnail(int width = 0, int height = 0);

    void setWebTab(WebTab *webTab);

    KUrl url();
    QString title();
    QPixmap& thumbnail();

private:
    KUrl m_url;
    QString m_title;
    QPixmap m_thumbnail;
    QWeakPointer<WebTab> m_webTab;
};

#endif // SESSION_TAB_DATA_H