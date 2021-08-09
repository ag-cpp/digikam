#include "itemiconview_p.h"

#include "setupimagequalitysorter.h"
#include "maintenancethread.h"

namespace Digikam
{


void ItemIconView::slotImageQualitySorter()
{
    QStringList paths;

    for (const auto& url : selectedUrls())
    {
        paths<<url.toLocalFile();
    }

    qInfo()<<paths;

    SetupImageQualitySorter* settingWidgets = new SetupImageQualitySorter();

    ImageQualityContainer settings = settingWidgets->getImageQualityContainer();

    MaintenanceThread* thread = new MaintenanceThread(this);

    thread->sortByImageQuality(paths, settings);
    
    thread->start();
}


}