/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-21
 * Description : MJPEG Stream Server configuration dialog
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "mjpegstreamdlg_p.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegStreamDlg::Private::Private()
    : dirty           (false),
      mngr            (MjpegServerMngr::instance()),
      srvButton       (nullptr),
      srvStatus       (nullptr),
      srvPreview      (nullptr),
      progress        (nullptr),
      aStats          (nullptr),
      separator       (nullptr),
      iStats          (nullptr),
      startOnStartup  (nullptr),
      spacing         (0),
      albumSupport    (false),
      albumSelector   (nullptr),
      listView        (nullptr),
      buttons         (nullptr),
      streamSettings  (nullptr),
      srvPort         (nullptr),
      delay           (nullptr),
      rate            (nullptr),
      quality         (nullptr),
      streamLoop      (nullptr),
      typeVal         (nullptr),
      effVal          (nullptr),
      effPreview      (nullptr),
      transVal        (nullptr),
      transPreview    (nullptr),
      tabView         (nullptr)
{
}

MjpegStreamDlg::Private::~Private()
{
}

} // namespace DigikamGenericMjpegStreamPlugin
