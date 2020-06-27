/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : a tool to export items to a local storage
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_FC_EXPORT_WIDGET_H
#define DIGIKAM_FC_EXPORT_WIDGET_H

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QWidget>
#include <QUrl>

// Local includes

#include "dinfointerface.h"
#include "ditemslist.h"

// C++ includes

#include <memory>

using namespace Digikam;

namespace DigikamGenericFileCopyPlugin
{

using ChangeImagePropertiesPtr = std::shared_ptr<struct ChangeImageProperties>;

class FCExportWidget: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent widget
     */
    explicit FCExportWidget(DInfoInterface* const iface, QWidget* const parent);

    /**
     * Destructor.
     */
    ~FCExportWidget();

    /**
     * Returns a pointer to the imagelist that is displayed.
     */
    DItemsList* imagesList() const;

    /**
     * Returns a pointer to the overwrite QCheckBox.
     */
    QCheckBox* overwriteBox() const;

    /**
     * Returns a pointer to the target QButtonGroup.
     */
    QButtonGroup* targetButtonGroup() const;

    /**
     * Returns the currently selected target url. Maybe invalid.
     */
    QUrl targetUrl() const;

    /**
     * Sets the target url this widget should point at.
     */
    void setTargetUrl(const QUrl& url);

    /**
     * @brief collects all change settings that should be executed to the images collection
     * @return properties or nullptr in case of no change necessary
     */
    ChangeImagePropertiesPtr getImageChangeProperties() const;

private Q_SLOTS:

    void slotLabelUrlChanged();

    void slotFileCopyButtonChanged(bool enabled);

Q_SIGNALS:

    void signalTargetUrlChanged(const QUrl& target);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericFileCopyPlugin

#endif // DIGIKAM_FC_EXPORT_WIDGET_H
