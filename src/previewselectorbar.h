/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2010 by Matthieu Gicquel <matgic78 at gmail dot com>
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


#ifndef PREVIEWSELECTORBAR_H
#define PREVIEWSELECTORBAR_H

// Rekonq Includes
#include "rekonq_defines.h"
#include "notificationbar.h"

// Forward Declarations
class QLabel;
class QPushButton;


class REKONQ_TESTS_EXPORT PreviewSelectorBar : public NotificationBar
{
    Q_OBJECT

public:
    PreviewSelectorBar(int index, QWidget *parent);

    inline void setIndex(int index)
    {
        m_previewIndex = index;
    }

private slots:
    void clicked();

    void loadProgress();
    void loadFinished();

    void verifyUrl();

private:
    QPushButton *m_button;
    QLabel *m_label;

    int m_previewIndex;
};

#endif // PREVIEWSELECTORBAR_H
