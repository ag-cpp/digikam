/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-02-12
 * Description : Table view column helpers
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tableview_columnfactory.h"

// KDE include

#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "itemfiltermodel.h"
#include "iteminfo.h"
#include "tableview_columns.h"

namespace Digikam
{

TableViewColumn::TableViewColumn(TableViewShared* const tableViewShared,
                                 const TableViewColumnConfiguration& pConfiguration,
                                 QObject* const parent)
    : QObject      (parent),
      s            (tableViewShared),
      configuration(pConfiguration)
{
}

// ---------------------------------------------------------------------------------------------

TableViewColumnFactory::TableViewColumnFactory(TableViewShared* const tableViewShared,
                                               QWidget* const parent)
    : QObject        (parent),
      m_displayWidget(parent),
      s              (tableViewShared)
{
}

TableViewColumn* TableViewColumnFactory::getColumn(const Digikam::TableViewColumnConfiguration& columnConfiguration)
{
    TableViewColumn* newColumn = nullptr;

    if (TableViewColumns::ColumnThumbnail::CreateFromConfiguration(s, columnConfiguration, &newColumn, m_displayWidget))
    {
        return newColumn;
    }

    if (TableViewColumns::ColumnDigikamProperties::CreateFromConfiguration<TableViewColumns::ColumnDigikamProperties>(s, columnConfiguration, &newColumn, this))
    {
        return newColumn;
    }

    if (TableViewColumns::ColumnPhotoProperties::CreateFromConfiguration<TableViewColumns::ColumnPhotoProperties>(s, columnConfiguration, &newColumn, this))
    {
        return newColumn;
    }

    if (TableViewColumns::ColumnFileProperties::CreateFromConfiguration<TableViewColumns::ColumnFileProperties>(s, columnConfiguration, &newColumn, this))
    {
        return newColumn;
    }

    if (TableViewColumns::ColumnGeoProperties::CreateFromConfiguration<TableViewColumns::ColumnGeoProperties>(s, columnConfiguration, &newColumn, this))
    {
        return newColumn;
    }

    if (TableViewColumns::ColumnItemProperties::CreateFromConfiguration<TableViewColumns::ColumnItemProperties>(s, columnConfiguration, &newColumn, this))
    {
        return newColumn;
    }

    if (TableViewColumns::ColumnAudioVideoProperties::CreateFromConfiguration<TableViewColumns::ColumnAudioVideoProperties>(s, columnConfiguration, &newColumn, this))
    {
        return newColumn;
    }

    return nullptr;
}

QList<TableViewColumnDescription> TableViewColumnFactory::getColumnDescriptionList()
{
    QList<TableViewColumnDescription> descriptionList;

    descriptionList << TableViewColumns::ColumnThumbnail::getDescription();
    descriptionList << TableViewColumns::ColumnFileProperties::getDescription();
    descriptionList << TableViewColumns::ColumnItemProperties::getDescription();
    descriptionList << TableViewColumns::ColumnPhotoProperties::getDescription();
    descriptionList << TableViewColumns::ColumnDigikamProperties::getDescription();
    descriptionList << TableViewColumns::ColumnGeoProperties::getDescription();
    descriptionList << TableViewColumns::ColumnAudioVideoProperties::getDescription();

    return descriptionList;
}

// ---------------------------------------------------------------------------------------------

bool TableViewColumn::paint(QPainter* const painter,
                            const QStyleOptionViewItem& option,
                            TableViewModel::Item* const item) const
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(item)

    return false;
}

QSize TableViewColumn::sizeHint(const QStyleOptionViewItem& option, TableViewModel::Item* const item) const
{
    Q_UNUSED(option)
    Q_UNUSED(item)

    return QSize();
}

QVariant TableViewColumn::data(TableViewModel::Item* const item, const int role) const
{
    Q_UNUSED(item)
    Q_UNUSED(role)

    return QVariant();
}

TableViewColumn::ColumnFlags TableViewColumn::getColumnFlags() const
{
    return ColumnNoFlags;
}

TableViewColumnConfiguration TableViewColumn::getConfiguration() const
{
    return configuration;
}

/**
 * This function should never be called, because subclasses have to do the comparison on their own. But it can not be
 * pure, since then every subclass which does not do custom comparison would have to implement an empty stub.
 */
TableViewColumn::ColumnCompareResult TableViewColumn::compare(TableViewModel::Item* const itemA, TableViewModel::Item* const itemB) const
{
    Q_UNUSED(itemA)
    Q_UNUSED(itemB)

    qCWarning(DIGIKAM_GENERAL_LOG) << "Unimplemented custom comparison. Make sure getColumnFlags() does not return ColumnCustomSorting.";

    return CmpEqual;
}

TableViewColumnConfigurationWidget* TableViewColumn::getConfigurationWidget(QWidget* const parentWidget) const
{
    Q_UNUSED(parentWidget)

    return nullptr;
}

void TableViewColumn::setConfiguration(const TableViewColumnConfiguration& newConfiguration)
{
    Q_UNUSED(newConfiguration)
}

void TableViewColumn::updateThumbnailSize()
{
}

bool TableViewColumn::compareHelperBoolFailCheck(const bool okA, const bool okB, ColumnCompareResult* const result)
{
    if (okA && okB)
    {
        return true;
    }

    if (okA && !okB)
    {
        *result = CmpABiggerB;

        return false;
    }

    if (okB && !okA)
    {
        *result = CmpALessB;

        return false;
    }

    *result = CmpEqual;

    return false;
}

bool TableViewColumn::columnAffectedByChangeset(const ImageChangeset& /*imageChangeset*/) const
{
    return true;
}

// ---------------------------------------------------------------------------------------------

void TableViewColumnProfile::loadSettings(const KConfigGroup& configGroup)
{
    name               = configGroup.readEntry("Profile Name", QString());
    headerState        = configGroup.readEntry("Header State", QByteArray());
    headerState        = QByteArray::fromBase64(headerState);
    const int nColumns = configGroup.readEntry("Column Count", int(0));

    for (int i = 0 ; i < nColumns ; ++i)
    {
        /// @todo check for invalid column configurations

        const QString configSubGroupName = QString::fromUtf8("Column %1").arg(i);
        const KConfigGroup subGroup      = configGroup.group(configSubGroupName);

        /// @todo move loading into TableViewColumnConfiguration

        TableViewColumnConfiguration columnConfiguration;
        columnConfiguration.loadSettings(subGroup);

        columnConfigurationList << columnConfiguration;
    }

    if (columnConfigurationList.isEmpty())
    {
        // no data loaded, create default entries

        /// @todo Set the default sorting column to something other than the thumbnail.

        TableViewColumnDescription::List allColumns = TableViewColumnFactory::getColumnDescriptionList();

        TableViewColumnDescription nextDesc;

        if (TableViewColumnDescription::FindInListById(allColumns, QLatin1String("thumbnail"), &nextDesc))
        {
            columnConfigurationList << nextDesc.toConfiguration();
        }

        if (TableViewColumnDescription::FindInListById(allColumns, QLatin1String("filename"), &nextDesc))
        {
            columnConfigurationList << nextDesc.toConfiguration();
        }

        if (TableViewColumnDescription::FindInListById(allColumns, QLatin1String("digikam-rating"), &nextDesc))
        {
            columnConfigurationList << nextDesc.toConfiguration();
        }

        if (TableViewColumnDescription::FindInListById(allColumns, QLatin1String("itemcreationdatetime"), &nextDesc))
        {
            columnConfigurationList << nextDesc.toConfiguration();
        }

        if (TableViewColumnDescription::FindInListById(allColumns, QLatin1String("digikam-title"), &nextDesc))
        {
            columnConfigurationList << nextDesc.toConfiguration();
        }

        if (TableViewColumnDescription::FindInListById(allColumns, QLatin1String("digikam-tags"), &nextDesc))
        {
            columnConfigurationList << nextDesc.toConfiguration();
        }
    }
}

void TableViewColumnProfile::saveSettings(KConfigGroup& configGroup)
{
    configGroup.writeEntry(QLatin1String("Profile Name"), name);
    const int nColumns = columnConfigurationList.count();
    configGroup.writeEntry(QLatin1String("Column Count"), nColumns);
    configGroup.writeEntry(QLatin1String("Header State"), headerState.toBase64());

    for (int i = 0 ; i < nColumns ; ++i)
    {
        const QString configSubGroupName                        = QString::fromUtf8("Column %1").arg(i);
        KConfigGroup subGroup                                   = configGroup.group(configSubGroupName);
        const TableViewColumnConfiguration& columnConfiguration = columnConfigurationList.at(i);;
        columnConfiguration.saveSettings(subGroup);
    }
}

// ---------------------------------------------------------------------------------------------

void TableViewColumnConfiguration::loadSettings(const KConfigGroup& configGroup)
{
    columnId            = configGroup.readEntry(QLatin1String("Column Id"), QString());
    const int nSettings = configGroup.readEntry(QLatin1String("NSettings"), int(0));

    for (int i = 0 ; i < nSettings ; ++i)
    {
        const QString& key   = configGroup.readEntry(QString::fromUtf8("Key %1").arg(i),   QString());
        const QString& value = configGroup.readEntry(QString::fromUtf8("Value %1").arg(i), QString());

        if (!key.isEmpty())
        {
            columnSettings.insert(key, value);
        }
    }
}

void TableViewColumnConfiguration::saveSettings(KConfigGroup& configGroup) const
{
    configGroup.writeEntry(QLatin1String("Column Id"), columnId);

    const int nSettings = columnSettings.count();
    configGroup.writeEntry(QLatin1String("NSettings"), nSettings);

    QHashIterator<QString, QString> settingsIterator(columnSettings);

    for (int i = 0 ; settingsIterator.hasNext() ; ++i)
    {
        settingsIterator.next();
        configGroup.writeEntry(QString::fromUtf8("Key %1").arg(i),   settingsIterator.key());
        configGroup.writeEntry(QString::fromUtf8("Value %1").arg(i), settingsIterator.value());
    }
}

// ---------------------------------------------------------------------------------------------

TableViewColumnConfigurationWidget::TableViewColumnConfigurationWidget(TableViewShared* const sharedObject,
                                                                       const TableViewColumnConfiguration& currentConfiguration,
                                                                       QWidget* const parent)
    : QWidget      (parent),
      s            (sharedObject),
      configuration(currentConfiguration)
{
}

} // namespace Digikam

#include "moc_tableview_columnfactory.cpp"
