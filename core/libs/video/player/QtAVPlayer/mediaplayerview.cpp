/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to host media player based on QtAVPlayer.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mediaplayerview.h"

// Qt includes

#include <QApplication>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QPushButton>
#include <QFileInfo>
#include <QToolBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QStyle>
#include <QTimer>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"
#include "thememanager.h"
#include "dlayoutbox.h"
#include "metaengine.h"
#include "dmetadata.h"

namespace Digikam
{

class Q_DECL_HIDDEN MediaPlayerMouseClickFilter : public QObject
{
    Q_OBJECT

public:

    explicit MediaPlayerMouseClickFilter(QObject* const parent)
        : QObject (parent),
          m_parent(parent)
    {
    }

protected:

    bool eventFilter(QObject* obj, QEvent* event) override
    {
        if ((event->type() == QEvent::MouseButtonPress) || (event->type() == QEvent::MouseButtonDblClick))
        {
            bool singleClick              = qApp->style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick);
            QMouseEvent* const mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (m_parent && mouseEvent)
            {
                MediaPlayerView* const mplayer = dynamic_cast<MediaPlayerView*>(m_parent);

                if (mplayer)
                {
                    if      (
                             (mouseEvent->button() == Qt::LeftButton) &&
                             (
                              (singleClick  && (event->type() == QEvent::MouseButtonPress)) ||
                              (!singleClick && (event->type() == QEvent::MouseButtonDblClick))
                             )
                            )
                    {
                        mplayer->slotEscapePressed();
                    }
                    else if (
                             (mouseEvent->button() == Qt::RightButton) &&
                             (event->type()        == QEvent::MouseButtonPress)
                            )
                    {
                        mplayer->slotRotateVideo();
                    }

                    return true;
                }
            }
        }

        return QObject::eventFilter(obj, event);
    }

private:

    QObject* m_parent = nullptr;
};

// --------------------------------------------------------

class Q_DECL_HIDDEN PlayerVideoStyle : public QProxyStyle
{
    Q_OBJECT

public:

    using QProxyStyle::QProxyStyle;

    int styleHint(QStyle::StyleHint hint,
                  const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override
    {
        if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
        {
            return (Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

// --------------------------------------------------------

class Q_DECL_HIDDEN MediaPlayerView::Private
{

public:

    enum MediaPlayerViewMode
    {
        ErrorView = 0,
        PlayerView
    };

public:

    Private() = default;

    QFrame*              errorView          = nullptr;
    QFrame*              playerView         = nullptr;

    QAction*             prevAction         = nullptr;
    QAction*             nextAction         = nullptr;
    QAction*             playAction         = nullptr;
    QAction*             grabAction         = nullptr;

    QPushButton*         loopPlay           = nullptr;

    QToolBar*            toolBar            = nullptr;

    DInfoInterface*      iface              = nullptr;

    DVideoWidget*        videoWidget        = nullptr;

    QSlider*             slider             = nullptr;
    QSlider*             volume             = nullptr;
    QLabel*              tlabel             = nullptr;
    QLabel*              speaker            = nullptr;
    QUrl                 currentItem;

    qint64               capturePosition    = 0;
    qint64               sliderTime         = 0;

    bool                 playLoop           = false;
};

MediaPlayerView::MediaPlayerView(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const int spacing      = layoutSpacing();


    d->prevAction          = new QAction(QIcon::fromTheme(QLatin1String("go-previous")),
                                         i18nc("go to previous image", "Back"),   this);
    d->nextAction          = new QAction(QIcon::fromTheme(QLatin1String("go-next")),
                                         i18nc("go to next image", "Forward"),    this);
    d->playAction          = new QAction(QIcon::fromTheme(QLatin1String("media-playback-start")),
                                         i18nc("pause/play video", "Pause/Play"), this);
    d->grabAction          = new QAction(QIcon::fromTheme(QLatin1String("view-preview")),
                                         i18nc("capture video frame", "Capture"), this);

    d->errorView           = new QFrame(this);
    QLabel* const errorMsg = new QLabel(i18n("An error has occurred with the media player..."), this);

    errorMsg->setAlignment(Qt::AlignCenter);
    d->errorView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->errorView->setLineWidth(1);

    QVBoxLayout* const vbox1 = new QVBoxLayout(d->errorView);
    vbox1->addWidget(errorMsg, 10);
    vbox1->setContentsMargins(QMargins());
    vbox1->setSpacing(spacing);

    insertWidget(Private::ErrorView, d->errorView);

    // --------------------------------------------------------------------------

    d->playerView     = new QFrame(this);
    d->playerView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    d->playerView->setLineWidth(1);
    d->playerView->installEventFilter(this);

    d->videoWidget    = new DVideoWidget(this);

    DHBox* const hbox = new DHBox(this);
    d->slider         = new QSlider(Qt::Horizontal, hbox);
    d->slider->setStyle(new PlayerVideoStyle());
    d->slider->setRange(0, 0);
    d->tlabel         = new QLabel(hbox);
    d->tlabel->setText(QLatin1String("00:00:00 / 00:00:00"));
    d->loopPlay       = new QPushButton(hbox);
    d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    d->loopPlay->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d->loopPlay->setToolTip(i18n("Toggle playing in a loop"));
    d->loopPlay->setFocusPolicy(Qt::NoFocus);
    d->loopPlay->setMinimumSize(22, 22);
    d->loopPlay->setCheckable(true);
    d->speaker        = new QLabel(hbox);
    d->speaker->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
    d->volume         = new QSlider(Qt::Horizontal, hbox);
    d->volume->setRange(0, 100);
    d->volume->setValue(50);

    hbox->setStretchFactor(d->slider, 10);
    hbox->setContentsMargins(0, 0, 0, spacing);
    hbox->setSpacing(spacing);

    QVBoxLayout* const vbox2 = new QVBoxLayout(d->playerView);
    vbox2->addWidget(d->videoWidget, 10);
    vbox2->addWidget(hbox,            0);
    vbox2->setContentsMargins(QMargins());
    vbox2->setSpacing(spacing);

    insertWidget(Private::PlayerView, d->playerView);

    d->toolBar = new QToolBar(this);
    d->toolBar->addAction(d->prevAction);
    d->toolBar->addAction(d->nextAction);
    d->toolBar->addAction(d->playAction);
    d->toolBar->addAction(d->grabAction);
    d->toolBar->setStyleSheet(toolButtonStyleSheet());

    setPreviewMode(Private::PlayerView);

    d->errorView->installEventFilter(new MediaPlayerMouseClickFilter(this));
    d->videoWidget->view()->installEventFilter(new MediaPlayerMouseClickFilter(this));

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Media Player Settings"));
    int volume                = group.readEntry("Volume", 50);

    d->videoWidget->audioOutput()->setVolume(volume);
    d->volume->setValue(volume);

    // --------------------------------------------------------------------------

    connect(ThemeManager::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    connect(d->prevAction, SIGNAL(triggered()),
            this, SIGNAL(signalPrevItem()));

    connect(d->nextAction, SIGNAL(triggered()),
            this, SIGNAL(signalNextItem()));

    connect(d->playAction, SIGNAL(triggered()),
            this, SLOT(slotPausePlay()));

    connect(d->grabAction, SIGNAL(triggered()),
            this, SLOT(slotCapture()));

    connect(d->slider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotPosition(int)));

    connect(d->slider, SIGNAL(valueChanged(int)),
            this, SLOT(slotPosition(int)));

    connect(d->volume, SIGNAL(valueChanged(int)),
            this, SLOT(slotVolumeChanged(int)));

    connect(d->loopPlay, SIGNAL(toggled(bool)),
            this, SLOT(slotLoopToggled(bool)));

    connect(d->videoWidget->player(), SIGNAL(stateChanged(QAVPlayer::State)),
            this, SLOT(slotPlayerStateChanged(QAVPlayer::State)));

    connect(d->videoWidget, SIGNAL(positionChanged(qint64)),
            this, SLOT(slotPositionChanged(qint64)));

    connect(d->videoWidget->player(), SIGNAL(durationChanged(qint64)),
            this, SLOT(slotDurationChanged(qint64)));

    connect(d->videoWidget->player(), SIGNAL(errorOccurred(QAVPlayer::Error,QString)),
            this, SLOT(slotHandlePlayerError(QAVPlayer::Error,QString)));

    connect(d->videoWidget->player(), SIGNAL(mediaStatusChanged(QAVPlayer::MediaStatus)),
            this, SLOT(slotMediaStatusChanged(QAVPlayer::MediaStatus)));
}

MediaPlayerView::~MediaPlayerView()
{
    escapePreview();
    delete d;
}

void MediaPlayerView::setInfoInterface(DInfoInterface* const iface)
{
    d->iface = iface;
}

void MediaPlayerView::reload()
{
    d->videoWidget->player()->stop();
    d->videoWidget->player()->setSource(d->currentItem.toLocalFile());
    d->videoWidget->player()->play();
}

void MediaPlayerView::slotPlayerStateChanged(QAVPlayer::State newState)
{
    if      (newState == QAVPlayer::PlayingState)
    {
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));
    }
    else if (newState == QAVPlayer::PausedState)
    {
        d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
    }
    else if (newState == QAVPlayer::StoppedState)
    {
        if (d->playLoop)
        {
            reload();
        }
        else
        {
            d->playAction->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));
        }
    }
}

void MediaPlayerView::slotMediaStatusChanged(QAVPlayer::MediaStatus newStatus)
{
    if      (newStatus == QAVPlayer::LoadedMedia)
    {
        slotPlayingStateChanged();

        QTimer::singleShot(250, this, SLOT(slotPlayingStateChanged()));
    }
    else if (newStatus == QAVPlayer::InvalidMedia)
    {
        setPreviewMode(Private::ErrorView);
    }
}

void MediaPlayerView::escapePreview()
{
    d->videoWidget->player()->stop();
    d->videoWidget->player()->setSource(QString());
}

void MediaPlayerView::slotThemeChanged()
{
    QPalette palette;
    palette.setColor(d->errorView->backgroundRole(), qApp->palette().color(QPalette::Base));
    d->errorView->setPalette(palette);

    QPalette palette2;
    palette2.setColor(d->playerView->backgroundRole(), qApp->palette().color(QPalette::Base));
    d->playerView->setPalette(palette2);

    d->speaker->setPixmap(QIcon::fromTheme(QLatin1String("audio-volume-high")).pixmap(22, 22));
}

void MediaPlayerView::slotEscapePressed()
{
    escapePreview();

    Q_EMIT signalEscapePreview();
}

void MediaPlayerView::slotRotateVideo()
{
    if (d->videoWidget->player()->state() == QAVPlayer::PlayingState)
    {
        int orientation = 0;

        switch (d->videoWidget->videoItemOrientation())
        {
            case 0:
            {
                orientation = 90;
                break;
            }

            case 90:
            {
                orientation = 180;
                break;
            }

            case 180:
            {
                orientation = 270;
                break;
            }

            default:
            {
                orientation = 0;
                break;
            }
        }

        d->videoWidget->setVideoItemOrientation(orientation);
    }
}

void MediaPlayerView::slotPausePlay()
{
    if (d->videoWidget->player()->state() != QAVPlayer::PlayingState)
    {
        d->videoWidget->player()->play();
        return;
    }

    if (d->videoWidget->player()->state() != QAVPlayer::PausedState)
    {
       d->videoWidget->player()->pause();
    }
}

void MediaPlayerView::slotCapture()
{
    if (d->videoWidget->player()->state() != QAVPlayer::StoppedState)
    {
        int capturePosition    = d->videoWidget->player()->position();
        QVideoFrame frame      = d->videoWidget->videoFrame();
        QImage image           = frame.image();

        if (!image.isNull() && d->currentItem.isValid())
        {
            QFileInfo info(d->currentItem.toLocalFile());
            QString tempPath = QString::fromUtf8("%1/%2-%3.digikamtempfile.jpg")
                              .arg(info.path())
                              .arg(info.baseName())
                              .arg(capturePosition);

            if (image.save(tempPath, "JPG", 100))
            {
                QScopedPointer<DMetadata> meta(new DMetadata);

                if (meta->load(tempPath))
                {
                    QDateTime dateTime;
                    MetaEngine::ImageOrientation orientation = MetaEngine::ORIENTATION_NORMAL;

                    if (d->iface)
                    {
                        DItemInfo dinfo(d->iface->itemInfo(d->currentItem));

                        dateTime    = dinfo.dateTime();
                        orientation = (MetaEngine::ImageOrientation)dinfo.orientation();
                    }
                    else
                    {
                        QScopedPointer<DMetadata> meta2(new DMetadata);

                        if (meta2->load(d->currentItem.toLocalFile()))
                        {
                            dateTime    = meta2->getItemDateTime();
                            orientation = meta2->getItemOrientation();
                        }
                    }

                    if (dateTime.isValid())
                    {
                        dateTime = dateTime.addMSecs(capturePosition);
                    }
                    else
                    {
                        dateTime = QDateTime::currentDateTime();
                    }

                    if (orientation == MetaEngine::ORIENTATION_UNSPECIFIED)
                    {
                        orientation = MetaEngine::ORIENTATION_NORMAL;
                    }

                    meta->setImageDateTime(dateTime, true);
                    meta->setItemDimensions(image.size());
                    meta->setItemOrientation(orientation);
                    meta->save(tempPath, true);
                }

                QString finalPath = QString::fromUtf8("%1/%2-%3.jpg")
                                   .arg(info.path())
                                   .arg(info.baseName())
                                   .arg(capturePosition);

                if (QFile::rename(tempPath, finalPath))
                {
                    if (d->iface)
                    {
                        d->iface->slotMetadataChangedForUrl(QUrl::fromLocalFile(finalPath));
                    }
                }
                else
                {
                    QFile::remove(tempPath);
                }
            }
        }
    }
}

int MediaPlayerView::previewMode()
{
    return indexOf(currentWidget());
}

void MediaPlayerView::setPreviewMode(int mode)
{
    if ((mode != Private::ErrorView) && (mode != Private::PlayerView))
    {
        return;
    }

    setCurrentIndex(mode);

    d->toolBar->adjustSize();
    d->toolBar->raise();
}

void MediaPlayerView::setCurrentItem(const QUrl& url, bool hasPrevious, bool hasNext)
{
    d->prevAction->setEnabled(hasPrevious);
    d->nextAction->setEnabled(hasNext);
    d->videoWidget->adjustVideoSize();

    if (url.isEmpty())
    {
        d->currentItem = url;

        d->videoWidget->player()->stop();
        d->videoWidget->player()->setSource(QString());

        return;
    }

    if (d->currentItem == url)
    {
        return;
    }

    d->currentItem = url;

    d->videoWidget->player()->stop();
    d->videoWidget->player()->setSource(QString());

    int orientation = 0;

    if (d->iface)
    {
        DItemInfo info(d->iface->itemInfo(url));

        orientation = info.orientation();
    }

    switch (orientation)
    {
        case MetaEngine::ORIENTATION_ROT_90:
        case MetaEngine::ORIENTATION_ROT_90_HFLIP:
        case MetaEngine::ORIENTATION_ROT_90_VFLIP:
        {
            d->videoWidget->setVideoItemOrientation(90);
            break;
        }

        case MetaEngine::ORIENTATION_ROT_180:
        {
            d->videoWidget->setVideoItemOrientation(180);
            break;
        }

        case MetaEngine::ORIENTATION_ROT_270:
        {
            d->videoWidget->setVideoItemOrientation(270);
            break;
        }

        default:
        {
            d->videoWidget->setVideoItemOrientation(0);
            break;
        }
    }

    d->videoWidget->player()->setSource(d->currentItem.toLocalFile());
    setPreviewMode(Private::PlayerView);
    d->videoWidget->player()->play();
}

void MediaPlayerView::slotPositionChanged(qint64 position)
{
    if (
        (d->sliderTime < position)       &&
        ((d->sliderTime + 100) > position)
       )
    {
        return;
    }

    d->sliderTime = position;

    if (!d->slider->isSliderDown())
    {
        d->slider->blockSignals(true);
        d->slider->setValue(position);
        d->slider->blockSignals(false);
    }

    d->tlabel->setText(QString::fromLatin1("%1 / %2")
                       .arg(QTime(0, 0, 0).addMSecs(position).toString(QLatin1String("HH:mm:ss")))
                       .arg(QTime(0, 0, 0).addMSecs(d->slider->maximum()).toString(QLatin1String("HH:mm:ss"))));
}

void MediaPlayerView::slotVolumeChanged(int volume)
{
    d->videoWidget->audioOutput()->setVolume((qreal)volume / 100.0);

    if (objectName() != QLatin1String("main_media_player"))
    {
        return;
    }

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Media Player Settings"));
    group.writeEntry("Volume", volume);
}

void MediaPlayerView::slotLoopToggled(bool loop)
{
    if (loop)
    {
        d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-repeat")));
    }
    else
    {
        d->loopPlay->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    }

    d->playLoop = loop;
}

void MediaPlayerView::slotDurationChanged(qint64 duration)
{
    qint64 max = qMax((qint64)1, duration);
    d->slider->setRange(0, max);
}

void MediaPlayerView::slotPosition(int position)
{
    if (d->videoWidget->player()->isSeekable())
    {
        d->videoWidget->player()->seek((qint64)position);
    }
}

void MediaPlayerView::slotHandlePlayerError(QAVPlayer::Error /*err*/, const QString& message)
{
    setPreviewMode(Private::ErrorView);
    qCDebug(DIGIKAM_GENERAL_LOG) << "QtAVPlayer Error: " << message;
}

void MediaPlayerView::slotPlayingStateChanged()
{
    int rotate = d->videoWidget->videoMediaOrientation();

    qCDebug(DIGIKAM_GENERAL_LOG) << "Video orientation from QtAVPlayer:"
                                 << rotate;

    rotate     = d->videoWidget->videoItemOrientation();

    d->videoWidget->setVideoItemOrientation(rotate);
}

bool MediaPlayerView::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == d->playerView) && (event->type() == QEvent::Resize))
    {
        d->videoWidget->adjustVideoSize();
    }

    return QStackedWidget::eventFilter(watched, event);
}

} // namespace Digikam

#include "mediaplayerview.moc"

#include "moc_mediaplayerview.cpp"
