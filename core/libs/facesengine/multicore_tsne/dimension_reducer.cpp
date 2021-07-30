#include "dimension_reducer.h"

#include "tsne.h"

namespace Digikam 
{
cv::Mat DimensionReducer::reduceDimension(cv::Mat data, int outputDimension, int nbCore)
{
    cv::Mat projectedData(data.rows, outputDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(data.data), 
                          data.rows, 
                          data.cols, 
                          reinterpret_cast<float*>(projectedData.data), 
                          outputDimension, 
                          30, 
                          0.5, 
                          nbCore);
    
    return projectedData;
}

class DimensionReducer::Private
{
public:
    explicit Private(int capacity, int targetNbDimension, int nbCore)
        : capacity(capacity),
          targetNbDimension(targetNbDimension),
          nbCore(nbCore)
    {
    }

public:
    cv::Mat buffer;
    int capacity;
    int targetNbDimension;
    int nbCore;
};

DimensionReducer::DimensionReducer(int bufferCapacity, int targetNbDimension, int nbCore)
    : d(new Private(bufferCapacity, targetNbDimension, nbCore))
{
}

DimensionReducer::~DimensionReducer()
{
    delete d;
}

cv::Mat DimensionReducer::project(cv::Mat data) 
{
    d->buffer = d->buffer(cv::Range(std::max(0, d->buffer.rows - d->capacity + data.rows), d->buffer.rows), cv::Range(0, d->buffer.cols)); 
    d->buffer.push_back(data);

    cv::Mat projectedData(tempBuffer.rows, d->targetNbDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(tempBuffer.data), 
                          tempBuffer.rows, 
                          tempBuffer.cols, 
                          reinterpret_cast<float*>(projectedData.data), 
                          d->targetNbDimension, 
                          30, 
                          0.5, 
                          d->nbCore);

    cv::Mat output;
    projectedData(cv::Range(std::max(0, projectedData.rows - data.rows), projectedData.rows), cv::Range(0, projectedData.cols)).copyTo(output);

    d->buffer = tempBuffer;

    return output;
}

} // namespace Digikam 