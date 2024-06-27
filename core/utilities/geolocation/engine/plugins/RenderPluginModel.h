// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#pragma once

#include <QStandardItemModel>
#include <QList>

#include "PluginInterface.h"

#include "digikam_export.h"

namespace Marble
{

class DialogConfigurationInterface;
class PluginInterface;
class RenderPlugin;

/**
 * @brief Provides common access to various kinds of plugins without having to know about their details.
 */
class DIGIKAM_EXPORT RenderPluginModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /**
     * This enum contains the data roles for the QStandardItems.
     */
    enum ItemDataRole
    {
        Name = Qt::DisplayRole,          // QString
        Icon = Qt::DecorationRole,       // QIcon
        Description = Qt::ToolTipRole,   // QString
        NameId = Qt::UserRole + 2,       // QString
        ConfigurationDialogAvailable,    // bool
        BackendTypes,                    // QStringList
        Version,                         // QString
        AboutDataText,                   // QString
        CopyrightYears                   // QString
    };

    explicit RenderPluginModel(QObject* parent = nullptr);

    ~RenderPluginModel() override;

    /**
     * @brief Set the RenderPlugins the model should manage.
     *
     * The given plugins must not be deleted as long as the model has a hold on them,
     * i.e. until the model is deleted or a different set of plugins is assigned.
     *
     * @param renderPlugins the RenderPlugins to be managed
     */
    void setRenderPlugins(const QList<RenderPlugin*>& renderPlugins);

    QVector<PluginAuthor> pluginAuthors(const QModelIndex& index) const;

    DialogConfigurationInterface* pluginDialogConfigurationInterface(const QModelIndex& index);
    RenderPlugin* plugin(const QModelIndex& index);
    PluginInterface* pluginIface(const QModelIndex& index);

public Q_SLOTS:

    /**
     * Retrieve the current plugin state for the user interface.
     */
    void retrievePluginState();

    /**
     * Apply the plugin state from the user interface.
     */
    void applyPluginState();

private:

    class Private;
    Private* const d = nullptr;

    friend class Private;
};

} // namespace Marble
