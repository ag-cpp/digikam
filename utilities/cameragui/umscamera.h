/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-12-21
 * Description : USB Mass Storage Implementation of abstract
 * type DKCamera
 * 
 * Copyright 2004-2005 by Renchi Raju
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
 * ============================================================ */

#ifndef UMSCAMERA_H
#define UMSCAMERA_H

// Qt includes.

#include <qstringlist.h>

// Local includes.

#include "dkcamera.h"

namespace Digikam
{

class UMSCameraPriv;

class UMSCamera : public DKCamera
{
public:

    UMSCamera(const QString& model,
              const QString& port,
              const QString& path);
    ~UMSCamera();

    bool connect();
    void cancel();

    void getAllFolders(const QString& folder,
                       QStringList& subFolderList);
    bool getItemsInfoList(const QString& folder,
                          GPItemInfoList& infoList);
    bool getThumbnail(const QString& folder,
                      const QString& itemName,
                      QImage& thumbnail);
    bool getExif(const QString& folder,
                 const QString& itemName,
                 char **edata, int& esize);

    bool downloadItem(const QString& folder,
                      const QString& itemName,
                      const QString& saveFile);
    bool deleteItem(const QString& folder,
                    const QString& itemName);
    bool uploadItem(const QString& folder,
                    const QString& itemName,
                    const QString& localFile);

private:

    void    listFolders(const QString& folder,
                        QStringList& subFolderList);
    QString mimeType(const QString& fileext) const;

private :

    UMSCameraPriv* d;

};

}  // namespace Digikam

#endif /* UMSCAMERA_H */
