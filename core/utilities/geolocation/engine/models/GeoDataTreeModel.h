/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QAbstractItemModel>

// Local includes

#include "digikam_export.h"

class QItemSelectionModel;

namespace Marble
{

class GeoDataObject;
class GeoDataDocument;
class GeoDataFeature;
class GeoDataContainer;
class GeoDataTourPrimitive;

/**
 * @short The representation of GeoData in a model
 * This class represents all available data given by kml-data files.
 */
class DIGIKAM_EXPORT GeoDataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /**
     * Creates a new GeoDataTreeModel.
     *
     * @param parent The parent object.
     */
    explicit GeoDataTreeModel(QObject* parent = nullptr);

    /**
     * Destroys the GeoDataModel.
     */
    ~GeoDataTreeModel() override;

    /**
     * Return the number of Items in the Model.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex& index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex index(const GeoDataObject* object) const;

    QModelIndex parent(const QModelIndex& index) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    QItemSelectionModel* selectionModel();

    GeoDataDocument* rootDocument();

public Q_SLOTS:

    /**
      * Sets the root document to use. This replaces previously loaded data, if any.
      * @param document The new root document. Ownership retains with the caller,
      *   i.e. GeoDataTreeModel will not delete the passed document at its destruction.
      */
    void setRootDocument(GeoDataDocument* document);

    int addFeature(GeoDataContainer* parent, GeoDataFeature* feature, int row = -1);

    bool removeFeature(GeoDataContainer* parent, int index);

    int removeFeature(GeoDataFeature* feature);

    void updateFeature(GeoDataFeature* feature);

    int addDocument(GeoDataDocument* document);

    void removeDocument(int index);

    void removeDocument(GeoDataDocument* document);

    int addTourPrimitive(const QModelIndex& parent, GeoDataTourPrimitive* primitive, int row = -1);
    bool removeTourPrimitive(const QModelIndex& parent, int index);
    bool swapTourPrimitives(const QModelIndex& parent, int indexA, int indexB);

Q_SIGNALS:

    /// insert and remove row don't trigger any signal that proxies forward
    /// this signal will refresh geometry layer and placemark layout
    void removed(GeoDataObject* object);
    void added(GeoDataObject* object);

private:

    Q_DISABLE_COPY(GeoDataTreeModel)

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Marble
