/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect claws-mail binary program
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_CLAWS_MAIL_BINARY_H
#define DIGIKAM_CLAWS_MAIL_BINARY_H

// Local includes

#include "dbinaryiface.h"

using namespace Digikam;

namespace DigikamGenericSendByMailPlugin
{

class ClawsMailBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit ClawsMailBinary(QObject* const parent = nullptr);
    ~ClawsMailBinary() override;
};

} // namespace DigikamGenericSendByMailPlugin

#endif // DIGIKAM_CLAWS_MAIL_BINARY_H
