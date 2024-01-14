/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-14
 * Description : QtAVPlayer integration class.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DVIDEO_WIDGET_H
#define DIGIKAM_DVIDEO_WIDGET_H

// Qt includes

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QMediaService>
#include <QMediaObject>
#include <QVideoRendererControl>
#include <QVideoWidget>
#include <QVideoFrame>

// QtAVPlayer includes

#include "qavplayer.h"
#include "qavvideoframe.h"
#include "qavaudioframe.h"
#include "qavaudiooutput.h"

namespace Digikam
{

class Q_DECL_HIDDEN VideoRenderer : public QVideoRendererControl
{
public:

    QAbstractVideoSurface* surface() const override
    {
        return m_surface;
    }

    void setSurface(QAbstractVideoSurface* surface) override
    {
        m_surface = surface;
    }

    QAbstractVideoSurface* m_surface = nullptr;
};

// -------------------------------------------------------

class Q_DECL_HIDDEN DVideoWidget : public QVideoWidget
{
    Q_OBJECT

public:

    explicit DVideoWidget(QWidget* const parent);
    ~DVideoWidget();

    QAVPlayer*      player()      const;
    QAVAudioOutput* audioOutput() const;
    VideoRenderer*  videoRender() const;
    QVideoFrame videoFrame()      const;
    int videoMediaOrientation()   const;

    bool setMediaObject(QMediaObject* object) override;

Q_SIGNALS:

    void positionChanged(qint64);

private Q_SLOTS:

    void slotAudioFrame(const QAVAudioFrame& frame);
    void slotVideoFrame(const QAVVideoFrame& frame);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DVIDEO_WIDGET_H
