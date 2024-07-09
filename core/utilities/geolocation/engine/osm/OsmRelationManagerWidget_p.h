/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "ui_OsmRelationManagerWidget.h"

namespace Marble
{

class OsmRelationManagerWidget;
class GeoDataPlacemark;
class OsmPlacemarkData;

class Q_DECL_HIDDEN OsmRelationManagerWidgetPrivate : public Ui::OsmRelationManagerWidgetPrivate
{
public:

    OsmRelationManagerWidgetPrivate();
    ~OsmRelationManagerWidgetPrivate();
    void populateRelationsList();
    void populateDropMenu();

private:

    friend class OsmRelationManagerWidget;

    GeoDataPlacemark* m_placemark                           = nullptr;
    const QHash< qint64, OsmPlacemarkData >* m_allRelations = nullptr;
    QMenu* m_relationDropMenu                               = nullptr;
};

} // namespace Marble
