/* ============================================================
 * File  : imageplugin_emboss.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2004-08-26
 * Description : 
 * 
 * Copyright 2004 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */


#ifndef IMAGEPLUGIN_EMBOSS_H
#define IMAGEPLUGIN_EMBOSS_H

// Digikam includes.

#include <imageplugin.h>

class ImagePlugin_Emboss : public Digikam::ImagePlugin
{
    Q_OBJECT
    
public:

    ImagePlugin_Emboss(QObject *parent, const char* name,
                         const QStringList &args);
    ~ImagePlugin_Emboss();

    QStringList guiDefinition() const;

private slots:

    void slotEmboss();

};
    
#endif /* IMAGEPLUGIN_EMBOSS_H */
