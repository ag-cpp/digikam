/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-18
 * Description : Qt5 and Qt6 interface for exiftool.
 *               Based on ZExifTool Qt interface published at 18 Feb 2021
 *               https://github.com/philvl/ZExifTool
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2021 by Philippe Vianney Liaud <philvl dot dev at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_EXIFTOOL_PROCESS_H
#define DIGIKAM_EXIFTOOL_PROCESS_H

// Qt includes

#include <QProcess>
#include <QPointer>
#include <QString>
#include <QMutex>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExifToolProcess : public QProcess
{
    Q_OBJECT

public:

    /**
     * ExifTool actions to process.
     */
    enum Action
    {
        LOAD_METADATA       = 0,                    ///< Load all metadata from a file with ExifTool.
        LOAD_CHUNKS,                                ///< Load Exif, Iptc, and Xmp chunks from a file as byte-array for MetaEngine.
        APPLY_CHANGES,                              ///< Apply tag changes in a file with ExifTool.
        APPLY_CHANGES_EXV,                          ///< Apply tag changes in a file with ExifTool using an EXV container.
        READ_FORMATS,                               ///< Return the list of readable ExifTool file formats.
        WRITE_FORMATS,                              ///< Return the list of writable ExifTool file formats.
        TRANSLATIONS_LIST,                          ///< List of ExifTool languages available for translations.
        TAGS_DATABASE,                              ///< List of ExifTool tags from database.
        VERSION_STRING,                             ///< Return the ExifTool version as string.
        COPY_TAGS,                                  ///< Copy tags from one file to another one. See CopyTagsSource enum for details.
        TRANS_TAGS,                                 ///< Translate tags in file. See TranslateTagsOps enum for details.
        NO_ACTION                                   ///< Last value from this list. Do nothing.
    };

    /**
     * Possible copying tags operations to OR combine with COPY_TAGS action.
     */
    enum CopyTagsSource
    {
        COPY_EXIF           = 0x01,                 ///< Copy all Exif Tags from source file.
        COPY_MAKERNOTES     = 0x02,                 ///< Copy all Makernotes tags from source file.
        COPY_IPTC           = 0x04,                 ///< Copy all Iptc tags from source file.
        COPY_XMP            = 0x08,                 ///< Copy all Xmp tags from source file.
        COPY_ICC            = 0x10,                 ///< Copy ICC profile from source file.
        COPY_ALL            = 0x20,                 ///< Copy all tags from source file.
        COPY_NONE           = 0x40                  ///< No copy operation.
    };

    /**
     * Possible writing tags mode to OR combine with COPY_TAGS action.
     */
    enum WritingTagsMode
    {
        WRITE_EXISTING_TAGS = 0x01,                 ///< Overwrite existing tags.
        CREATE_NEW_TAGS     = 0x02,                 ///< Create new tags.
        CREATE_NEW_GROUPS   = 0x04,                 ///< Create new groups if necessary.
        ALL_MODES           = WRITE_EXISTING_TAGS |
                              CREATE_NEW_TAGS     |
                              CREATE_NEW_GROUPS
    };

    /**
     * Possible translating tags operations to OR combine with COPY_TAGS action.
     */
    enum TranslateTagsOps
    {
        TRANS_ALL_XMP       = 0x01,                 ///< Translate all existing Tags from source file to Xmp.
        TRANS_ALL_IPTC      = 0x02,                 ///< Translate all existing Tags from source file to Iptc.
        TRANS_ALL_EXIF      = 0x04                  ///< Translate all existing Tags from source file to Exif.
    };

public:

    /**
     * Constructs a ExifToolProcess.
     */
    explicit ExifToolProcess();

    /**
     * Destructs the ExifToolProcess object, i.e., killing the process.
     * Note that this function will not return until the process is terminated.
     */
    ~ExifToolProcess();

    /**
     * @brief internalPtr - singleton implementation
     */
    static QPointer<ExifToolProcess> internalPtr;
    static ExifToolProcess*          instance();
    static bool                      isCreated();

    void setupConnections();

public:

    /**
     * Setup the ExifTool configuration. This function must be called before start().
     */
    void setProgram(const QString& etExePath,
                    const QString& perlExePath = QString());

    QString program() const;

    bool checkExifToolProgram();

public:

    /**
     * Returns true if ExifToolProcess is running (process state == Running)
     */
    bool                   isRunning()                  const;

    /**
     * Returns true if a command is running
     */
    bool                   isBusy()                     const;

    /**
     * Returns the type of error that occurred last.
     */
    QProcess::ProcessError error()                      const;

    /**
     * Returns an error message.
     */
    QString                errorString()                const;

    /**
     * Send a command to exiftool process
     * Return 0: ExitTool not running, write channel is closed or args is empty
     */
    int command(const QByteArrayList& args, Action ac);

Q_SIGNALS:

    void signalExecNextCmd();

    void signalStartExifTool();

    void signalStarted(int cmdId,
                       int cmdAction);

    void signalStateChanged(int cmdId,
                            int cmdAction,
                            QProcess::ProcessState newState);

    void signalErrorOccurred(int cmdId,
                             int cmdAction,
                             QProcess::ProcessError error);

    void signalFinished(int cmdId,
                        int cmdAction,
                        int exitCode,
                        QProcess::ExitStatus exitStatus);

    void signalCmdCompleted(int cmdId,
                            int cmdAction,
                            int execTime,
                            const QByteArray& cmdOutputChannel,
                            const QByteArray& cmdErrorChannel);

public Q_SLOTS:

    void slotMetaEngineSettingsChanged();

private:

    /**
     * Starts exiftool in a new process.
     */
    bool startExifTool();

    /**
     * Attempts to terminate the process.
     */
    void terminateExifTool();

    /**
     * Kills the current process, causing it to exit immediately.
     * On Windows, kill() uses TerminateProcess, and on Unix and macOS,
     * the SIGKILL signal is sent to the process.
     */
    void killExifTool();

    QString exifToolBin()                               const;

private Q_SLOTS:

    void slotStarted();
    void slotExecNextCmd();
    void slotStateChanged(QProcess::ProcessState newState);
    void slotErrorOccurred(QProcess::ProcessError error);
    void slotReadyReadStandardOutput();
    void slotReadyReadStandardError();
    void slotFinished(int exitCode,
                      QProcess::ExitStatus exitStatus);

private:

    // Disable
    explicit ExifToolProcess(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PROCESS_H
