/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef AV_PLAYER_MAIN_WINDOW_H
#define AV_PLAYER_MAIN_WINDOW_H

// Qt includes

#include <QWidget>
#include <QUrl>

namespace QtAV
{
    class VideoRenderer;
    class AVError;
}

namespace AVPlayer
{

class MainWindow : public QWidget
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget* const parent = nullptr);
    ~MainWindow();

//@{
/// Setup methods: MainWindow_setup.cpp

public:

    void setAudioBackends(const QStringList& backends);
    bool setRenderer(QtAV::VideoRenderer* const renderer);
    void setVideoDecoderNames(const QStringList& vd);

private Q_SLOTS:

    void initPlayer();
    void initAudioTrackMenu();
    void setupUi();
    void setFrameRate();
    void setVolume();
    void setup();
    void changeClockType(QAction* action);
    void setRepeateMax(int);
    void changeVO(QAction* action);
    void changeChannel(QAction* action);
    void changeAudioTrack(QAction* action);

//@{
/// IO operations methods: MainWindow_io.cpp

public Q_SLOTS:

    void play(const QString& name);
    void play(const QUrl& url);
    void openFile();

private Q_SLOTS:

    void stopUnload();
    void openUrl();

//@}

public Q_SLOTS:

    void togglePlayPause();
    void showNextOSD();

Q_SIGNALS:

    void ready();

private Q_SLOTS:

    void about();
    void updateChannelMenu();
    void switchAspectRatio(QAction* action);
    void toggleRepeat(bool);
    void playOnlineVideo(QAction* action);
    void onPlayListClick(const QString& key, const QString& value);
    void processPendingActions();
    void onSpinBoxChanged(double v);
    void onStartPlay();
    void onStopPlay();
    void onPaused(bool p);
    void onSpeedChange(qreal speed);
    void seek();
    void seek(int);
    void showHideVolumeBar();
    void tryHideControlBar();
    void tryShowControlBar();
    void showInfo();
    void onPositionChange(qint64 pos);
    void repeatAChanged(const QTime& t);
    void repeatBChanged(const QTime& t);

    void onTimeSliderHover(int pos, int value);
    void onTimeSliderLeave();
    void handleError(const QtAV::AVError& e);
    void onMediaStatusChanged();
    void onBufferProgress(qreal percent);

    void onVideoEQEngineChanged();
    void onBrightnessChanged(int b);
    void onContrastChanged(int c);
    void onHueChanged(int h);
    void onSaturationChanged(int s);

    void onSeekFinished(qint64 pos);
    void onCaptureConfigChanged();
    void onAVFilterVideoConfigChanged();
    void onAVFilterAudioConfigChanged();
    void onBufferValueChanged();
    void onAbortOnTimeoutChanged();

    void onUserShaderChanged();

    void handleFullscreenChange();
    void toggoleSubtitleEnabled(bool value);
    void toggleSubtitleAutoLoad(bool value);
    void openSubtitle();
    void setSubtitleCharset(const QString& charSet);
    void setSubtitleEngine(const QString& value);

    void syncVolumeUi(qreal value);

protected:

    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);

protected:

    virtual void closeEvent(QCloseEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void timerEvent(QTimerEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);

private:

    class Private;
    Private* const d;
};

} // namespace AVPlayer

#endif // AV_PLAYER_MAIN_WINDOW_H
