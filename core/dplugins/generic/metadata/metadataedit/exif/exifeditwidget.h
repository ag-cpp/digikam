/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a Widget to edit EXIF metadata
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Victor Dodon <dodon dot victor at gmail dot com>
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

#ifndef DIGIKAM_EXIF_EDIT_WIDGET_H
#define DIGIKAM_EXIF_EDIT_WIDGET_H

// Local includes

#include "dconfigdlgwidgets.h"

using namespace Digikam;

namespace DigikamGenericMetadataEditPlugin
{

class MetadataEditDialog;

class EXIFEditWidget : public DConfigDlgWdg
{
    Q_OBJECT

public:

    explicit EXIFEditWidget(MetadataEditDialog* const parent);
    ~EXIFEditWidget() override;

    bool isModified() const;
    void saveSettings();
    void apply();

Q_SIGNALS:

    void signalSetReadOnly(bool);
    void signalModified();

public Q_SLOTS:

    void slotModified();
    void slotItemChanged();

private:

    void readSettings();

    int  activePageIndex() const;
    void showPage(int page);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_EXIF_EDIT_WIDGET_H
