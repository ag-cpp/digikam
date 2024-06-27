// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#pragma once

#include <QWidget>
#include "digikam_export.h"

#include "MarbleGlobal.h"

class QTreeWidgetItem;

namespace Marble
{

class OsmTagEditorWidgetPrivate;
class GeoDataPlacemark;
class GeoDataFeature;
class OsmPlacemarkData;

class DIGIKAM_EXPORT OsmTagEditorWidget : public QWidget
{
    Q_OBJECT

public:

    explicit OsmTagEditorWidget( GeoDataPlacemark *placemark, QWidget *parent = nullptr );
    ~OsmTagEditorWidget() override;

    /**
     * @brief returns the current tag set for the placemark
     */
    OsmPlacemarkData placemarkData() const;

public Q_SLOTS:

    void update();
    void addSelectedTag();
    void removeSelectedTag();
    void handleDoubleClick( QTreeWidgetItem *item, int column );
    void handleItemChanged( QTreeWidgetItem *item, int column );

Q_SIGNALS:

    void placemarkChanged( GeoDataFeature *);

private:

    friend class OsmTagEditorWidgetPrivate;

    OsmTagEditorWidgetPrivate* const d = nullptr;
};

} // namespace Marble

