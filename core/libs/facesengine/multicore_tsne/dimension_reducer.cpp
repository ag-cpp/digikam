#include "dimension_reducer.h"

#include "tsne.h"

namespace Digikam 
{
cv::Mat DimensionReducer::reduceDimension(cv::Mat data, int outputDimension, int nbCPU)
{
    cv::Mat projectedData(data.rows, outputDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(data.data), 
                          data.rows, 
                          data.cols, 
                          reinterpret_cast<float*>(projectedData.data), 
                          outputDimension, 
                          30, 
                          0.5, 
                          nbCPU);
    
    return projectedData;
}

cv::Mat DimensionReducer::reduceDimension(cv::Mat trainData, cv::Mat newData, int outputDimension, int nbCPU) 
{
    cv::Mat buffer = trainData.clone();
    buffer.push_back(newData);

    cv::Mat projectedData(buffer.rows, outputDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(buffer.data), 
                         buffer.rows, 
                         buffer.cols, 
                         reinterpret_cast<float*>(projectedData.data), 
                         outputDimension, 
                         30, 
                         0.5, 
                         nbCPU);

    cv::Mat output;
    projectedData(cv::Range(std::max(0, projectedData.rows - newData.rows), projectedData.rows), cv::Range(0, projectedData.cols)).copyTo(output);

    return output;
}

class DimensionReducer::Private
{
public:
    explicit Private(int capacity, int targetNbDimension, int nbCPU)
        : capacity(capacity),
          targetNbDimension(targetNbDimension),
          nbCPU(nbCPU)
    {
    }

public:
    cv::Mat buffer;
    int capacity;
    int targetNbDimension;
    int nbCPU;
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

    cv::Mat projectedData(d->buffer.rows, d->targetNbDimension, CV_32F);
    TSNE::tsne_run_float(reinterpret_cast<float*>(d->buffer.data), 
                          d->buffer.rows, 
                          d->buffer.cols, 
                          reinterpret_cast<float*>(projectedData.data), 
                          d->targetNbDimension, 
                          30, 
                          0.5, 
                          d->nbCPU);

    cv::Mat output;
    projectedData(cv::Range(std::max(0, projectedData.rows - data.rows), projectedData.rows), cv::Range(0, projectedData.cols)).copyTo(output);

    return output;
}

} // namespace Digikam 
