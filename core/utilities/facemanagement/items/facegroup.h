/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Managing of face tag region items on a GraphicsDImgView
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FACE_GROUP_H
#define DIGIKAM_FACE_GROUP_H

// Qt includes

#include <QObject>
#include <QPointF>

class QGraphicsSceneHoverEvent;

namespace Digikam
{

class Album;
class ItemInfo;
class GraphicsDImgView;
class RegionFrameItem;
class TaggingAction;

class FaceGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

public:

    /**
     * Constructs a new face group, managing RegionFrameItems for faces
     * of a particular image, displayed on a GraphicsDImgView.
     */
    explicit FaceGroup(GraphicsDImgView* const view);
    ~FaceGroup() override;

    bool isVisible()                                                     const;
    ItemInfo info()                                                      const;
    bool hasVisibleItems()                                               const;

    QList<RegionFrameItem*> items()                                      const;

    /**
     * Auto suggest applies if an image has not been scanned,
     * or an unknown face is registered.
     * In this case, a new scan will be triggered.
     */
    void setAutoSuggest(bool doAutoSuggest);
    bool autoSuggest()                                                   const;

    /**
     * Even if visible() is false, show the item under the mouse cursor
     */
    void setShowOnHover(bool show);
    bool showOnHover()                                                   const;

    /**
     * Returns the item in this group closest to scene position p.
     * If given, manhattanLength is set to the manhattan length between
     * p and the closest point of the returned item's bounding rectangle.
     * In particular, if p is inside the item's rectangle, manhattanLength is 0.
     */
    RegionFrameItem* closestItem(const QPointF& p,
                                 qreal* const manhattanLength = nullptr) const;

    bool acceptsMouseClick(const QPointF& scenePos);
    void itemHoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void itemHoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void itemHoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);

public Q_SLOTS:

    /**
     * Shows or hides the frames
     */
    void setVisible(bool visible);
    void setVisibleItem(RegionFrameItem* item);

    /**
     * Sets the current ItemInfo
     */
    void setInfo(const ItemInfo& info);

    /**
     * Prepares to load a new info.
     * Closes the face group for editing.
     * Pass a null info if about to close.
     */
    void aboutToSetInfo(const ItemInfo& info);

    void aboutToSetInfoAfterRotate(const ItemInfo& info);

    /**
     * Enters a special state where by click + drag a new face can be created
     */
    void addFace();

    /**
     * Rejects (clears) all faces on the current image
     */
    void rejectAll();

protected:

    void load();
    void clear();
    void applyItemGeometryChanges();

protected Q_SLOTS:

    void itemStateChanged(int);
    void startAutoSuggest();
    void slotAlbumsUpdated(int type);
    void slotAlbumRenamed(Album* album);
    void slotAssigned(const TaggingAction& action, const ItemInfo& info, const QVariant& faceIdentifier);
    void slotRejected(const ItemInfo& info, const QVariant& faceIdentifier);
    void slotLabelClicked(const ItemInfo& info, const QVariant& faceIdentifier);
    void slotAddItemStarted(const QPointF& pos);
    void slotAddItemMoving(const QRectF& rect);
    void slotAddItemFinished(const QRectF& rect);
    void cancelAddItem();

private:

    // Disable
    FaceGroup();
    FaceGroup(const FaceGroup&);
    FaceGroup& operator=(const FaceGroup&);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_GROUP_H
