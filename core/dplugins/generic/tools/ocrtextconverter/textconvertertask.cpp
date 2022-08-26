#include "textconvertertask.h"

// Local includes

#include "ocrtesseractengine.h"
#include "ocroptions.h"


using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterTask::Private

{
public:

    Private()
      : language(int(OcrOptions::Languages::DEFAULT)),
        psm(int(OcrOptions::PageSegmentationModes::DEFAULT)),
        oem(int(OcrOptions::EngineModes::DEFAULT)),
        dpi(300),
        isSaveTextFile(true),
        isSaveXMP(true),
        cancel(false)
    {
    }

    int                  language;
    int                  psm;
    int                  oem;
    int                  dpi;
    bool                 isSaveTextFile;
    bool                 isSaveXMP;

    bool                 cancel;

    QUrl                 url;
    TextConverterAction  action;

    OcrTesseracrEngine   ocrEngine;
};

TextConverterTask::TextConverterTask(QObject* const parent,
                                     const QUrl& fileUrl,
                                     const TextConverterAction& action)
    : ActionJob(parent),
      d        (new Private)
{
    d->url    = fileUrl;
    d->action = action;
}

TextConverterTask::~TextConverterTask()
{
    slotCancel();
    delete d;
}


void TextConverterTask::setLanguagesMode(int mode)
{
    d->language = mode;
}

void TextConverterTask::setPSMMode(int mode)
{
    d->psm = mode;
}

void TextConverterTask::setOEMMode(int mode)
{
    d->oem = mode;
}

void TextConverterTask::setDpi(int value)
{
    d->dpi = value;
}

void TextConverterTask::setIsSaveTextFile(bool check)
{
    d->isSaveTextFile = check;
}

void TextConverterTask::setIsSaveXMP(bool check)
{
    d->isSaveXMP = check;
}

void TextConverterTask::run()
{

    if (d->cancel)
    {
        return;
    }

    switch (d->action)
    {
        case PROCESS:
        {
            TextConverterActionData ad1;
            ad1.action    = PROCESS;
            ad1.fileUrl   = d->url;
            ad1.starting  = true;
            Q_EMIT signalStarting(ad1);

            d->ocrEngine.setInputFile(d->url.toLocalFile());
            d->ocrEngine.setIsSaveTextFile(d->isSaveTextFile);
            d->ocrEngine.setIsSaveXMP(d->isSaveXMP);
            d->ocrEngine.setLanguagesMode(d->language);
            d->ocrEngine.setPSMMode(d->psm);
            d->ocrEngine.setOEMMode(d->oem);
            d->ocrEngine.setDpi(d->dpi);
            int ret = d->ocrEngine.runOcrProcess();

            TextConverterActionData ad2;
            ad2.action     = PROCESS;
            ad2.fileUrl    = d->url;
            ad2.destPath   = d->ocrEngine.outputFile();
            ad2.result     = ret;
            ad2.outputText = d->ocrEngine.outputText();

            Q_EMIT signalFinished(ad2);
            break;
        }

        default:
        {
            qCritical() << "Unknown action specified";
            break;
        }
    }

    Q_EMIT signalDone();

}

void TextConverterTask::slotCancel()
{
    d->cancel = true;
}


} // namespace DigikamGenericTextConverterPlugin
