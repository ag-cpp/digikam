#include <memory>

#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QElapsedTimer>

#include "dimension_reducer.h"

// --------------------------------------------------------

std::shared_ptr<QCommandLineParser> parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("in"), QLatin1String("Data file"), QLatin1String("path relative to data file")));
    parser->addOption(QCommandLineOption(QLatin1String("out"), QLatin1String("Output file"), QLatin1String("path relative to output file")));
    parser->addHelpOption();
    parser->process(app);

    return std::shared_ptr<QCommandLineParser>(parser);
}

/**
 * @brief loadData: load data from csv file into a train dataset
 */ 
std::pair<cv::Mat, cv::Mat> loadData(const QString& fileName) 
{
    cv::Mat predictors, labels;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) 
    {
        qDebug() << file.errorString();
        return std::make_pair(predictors, labels);
    }

    while (!file.atEnd())
    {
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

    return std::make_pair(predictors, labels);
}

cv::Mat extractTrainData(std::pair<cv::Mat, cv::Mat> data, int nbPoints)
{
    cv::Mat trainData;
    std::unordered_map<int, int> counters;

    for (int i = 0; i < data.first.rows; ++i) 
    {
        if (counters[data.second.row(i).at<int>(0)] < nbPoints)
        {
            ++counters[data.second.row(i).at<int>(0)];
            trainData.push_back(data.first.row(i));
        }
    }

    return trainData;
}

void save(std::pair<cv::Mat, cv::Mat> data, const QString& fileName) 
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) 
    {
        qDebug() << file.errorString();
        return;
    }

    QTextStream streamOut(&file);

    cv::Mat samples = data.first;
    cv::Mat labels = data.second;

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

int main(int argc, char** argv) 
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));
    std::shared_ptr<QCommandLineParser> parser = parseOptions(app);

    std::pair<cv::Mat, cv::Mat> data = loadData(parser->value(QLatin1String("in")));
    //cv::Mat trainData = extractTrainData(data, 100);

    cv::Mat samples = data.first;
    
    QElapsedTimer timer;
    timer.start();
    
    cv::Mat projectedData = Digikam::DimensionReducer::reduceDimension(samples, 2, 4);

    qDebug() << "Parse through" << samples.rows << "in" << timer.elapsed();
    /*
    Digikam::DimensionReducer reducer(1000, 2, 4);

    int i = 0;
    while (i < samples.rows)
    {
        QElapsedTimer timer;
        timer.start();

        cv::Mat buffer;
        samples(cv::Range(i, std::min(i+100, samples.rows)), cv::Range(0, samples.cols)).copyTo(buffer);


        cv::Mat projected = Digikam::DimensionReducer::reduceDimension(trainData, buffer, 2, 4);

        projectedData.push_back(projected);
        
        qDebug() << "Parse through" << 100 << "in" << timer.elapsed();
        i += 100;
    }
    */
    
    save(std::make_pair(projectedData, data.second),parser->value(QLatin1String("out")));
}