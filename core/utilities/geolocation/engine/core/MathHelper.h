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

#pragma once

// Qt includes

#include <qglobal.h>

#ifdef Q_CC_MSVC

#   include <math.h>

namespace Marble
{

qreal msvc_asinh(qreal x);

#   define asinh msvc_asinh

qreal msvc_atanh(qreal x);

#   define atanh msvc_atanh

} // Namespace Marble

#endif // Q_CC_MSVC
