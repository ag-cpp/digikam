/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-04-30
 * Description : selection icon view item at mouse hover
 *
 * Copyright (C) 2008 by Peter Penz <peter.penz@gmx.at>
 * Copyright (C) 2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "facerejectionoverlay.h"

// KDE includes

#include <kiconloader.h>
#include <klocale.h>

// Local includes

#include "imagecategorizedview.h"
#include "itemviewhoverbutton.h"
#include "imageinfo.h"
#include "faceiface.h"
#include "digikamimagefacedelegate.h"
#include "imagemodel.h"

namespace Digikam
{

FaceRejectionOverlayButton::FaceRejectionOverlayButton(QAbstractItemView *parentView)
                           : ItemViewHoverButton(parentView)
{
}

QSize FaceRejectionOverlayButton::sizeHint() const
{
    return QSize(16, 16);
}

QPixmap FaceRejectionOverlayButton::icon()
{
    const char* icon = "dialog-close";
    return KIconLoader::global()->loadIcon(icon,
                                           KIconLoader::NoGroup,
                                           KIconLoader::SizeSmall);
}

void FaceRejectionOverlayButton::updateToolTip()
{
    setToolTip(i18nc("@info:tooltip", "If this is not a face, click to reject it."));
}

// --------------------------------------------------------------------

FaceRejectionOverlay::FaceRejectionOverlay(QObject *parent)
                     : HoverButtonDelegateOverlay(parent)
{
}

void FaceRejectionOverlay::setActive(bool active)
{
    HoverButtonDelegateOverlay::setActive(active);

    if (active)
    {
        connect(button(), SIGNAL(clicked(bool)),
                this, SLOT(slotClicked()));

        connect(m_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                this, SLOT(slotSelectionChanged(const QItemSelection&, const QItemSelection&)));
    }
    else
    {
        // button is deleted

        if (m_view)
        {
            disconnect(m_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                       this, SLOT(slotSelectionChanged(const QItemSelection&, const QItemSelection&)));
        }
    }
}

ItemViewHoverButton *FaceRejectionOverlay::createButton()
{
    return new FaceRejectionOverlayButton(view());
}

void FaceRejectionOverlay::updateButton(const QModelIndex& index)
{
    const QRect rect = m_view->visualRect(index);

    const int gap = 5;
    const int x   = rect.right() - button()->sizeHint().width() - gap;
    const int y   = rect.top() + gap;
    button()->move(QPoint(x, y));
}

void FaceRejectionOverlay::slotClicked()
{
    QModelIndex index = button()->index();
    if (index.isValid())
    {
        emit rejectFace(index);
    }
}

void FaceRejectionOverlay::slotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    QModelIndex index = button()->index();
    if (index.isValid())
    {
        if (selected.contains(index))
            button()->setChecked(true);
        else if (deselected.contains(index))
            button()->setChecked(false);
    }
}

} // namespace Digikam
