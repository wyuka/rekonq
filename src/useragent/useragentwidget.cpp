/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2010-2011 by Andrea Diamantini <adjam7 at gmail dot com>
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
#include "useragentwidget.h"
#include "useragentwidget.moc"

// KDE Includes
#include <KProtocolManager>


UserAgentWidget::UserAgentWidget(QWidget *parent)
        : QWidget(parent)
{
    setupUi(this);

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteUserAgent()));
    connect(deleteAllButton, SIGNAL(clicked()), this, SLOT(deleteAll()));

    KConfig config("kio_httprc", KConfig::NoGlobals);

    QStringList hosts = config.groupList();
    kDebug() << "HOSTS" << hosts;
    Q_FOREACH(const QString &host, hosts)
    {
        QStringList tmp;
        tmp << host;

        KConfigGroup hostGroup(&config, host);
        tmp <<  hostGroup.readEntry(QL1S("UserAgent"), QString());

        kDebug() << "TMP: " << tmp;
        QTreeWidgetItem *item = new QTreeWidgetItem(sitePolicyTreeWidget, tmp);
        sitePolicyTreeWidget->addTopLevelItem(item);
    }
}


void UserAgentWidget::deleteUserAgent()
{
    QTreeWidgetItem *item = sitePolicyTreeWidget->currentItem();
    if (!item)
        return;

    sitePolicyTreeWidget->takeTopLevelItem(sitePolicyTreeWidget->indexOfTopLevelItem(item));

    QString host = item->text(0);
    kDebug() << "HOST: " << host;

    KConfig config("kio_httprc", KConfig::NoGlobals);
    KConfigGroup group(&config, host);
    if (group.exists())
    {
        group.deleteGroup();
        KProtocolManager::reparseConfiguration();
    }
}


void UserAgentWidget::deleteAll()
{
    sitePolicyTreeWidget->clear();

    KConfig config("kio_httprc", KConfig::NoGlobals);

    QStringList list = config.groupList();
    Q_FOREACH(const QString &groupName, list)
    {
        kDebug() << "HOST: " << groupName;

        KConfigGroup group(&config, groupName);
        group.deleteGroup();
    }
    KConfigGroup group(&config, QString());
    group.deleteGroup();

    KProtocolManager::reparseConfiguration();
}
