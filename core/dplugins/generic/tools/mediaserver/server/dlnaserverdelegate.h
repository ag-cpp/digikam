/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-09-24
 * Description : a media server to export collections through DLNA.
 *               Implementation inspired on Platinum File Media Server.
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DLNA_SERVER_DELEGATE_H
#define DIGIKAM_DLNA_SERVER_DELEGATE_H

// Qt includes

#include <QString>
#include <QUrl>
#include <QList>
#include <QMap>
#include <QImage>
#include <QByteArray>
#include <QBuffer>

// Local includes

#include "digikam_debug.h"
#include "previewloadthread.h"
#include "dimg.h"
#include "drawdecoder.h"

// Local includes

#include "dmediaserver.h"

// Platinum includes

#include "Neptune.h"
#include "PltMediaServer.h"
#include "PltMediaCache.h"

namespace Digikam
{

/**
 * File Media Server Delegate for digiKam.
 * The DLNAMediaServerDelegate class is based on PLT_MediaServerDelegate
 * implementation for a file system backed Media Server.
 */
class DLNAMediaServerDelegate : public PLT_MediaServerDelegate
{
public:

    /// Constructor and destructor
    explicit DLNAMediaServerDelegate(const char* url_root,
                                     bool use_cache = false);
    ~DLNAMediaServerDelegate() override;

    /// Class methods
    static NPT_String BuildSafeResourceUri(const NPT_HttpUrl& base_uri,
                                           const char*        host,
                                           const char*        file_path);

    void addAlbumsOnServer(const MediaServerMap& map);

protected:

    /// PLT_MediaServerDelegate methods
    NPT_Result OnBrowseMetadata(PLT_ActionReference&          action,
                                const char*                   object_id,
                                const char*                   filter,
                                NPT_UInt32                    starting_index,
                                NPT_UInt32                    requested_count,
                                const char*                   sort_criteria,
                                const PLT_HttpRequestContext& context)          override;

    NPT_Result OnBrowseDirectChildren(PLT_ActionReference&          action,
                                      const char*                   object_id,
                                      const char*                   filter,
                                      NPT_UInt32                    starting_index,
                                      NPT_UInt32                    requested_count,
                                      const char*                   sort_criteria,
                                      const PLT_HttpRequestContext& context)    override;

    NPT_Result OnSearchContainer(PLT_ActionReference&          action,
                                 const char*                   object_id,
                                 const char*                   search_criteria,
                                 const char*                   filter,
                                 NPT_UInt32                    starting_index,
                                 NPT_UInt32                    requested_count,
                                 const char*                   sort_criteria,
                                 const PLT_HttpRequestContext& context)         override;

    NPT_Result ProcessFileRequest(NPT_HttpRequest&              request,
                                  const NPT_HttpRequestContext& context,
                                  NPT_HttpResponse&             response)       override;

    /// Overridable methods
    virtual NPT_Result ExtractResourcePath(const NPT_HttpUrl& url,
                                           NPT_String& file_path);

    virtual NPT_String BuildResourceUri(const NPT_HttpUrl& base_uri,
                                        const char* host,
                                        const char* file_path);

    virtual NPT_Result ServeFile(const NPT_HttpRequest&        request,
                                 const NPT_HttpRequestContext& context,
                                 NPT_HttpResponse&             response,
                                 const NPT_String&             file_path);

    virtual NPT_Result GetFilePath(const char* object_id,
                                   NPT_String& filepath);

    virtual bool       ProcessFile(const NPT_String&,
                                   const char* filter = nullptr);

    virtual PLT_MediaObject* BuildFromFilePath(const NPT_String&             filepath,
                                               const PLT_HttpRequestContext& context,
                                               bool                          with_count = true,
                                               bool                          keep_extension_in_title = false,
                                               bool                          allip = false);

protected:

    friend class PLT_MediaItem;

    class Private;
    Private* const d;

private:

    Q_DISABLE_COPY(DLNAMediaServerDelegate)
};

} // namespace Digikam

#endif // DIGIKAM_DLNA_SERVER_DELEGATE_H
