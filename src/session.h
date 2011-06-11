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


#ifndef SESSION_H
#define SESSION_H


// Rekonq Includes
#include "rekonq_defines.h"

//Local Includes
#include "sessiontabdata.h"

// Qt Includes
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QWeakPointer>

class MainWindow;
class QDomDocument;
class WebTab;

typedef QList<SessionTabData* > TabDataList;

class Session : public QObject
{
    Q_OBJECT

public:
    explicit Session(QObject* parent = 0);
    void setTitle(QString title);
    void setXml(QDomElement sessionDom);
    void setWindow(MainWindow *w);

    QString title();
    QDomElement getXml(QDomDocument& document);
    MainWindow* window();

    inline bool isActive() const
    {
        return m_active;
    }
    TabDataList tabDataList();

    inline Session* currentTabData()
    {
        return m_currentTabData.data();
    }

signals:
    void changesMade();

public slots:
    bool load();
    void deactivate();

protected slots:
    void addTabData(WebTab* webTab);
    void removeTabData(WebTab* webTab);
    void changeTabData(WebTab* webTab);
    void setCurrentTabData(WebTab* webTab);

    //utility function
    void clearSession();

private:
    MainWindow *m_window;
    QMap<WebTab*,SessionTabData*> m_webTabMap;
    QWeakPointer<SessionTabData> m_currentTabData;
    TabDataList m_tabDataList;
    QString m_title;
    bool m_active;
};

#endif // SESSION_H
