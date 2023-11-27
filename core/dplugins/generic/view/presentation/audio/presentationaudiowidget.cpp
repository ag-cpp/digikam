/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * SPDX-FileCopyrightText: 2012-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "presentationaudiowidget.h"

// Qt includes

#include <QTime>
#include <QUrl>
#include <QKeyEvent>
#include <QIcon>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

#   include <QAudioOutput>

#else

// QtAV includes

#   include "AVError.h"
#   include "AVPlayerCore.h"

using namespace QtAV;

#endif

// Local includes

#include "presentationcontainer.h"
#include "digikam_debug.h"

using namespace Digikam;

namespace DigikamGenericPresentationPlugin
{

class Q_DECL_HIDDEN PresentationAudioWidget::Private
{

public:

    Private() = default;

    PresentationContainer* sharedData  = nullptr;
    QList<QUrl>            urlList;
    int                    currIndex   = 0;
    bool                   canHide     = true;
    bool                   isZeroTime  = false;
    bool                   playingNext = false;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

    QMediaPlayer*          mediaObject = nullptr;

#else

    AVPlayerCore*          mediaObject = nullptr;

#endif

};

PresentationAudioWidget::PresentationAudioWidget(QWidget* const parent, const QList<QUrl>& urls, PresentationContainer* const sharedData)
    : QWidget(parent),
      d      (new Private)
{
    setupUi(this);

    d->sharedData = sharedData;

    m_soundLabel->setPixmap(QIcon::fromTheme(QLatin1String("speaker")).pixmap(64, 64));

    m_prevButton->setText(QLatin1String(""));
    m_nextButton->setText(QLatin1String(""));
    m_playButton->setText(QLatin1String(""));
    m_stopButton->setText(QLatin1String(""));

    m_prevButton->setIcon(QIcon::fromTheme(QLatin1String("media-skip-backward")));
    m_nextButton->setIcon(QIcon::fromTheme(QLatin1String("media-skip-forward")));
    m_playButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    m_stopButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-stop")));

    connect(m_prevButton, SIGNAL(clicked()),
            this, SLOT(slotPrev()));

    connect(m_nextButton, SIGNAL(clicked()),
            this, SLOT(slotNext()));

    connect(m_playButton, SIGNAL(clicked()),
            this, SLOT(slotPlay()));

    connect(m_stopButton, SIGNAL(clicked()),
            this, SLOT(slotStop()));

    if (urls.isEmpty())
    {
        qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Tracks list is empty...";
        setEnabled(false);
        return;
    }

    // Waiting for files to be enqueued.

    m_playButton->setEnabled(false);
    m_prevButton->setEnabled(false);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

    d->mediaObject = new QMediaPlayer(this);

    connect(d->mediaObject, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(slotMediaStateChanged(QMediaPlayer::MediaStatus)));

    connect(d->mediaObject, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)),
            this, SLOT(slotPlayerStateChanged(QMediaPlayer::PlaybackState)));

    connect(d->mediaObject, SIGNAL(errorOccurred(QMediaPlayer::Error,QString)),
            this, SLOT(slotPlayerError(QMediaPlayer::Error)));

    connect(d->mediaObject, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotTimeUpdaterTimeout()));

#else

    d->mediaObject = new AVPlayerCore(this);

    connect(d->mediaObject, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),
            this, SLOT(slotMediaStateChanged(QtAV::MediaStatus)));

    connect(d->mediaObject, SIGNAL(stateChanged(QtAV::AVPlayerCore::State)),
            this, SLOT(slotPlayerStateChanged(QtAV::AVPlayerCore::State)));

    connect(d->mediaObject, SIGNAL(error(QtAV::AVError)),
            this, SLOT(slotPlayerError(QtAV::AVError)));

    connect(d->mediaObject, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotTimeUpdaterTimeout()));

#endif

    connect(m_volumeWidget, SIGNAL(valueChanged(int)),
            this, SLOT(slotSetVolume(int)));

    enqueue(urls);

    setZeroTime();
}

PresentationAudioWidget::~PresentationAudioWidget()
{
    if (!d->urlList.isEmpty())
    {
        d->mediaObject->stop();
    }

    delete d;
}

void PresentationAudioWidget::slotSetVolume(int v)
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

    if (d->mediaObject->audioOutput())
    {
        d->mediaObject->audioOutput()->setVolume(v / 100.0);
    }

#else

    if (d->mediaObject->audio())
    {
        d->mediaObject->audio()->setVolume(v / 100.0);
    }

#endif

}

bool PresentationAudioWidget::canHide() const
{
    return d->canHide;
}

bool PresentationAudioWidget::isPaused() const
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

    return (d->mediaObject->playbackState() == QMediaPlayer::PausedState);

#else

    return d->mediaObject->isPaused();

#endif

}

void PresentationAudioWidget::checkSkip()
{
    m_prevButton->setEnabled(true);
    m_nextButton->setEnabled(true);

    if (!d->sharedData->soundtrackLoop)
    {
        if (d->currIndex == 0)
        {
            m_prevButton->setEnabled(false);
        }

        if (d->currIndex == d->urlList.count() - 1)
        {
            m_nextButton->setEnabled(false);
        }
    }
}

void PresentationAudioWidget::setZeroTime()
{
    QTime zeroTime(0, 0, 0);
    m_elapsedTimeLabel->setText(zeroTime.toString(QLatin1String("H:mm:ss")));
    m_totalTimeLabel->setText(zeroTime.toString(QLatin1String("H:mm:ss")));
    d->isZeroTime = true;
}

void PresentationAudioWidget::enqueue(const QList<QUrl>& urls)
{
    d->urlList   = urls;
    d->currIndex = 0;

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Tracks : " << d->urlList;

    if (d->urlList.isEmpty())
    {
        return;
    }

    m_playButton->setEnabled(true);
}

void PresentationAudioWidget::setPaused(bool val)
{
    if (val == isPaused())
    {
        return;
    }

    slotPlay();
}

void PresentationAudioWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case (Qt::Key_Space):
        {
            m_playButton->animateClick();
            break;
        }

        case (Qt::Key_A):
        {
            if (m_prevButton->isEnabled())
            {
                m_prevButton->animateClick();
            }

            break;
        }

        case (Qt::Key_S):
        {
            if (m_nextButton->isEnabled())
            {
                m_nextButton->animateClick();
            }

            break;
        }

        case (Qt::Key_Escape):
        {
            if (m_stopButton->isEnabled())
            {
                m_stopButton->animateClick();
            }

            break;
        }

        default:
        {
            break;
        }
    }

    event->accept();
}

void PresentationAudioWidget::slotPlay()
{
    if (!d->mediaObject)
    {
        qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Internal Media Object is null!";

        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

    if ((d->mediaObject->playbackState() == QMediaPlayer::PlayingState) || (d->mediaObject->playbackState() == QMediaPlayer::PausedState))

#else

    if (!d->mediaObject->isPlaying() || d->mediaObject->isPaused())

#endif

    {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

        if (d->mediaObject->playbackState() == QMediaPlayer::PlayingState)

#else

        if (!d->mediaObject->isPlaying())

#endif

        {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

            d->mediaObject->setSource(d->urlList[d->currIndex]);

#else

            d->mediaObject->setFile(d->urlList[d->currIndex].toLocalFile());

#endif

            qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Playing:" << d->urlList[d->currIndex];
            d->mediaObject->play();
            setZeroTime();
        }
        else
        {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

            d->mediaObject->pause();

#else

            d->mediaObject->pause(false);

#endif

        }

        d->canHide = true;

        Q_EMIT signalPlay();
    }
    else
    {
        qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Pausing:" << d->urlList[d->currIndex];

        d->mediaObject->pause();
        d->canHide = false;

        Q_EMIT signalPause();
    }
}

void PresentationAudioWidget::slotStop()
{
    if (!d->mediaObject)
    {
        qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Internal Media Object is null!";

        return;
    }

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Stoping:" << d->urlList[d->currIndex];
    d->playingNext = false;
    d->mediaObject->stop();
    d->currIndex   = 0;
    setZeroTime();
    checkSkip();
}

void PresentationAudioWidget::slotPrev()
{
    d->currIndex--;

    if (d->currIndex < 0)
    {
        if (d->sharedData->soundtrackLoop)
        {
            d->currIndex = d->urlList.count() - 1;
        }
        else
        {
            d->currIndex = 0;
            return;
        }
    }

    d->playingNext = false;
    d->mediaObject->stop();
    slotPlay();
}

void PresentationAudioWidget::slotNext()
{
    d->currIndex++;

    if (d->currIndex >= d->urlList.count())
    {
        if (d->sharedData->soundtrackLoop)
        {
            d->currIndex = 0;
        }
        else
        {
            d->currIndex = d->urlList.count() - 1;
            return;
        }
    }

    d->playingNext = false;
    d->mediaObject->stop();
    slotPlay();
}

void PresentationAudioWidget::slotTimeUpdaterTimeout()
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

    if (d->mediaObject->error() != QMediaPlayer::NoError)

#else

    if (d->mediaObject->mediaStatus() == QtAV::InvalidMedia)

#endif

    {
        slotError();
        return;
    }

    qint64 current = d->mediaObject->position();
    int hours      = (int)(current  / (qint64)(60 * 60 * 1000));
    int mins       = (int)((current / (qint64)(60 * 1000)) - (qint64)(hours * 60));
    int secs       = (int)((current / (qint64)1000) - (qint64)(hours * 60 + mins * 60));
    QTime elapsedTime(hours, mins, secs);

    if (d->isZeroTime && d->mediaObject->duration() > 0)
    {
        d->isZeroTime = false;
        qint64 total  = d->mediaObject->duration();
        hours         = (int)(total  / (qint64)(60 * 60 * 1000));
        mins          = (int)((total / (qint64)(60 * 1000)) - (qint64)(hours * 60));
        secs          = (int)((total / (qint64)1000) - (qint64)(hours * 60 + mins * 60));
        QTime totalTime(hours, mins, secs);
        m_totalTimeLabel->setText(totalTime.toString(QLatin1String("H:mm:ss")));
    }

    m_elapsedTimeLabel->setText(elapsedTime.toString(QLatin1String("H:mm:ss")));
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined HAVE_QTMULTIMEDIA

void PresentationAudioWidget::slotMediaStateChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        slotNext();
    }
}

void PresentationAudioWidget::slotPlayerError(QMediaPlayer::Error err)
{
    if (err != QMediaPlayer::NoError)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "An error as occured while playing (" << err << ")";
        slotError();
    }
}

void PresentationAudioWidget::slotPlayerStateChanged(QMediaPlayer::PlaybackState state)
{
    switch (state)
    {
        case QMediaPlayer::PausedState:
        case QMediaPlayer::StoppedState:
        {
            m_playButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
            checkSkip();
            break;
        }

        case QMediaPlayer::PlayingState:
        {
            m_playButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
            d->playingNext = true;
            checkSkip();
            break;
        }

        default:
        {
            break;
        }
    }
}

#else

void PresentationAudioWidget::slotMediaStateChanged(QtAV::MediaStatus status)
{
    if (d->playingNext && (status == QtAV::EndOfMedia))
    {
        slotNext();
    }
}

void PresentationAudioWidget::slotPlayerError(const QtAV::AVError& err)
{
    if (err.error() != AVError::NoError)
    {
        qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "An error as occurred while playing (" << err.string() << ")";
        slotError();
    }
}

void PresentationAudioWidget::slotPlayerStateChanged(QtAV::AVPlayerCore::State state)
{
    switch (state)
    {
        case QtAV::AVPlayerCore::PausedState:
        case QtAV::AVPlayerCore::StoppedState:
        {
            m_playButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
            checkSkip();
            break;
        }

        case QtAV::AVPlayerCore::PlayingState:
        {
            m_playButton->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
            d->playingNext = true;
            checkSkip();
            break;
        }

        default:
        {
            break;
        }
    }
}

#endif

void PresentationAudioWidget::slotError()
{
    qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "An error as occurred!";

    /* TODO:
     * Display error on slideshow.
     * A QWidget pop-up could help
     */

    slotNext();
}

} // namespace DigikamGenericPresentationPlugin
