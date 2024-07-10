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

//
// This class is a crosshairs plugin.
//

#pragma once

// Qt includes

#include <QPixmap>

// Local includes

#include "RenderPlugin.h"
#include "DialogConfigurationInterface.h"

class QSvgRenderer;

namespace Ui
{
class CrosshairsConfigWidget;
}

namespace Marble
{

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 */
class CrosshairsPlugin : public RenderPlugin,
    public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CrosshairsPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(CrosshairsPlugin)

public:

    CrosshairsPlugin();

    explicit CrosshairsPlugin(const MarbleModel* marbleModel);

    ~CrosshairsPlugin() override;

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

    RenderType renderType() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    void initialize() override;

    bool isInitialized() const override;

    bool render(GeoPainter* painter, ViewportParams* viewport, const QString& renderPos, GeoSceneLayer* layer = nullptr) override;

    QDialog* configDialog() override;

    QHash<QString, QVariant> settings() const override;

    void setSettings(const QHash<QString, QVariant>& settings) override;

private Q_SLOTS:

    void readSettings();

    void writeSettings();

private:

    Q_DISABLE_COPY(CrosshairsPlugin)

    bool                        m_isInitialized;

    QSvgRenderer*               m_svgobj            = nullptr;
    QPixmap                     m_crosshairs;
    int                         m_themeIndex;

    QString                     m_theme;

    QDialog*                    m_configDialog      = nullptr;
    Ui::CrosshairsConfigWidget* m_uiConfigWidget    = nullptr;
};

} // namespace Marble
