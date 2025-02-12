/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "ProgressFloatItem.h"

// Qt includes

#include <QRect>
#include <QColor>
#include <QPaintDevice>
#include <QPainter>
#include <QPainterPath>
#include <QThread>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "ViewportParams.h"
#include "HttpDownloadManager.h"
#include "digikam_debug.h"

namespace Marble
{

ProgressFloatItem::ProgressFloatItem(const MarbleModel* marbleModel)
    : AbstractFloatItem(marbleModel, QPointF(-10.5, -150.5), QSizeF(40.0, 40.0)),
      m_isInitialized(false),
      m_totalJobs(0),
      m_completedJobs(0),
      m_completed(1),
      m_progressHideTimer(),
      m_progressShowTimer(),
      m_active(false),
      m_fontSize(0),
      m_repaintTimer()
{
    // This timer is responsible to activate the automatic display with a small delay

    m_progressShowTimer.setInterval(250);
    m_progressShowTimer.setSingleShot(true);

    connect(&m_progressShowTimer, SIGNAL(timeout()),
            this, SLOT(show()));

    // This timer is responsible to hide the automatic display when downloads are finished

    m_progressHideTimer.setInterval(750);
    m_progressHideTimer.setSingleShot(true);

    connect(&m_progressHideTimer, SIGNAL(timeout()),
            this, SLOT(hideProgress()));

    // Repaint timer

    m_repaintTimer.setSingleShot(true);
    m_repaintTimer.setInterval(1000);

    connect(&m_repaintTimer, SIGNAL(timeout()),
            this, SIGNAL(repaintNeeded()));

    // Plugin is enabled by default

    setEnabled(true);

    // Plugin is visible by default on devices with small screens only

    setVisible(MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen);
}

ProgressFloatItem::~ProgressFloatItem()
{
    // nothing to do
}

QStringList ProgressFloatItem::backendTypes() const
{
    return QStringList(QStringLiteral("progress"));
}

QString ProgressFloatItem::name() const
{
    return i18n("Download Progress Indicator");
}

QString ProgressFloatItem::guiString() const
{
    return i18n("&Download Progress");
}

QString ProgressFloatItem::nameId() const
{
    return QStringLiteral("progress");
}

QString ProgressFloatItem::version() const
{
    return QStringLiteral("1.0");
}

QString ProgressFloatItem::description() const
{
    return i18n("A plugin to show a pie chart download progress indicator over the map");
}

QString ProgressFloatItem::copyrightYears() const
{
    return QStringLiteral("2010, 2011");
}

QVector<PluginAuthor> ProgressFloatItem::pluginAuthors() const
{
    return QVector<PluginAuthor>()
           << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
           << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

QIcon ProgressFloatItem::icon() const
{
    return QIcon::fromTheme(QStringLiteral("download-later"));
}

void ProgressFloatItem::initialize()
{
    const HttpDownloadManager* manager = marbleModel()->downloadManager();

    Q_ASSERT(manager);

    connect(manager, SIGNAL(progressChanged(int,int)),
            this, SLOT(handleProgress(int,int)),
            Qt::UniqueConnection);

    connect(manager, SIGNAL(jobRemoved()),
            this, SLOT(removeProgressItem()),
            Qt::UniqueConnection);

    // Calculate font size

    QFont myFont = font();
    const QString text = QLatin1String("100%");
    int fontSize = myFont.pointSize();

    while (QFontMetrics(myFont).boundingRect(text).width() < contentRect().width() - 2)
    {
        ++fontSize;
        myFont.setPointSize(fontSize);
    }

    m_fontSize = fontSize - 1;

    m_isInitialized = true;
}

bool ProgressFloatItem::isInitialized() const
{
    return m_isInitialized;
}

QPainterPath ProgressFloatItem::backgroundShape() const
{
    QPainterPath path;

    if (active())
    {
        // Circular shape if active, invisible otherwise

        QRectF rect  = contentRect();
        qreal width  = rect.width();
        qreal height = rect.height();
        path.addEllipse(marginLeft() + 2 * padding(), marginTop() + 2 * padding(), width, height);
    }

    return path;
}

void ProgressFloatItem::paintContent(QPainter* painter)
{
    // Timers cannot be stopped from another thread (e.g. from QtQuick RenderThread).

    if (QThread::currentThread() == QCoreApplication::instance()->thread())
    {
        // Stop repaint timer if it is already running

        if (m_repaintTimer.isActive())
        {
            m_repaintTimer.stop();
        }
    }

    if (!active())
    {
        return;
    }

    painter->save();

    // Paint progress pie

    int startAngle =  90 * 16; // 12 o' clock
    int spanAngle  = -ceil(360 * 16 * m_completed);
    QRectF rect(contentRect());
    rect.adjust(1, 1, -1, -1);

    painter->setBrush(QColor(Qt::white));
    painter->setPen(Qt::NoPen);
    painter->drawPie(rect, startAngle, spanAngle);

    // Paint progress label

    QFont myFont = font();
    myFont.setPointSize(m_fontSize);
    const QString done = QString::number((int)(m_completed * 100)) + QLatin1Char('%');
    int fontWidth = QFontMetrics(myFont).boundingRect(done).width();
    QPointF baseline(padding() + 0.5 * (rect.width() - fontWidth), 0.75 * rect.height());
    QPainterPath path;
    path.addText(baseline, myFont, done);

    painter->setFont(myFont);
    painter->setBrush(QBrush());
    painter->setPen(QPen());
    painter->drawPath(path);

    painter->restore();
}

void ProgressFloatItem::removeProgressItem()
{
    m_jobMutex.lock();
    ++m_completedJobs;
    m_jobMutex.unlock();

    if (enabled())
    {
        if      (!active() && !m_progressShowTimer.isActive())
        {
            m_progressShowTimer.start();
            m_progressHideTimer.stop();
        }
        else if (active())
        {
            update();
            scheduleRepaint();
        }
    }
}

void ProgressFloatItem::handleProgress(int current, int queued)
{
    m_jobMutex.lock();

    if (current < 1)
    {
        m_totalJobs = 0;
        m_completedJobs = 0;
    }
    else
    {
        m_totalJobs = qMax<int>(m_totalJobs, queued + current);
    }

    m_jobMutex.unlock();

    if (enabled())
    {
        if      (!active() && !m_progressShowTimer.isActive() && m_totalJobs > 0)
        {
            m_progressShowTimer.start();
            m_progressHideTimer.stop();
        }
        else if (active())
        {
            if (m_totalJobs < 1 || m_completedJobs == m_totalJobs)
            {
                m_progressShowTimer.stop();
                m_progressHideTimer.start();
            }

            update();
            scheduleRepaint();
        }

        m_completed = 1.0;

        if (m_totalJobs && m_completedJobs <= m_totalJobs)
        {
            m_completed = (qreal) m_completedJobs / (qreal) m_totalJobs;
        }
    }
}

void ProgressFloatItem::hideProgress()
{
    if (enabled())
    {
        setActive(false);

        update();

        Q_EMIT repaintNeeded(QRegion());
    }
}

bool ProgressFloatItem::active() const
{
    return m_active;
}

void ProgressFloatItem::setActive(bool active)
{
    m_active = active;
    update();
}

void ProgressFloatItem::show()
{
    setActive(true);

    update();

    Q_EMIT repaintNeeded(QRegion());
}

void ProgressFloatItem::scheduleRepaint()
{
    if (!m_repaintTimer.isActive())
    {
        m_repaintTimer.start();
    }
}

} // namespace Marble

#include "moc_ProgressFloatItem.cpp"
