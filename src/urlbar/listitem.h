/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009-2011 by Andrea Diamantini <adjam7 at gmail dot com>
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


#ifndef LISTITEM_H
#define LISTITEM_H


// Rekonq Includes
#include "rekonq_defines.h"

// Local Includes
#include "urlresolver.h"

// KDE Includes
#include <KToolBar>
#include <KService>
#include <KIcon>

// Qt Includes
#include <QWidget>
#include <QStyleOptionViewItemV4>
#include <QLabel>

// Forward Declarations
class UrlSearchItem;
class KAction;
class QActionGroup;


class ListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ListItem(const UrlSearchItem &item, QWidget *parent = 0);
    virtual ~ListItem();

    void activate();
    void deactivate();

    KUrl url();
    virtual QString text();

public slots:
    virtual void nextItemSubChoice();

signals:
    void itemClicked(ListItem *item, Qt::MouseButton, Qt::KeyboardModifiers);
    void updateList();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mousePressEvent(QMouseEvent *e);

private:
    QStyleOptionViewItemV4 m_option;

protected:
    KUrl m_url;
};


// -------------------------------------------------------------------------


class TypeIconLabel : public QLabel
{
    Q_OBJECT

public:
    explicit TypeIconLabel(int type, QWidget *parent = 0);

private:
    QLabel *getIcon(QString icon);
};


// -------------------------------------------------------------------------


class IconLabel : public QLabel
{
    Q_OBJECT

public:
    explicit IconLabel(const QString &icon, QWidget *parent = 0);
    explicit IconLabel(const KIcon &icon, QWidget *parent = 0);
};


// -------------------------------------------------------------------------


class REKONQ_TESTS_EXPORT TextLabel : public QLabel
{
    Q_OBJECT

public:
    explicit TextLabel(const QString &text, const QString &textToPointOut = QString(), QWidget *parent = 0);
    explicit TextLabel(QWidget *parent = 0);

    void setEngineText(const QString &engine, const QString &text);
};

// -------------------------------------------------------------------------


class DescriptionLabel : public QLabel
{
    Q_OBJECT

public:
    explicit DescriptionLabel(const QString &text, QWidget *parent = 0);
};


// -------------------------------------------------------------------------


class EngineBar : public KToolBar
{
    Q_OBJECT

public:
    explicit EngineBar(KService::Ptr selectedEngine, QWidget *parent = 0);
    void selectNextEngine();

signals:
    void searchEngineChanged(KService::Ptr engine);

private slots:
    void changeSearchEngine();

private:
    KAction *newEngineAction(KService::Ptr engine, KService::Ptr selectedEngine);
    QActionGroup *m_engineGroup;
};


// -------------------------------------------------------------------------


class SearchListItem : public ListItem
{
    Q_OBJECT

public:
    explicit SearchListItem(const UrlSearchItem &item, const QString &text, QWidget *parent = 0);
    QString text();

public slots:
    virtual void nextItemSubChoice();

private slots:
    void changeSearchEngine(KService::Ptr engine);

private:
    TextLabel* m_titleLabel;
    IconLabel* m_iconLabel;
    EngineBar* m_engineBar;
    QString m_text;
    KService::Ptr m_currentEngine;
};


// -------------------------------------------------------------------------


class SuggestionListItem : public ListItem
{
    Q_OBJECT

public:
    SuggestionListItem(const UrlSearchItem &item, const QString &text, QWidget *parent = 0);
    QString text();

private:
    QString m_text;
};


// -------------------------------------------------------------------------


class VisualSuggestionListItem : public ListItem
{
    Q_OBJECT

public:
    VisualSuggestionListItem(const UrlSearchItem &item, const QString &text, QWidget *parent = 0);
    QString text();

private:
    QString m_text;
};


// -------------------------------------------------------------------------


class PreviewListItem : public ListItem
{
    Q_OBJECT

public:
    PreviewListItem(const UrlSearchItem &item, const QString &text, QWidget *parent = 0);
};


// -------------------------------------------------------------------------


class PreviewLabel : public QLabel
{
    Q_OBJECT

public:
    PreviewLabel(const QString &url, int width, int height, QWidget *parent = 0);
};


// -------------------------------------------------------------------------

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel(const QString &url, int width, int height, QWidget *parent);

private:
    QString m_url;
    QByteArray m_data;

private slots:
    void slotData(KIO::Job* job, const QByteArray& data);
    void slotResult(KJob* job);
};


// -------------------------------------------------------------------------


class BrowseListItem : public ListItem
{
    Q_OBJECT

public:
    BrowseListItem(const UrlSearchItem &item, const QString &text, QWidget *parent = 0);
};


//-------------------------------------------------------------------------------------------------


class ListItemFactory
{
public:
    static ListItem *create(const UrlSearchItem &item, const QString &text, QWidget *parent);
};


#endif
