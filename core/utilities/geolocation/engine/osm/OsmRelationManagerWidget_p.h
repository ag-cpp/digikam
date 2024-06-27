// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#pragma once

#include "ui_OsmRelationManagerWidget.h"

namespace Marble
{

class OsmRelationManagerWidget;
class GeoDataPlacemark;
class OsmPlacemarkData;

class OsmRelationManagerWidgetPrivate : public Ui::OsmRelationManagerWidgetPrivate
{
public:

    OsmRelationManagerWidgetPrivate();
    ~OsmRelationManagerWidgetPrivate();
    void populateRelationsList();
    void populateDropMenu();

private:

    friend class OsmRelationManagerWidget;

    GeoDataPlacemark *m_placemark                           = nullptr;
    const QHash< qint64, OsmPlacemarkData > *m_allRelations = nullptr;
    QMenu *m_relationDropMenu                               = nullptr;
};

} // namespace Marble
