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
    
    //cv::Mat normalizedData;
    //cv::normalize(projectedData, normalizedData, 1.0, 0.0, cv::NORM_L2);

    return projectedData;
}


} // namespace Digikam 
