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

#include "application.h"
#include "activatebutton.h"
#include "mainview.h"
#include "similaritemlayout.h"
#include "previewwidget.h"
#include "panoramascene.h"
#include "removebutton.h"
#include "session.h"
#include "sessiontabdata.h"
#include "stretcherwidget.h"
#include "sessionmanager.h"
#include "webtab.h"


#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsGridLayout>
#include <QMapIterator>
#include <QGraphicsProxyWidget>
#include <QLineEdit>

//FIXME: the following headers are for occasional testing, needs to be removed.
#include <QGraphicsLinearLayout>
#include <mainwindow.h>

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
    m_dropShadow->setEnabled(false);

    m_gridLayout = new QGraphicsGridLayout;
    m_gridLayout->setSpacing(10);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    setLayout(m_gridLayout);

    setupTitleEdit();
    setupStretcher();
    setupActivateButton();
    setupRemoveButton();
    setupPreview();

    setSessionActive(session->isActive());
    QSizePolicy sp = QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sp.setHeightForWidth(false);
    setSizePolicy(sp);

    setAcceptDrops(true);

    connect(session, SIGNAL(tabAdded(SessionTabData*)), this, SLOT(addTabPreview(SessionTabData*)));
    connect(session, SIGNAL(tabRemoved(SessionTabData*)), this, SLOT(removeTabPreview(SessionTabData*)));
    connect(session, SIGNAL(tabChanged(SessionTabData*)), this, SLOT(updateTabPreview(SessionTabData*)));
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
    m_gridLayout->addItem(titleEdit, 0, 1);
    m_titleLineEdit->setText(session()->title());
    connect(m_titleLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setTitleForSession(QString)));
    connect(session(), SIGNAL(titleChanged(QString)), this, SLOT(setTitleFromSession(QString)));
}


void SessionWidget::setupStretcher()
{
    m_stretcher = new StretcherWidget(this);
    m_gridLayout->addItem(m_stretcher, 2, 2);
}


void SessionWidget::setupActivateButton()
{
    m_activateButton = new ActivateButton(this);
    m_gridLayout->addItem(m_activateButton, 0, 0);
    connect(m_activateButton, SIGNAL(toggled()), this, SLOT(toggleActivate()));
}


void SessionWidget::toggleActivate()
{
    if (session() == 0)
        return;
    if (session()->isActive())
    {
        session()->window()->setClosedForPanorama();
        session()->window()->close();
    }
    else
    {
        rApp->sessionManager()->activateSession(session());
    }
}


void SessionWidget::setupRemoveButton()
{
    m_removeButton = new RemoveButton(this);
    m_gridLayout->addItem(m_removeButton, 0, 2);
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeSession()));
}


void SessionWidget::removeSession()
{
    if (session() == 0)
        return;
    if (session()->isActive() && session()->window())
    {
        session()->window()->setClosedForPanorama();
        session()->window()->close();
    }
    rApp->sessionManager()->removeSession(session());
}


void SessionWidget::setupPreview()
{
    m_layout = new SimilarItemLayout(m_gridLayout);
    m_layout->setSpacing(10);
    m_gridLayout->addItem(m_layout, 1, 0, 1, 3);
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


void SessionWidget::setTitleForSession(QString title)
{
    session()->setTitle(title);
}


void SessionWidget::setTitleFromSession(QString title)
{
    m_titleLineEdit->setText(title);
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
    update();
    kDebug() << "tab preview added" << tabData->url();
}


void SessionWidget::removeTabPreview(SessionTabData* tabData)
{
    PreviewWidget *pw;
    if ((pw = m_tabMap[tabData]) != 0)
    {
        m_layout->removeItem(pw);
        m_tabMap.remove(tabData);
        kDebug() << "deleted tab preview" << tabData->url();
        pw->deleteLater();
        if (m_tabMap.isEmpty() && session()->isActive() == false)
        {
            SessionTabData * tabData = new SessionTabData(session());
            tabData->setUrl(KUrl("about:blank"));
            tabData->setTitle("Blank Page");
            session()->addTab(tabData);
        }
    }
}


void SessionWidget::updateTabPreview(SessionTabData* tabData)
{
    PreviewWidget *pw;
    if ((pw = m_tabMap[tabData]) != 0)
    {
        pw->update();
    }
}


void SessionWidget::setSessionActive(bool active)
{
    if (active)
    {
        m_titleLineEdit->setStyleSheet(
            "QLineEdit { border: 1px solid darkgray;\
            border-radius: 3px;\
            margin: 0px 0px;\
            color: lightgray;\
            background: transparent;\
            selection-background-color: transparent;\
            selection-color: white; }\
            ");
        m_backBrush = QColor(0, 0, 0, 160);
        m_borderPen = QColor(220, 220, 220);
        m_borderPen.setWidth(3);
        m_stretcher->setColor(Qt::gray);
        m_activateButton->setActive(true);
    }
    else
    {
        m_titleLineEdit->setStyleSheet(
            "QLineEdit { border: 1px solid #999999;\
            border-radius: 3px;\
            margin: 0px 0px;\
            color: #777777;\
            background: transparent;\
            selection-background-color: transparent;\
            selection-color: black; }\
            ");
        m_backBrush = QColor(224, 224, 224);
        m_borderPen = QColor(240, 240, 240);
        m_borderPen.setWidth(3);
        m_stretcher->setColor(Qt::gray);
        m_activateButton->setActive(false);
    }
}


void SessionWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    m_inMotion = false;
}


void SessionWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    emit mousePressed();
    //rApp->sessionManager()->panoramaScene()->setCurrentSessionWidget(this);
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


void SessionWidget::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->text() == "previewwidget")
    {
        event->setAccepted(true);
    }
    else
    {
        event->setAccepted(false);
    }
}


void SessionWidget::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    PreviewWidget* draggedWidget = rApp->sessionManager()->panoramaScene()->currentlyDragged();
    if (draggedWidget->parentSessionWidget() == this)
    {
        draggedWidget = 0;
        return;
    }
    if (draggedWidget->parentSessionWidget()->session()->isActive())
    {
        if (session()->isActive())
        {
            WebTab *wt = draggedWidget->tabData()->webTab();

            int index = draggedWidget->parentSessionWidget()->session()->window()->mainView()->indexOf(wt);
            draggedWidget->parentSessionWidget()->session()->window()->mainView()->detachTab(index, session()->window());
        }
        else
        {
            WebTab *wt = draggedWidget->tabData()->webTab();

            int index = draggedWidget->parentSessionWidget()->session()->window()->mainView()->indexOf(wt);
            SessionTabData *tabData = new SessionTabData(session());
            tabData->setTitle(draggedWidget->tabData()->title());
            tabData->setUrl(draggedWidget->tabData()->url());
            session()->addTab(tabData);

            draggedWidget->parentSessionWidget()->session()->window()->mainView()->closeTab(index);
        }
    }
    else
    {
        if (session()->isActive())
        {
            KUrl url = draggedWidget->tabData()->url();
            draggedWidget->parentSessionWidget()->session()->removeTab(draggedWidget->tabData());
            rApp->loadUrl(url, Rekonq::NewBackTab, session()->window());
        }
        else
        {
            SessionTabData *tabData = new SessionTabData(session());
            tabData->setTitle(draggedWidget->tabData()->title());
            tabData->setUrl(draggedWidget->tabData()->url());
            session()->addTab(tabData);

            draggedWidget->parentSessionWidget()->session()->removeTab(draggedWidget->tabData());
        }
    }
    draggedWidget = 0;
}
