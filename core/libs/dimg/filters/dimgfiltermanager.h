/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-24
 * Description : manager for filters (registering, creating etc)
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef DIGIKAM_DIMG_FILTER_MANAGER_H
#define DIGIKAM_DIMG_FILTER_MANAGER_H

// Qt includes

#include <QStringList>
#include <QList>
#include <QString>

// Local includes

#include "dimgfiltergenerator.h"
#include "digikam_export.h"

namespace Digikam
{

class FilterAction;

class DIGIKAM_EXPORT DImgFilterManager : public DImgFilterGenerator
{
public:

    static DImgFilterManager* instance();

    /**
     * Returns a list of the supported filter identifiers
     */
    QStringList supportedFilters()                                  override;

    /**
     * Returns a list of supported versions of the given filter
     */
    QList<int> supportedVersions(const QString& filterIdentifier)   override;

    /**
     * Returns the (untranslated) displayable name for the given identifier.
     * This is only possible for supported filters. If you have a FilterAction,
     * it may already contain a displayable name.
     */
    QString displayableName(const QString& filterIdentifier)        override;

    /**
     * Returns the translated displayable name
     */
    QString i18nDisplayableName(const QString& filterIdentifier);
    QString i18nDisplayableName(const FilterAction& action);

    /**
     * Returns an icon for the given filter.
     * If no icon is known, returns a null string.
     */
    QString filterIcon(const QString& filterIdentifier);
    QString filterIcon(const FilterAction& action);

    /**
     * Returns true if the given filter, or, more specifically,
     * the given filter in the given version is supported.
     */
    bool isSupported(const QString& filterIdentifier)               override;
    bool isSupported(const QString& filterIdentifier, int version)  override;

    /**
     * Returns true if the given filter is to be considered
     * as a step converting a RAW image to a normal image.
     */
    bool isRawConversion(const QString& filterIdentifier);

    /**
     * Create a filter from an installed manager.
     * Returns 0 if no filter could be created. This is true
     * if identifier/version is not supported, or the filter is builtin.
     * Note: You probably want to use FilterActionFilter.
     */
    DImgThreadedFilter* createFilter(const QString& filterIdentifier,
                                     int version)                   override;

    /**
     * Registers all filter provided by this generator.
     */
    void addGenerator(DImgFilterGenerator* const generator);
    void removeGenerator(DImgFilterGenerator* const generator);

private:

    DImgFilterManager();                                    ///< Disable
    ~DImgFilterManager() override;                                   ///< Disable
    DImgFilterManager(const DImgFilterManager&);            ///< Disable
    DImgFilterManager& operator=(const DImgFilterManager&); ///< Disable

private:

    friend class DImgFilterManagerCreator;

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_FILTER_MANAGER_H
