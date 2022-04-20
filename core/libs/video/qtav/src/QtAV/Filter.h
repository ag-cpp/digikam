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

#ifndef QTAV_FILTER_H
#define QTAV_FILTER_H

// Qt includes

#include <QObject>

// Local includes

#include "QtAV_Global.h"
#include "FilterContext.h"

namespace QtAV
{

class AudioFormat;
class AVOutput;
class AVPlayer;
class FilterPrivate;
class Statistics;
class Frame;

class Q_AV_EXPORT Filter : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(Filter)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:

    virtual ~Filter();
    bool isEnabled() const;

    /*!
     * \brief setOwnedByTarget
     * If a filter is owned by target, it's not safe to access the filter after it's installed to a target.
     * QtAV will delete the installed filter internally if filter is owned by target AND it's parent (QObject) is null.
     */
    void setOwnedByTarget(bool value = true);

    // default is false

    bool isOwnedByTarget() const;

    // setInput/Output: no need to call installTo
    // bool setInput(Filter*);
    // bool setOutput(Filter*);

    /*!
     * \brief installTo
     * Install filter to player can process every frame before rendering.
     * Equals to player->installFilter(this)
     */
    virtual bool installTo(AVPlayer *player) = 0;

    // called in destructor automatically

    bool uninstall();

public Q_SLOTS:

    void setEnabled(bool enabled = true);

Q_SIGNALS:

    void enabledChanged(bool);

protected:

    /**
     * If the filter is in AVThread, it's safe to operate on ref.
     */
    Filter(FilterPrivate& d, QObject *parent = 0);

    DPTR_DECLARE(Filter)
};

// ----------------------------------------------------------------------------------

class VideoFilterPrivate;

class Q_AV_EXPORT VideoFilter : public Filter
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VideoFilter)

public:

    VideoFilter(QObject* parent = 0);

    VideoFilterContext* context();
    virtual bool isSupported(VideoFilterContext::Type ct) const;
    bool installTo(AVPlayer *player);

    /*!
     * \brief installTo
     * The process() function is in rendering thread. Used by
     * 1. GPU filters
     * 2. QPainter rendering on widget based renderers. Changing the frame has no effect
     * \return false if already installed
     */
    bool installTo(AVOutput *output); // only for video. move to video filter installToRenderer
    void apply(Statistics* statistics, VideoFrame *frame = 0);

    bool prepareContext(VideoFilterContext*& ctx, Statistics* statistics = 0, VideoFrame* frame = 0); // internal use

protected:

    VideoFilter(VideoFilterPrivate& d, QObject *parent = 0);
    virtual void process(Statistics* statistics, VideoFrame* frame = 0) = 0;
};

// -------------------------------------------------------------------------------------

class AudioFrame;
class AudioFilterPrivate;

class Q_AV_EXPORT AudioFilter : public Filter
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(AudioFilter)

public:

    AudioFilter(QObject* parent = 0);
    bool installTo(AVPlayer *player);
    void apply(Statistics* statistics, AudioFrame *frame = 0);

protected:

    AudioFilter(AudioFilterPrivate& d, QObject *parent = 0);
    virtual void process(Statistics* statistics, AudioFrame* frame = 0) = 0;
};

} // namespace QtAV

#endif // QTAV_FILTER_H
