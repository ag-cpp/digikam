/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-23
 * Description : face marquer widget for FacesEngine Demo
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2010 by Adrien Bustany <madcat at mymadcat dot com>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#ifndef DIGIKAM_FACE_ENGINE_DEMO_MARQUEE_H
#define DIGIKAM_FACE_ENGINE_DEMO_MARQUEE_H

// Qt includes

#include <QObject>
#include <QRectF>
#include <QGraphicsItemGroup>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

namespace FaceEngineDemo
{

class FancyRect;

class Marquee : public QObject,
                public QGraphicsItemGroup
{
    Q_OBJECT

public:

    explicit Marquee(FancyRect* const rect, QGraphicsItem* const parent = nullptr);
    ~Marquee() override;

    QRectF boundingRect() const override;
    QRectF toRectF()      const;

Q_SIGNALS:

    void selected(Marquee* m);
    void changed();
    void entered();
    void left();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent* e)   override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e)    override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override;

private:

    void createHandles();
    void placeHandles();
    void placeLabel();

private:

    // Disable
    explicit Marquee(QObject*);

    class Private;
    Private* const d;
};

} // namespace FaceEngineDemo

#endif // DIGIKAM_FACE_ENGINE_DEMO_MARQUEE_H
