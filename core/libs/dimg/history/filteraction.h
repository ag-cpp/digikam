/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-02
 * Description : class that holds list of applied filters to image
 *
 * SPDX-FileCopyrightText: 2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QFlags>
#include <QHash>
#include <QString>
#include <QVariant>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FilterAction
{
public:

    enum Category
    {
        /**
         * NOTE: Do not change existing values, they are written to XML in files!
         */

        /**
         * When given the set of stored parameters and the original data,
         *  an identical result will be produced.
         */
        ReproducibleFilter = 0,

        /**
         * The operation is documented and a number of parameters may be known,
         * but the identical result cannot be reproduced.
         * It may be possible to produce a sufficiently similar result.
         */
        ComplexFilter      = 1,

        /**
         * The source images are known, a textual description may be added,
         * but there is no way to automatically replay
         */
        DocumentedHistory  = 2,

        CategoryFirst      = ReproducibleFilter,
        CategoryLast       = DocumentedHistory
    };

    enum Flag
    {
        /**
         * The editing step of this filter action explicitly branches from the parent.
         * This is an optional hint that the result is meant as a new version.
         */
        ExplicitBranch     = 1 << 0
    };
    Q_DECLARE_FLAGS(Flags, Flag)

public:

    FilterAction() = default;
    FilterAction(const QString& identifier, int version, Category category = ReproducibleFilter);

    bool isNull()                                                   const;

    bool operator==(const FilterAction& other)                      const;

    Category category()                                             const;

    /**
     * Returns a technical identifier for the filter used to produce this action.
     * Can include a namespace. Example: digikam:charcoal
     */
    QString  identifier()                                           const;

    /**
     * Returns the version (>= 1) of the filter used to produce this action.
     * When a filter / tool is found by the identifier, it can decide
     * by the version if it supports this action and which parameters it expects.
     */
    int      version()                                              const;

    /**
     * Returns a description / comment for this action.
     * In the case of DocumentedHistory, this may be the most useful value.
     */
    QString  description()                                          const;
    void     setDescription(const QString& description);

    QString  displayableName()                                      const;
    void     setDisplayableName(const QString& displayableName);

    Flags flags()                                                   const;
    void  setFlags(Flags flags);
    void  addFlag(Flags flags);
    void  removeFlag(Flags flags);

    /**
     * Access parameters.
     * A parameters is a key -> value pair.
     * Keys need to be unique.
     */
    bool                           hasParameters()                  const;
    const QHash<QString,QVariant>& parameters()                     const;
    QHash<QString, QVariant>&      parameters();

    bool                           hasParameter(const QString& key) const;
    const QVariant                 parameter(const QString& key)    const;
    QVariant&                      parameter(const QString& key);

    /**
     * Returns parameter converted from QVariant to given type
     */
    template <typename T>
    T parameter(const QString& key)                                 const
    {
        return parameter(key).value<T>();
    }

    /**
     * Read parameter with a default value:
     * If there is a parameter for the given key, return it converted
     *  from QVariant to the template type.
     * If there is no parameter, return the given default value.
     */
    template <typename T>
    T parameter(const QString& key, const T& defaultValue)          const
    {
        QVariant var = parameter(key);
        return (
                (var.isValid()) ? var.value<T>()
                                : defaultValue
               );
    }

    /// Sets parameter, removing all other values for the same key
    void addParameter(const QString& key, const QVariant& value);

    /// Removes all parameters for key
    void removeParameters(const QString& key);

    /// Clear all parameters
    void clearParameters();

    /// Replaces parameters
    void setParameters(const QHash<QString, QVariant>& params);

protected:

    /// NOTE: Value class, do not create a d-pointer
    Category                 m_category         = ReproducibleFilter;
    Flags                    m_flags;
    QString                  m_identifier;
    int                      m_version          = 0;
    QString                  m_description;
    QString                  m_displayableName;
    QHash<QString, QVariant> m_params;
};

} // namespace Digikam

Q_DECLARE_METATYPE(Digikam::FilterAction)
Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::FilterAction::Flags)
