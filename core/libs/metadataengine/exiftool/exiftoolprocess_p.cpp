/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-18
 * Description : Qt5 and Qt6 interface for exiftool - private container.
 *               Based on ZExifTool Qt interface published at 18 Feb 2021
 *               https://github.com/philvl/ZExifTool
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Philippe Vianney Liaud <philvl dot dev at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "exiftoolprocess_p.h"

namespace Digikam
{

ExifToolProcess::Private::Private(ExifToolProcess* const q)
    : QObject             (q),
      pp                  (q),
      cmdRunning          (0),
      cmdAction           (ExifToolProcess::LOAD_METADATA),
      writeChannelIsClosed(true),
      processError        (QProcess::UnknownError),
      nextCmdId           (CMD_ID_MIN)
{
    outAwait[0] = false;
    outAwait[1] = false;
    outReady[0] = false;
    outReady[1] = false;
}

void ExifToolProcess::Private::slotExecNextCmd()
{
    if ((pp->state() != QProcess::Running) ||
        writeChannelIsClosed)
    {
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::execNextCmd(): ExifTool is not running";
        return;
    }

    QMutexLocker locker(&cmdMutex);

    if (cmdRunning || cmdQueue.isEmpty())
    {
        return;
    }

    // Clear QProcess buffers

    pp->readAllStandardOutput();
    pp->readAllStandardError();

    // Clear internal buffers

    outBuff[0]      = QByteArray();
    outBuff[1]      = QByteArray();
    outAwait[0]     = false;
    outAwait[1]     = false;
    outReady[0]     = false;
    outReady[1]     = false;

    // Exec Command

    execTimer.start();

    runCommand = cmdQueue.takeFirst();
    cmdRunning = runCommand.id;
    cmdAction  = runCommand.ac;

    QTimer::singleShot(1, this, [=]()
        {
            pp->write(runCommand.argsStr);
        }
    );
}

void ExifToolProcess::Private::readOutput(const QProcess::ProcessChannel channel)
{
    if (cmdRunning == 0)
    {
        return;
    }

    pp->setReadChannel(channel);

    while (pp->canReadLine() && !outReady[channel])
    {
        QByteArray line = pp->readLine();

        if (line.endsWith(QByteArray("\r\n")))
        {
            line.remove(line.size() - 2, 1); // Remove '\r' character
        }
/*
        qCDebug(DIGIKAM_METAENGINE_LOG) << channel << line;
*/
        if (!outAwait[channel])
        {
            if (line.startsWith(QByteArray("{await")) && line.endsWith(QByteArray("}\n")))
            {
                outAwait[channel] = line.mid(6, line.size() - 8).toInt();
            }

            continue;
        }

        outBuff[channel] += line;

        if (line.endsWith(QByteArray("{ready}\n")))
        {
            outBuff[channel].chop(8);
            outReady[channel] = true;

            break;
        }
    }

    // Check if outputChannel and errorChannel are both ready

    if (!(outReady[QProcess::StandardOutput] &&
        outReady[QProcess::StandardError]))
    {
/*
        qCWarning(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput(): ExifTool read channels are not ready";
*/
        return;
    }

    if (
        (cmdRunning != outAwait[QProcess::StandardOutput]) ||
        (cmdRunning != outAwait[QProcess::StandardError])
       )
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput: Sync error between CmdID("
                                           << cmdRunning
                                           << "), outChannel("
                                           << outAwait[0]
                                           << ") and errChannel("
                                           << outAwait[1]
                                           << ")";

        setProcessErrorAndEmit(QProcess::ReadError, i18n("Synchronization error between the channels"));
    }
    else
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "ExifToolProcess::readOutput(): ExifTool command completed";

        if (runCommand.parser)
        {
            runCommand.parser->cmdCompleted(cmdRunning,
                                            cmdAction,
                                            execTimer.elapsed(),
                                            outBuff[QProcess::StandardOutput],
                                            outBuff[QProcess::StandardError]);
            runCommand.parser = nullptr;
        }
    }

    cmdRunning = 0;    // No command is running

    slotExecNextCmd(); // Exec next command
}

void ExifToolProcess::Private::setProcessErrorAndEmit(QProcess::ProcessError error, const QString& description)
{
    processError = error;
    errorString  = description;

    if (runCommand.parser)
    {
        runCommand.parser->cmdErrorOccurred(cmdRunning, cmdAction, error, description);
        runCommand.parser = nullptr;
    }
}

} // namespace Digikam
