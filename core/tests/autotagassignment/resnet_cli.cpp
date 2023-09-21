
// Qt includes

#include <QApplication>
#include <QCommandLineParser>
#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QLabel>
#include <QPen>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QHash>
#include <QFileInfo>
#include <QRandomGenerator>

// Local includes

#include "digikam_debug.h"
#include "dnnbasedetectormodel.h"
#include "dnnresnetdetector.h"

using namespace Digikam;

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("dataset"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}

void showObjects(const QString& imagePath, const QMap<QString, QVector<QRect>>& detectedBoxes)
{
    qCDebug(DIGIKAM_TESTS_LOG) << "Loading " << imagePath;
    QImage img(imagePath);

    QWidget* const mainWidget = new QWidget;
    QScrollArea* const scrollArea = new QScrollArea;
    scrollArea->setWidget(mainWidget);
    scrollArea->setWidgetResizable(true);

    QHBoxLayout* const layout = new QHBoxLayout(mainWidget);
    QLabel* const fullImage = new QLabel;
    fullImage->setScaledContents(true);
    layout->addWidget(fullImage);

    // draw rect objects
    QPainter painter(&img);
    QPen paintPen(Qt::red);
    paintPen.setWidth(1);
    painter.setPen(paintPen);

    for (QMap<QString, QVector<QRect>>::const_iterator it = detectedBoxes.constBegin();
        it != detectedBoxes.constEnd(); it++)
    { 
        painter.drawText(QPoint(20, 30), it.key());
    }

    // Only setPixmap after finishing drawing bboxes around detected objects
    fullImage->setPixmap(QPixmap::fromImage(img));

    scrollArea->show();
    scrollArea->setWindowTitle(imagePath);
    qApp->processEvents(); // dirty hack
}


int main(int argc, char** argv)
{   
    QApplication app(argc, argv);
    // Options for commandline parser

    QCommandLineParser* const parser = parseOptions(app);
    if (!parser->isSet(QLatin1String("dataset")))
    {
        qWarning("Data set is not set !!!");
    }
 
    QDir dataset(parser->value(QLatin1String("dataset"))); 
    QFileInfoList filesInfo = dataset.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    std::vector<cv::Mat> cvImages;
    QScopedPointer<DNNResnetDetector> resnetDetector (new DNNResnetDetector());

    for (int j = 0; j < filesInfo.size(); j++)
    {
        cv::Mat cvImage = cv::imread(filesInfo[j].absoluteFilePath().toStdString());
        showObjects(filesInfo[j].absoluteFilePath(),resnetDetector->detectObjects(cvImage));
    }

    return app.exec();
}