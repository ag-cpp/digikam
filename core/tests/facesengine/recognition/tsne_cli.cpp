#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>


#include "dimension_reducer.h"

/**
 * @brief loadData: load data from csv file into a train dataset
 */ 
cv::Ptr<cv::ml::TrainData> loadData(const QString& fileName) 
{
    cv::Mat predictors, labels;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return cv::ml::TrainData::create(predictors, 0, labels);
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList<QByteArray> data = line.split(',');

        cv::Mat predictor(1, data.size()-1, CV_32F);
        for (int i = 1; i < data.size(); ++i) 
        {
            predictor.at<float>(i-1) = data[i].toFloat();
        }

        labels.push_back(data[0].toInt());
        predictors.push_back(predictor);
    }

    return cv::ml::TrainData::create(predictors, 0, labels);
}

void save(cv::Ptr<cv::ml::TrainData> data, const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << file.errorString();
        return;
    }

    QTextStream streamOut(&file);

    cv::Mat samples = data->getSamples();
    cv::Mat labels = data->getResponses();

    for (int i = 0; i < samples.rows; ++i) 
    {
        QStringList line;
        line << QString::number(labels.row(i).at<int>(0));

        for(int j=0; j<samples.row(i).cols; ++j)
        {
            line << QString::number(double(samples.row(i).at<float>(j)));
        } 

        streamOut << line.join(QLatin1Char(',')) << "\n"; 
    }
}

int main() 
{
    Digikam::DimensionReducer reducer;
    cv::Ptr<cv::ml::TrainData> data = loadData(QLatin1String("facenet_data.txt"));

    cv::Mat projectedData = reducer.project(data->getSamples(), 2);

    save(cv::ml::TrainData::create(projectedData, 0, data->getResponses()), QLatin1String("facenet_tsne_data.txt"));
}