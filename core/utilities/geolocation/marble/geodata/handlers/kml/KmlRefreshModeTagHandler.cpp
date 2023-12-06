#include "KmlRefreshModeTagHandler.h"
#include "digikam_debug.h"
#include "KmlElementDictionary.h"
#include "GeoDataLink.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( refreshMode )

GeoNode* KmlrefreshModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_refreshMode)));

    GeoStackItem parentItem = parser.parentElement();
    QString content = parser.readElementText().trimmed();
    GeoDataLink::RefreshMode mode;

    if ( parentItem.is<GeoDataLink>()) {
        if (content == QLatin1String("onChange")) {
            mode = GeoDataLink::OnChange;
        } else if (content == QLatin1String("onExpire")) {
            mode = GeoDataLink::OnExpire;
        } else if (content == QLatin1String("onInterval")) {
            mode = GeoDataLink::OnInterval;
        } else {
            mode = GeoDataLink::OnChange;
            qCDebug(DIGIKAM_MARBLE_LOG) << "Value " << content << "set in kml file is invalid."
            << "Value of <refreshMode> has been reset to onChange" ;
        }
        parentItem.nodeAs<GeoDataLink>()->setRefreshMode( mode );
    }

    return nullptr;
}

}
}

