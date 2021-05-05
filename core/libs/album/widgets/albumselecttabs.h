/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-26-02
 * Description : a widget to select albums using a tab of folder views.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ALBUM_SELECT_TABS_H
#define DIGIKAM_ALBUM_SELECT_TABS_H

// Qt includes

#include <QTabWidget>

// Local includes

#include "album.h"

namespace Digikam
{

class AbstractCheckableAlbumModel;
class AlbumLabelsSearchHandler;

class AlbumSelectTabs : public QTabWidget
{
    Q_OBJECT

public:

    explicit AlbumSelectTabs(const QString& name, QWidget* const parent = nullptr);
    ~AlbumSelectTabs()                                      override;

    AlbumList selectedAlbums()                        const;
    void enableVirtualAlbums(bool flag = true);

    QList<AbstractCheckableAlbumModel*> albumModels() const;
    AlbumLabelsSearchHandler* albumLabelsHandler()    const;

Q_SIGNALS:

    void signalAlbumSelectionChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ALBUM_SELECT_TABS_H
