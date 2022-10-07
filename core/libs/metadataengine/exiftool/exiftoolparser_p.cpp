/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser - private container.
 *
 * SPDX-FileCopyrightText: 2020-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "exiftoolparser_p.h"

namespace Digikam
{

ExifToolParser::Private::Private(ExifToolParser* const q)
    : pp          (q),
      proc        (nullptr),
      async       (false),
      cmdRunning  (0)
{
    argsFile.setAutoRemove(false);
}

ExifToolParser::Private::~Private()
{
    if (argsFile.exists())
    {
        argsFile.remove();
    }
}

void ExifToolParser::Private::prepareProcess()
{
    currentPath.clear();
    errorString.clear();
    exifToolData.clear();
}

bool ExifToolParser::Private::startProcess(const QByteArrayList& cmdArgs, ExifToolProcess::Action cmdAction)
{
    // Send command to ExifToolProcess

    if (async)
    {
        QMutexLocker locker(&mutex);

        int cmdId = proc->command(cmdArgs, cmdAction);

        if (cmdId == 0)
        {
            qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool cannot be sent:" << actionString(cmdAction);

            return false;
        }

        asyncRunning << cmdId;

        return true;
    }

    cmdRunning = proc->command(cmdArgs, cmdAction);

    if (cmdRunning == 0)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool cannot be sent:" << actionString(cmdAction);

        return false;
    }

    qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool" << actionString(cmdAction) << cmdArgs.join(QByteArray(" "));

    ExifToolProcess::Result result = proc->getExifToolResult(cmdRunning);

    while ((result.cmdRunResult != cmdRunning) && (result.commandState != ExifToolProcess::FINISH_RESULT))
    {
        result = proc->waitForExifToolResult(cmdRunning);

        if ((result.cmdRunResult == cmdRunning) && result.cmdWaitError)
        {
            qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifTool timed out:" << actionString(cmdAction);

            return false;
        }
    }

    jumpToResultCommand(result);

    if (currentPath.isEmpty())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete" << actionString(cmdAction);
    }
    else
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifTool complete" << actionString(cmdAction) << "for" << currentPath;
    }

    return true;
}

QByteArray ExifToolParser::Private::filePathEncoding(const QFileInfo& fi) const
{
    return (QDir::toNativeSeparators(fi.filePath()).toUtf8());
}

void ExifToolParser::Private::jumpToResultCommand(const ExifToolProcess::Result& result)
{
    switch (result.commandState)
    {
        case ExifToolProcess::COMMAND_RESULT:
        {
            pp->cmdCompleted(result.cmdRunResult,
                             result.cmdRunAction,
                             result.elapsedTimer,
                             result.outputBuffer,
                             QByteArray());
            break;
        }

        case ExifToolProcess::ERROR_RESULT:
        {
            pp->errorOccurred(result.cmdRunResult,
                              result.cmdRunAction,
                              proc->exifToolError(),
                              proc->exifToolErrorString());
            break;
        }

        case ExifToolProcess::FINISH_RESULT:
        {
            pp->finished(result.cmdRunResult);
            break;
        }
    }
}

QString ExifToolParser::Private::actionString(int cmdAction) const
{
    switch (cmdAction)
    {
        case ExifToolProcess::LOAD_METADATA:
        {
            return QLatin1String("Load Metadata");
        }

        case ExifToolProcess::LOAD_CHUNKS:
        {
            return QLatin1String("Load Chunks");
        }

        case ExifToolProcess::APPLY_CHANGES:
        {
            return QLatin1String("Apply Changes");
        }

        case ExifToolProcess::APPLY_CHANGES_EXV:
        {
            return QLatin1String("Apply Changes EXV");
        }

        case ExifToolProcess::READ_FORMATS:
        {
            return QLatin1String("Readable Formats");
        }

        case ExifToolProcess::WRITE_FORMATS:
        {
            return QLatin1String("Writable Formats");
        }

        case ExifToolProcess::TRANSLATIONS_LIST:
        {
            return QLatin1String("Translations List");
        }

        case ExifToolProcess::TAGS_DATABASE:
        {
            return QLatin1String("Tags Database");
        }

        case ExifToolProcess::VERSION_STRING:
        {
            return QLatin1String("Version String");
        }

        case ExifToolProcess::COPY_TAGS:
        {
            return QLatin1String("Copy Tags");
        }

        case ExifToolProcess::TRANS_TAGS:
        {
            return QLatin1String("Translate Tags");
        }

        default: // ExifToolProcess::NO_ACTION
        {
            break;
        }
    }

    return QString();
}

} // namespace Digikam
