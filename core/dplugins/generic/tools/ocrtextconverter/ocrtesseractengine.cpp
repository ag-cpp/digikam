/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR Tesseract engine
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ocrtesseractengine.h"

// Qt includes

#include <QDir>
#include <QUrl>
#include <QProcess>
#include <QFileInfo>

// local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "localizeselector.h"

namespace DigikamGenericTextConverterPlugin
{

class OcrTesseractEngine::Private
{

public:

    Private()
      : cancel(false)
    {
    }

    OcrOptions opt;

    bool       cancel;

    QString    inputFile;
    QString    outputFile;
    QString    ocrResult;
};

OcrTesseractEngine::OcrTesseractEngine()
    : d(new Private())
{
}

OcrTesseractEngine::~OcrTesseractEngine()
{
    delete d;
}

void OcrTesseractEngine::setOcrOptions(const OcrOptions& opt)
{
    d->opt = opt;
}

OcrOptions OcrTesseractEngine::ocrOptions() const
{
    return d->opt;
}

QString OcrTesseractEngine::inputFile() const
{
    return d->inputFile;
}

QString OcrTesseractEngine::outputFile() const
{
    return d->outputFile;
}

QString OcrTesseractEngine::outputText() const
{
    return d->ocrResult;
}

void OcrTesseractEngine::setInputFile(const QString& filePath)
{
    d->inputFile = filePath;
}

void OcrTesseractEngine::setOutputFile(const QString& filePath)
{
    d->outputFile = filePath;
}

int OcrTesseractEngine::runOcrProcess()
{
    d->cancel = false;
    QScopedPointer<QProcess> ocrProcess (new QProcess());
    ocrProcess->setProcessEnvironment(adjustedEnvironmentForAppImage());
    ocrProcess->setProcessChannelMode(QProcess::SeparateChannels);

    try
    {
        // ------------------------- IN/OUT ARGUMENTS -------------------------

        QStringList args;

        // add configuration image

        if (!d->inputFile.isEmpty())
        {
            args << d->inputFile;
        }

        // output base name

        QString mess;

        args << QLatin1String("stdout");

        // ----------------------------- OPTIONS -----------------------------

        // page Segmentation mode

        QString val = d->opt.PsmCodeToValue(static_cast<OcrOptions::PageSegmentationModes>(d->opt.psm));

        if (!val.isEmpty())
        {
            args << QLatin1String("--psm") << val;
        }

        // OCR enginge mode

        val = d->opt.OemCodeToValue(static_cast<OcrOptions::EngineModes>(d->opt.oem));

        if (!val.isEmpty())
        {
            args << QLatin1String("--oem") << val;
        }

        // Language

        val = d->opt.language;

        if (!val.isEmpty())
        {
            args << QLatin1String("-l") << val;
        }

        // dpi

        val = QString::fromLatin1("%1").arg(d->opt.dpi);

        if (!val.isEmpty())
        {
            args << QLatin1String("--dpi") << val;
        }

        // ------------------  Running tesseract process ------------------

        ocrProcess->setWorkingDirectory(QDir::tempPath());
        ocrProcess->setProgram(d->opt.tesseractPath);
        ocrProcess->setArguments(args);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Running OCR : "
                                     << ocrProcess->program()
                                     << ocrProcess->arguments();

        ocrProcess->start();

        bool successFlag = ocrProcess->waitForFinished(-1) && (ocrProcess->exitStatus() == QProcess::NormalExit);

        if (!successFlag)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Error starting OCR Process";

            return PROCESS_FAILED;
        }

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }
    }
    catch (const QProcess::ProcessError& e)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Text Converter has error" << e;

        return PROCESS_FAILED;
    }

    d->ocrResult = QString::fromLocal8Bit(ocrProcess->readAllStandardOutput());

    saveOcrResult();

    return PROCESS_COMPLETE;
}

void OcrTesseractEngine::saveOcrResult()
{
    MetaEngine::AltLangMap commentsMap;
    commentsMap.insert(QLatin1String("x-default"), d->ocrResult);

    if (d->opt.isSaveTextFile || d->opt.isSaveXMP)
    {
        translate(commentsMap, d->opt.translations);
    }

    if (d->opt.isSaveTextFile)
    {
        saveTextFile(d->inputFile, d->outputFile, commentsMap);
    }

    if (d->opt.isSaveXMP)
    {
        saveXMP(QUrl::fromLocalFile(d->inputFile), commentsMap, d->opt.iface);
    }
}

void OcrTesseractEngine::translate(MetaEngine::AltLangMap& commentsMap,
                                   const QStringList& langs)
{
    QString text = commentsMap[QLatin1String("x-default")];

    Q_FOREACH (const QString& lg, langs)
    {
        QString tr;
        QString error;

        bool b = s_inlineTranslateString(text, lg, tr, error);

        if (b)
        {
            commentsMap.insert(lg, tr);
        }
        else
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Error while translating in" << lg << ":" << error;
        }
    }
}

void OcrTesseractEngine::saveTextFile(const QString& inFile,
                                      QString& outFile,
                                      const MetaEngine::AltLangMap& commentsMap)
{
    Q_FOREACH (const QString& lg, commentsMap.keys())
    {
        QFileInfo fi(inFile);
        outFile = fi.absolutePath()  +
                  QLatin1String("/") +
                  (QString::fromLatin1("%1-ocr-%2.txt").arg(fi.fileName()).arg(lg));

        QFile file(outFile);

        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
        {
            QTextStream stream(&file);
            stream << commentsMap[lg];
            file.close();
        }
    }
}

void OcrTesseractEngine::saveXMP(const QUrl& url,
                                 const MetaEngine::AltLangMap& commentsMap,
                                 DInfoInterface* const iface)
{
    CaptionsMap commentsSet;
    QString   author = QLatin1String("digiKam OCR Text Converter Plugin");
    QDateTime dt     = QDateTime::currentDateTime();

    MetaEngine::AltLangMap authorsMap;
    MetaEngine::AltLangMap datesMap;

    Q_FOREACH (const QString& lg, commentsMap.keys())
    {
        datesMap.insert(lg,   dt.toString(Qt::ISODate));
        authorsMap.insert(lg, author);
    }

    commentsSet.setData(commentsMap, authorsMap, QString(), datesMap);

    // --- Version using DInfoInterface

    DItemInfo witem;
    witem.setCaptions(commentsSet);
    iface->setItemInfo(url, witem.infoMap());
}

} // namespace DigikamGenericTextConverterPlugin
