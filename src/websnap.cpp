/* ============================================================
*
* This file is a part of the rekonq project
*
* Copyright (C) 2009 Nokia Corporation <qt-info@nokia.com>
* Copyright (C) 2009-2011 by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010 by Matthieu Gicquel <matgic78 at gmail dot com>
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
#include "websnap.h"
#include "websnap.moc"

// Local Includes
#include "newtabpage.h"

// KDE Includes
#include <KStandardDirs>

// Qt Includes
#include <QtCore/QSize>
#include <QtCore/QFile>

#include <QtGui/QPainter>
#include <QtGui/QAction>

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>


WebSnap::WebSnap(const KUrl& url, QObject *parent)
        : QObject(parent)
        , m_url(url)
{
    // this to not register websnap history
    m_page.settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);

    // this to not let this page open other windows
    m_page.settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    m_page.settings()->setAttribute(QWebSettings::JavascriptEnabled, false);

    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(saveResult(bool)));

    QMetaObject::invokeMethod(this, "load", Qt::QueuedConnection);
}


WebSnap::~WebSnap()
{
    kDebug() << "oh oh..";
    m_page.action(QWebPage::Stop)->trigger();
    m_page.deleteLater();
}


void WebSnap::load()
{
    m_page.mainFrame()->load(m_url);
}



QPixmap WebSnap::render(const QWebPage &page, int w, int h)
{
    // create the page image
    QPixmap pageImage = QPixmap(w, h);
    pageImage.fill(Qt::transparent);

    // render it
    QPainter p(&pageImage);
    page.mainFrame()->render(&p, QWebFrame::ContentsLayer);
    p.end();

    return pageImage;
}


QPixmap WebSnap::renderTabPreview(const QWebPage &page, int w, int h)
{
    QSize oldSize = page.viewportSize();
    int width = page.mainFrame()->contentsSize().width();
    page.setViewportSize(QSize(width, width * ((0.0 + h) / w)));
    QPixmap pageImage = WebSnap::render(page, page.viewportSize().width(), page.viewportSize().height());
    page.setViewportSize(oldSize);
    return pageImage.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

/*
// This code is an attempt to render a page currently displayed in a tab without alterate it.
// It currently does not work but can give ideas:
// - activate/disable scrollbars of a webview causes problems => this method try to not do that
// - the viewport and the scroll position must be modified an restored
// - page.setViewportSize(size); does not seem to work when the page is currently displayed in a webview

QPixmap WebSnap::renderVisiblePagePreview(const QWebPage &page, int w, int h)
{
    // save page settings
    QSize oldSize = page.viewportSize();
    QPoint oldScrollPosition = page.mainFrame()->scrollPosition();

    // minimum width
    int width = 640;

    // find best width
    QSize size;
    while(page.mainFrame()->scrollBarMaximum(Qt::Horizontal) && width<1920)
    {
        width+=5;
        size = QSize(width, width * ((0.0 + h) / w));
        page.setViewportSize(size);
    }

    // scroll to top
    page.mainFrame()->setScrollBarValue(Qt::Vertical, 0);

    // render
    QPixmap pageImage = WebSnap::render(page, page.viewportSize().width(), page.viewportSize().height());

    // detect scrollbar size
    int scrollbarWidth = (page.mainFrame()->scrollBarMaximum(Qt::Horizontal) ? 17  : 0); //TODO: detect QStyle size for scrollbars
    int scrollbarHeight = (page.mainFrame()->scrollBarMaximum(Qt::Vertical) ? 17 : 0);

    // resize image
    pageImage = pageImage.copy(0, 0, width - scrollbarWidth, size.height() - scrollbarHeight);
    pageImage = pageImage.scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // restore page settings
    page.setViewportSize(oldSize);
    page.mainFrame()->setScrollPosition(oldScrollPosition);

    return pageImage;
}
*/

QPixmap WebSnap::renderClosingPagePreview(const QWebPage &page, int w, int h)
{
    //scroll to top
    page.mainFrame()->setScrollBarValue(Qt::Vertical, 0);

    // reduce as much as possible
    page.setViewportSize(QSize(10, 10));

    return renderPagePreview(page, w, h);
}


QPixmap WebSnap::renderPagePreview(const QWebPage &page, int w, int h)
{
    //prepare page
    page.mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    int width = page.mainFrame()->contentsSize().width();
    page.setViewportSize(QSize(width, width * ((0.0 + h) / w)));

    //render
    QPixmap pageImage = WebSnap::render(page, page.viewportSize().width(), page.viewportSize().height());

    // resize image
    pageImage = pageImage.scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    return pageImage;
}


QString WebSnap::imagePathFromUrl(const KUrl &url)
{
    QUrl temp = QUrl(url.url());
    QByteArray name = temp.toEncoded(QUrl::RemoveScheme | QUrl::RemoveUserInfo | QUrl::StripTrailingSlash);

    return KStandardDirs::locateLocal("cache", QString("thumbs/") + name.toBase64() + ".png", true);
}


void WebSnap::saveResult(bool ok)
{
    if (ok)
    {
        QPixmap image = renderPagePreview(m_page, defaultWidth, defaultHeight);
        QString path = imagePathFromUrl(m_url);
        QFile::remove(path);
        image.save(path);
    }

    emit snapDone(ok);
    kDebug() << "SAVE RESULTS: " << ok << " URL: " << m_url;

    this->deleteLater();
}


bool WebSnap::existsImage(const KUrl &u)
{
    return QFile::exists(imagePathFromUrl(u));
}
