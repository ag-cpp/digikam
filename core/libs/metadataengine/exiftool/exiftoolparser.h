/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EXIFTOOL_PARSER_H
#define DIGIKAM_EXIFTOOL_PARSER_H

// Qt Core

#include <QFileInfo>
#include <QVariant>
#include <QHash>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QProcess>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class ExifToolProcess;

class DIGIKAM_EXPORT ExifToolParser : public QObject
{
    Q_OBJECT

public:

    /**
     * A map used to store ExifTool data shared with ExifToolProcess class:
     *
     * With load() method, the container is used to get a map of
     * ExifTool tag name as key and tags properties as values:
     * key    = ExifTool Tag name           (QString)
     * values = ExifTool Tag value          (QString).
     *          ExifTool Tag type           (QString).
     *          ExifTool Tag description    (QString).
     *
     * With loadChunk() method, the container is used to get
     * a EXV chunk as value:
     * key   = "EXV"                        (QString).
     * value = the Exiv2 metadata container (QByteArray).
     *
     * With applyChanges() method, the container is used as argument to
     * store tupple of ExifTool tag name as key and Tag value:
     * key   = ExifTool tag name            (QString).
     * value = ExifTool Tag value           (QString).
     *
     * With readableFormats() method, the cointainer is used to get
     * a list of upper-case file format extensions supported by ExifTool for reading.
     * key   = "READ_FORMAT"                (QString).
     * value = list                         (QStringList)
     *
     * With writableFormats() method, the cointainer is used to get
     * a list of upper-case file format extensions supported by ExifTool for writing.
     * key   = "WRITE_FORMAT"               (QString).
     * value = list                         (QStringList).
     */
    typedef QHash<QString, QVariantList> ExifToolData;

public:

    explicit ExifToolParser(QObject* const parent = nullptr);
    ~ExifToolParser();

    /**
     * Load all metadata with ExifTool from a file.
     * Use currentData() to get the ExifTool map.
     */
    bool load(const QString& path);

    /**
     * Load Exif, Iptc, and Xmp chunk as Exiv2 EXV bytearray from a file.
     * Use currentData() to get the container.
     */
    bool loadChunk(const QString& path);

    /**
     * Apply tag changes to a file with ExifTool.
     * Tags can already exists in file or are new ones.
     * To remove a tag, pass an empty string as value.
     */
    bool applyChanges(const QString& path, const ExifToolData& newTags);

    bool readableFormats();
    bool writableFormats();

    QString      currentPath()        const;
    ExifToolData currentData()        const;
    QString      currentErrorString() const;

    /// Public method for unit test purpose about ExifTool stream parsing.
    void setOutputStream(int cmdAction,
                         const QByteArray& cmdOutputChannel,
                         const QByteArray& cmdErrorChannel);

private Q_SLOTS:

    void slotCmdCompleted(int cmdAction,
                          int execTime,
                          const QByteArray& cmdOutputChannel,
                          const QByteArray& cmdErrorChannel);

    void slotErrorOccurred(int cmdAction, QProcess::ProcessError error);

    void slotFinished(int cmdAction, int exitCode, QProcess::ExitStatus exitStatus);

    void slotMetaEngineSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PARSER_H
