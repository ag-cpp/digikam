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

// Qt includes

#include <QObject>

// Local includes

#include "PluginInterface.h"

namespace Marble
{

class SearchRunner;

/**
  * A plugin for Marble to execute a placemark search.
  */
class DIGIKAM_EXPORT SearchRunnerPlugin : public QObject, public PluginInterface
{
    Q_OBJECT

public:

    /** Constructor with optional parent object */
    explicit SearchRunnerPlugin(QObject* parent = nullptr);

    /** Destructor */
    ~SearchRunnerPlugin() override;

    /**
     * @brief Returns the string that should appear in the user interface.
     *
     * For example, "Nominatim" should be returned for the Nominatim search plugin.
     */
    virtual QString guiString() const = 0;

    /** Plugin factory method to create a new runner instance.
      * Method caller gets ownership of the returned object
      */
    virtual SearchRunner* newRunner() const = 0;

    /** True if the plugin supports its tasks on the given planet */
    bool supportsCelestialBody(const QString& celestialBodyId) const;

    /** True if the plugin can execute its tasks without network access */
    bool canWorkOffline() const;

    /**
     * @brief Returns @code true @endcode if the plugin is able to perform its claimed task.
     *
     * The default implementation returns @code true @endcode. This method can be
     * overridden for example to indicate an incomplete installation.
     */
    virtual bool canWork() const;

    // Overridden methods with default implementations

    QIcon icon() const override;

protected:

    void setSupportedCelestialBodies(const QStringList& celestialBodies);

    void setCanWorkOffline(bool canWorkOffline);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Marble

Q_DECLARE_INTERFACE(Marble::SearchRunnerPlugin, "org.kde.Marble.SearchRunnerPlugin/1.01")
