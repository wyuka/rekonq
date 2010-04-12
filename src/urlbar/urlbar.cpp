/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2008-2009 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2009 by Domrachev Alexandr <alexandr.domrachev@gmail.com>
* Copyright (C) 2009 by Paweł Prażak <pawelprazak at gmail dot com>
* Copyright (C) 2009 by Lionel Chauvin <megabigbug@yahoo.fr>
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
#include "urlbar.h"
#include "urlbar.moc"

// Auto Includes
#include "rekonq.h"

// Local Includes
#include "application.h"
#include "lineedit.h"
#include "mainwindow.h"
#include "webview.h"
#include "urlresolver.h"

// KDE Includes
#include <KDebug>
#include <KCompletionBox>
#include <KUrl>

// Qt Includes
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QTimer>
#include <QVBoxLayout>

// Defines
#define QL1S(x)  QLatin1String(x)


UrlBar::UrlBar(QWidget *parent)
    : LineEdit(parent)
    , _box(new CompletionWidget(this))
    , _tab(0)
    , _privateMode(false)
{
    _tab = qobject_cast<WebTab *>(parent);
    
    connect(_tab->view(), SIGNAL(urlChanged(const QUrl &)), this, SLOT(setQUrl(const QUrl &)));
    connect(_tab->view(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished()));
    
    // load urls on activated urlbar signal
    connect(this, SIGNAL(returnPressed(const QString&)), this, SLOT(activated(const QString&)));

    // suggestions
    installEventFilter(_box);
    connect(_box, SIGNAL(chosenUrl(const KUrl &, Rekonq::OpenType)), SLOT(activated(const KUrl &, Rekonq::OpenType)));
}


UrlBar::~UrlBar()
{
    delete _box;
}


void UrlBar::setQUrl(const QUrl& url)
{
    // find a better place? Ideally a loadStarted connected slot..
    clearRightIcons();
    
    if(url.scheme() == QL1S("about") )
    {
        iconButton()->setIcon( KIcon("arrow-right") );
        clear();
        setFocus();
    }
    else
    {
        LineEdit::setUrl(url);
        setCursorPosition(0);
        iconButton()->setIcon( Application::icon(url) );
    }
}


void UrlBar::activated(const KUrl& url, Rekonq::OpenType type)
{
    disconnect(this, SIGNAL(textChanged(const QString &)), this, SLOT(suggestUrls(const QString &)));
    
    clearFocus();
    setUrl(url);
    Application::instance()->loadUrl(url, type);
}


void UrlBar::paintEvent(QPaintEvent *event)
{
    QColor backgroundColor;
    if( _privateMode )
    {
        backgroundColor = QColor(220, 220, 220);  // light gray
    }
    else
    {
        backgroundColor = Application::palette().color(QPalette::Base);
    }
    
    // set background color of UrlBar
    QPalette p = palette();

    int progr = _tab->progress();
    if (progr == 0) 
    {
        if( _tab->url().scheme() == QL1S("https") )
        {
            backgroundColor = QColor(255, 255, 171);  // light yellow
        }
        p.setBrush(QPalette::Base, backgroundColor);
    } 
    else 
    {
        QColor loadingColor = QColor(116, 192, 250);
        
        QLinearGradient gradient(0, 0, width(), 0);
        gradient.setColorAt(0, loadingColor);
        gradient.setColorAt(((double)progr)/100, backgroundColor);
        p.setBrush(QPalette::Base, gradient);
    }
    setPalette(p);
    
    LineEdit::paintEvent(event);
}


void UrlBar::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        _box->hide();
        return;
    }
    
    // this handles the Modifiers + Return key combinations
    QString currentText = text().trimmed();
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        && !currentText.startsWith(QLatin1String("http://"), Qt::CaseInsensitive))
    {
        QString append;
        if (event->modifiers() == Qt::ControlModifier)
        {
            append = QLatin1String(".com");
        }
        else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
        {
            append = QLatin1String(".org");
        }
        else if (event->modifiers() == Qt::ShiftModifier)
        {
            append = QLatin1String(".net");
        }

        QUrl url(QLatin1String("http://www.") + currentText);
        QString host = url.host();
        if (!host.endsWith(append, Qt::CaseInsensitive))
        {
            host += append;
            url.setHost(host);
            setText(url.toString());
        }
    }
    
    LineEdit::keyPressEvent(event);
}


void UrlBar::suggestUrls(const QString &text)
{   
    if (!hasFocus())
    {
        return;
    }

    if(text.isEmpty())
    {
        _box->hide();
        return;
    }

    UrlResolver res(text);
    UrlSearchList list = res.orderedSearchItems();

    if(list.count() > 0)
    {
        _box->clear();
        _box->insertSearchList(list);
        _box->popup();
    }
}


void UrlBar::focusInEvent(QFocusEvent *event)
{
    // activate suggestions on edit text
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(suggestUrls(const QString &)));
    
    LineEdit::focusInEvent(event);
}


void UrlBar::setPrivateMode(bool on)
{
    _privateMode = on;
}


void UrlBar::dropEvent(QDropEvent *event)
{
    LineEdit::dropEvent(event);
    activated(text());
}


void UrlBar::loadFinished()
{
    if(_tab->progress() != 0)
        return;
    
    if(_tab->url().scheme() == QL1S("about") )
    {
        update();
        return;
    }
    
    // show KGet downloads??
    if(ReKonfig::kgetList())
    {
        IconButton *bt = addRightIcon(LineEdit::KGet);
        connect(bt, SIGNAL(clicked()), _tab->page(), SLOT(downloadAllContentsWithKGet()));
    }
    
    // show RSS
    if(_tab->hasRSSInfo())
    {
        IconButton *bt = addRightIcon(LineEdit::RSS);
        connect(bt, SIGNAL(clicked()), _tab, SLOT(showRSSInfo()));
    }
    
    // show SSL
    if(_tab->url().scheme() == QL1S("https") )
    {
        IconButton *bt = addRightIcon(LineEdit::SSL);
        connect(bt, SIGNAL(clicked()), _tab->page(), SLOT(showSSLInfo()));
    }
    
    update();
}
