
#include "imagequalitycalculator.h"

// Local includes 

// Qt includes

#include <QList>


namespace Digikam
{

class Q_DECL_HIDDEN ImageQualityCalculator::Private
{
public:

    explicit Private()
    {

    }

    QList<resultDetection> results;
};

void ImageQualityCalculator::addDetectionResult(const QString& name, const float score, const float weight) const
{
    resultDetection result;
    
    result.detetionType = name;
    result.weight       = weight;
    result.score        = score;

    d->results.append(result);
}

void ImageQualityCalculator::normalizeWeight()
{
    float sum = 0;

    for (const auto result : results)
    {
        sum += result.weight;
    }

    for (const auto result : results)
    {
        result.weight /= sum;
    }
}

float ImageQualityCalculator::calculateQuality()
{
    normalizeWeight();

    float quality;

    for (const auto result : results)
    {
        quality += result.score * result.weight;
    }

    return quality;
}




}