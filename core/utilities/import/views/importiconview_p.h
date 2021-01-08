/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-25-07
 * Description : Private Qt item view for images
 *
 * Copyright (C)      2012 by Islam Wazery <wazery at ubuntu dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_IMPORT_ICON_VIEW_PRIVATE_H
#define DIGIKAM_IMPORT_ICON_VIEW_PRIVATE_H

// Qt includes

#include <QObject>

// Local includes

#include "importiconview.h"
#include "importdelegate.h"
#include "importsettings.h"
#include "importoverlays.h"

namespace Digikam
{

class ImportNormalDelegate;

class Q_DECL_HIDDEN ImportIconView::Private : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ImportIconView)

public:

    explicit Private(ImportIconView* const qq);
    ~Private() override;

    void updateOverlays();

public:

    ItemViewUtilities*     utilities;
    ImportNormalDelegate*  normalDelegate;

    bool                   overlaysActive;

    ImportRotateOverlay*   rotateLeftOverlay;
    ImportRotateOverlay*   rotateRightOverlay;

private:

    ImportIconView*        q_ptr;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_ICON_VIEW_PRIVATE_H
