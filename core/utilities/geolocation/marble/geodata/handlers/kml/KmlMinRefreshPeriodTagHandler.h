// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef KMLMINREFRESHPERIODTAGHANDLER_H
#define KMLMINREFRESHPERIODTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlminRefreshPeriodTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
