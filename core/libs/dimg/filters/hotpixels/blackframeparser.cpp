/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-27
 * Description : black frames parser
 *
 * Copyright (C) 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * Part of the algorithm for finding the hot pixels was based on
 * the code of jpegpixi, which was released under the GPL license,
 * written by Martin Dickopp
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

/// Denominator for relative quantities.
#define DENOM (DENOM_SQRT * DENOM_SQRT)

/// Square root of denominator for relative quantities.
#define DENOM_SQRT 10000

/// Convert relative to absolute numbers. Care must be taken not to overflow integers.
#define REL_TO_ABS(n,m) \
    ((((n) / DENOM_SQRT) * (m) + ((n) % DENOM_SQRT) * (m) / DENOM_SQRT) / DENOM_SQRT)

#include "blackframeparser.h"

// Qt includes

#include <QStringList>
#include <QApplication>

// Local includes

#include "digikam_debug.h"
#include "dcolor.h"

namespace Digikam
{

BlackFrameParser::BlackFrameParser(QObject* const parent)
    : QObject(parent),
      m_imageLoaderThread(nullptr)
{
}

BlackFrameParser::~BlackFrameParser()
{
    if (!m_tempFilePath.isEmpty())
    {
        QFile::remove(m_tempFilePath);
    }

    delete m_imageLoaderThread;
}

void BlackFrameParser::parseHotPixels(const QString& file)
{
    parseBlackFrame(QUrl::fromLocalFile(file));
}

void BlackFrameParser::parseBlackFrame(const QUrl& url)
{
    QString localFile = url.toLocalFile();

    if (!m_imageLoaderThread)
    {
        m_imageLoaderThread = new LoadSaveThread();

        connect(m_imageLoaderThread, SIGNAL(signalLoadingProgress(LoadingDescription,float)),
                this, SLOT(slotLoadingProgress(LoadingDescription,float)));

        connect(m_imageLoaderThread, SIGNAL(signalImageLoaded(LoadingDescription,DImg)),
                this, SLOT(slotLoadImageFromUrlComplete(LoadingDescription,DImg)));
    }

    LoadingDescription desc = LoadingDescription(localFile, DRawDecoding());
    m_imageLoaderThread->load(desc);
}

void BlackFrameParser::slotLoadingProgress(const LoadingDescription&, float v)
{
    emit signalLoadingProgress(v);
}

void BlackFrameParser::slotLoadImageFromUrlComplete(const LoadingDescription&, const DImg& img)
{
    m_image = img;
    blackFrameParsing();

    emit signalLoadingComplete();
}

void BlackFrameParser::parseBlackFrame(DImg& img)
{
    m_image = img;
    blackFrameParsing();
}

DImg BlackFrameParser::image() const
{
    return m_image;
}

/**
 * Parses black frames
 */
void BlackFrameParser::blackFrameParsing()
{
    // Now find the hot pixels and store them in a list

    QList<HotPixelProps> hpList;

    // If you accidentally open a normal image for a black frame, the tool and host application will
    // freeze due to heavy calculation.
    // We should stop at a certain amount of hot pixels, to avoid the freeze.
    // 1000 of total hot pixels should be good enough for a trigger. Images with such an amount of hot pixels should
    // be considered as messed up anyway.

    const int maxHotPixels = 1000;

    for (int y = 0 ; y < (int)m_image.height() ; ++y)
    {
        for (int x = 0 ; x < (int)m_image.width() ; ++x)
        {
            // Get each point in the image

            DColor pixrgb = m_image.getPixelColor(x, y);
            QColor color;
            color.setRgb(pixrgb.getQColor().rgb());

            // Find maximum component value.

            int       maxValue;
            int       threshold       = DENOM / 10;
            const int threshold_value = REL_TO_ABS(threshold, 255);
            maxValue                  = (color.red() > color.blue()) ? color.red() : color.blue();

            if (color.green() > maxValue)
            {
                maxValue = color.green();
            }

            // If the component is bigger than the threshold, add the point

            if (maxValue > threshold_value)
            {
                HotPixelProps point;
                point.rect       = QRect (x, y, 1, 1);

                // TODO: check this

                point.luminosity = ((2 * DENOM) / 255 ) * maxValue / 2;

                hpList.append(point);
            }
        }

        if (hpList.count() > maxHotPixels)
        {
            break;
        }
    }

    // Now join points together into groups

    consolidatePixels(hpList);

    // And notify

    emit signalParsed(hpList);
}

/**
 * Consolidate adjacent points into larger points.
 */
void BlackFrameParser::consolidatePixels(QList<HotPixelProps>& list)
{
    if (list.isEmpty())
    {
        return;
    }

    // Consolidate horizontally.

    QList<HotPixelProps>::iterator it, prevPointIt;

    prevPointIt = list.begin();
    it          = list.begin();
    ++it;

    HotPixelProps tmp;
    HotPixelProps point;
    HotPixelProps point_below;

    for ( ; it != list.end() ; ++it)
    {
        while (1)
        {
            point = (*it);
            tmp   = point;

            QList<HotPixelProps>::iterator point_below_it;

            // find any intersecting hot pixels below tmp

            int i = list.indexOf(tmp);

            if (i == -1)
            {
                point_below_it = list.end();
            }
            else
            {
                point_below_it = list.begin() + i;
            }

            if (point_below_it != list.end())
            {
                point_below =* point_below_it;
                validateAndConsolidate(&point, &point_below);

                point.rect.setX(qMin(point.x(), point_below.x()));
                point.rect.setWidth(qMax(point.x() + point.width(),
                                         point_below.x() + point_below.width()) - point.x());
                point.rect.setHeight(qMax(point.y() + point.height(),
                                          point_below.y() + point_below.height()) - point.y());
                *it         = point;
                list.erase(point_below_it); // TODO: Check! this could remove it++?
            }
            else
            {
                break;
            }
        }
    }
}

void BlackFrameParser::validateAndConsolidate(HotPixelProps* const a, HotPixelProps* const b)
{
    a->luminosity = qMax(a->luminosity, b->luminosity);
}

} // namespace Digikam
