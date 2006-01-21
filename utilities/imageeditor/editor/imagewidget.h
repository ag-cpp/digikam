/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at free.fr>
 * Date   : 2004-02-14
 * Description : simple widget to display an image 
 * 
 * Copyright 2004 by Renchi Raju
 * Copyright 2005-2006 by Gilles Caulier
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

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

// Qt includes.

#include <qwidget.h>

// Local includes.

#include "dimg.h"
#include "digikam_export.h"

namespace Digikam
{

class ImageIface;
class ImageWidgetPriv;

class DIGIKAMIMAGEEDITOR_EXPORT ImageWidget : public QWidget
{
    Q_OBJECT

public:

    ImageWidget(int width, int height, QWidget *parent=0);
    ~ImageWidget();

    ImageIface* imageIface();

signals:

    void signalResized(void);

protected:

    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent * e);

private:

    ImageWidgetPriv* d;
    
};

}  // namespace Digikam

#endif /* IMAGEWIDGET_H */
