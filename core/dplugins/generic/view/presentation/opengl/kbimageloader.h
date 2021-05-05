/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a presentation tool.
 *
 * Copyright (C) 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold
 * <carsten dot weinhold at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_KB_IMAGE_LOADER_H
#define DIGIKAM_KB_IMAGE_LOADER_H

// Qt includes

#include <QList>
#include <QImage>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QPair>
#include <QString>

namespace DigikamGenericPresentationPlugin
{

class PresentationContainer;

class KBImageLoader : public QThread
{
    Q_OBJECT

public:

    KBImageLoader(PresentationContainer* const sharedData, int width, int height);
    ~KBImageLoader()                  override;

    void  quit();
    void  requestNewImage();

    void          ungrabImage();
    bool          grabImage();
    bool          ready()       const;
    const QImage& image()       const;
    float         imageAspect() const;

protected:

    void run()                        override;
    bool loadImage();
    void invalidateCurrentImageName();

private:

    // Disable
    explicit KBImageLoader(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_KB_IMAGE_LOADER_H
