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

#include "MarblePlacemarkModel.h"
#include "MarblePlacemarkModel_P.h"

// Qt includes

#include <QElapsedTimer>
#include <QImage>

// Local includes

#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataGeometry.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "digikam_debug.h"

namespace Marble
{

class Q_DECL_HIDDEN MarblePlacemarkModel::Private
{
public:

    Private()
        : m_size(0),
          m_placemarkContainer(nullptr)
    {
    }

    ~Private()
    {
    }

    int                             m_size;
    QVector<GeoDataPlacemark*>*     m_placemarkContainer = nullptr;
};

// ---------------------------------------------------------------------------

MarblePlacemarkModel::MarblePlacemarkModel(QObject* parent)
    : QAbstractListModel(parent),
      d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[DescriptionRole] = "description";
    roles[Qt::DisplayRole] = "name";
    roles[Qt::DecorationRole] = "icon";
    roles[IconPathRole] = "iconPath";
    roles[PopularityIndexRole] = "zoomLevel";
    roles[VisualCategoryRole] = "visualCategory";
    roles[AreaRole] = "area";
    roles[PopulationRole] = "population";
    roles[CountryCodeRole] = "countryCode";
    roles[StateRole] = "state";
    roles[PopularityRole] = "popularity";
    roles[GeoTypeRole] = "role";
    roles[CoordinateRole] = "coordinate";
    roles[StyleRole] = "style";
    roles[GmtRole] = "gmt";
    roles[DstRole] = "dst";
    roles[GeometryRole] = "geometry";
    roles[ObjectPointerRole] = "objectPointer";
    roles[LongitudeRole] = "longitude";
    roles[LatitudeRole] = "latitude";
    m_roleNames = roles;
}

MarblePlacemarkModel::~MarblePlacemarkModel()
{
    delete d;
}

void MarblePlacemarkModel::setPlacemarkContainer(QVector<GeoDataPlacemark*>* container)
{
    d->m_placemarkContainer = container;
}

int MarblePlacemarkModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        return d->m_size;
    }

    else
    {
        return 0;
    }
}

int MarblePlacemarkModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        return 1;
    }

    else
    {
        return 0;
    }
}

QHash<int, QByteArray> MarblePlacemarkModel::roleNames() const
{
    return m_roleNames;
}

QVariant MarblePlacemarkModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= d->m_placemarkContainer->size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        return d->m_placemarkContainer->at(index.row())->name();
    }

    else if (role == Qt::DecorationRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->style()->iconStyle().icon());
    }

    else if (role == IconPathRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->style()->iconStyle().iconPath());
    }

    else if (role == PopularityIndexRole)
    {
        return d->m_placemarkContainer->at(index.row())->zoomLevel();
    }

    else if (role == VisualCategoryRole)
    {
        return d->m_placemarkContainer->at(index.row())->visualCategory();
    }

    else if (role == AreaRole)
    {
        return d->m_placemarkContainer->at(index.row())->area();
    }

    else if (role == PopulationRole)
    {
        return d->m_placemarkContainer->at(index.row())->population();
    }

    else if (role == CountryCodeRole)
    {
        return d->m_placemarkContainer->at(index.row())->countryCode();
    }

    else if (role == StateRole)
    {
        return d->m_placemarkContainer->at(index.row())->state();
    }

    else if (role == PopularityRole)
    {
        return d->m_placemarkContainer->at(index.row())->popularity();
    }

    else if (role == DescriptionRole)
    {
        return d->m_placemarkContainer->at(index.row())->description();
    }

    else if (role == Qt::ToolTipRole)
    {
        return d->m_placemarkContainer->at(index.row())->description();
    }

    else if (role == GeoTypeRole)
    {
        return d->m_placemarkContainer->at(index.row())->role();
    }

    else if (role == CoordinateRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->coordinate());
    }

    else if (role == StyleRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->style().data());
    }

    else if (role == GmtRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->extendedData().value(QStringLiteral("gmt")).value());
    }

    else if (role == DstRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->extendedData().value(QStringLiteral("dst")).value());
    }

    else if (role == GeometryRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->geometry());
    }

    else if (role == ObjectPointerRole)
    {
        return QVariant::fromValue(dynamic_cast<GeoDataObject*>(d->m_placemarkContainer->at(index.row())));
    }

    else if (role == LongitudeRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->coordinate().longitude(GeoDataCoordinates::Degree));
    }

    else if (role == LatitudeRole)
    {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->coordinate().latitude(GeoDataCoordinates::Degree));
    }

    else
    {
        return QVariant();
    }
}

QModelIndexList MarblePlacemarkModel::approxMatch(const QModelIndex& start, int role,
                                                  const QVariant& value, int hits,
                                                  Qt::MatchFlags flags) const
{
    QList<QModelIndex> results;

    int         count = 0;

    QModelIndex entryIndex;
    QString     listName;
    QString     queryString = value.toString().toLower();
    QString     simplifiedListName;

    int         row = start.row();
    const int   rowNum = rowCount();

    while (row < rowNum && count != hits)
    {
        if (flags & Qt::MatchStartsWith)
        {
            entryIndex = index(row, 0);
            listName    = data(entryIndex, role).toString().toLower();
            simplifiedListName = GeoString::deaccent(listName);

            if (listName.startsWith(queryString)
                || simplifiedListName.startsWith(queryString)
               )
            {
                results << entryIndex;
                ++count;
            }
        }

        ++row;
    }

    return results;
}

void MarblePlacemarkModel::addPlacemarks(int start,
                                         int length)
{
    Q_UNUSED(start);

    // performance wise a reset is far better when the provided list
    // is significant. That is an issue because we have
    // MarbleControlBox::m_sortproxy as a sorting customer.
    // I leave the balance search as an exercise to the reader...

    QElapsedTimer t;
    t.start();
    //    beginInsertRows( QModelIndex(), start, start + length );
    d->m_size += length;
    //    endInsertRows();
    beginResetModel();
    endResetModel();
    Q_EMIT countChanged();
    qCDebug(DIGIKAM_MARBLE_LOG) << "addPlacemarks: Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
}

void  MarblePlacemarkModel::removePlacemarks(const QString& containerName,
                                             int start,
                                             int length)
{
    if (length > 0)
    {
        QElapsedTimer t;
        t.start();
        beginRemoveRows(QModelIndex(), start, start + length);
        d->m_size -= length;
        endRemoveRows();
        Q_EMIT layoutChanged();
        Q_EMIT countChanged();
        qCDebug(DIGIKAM_MARBLE_LOG) << "removePlacemarks(" << containerName << "): Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
    }
}

} // namespace Marble

#include "moc_MarblePlacemarkModel.cpp"
