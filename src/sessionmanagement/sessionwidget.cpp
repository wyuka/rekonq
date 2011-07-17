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

#include "sessionwidget.h"
#include "sessionwidget.moc"

#include "similaritemlayout.h"
#include "previewwidget.h"
#include "session.h"
#include "sessiontabdata.h"
#include "stretcherwidget.h"


#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsGridLayout>
#include <QMapIterator>
#include <QGraphicsProxyWidget>
#include <QLineEdit>

//FIXME: the following headers are for occasional testing, needs to be removed.
#include <QGraphicsLinearLayout>

SessionWidget::SessionWidget(Session *session, QGraphicsItem* parent)
        : QGraphicsWidget(parent)
        , m_current(false)
{
    m_session = session;
    m_dropShadow = new QGraphicsDropShadowEffect(this);
    m_dropShadow->setOffset(QPointF(1, 1));
    m_dropShadow->setColor(Qt::black);
    m_dropShadow->setBlurRadius(20);
    setGraphicsEffect(m_dropShadow);
    m_dropShadow->setEnabled(true);

    m_gridLayout = new QGraphicsGridLayout;
    m_gridLayout->setSpacing(10);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    setLayout(m_gridLayout);

    setupTitleEdit();
    setupStretcher();
    setupPreview();

    setSessionActive(session->isActive());
    QSizePolicy sp = QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sp.setHeightForWidth(false);
    setSizePolicy(sp);

    connect(session, SIGNAL(tabAdded(SessionTabData*)), this, SLOT(addTabPreview(SessionTabData*)));
    connect(session, SIGNAL(tabRemoved(SessionTabData*)), this, SLOT(removeTabPreview(SessionTabData*)));
}


SessionWidget::~SessionWidget()
{
}


void SessionWidget::setupTitleEdit()
{
    QGraphicsProxyWidget *titleEdit = new QGraphicsProxyWidget(this);
    m_titleLineEdit = new QLineEdit;
    titleEdit->setWidget(m_titleLineEdit);
    m_titleEdit = titleEdit;
    m_gridLayout->addItem(titleEdit, 0, 0);
}


void SessionWidget::setupStretcher()
{
    m_stretcher = new StretcherWidget(this);
    m_gridLayout->addItem(m_stretcher, 2, 1);
}


void SessionWidget::setupPreview()
{
    m_layout = new SimilarItemLayout(m_gridLayout);
    m_layout->setSpacing(10);
    m_gridLayout->addItem(m_layout, 1, 0);
}


void SessionWidget::setCurrent(bool current)
{
    m_current = current;
    if (m_current == false)
    {
        m_dropShadow->setEnabled(false);
        if (m_currentPreviewWidget.data())
            m_currentPreviewWidget.data()->setCurrent(false);
    }
    else
    {
        m_dropShadow->setEnabled(true);
        if (m_currentPreviewWidget.data())
            m_currentPreviewWidget.data()->setCurrent(true);
    }
    update();
}


void SessionWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (session() == 0)
    {
        return;
    }
    painter->setBrush(m_backBrush);
    painter->setPen(m_borderPen);
    painter->drawRoundedRect(boundingRect(), 5, 5);
    QGraphicsWidget::paint(painter, option, widget);
}


QSizeF SessionWidget::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    return QGraphicsWidget::sizeHint(which, constraint);
}


Session* SessionWidget::session()
{
    return m_session.data();
}


void SessionWidget::addTabPreview(SessionTabData *tabData)
{
    PreviewWidget *pw = new PreviewWidget(tabData, this);
    m_layout->addItem(pw);
    m_tabMap[tabData] = pw;
    kDebug() << "tab preview added" << tabData->url();
}


void SessionWidget::removeTabPreview(SessionTabData* tabData)
{
    PreviewWidget *pw;
    if ((pw = m_tabMap[tabData]) != 0)
    {
        m_layout->removeItem(pw);
        kDebug() << "deleted tab preview" << tabData->url();
        pw->deleteLater();
    }
}


void SessionWidget::setSessionActive(bool active)
{
    if (active)
    {
        m_titleLineEdit->setStyleSheet(
            "QLineEdit { border: 2px solid lightGray;\
            border-radius: 3px;\
            margin: 0px 0px;\
            color: white;\
            background: transparent;\
            selection-background-color: transparent;\
            selection-color: darkgray; }\
            ");
        m_backBrush = QColor(0, 0, 0, 160);
        m_borderPen = QPen(Qt::lightGray);//Qt::NoPen;
        m_borderPen.setWidth(3);
    }
    else
    {
        m_titleLineEdit->setStyleSheet(
            "QLineEdit { border: 2px solid lightGray;\
            border-radius: 3px;\
            margin: 0px 0px;\
            background: white;\
            selection-background-color: transparent;\
            selection-color: darkgray; }\
            ");
        m_backBrush = Qt::white;//QColor(224,224,224);
        //m_borderPen = QPen(Qt::lightGray);//QColor(240,240,240);
        //m_borderPen.setWidth(3);
        m_borderPen = Qt::NoPen;
    }
}


/*void SessionWidget::setSession(Session* session)
{
    m_session = session;

    TabDataList tabDataList = m_session->tabDataList();
    SessionTabData* currentTabData = session->currentTabData();
    foreach (SessionTabData* tabData, tabDataList)
    {
        PreviewWidget* pw = new PreviewWidget;
        pw->setTabData(tabData);
        connect(pw, SIGNAL(mousePressed()), this, SLOT(setCurrentPreviewWidget()));
        layout->addItem(pw);
        if (tabData == currentTabData)
        {
            m_currentPreviewWidget = pw;
            m_currentPreviewWidget.data()->setCurrent(true);
        }
    }
}*/


void SessionWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    m_inMotion = false;
}

void SessionWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //m_layout->removeItem(this);
    emit mousePressed();
    event->accept();
}

void SessionWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    m_inMotion = true;
    setPos(mapToParent(event->pos()-event->lastPos()));
}


void SessionWidget::setCurrentPreviewWidget()
{
    PreviewWidget* toBeCurrent = static_cast<PreviewWidget*>(sender());
    if (m_currentPreviewWidget.data() == toBeCurrent)
        return;

    if (m_currentPreviewWidget.data())
    {
        m_currentPreviewWidget.data()->setCurrent(false);
    }
    m_currentPreviewWidget = toBeCurrent;
    m_currentPreviewWidget.data()->setCurrent(true);
}