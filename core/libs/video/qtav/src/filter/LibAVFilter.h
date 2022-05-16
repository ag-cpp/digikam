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

#ifndef QTAV_LIBAV_FILTER_H
#define QTAV_LIBAV_FILTER_H

// Qt includes

#include <QObject>

// Local includes

#include "Filter.h"

namespace QtAV
{

class QTAV_EXPORT LibAVFilter
{
public:

    static QString filterDescription(const QString& filterName);
    static QStringList videoFilters();
    static QStringList audioFilters();

    /*!
     * \brief The Status enum
     * Status of filter graph.
     * If setOptions() is called, the status is NotConfigured, and will to configure when processing
     * a new frame.
     * Filter graph will be reconfigured if options, incoming video frame format or size changed
     */
    enum Status
    {
        NotConfigured,
        ConfigureFailed,
        ConfigureOk
    };

    LibAVFilter();
    virtual ~LibAVFilter();

    /*!
     * \brief setOptions
     * Set new option. Filter graph will be setup if receives a frame if options changed.
     * \param options option string for libavfilter. libav and ffmpeg are different
     */
    void setOptions(const QString& options);
    QString options() const;

    Status status() const;

protected:

    virtual QString sourceArguments() const = 0;
    bool pushVideoFrame(Frame* frame, bool changed);
    bool pushAudioFrame(Frame* frame, bool changed);
    void* pullFrameHolder();
    static QStringList registeredFilters(int type); // filters whose input/output type matches

    virtual void optionsChanged()
    {
    };

protected:

    class Private;
    Private* priv;

private:

    // Disable
    LibAVFilter(const LibAVFilter&)            = delete;
    LibAVFilter& operator=(const LibAVFilter&) = delete;
};

// ---------------------------------------------------------------------------------

class QTAV_EXPORT LibAVFilterVideo : public VideoFilter,
                                     public LibAVFilter
{
    Q_OBJECT
    Q_PROPERTY(QString options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QStringList filters READ filters)

public:

    explicit LibAVFilterVideo(QObject* const parent = nullptr);

    bool isSupported(VideoFilterContext::Type t) const override
    {
        return (t == VideoFilterContext::None);
    }

    QStringList filters() const; // the same as LibAVFilter::videoFilters

Q_SIGNALS:

    void optionsChanged() override;

protected:

    void process(Statistics* statistics, VideoFrame* frame) override;
    QString sourceArguments() const override;
};

// ---------------------------------------------------------------------------------

class QTAV_EXPORT LibAVFilterAudio : public AudioFilter,
                                     public LibAVFilter
{
    Q_OBJECT
    Q_PROPERTY(QString options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QStringList filters READ filters)

public:

    explicit LibAVFilterAudio(QObject* const parent = nullptr);

    QStringList filters() const; // the same as LibAVFilter::audioFilters

Q_SIGNALS:

    void optionsChanged() override;

protected:

    void process(Statistics* statistics, AudioFrame* frame) override;
    QString sourceArguments()                         const override;
};

} // namespace QtAV

#endif // QTAV_LIBAV_FILTER_H
