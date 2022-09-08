/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR Tesseract engine
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
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
#include "ocroptions.h"
#include "dmetadata.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class OcrTesseractEngine::Private
{

public:

    Private()
      : language        ((int)OcrOptions::Languages::DEFAULT),
        psm             ((int)OcrOptions::PageSegmentationModes::DEFAULT),
        oem             ((int)OcrOptions::EngineModes::DEFAULT),
        dpi             (300),
        isSaveTextFile  (true),
        isSaveXMP       (true),
        cancel          (false)
    {
    }

    int        language;
    int        psm;
    int        oem;
    int        dpi;

    bool       isSaveTextFile;
    bool       isSaveXMP;

    bool       cancel;

    QString    inputFile;
    QString    outputFile;
    QString    ocrResult;

    QString    tesseractPath;
};

OcrTesseractEngine::OcrTesseractEngine()
    : d(new Private())
{
}

OcrTesseractEngine::~OcrTesseractEngine()
{
    delete d;
}

void OcrTesseractEngine::setTesseractPath(const QString& path)
{
    d->tesseractPath = path;
}

void OcrTesseractEngine::setLanguagesMode(int mode)
{
    d->language = mode;
}

int OcrTesseractEngine::languagesMode() const
{
    return d->language;
}

void OcrTesseractEngine::setPSMMode(int mode)
{
    d->psm = mode;
}

int OcrTesseractEngine::PSMMode() const
{
    return d->psm;
}

void OcrTesseractEngine::setOEMMode(int mode)
{
    d->oem = mode;
}

int OcrTesseractEngine::OEMMode() const
{
    return d->oem;
}

void OcrTesseractEngine::setDpi(int value)
{
    d->dpi = value;
}

int OcrTesseractEngine::Dpi() const
{
    return d->dpi;
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

void OcrTesseractEngine::setIsSaveTextFile(bool check)
{
    d->isSaveTextFile = check;
}

bool OcrTesseractEngine::isSaveTextFile() const
{
    return d->isSaveTextFile;
}

void OcrTesseractEngine::setIsSaveXMP(bool check)
{
    d->isSaveXMP = check;
}

bool OcrTesseractEngine::isSaveXMP() const
{
    return d->isSaveXMP;
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

        OcrOptions ocropt;

        // page Segmentation mode

        QString val = ocropt.PsmCodeToValue(static_cast<OcrOptions::PageSegmentationModes>(d->psm));

        if (!val.isEmpty())
        {
            args << QLatin1String("--psm") << val;
        }

        // OCR enginge mode

        val = ocropt.OemCodeToValue(static_cast<OcrOptions::EngineModes>(d->oem));

        if (!val.isEmpty())
        {
            args << QLatin1String("--oem") << val;
        }

        // Language

        val = ocropt.LanguageCodeToValue(static_cast<OcrOptions::Languages>(d->language));

        if (!val.isEmpty())
        {
            args << QLatin1String("-l") << val;
        }

        // dpi

        val = QString::fromLatin1("%1").arg(d->dpi);

        if (!val.isEmpty())
        {
            args << QLatin1String("--dpi") << val;
        }

        // ------------------  Running tesseract process ------------------

        ocrProcess->setWorkingDirectory(QDir::tempPath());
        ocrProcess->setProgram(d->tesseractPath);
        ocrProcess->setArguments(args);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Running OCR : "
                                     << ocrProcess->program()
                                     << ocrProcess->arguments();

        ocrProcess->start();

        bool successFlag =  ocrProcess->waitForFinished(-1) &&  ocrProcess->exitStatus() == QProcess::NormalExit;

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
    catch(const QProcess::ProcessError& e)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Text Converter has error" << e;

        return PROCESS_FAILED;
    }


    d->ocrResult   = QString::fromLocal8Bit(ocrProcess->readAllStandardOutput());

    SaveOcrResult();

    return PROCESS_COMPLETE;
}

void OcrTesseractEngine::SaveOcrResult()
{
    if (d->isSaveTextFile)
    {
        QFileInfo fi(d->inputFile);
        d->outputFile =   fi.absolutePath()  +
                          QLatin1String("/") +
                          (QString::fromLatin1("%1-textconverter.txt").arg(fi.fileName()));

        saveTextFile(d->outputFile, d->ocrResult);
    }

    if (d->isSaveXMP)
    {
        saveXMP(d->inputFile, d->ocrResult);
    }
}

void OcrTesseractEngine::saveTextFile(const QString& filePath, const QString& text)
{
    QFile file(filePath);

    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        stream << text;
        file.close();
    }
}

void OcrTesseractEngine::saveXMP(const QString& filePath, const QString& text)
{
    QScopedPointer<DMetadata> dmeta(new DMetadata(filePath));

    MetaEngine::AltLangMap authorsMap;
    MetaEngine::AltLangMap datesMap;
    MetaEngine::AltLangMap commentsMap;

    Digikam::CaptionsMap commentsSet;

    commentsSet           = dmeta->getItemComments();
    QString   rezAuthor   = commentsSet.value(QLatin1String("x-default")).author;
    QDateTime rezDateTime = commentsSet.value(QLatin1String("x-default")).date;

    commentsMap.insert(QLatin1String("x-default"), text);
    datesMap.insert(QLatin1String("x-default"),    rezDateTime.toString());
    authorsMap.insert(QLatin1String("x-default"),  rezAuthor);

    commentsSet.setData(commentsMap, authorsMap, QString(), datesMap);
    dmeta->setItemComments(commentsSet);

    if (dmeta->applyChanges())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Success in hosting text in XMP";
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Errors in hosting text in XMP";
    }
}

} // namespace DigikamGenericTextConverterPlugin
