/* ============================================================
 * File  : imageplugin_freerotation.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2004-11-28
 * Description : 
 * 
 * Copyright 2004 by Gilles Caulier
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


#ifndef IMAGEPLUGIN_FREEROTATION_H
#define IMAGEPLUGIN_FREEROTATION_H

// Digikam includes.

#include <digikam/imageplugin.h>

class ImagePlugin_FreeRotation : public Digikam::ImagePlugin
{
    Q_OBJECT
    
public:

    ImagePlugin_FreeRotation(QObject *parent, const char* name,
                         const QStringList &args);
    ~ImagePlugin_FreeRotation();

    QStringList guiDefinition() const;

private slots:

    void slotFreeRotation();

};
    
#endif /* IMAGEPLUGIN_FREEROTATION_H */
