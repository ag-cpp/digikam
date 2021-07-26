#include "dimension_reducer.h"

#include "tsne.h"

namespace Digikam 
{

cv::Mat DimensionReducer::project(cv::Mat data, int nbDimension) 
{
    cv::Mat projectedData(data.rows, nbDimension, CV_32F);
    TSNE::tsne_run_double(reinterpret_cast<float*>(data.data), 
                          data.rows, 
                          data.cols, 
                          reinterpret_cast<float*>(projectedData.data), 
                          nbDimension, 
                          30, 
                          0.5, 
                          4);

    return projectedData;
}

} // namespace Digikam 