/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-18
 * Description : DarkTable raw import plugin.
 *
 * Copyright (C) 2019      by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Lua script inspired from Darktable Gimp plugin
 * Copyright (C) 2015-2017 by Tobias Ellinghaus
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "rawimportdarktableplugin.h"

// Qt includes

#include <QMessageBox>
#include <QApplication>
#include <QPointer>
#include <QDebug>
#include <QByteArray>
#include <QTextStream>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace DigikamRawImportDarkTablePlugin
{

const QString s_luaScriptData = QLatin1String("\n"                                                      \
"local dt = require \"darktable\"\n"                                                                    \
"\n"                                                                                                    \
"local min_api_version = \"2.1.0\"\n"                                                                   \
"if dt.configuration.api_version_string < min_api_version then\n"                                       \
"  dt.print(\"the exit export script requires at least darktable version 1.7.0\")\n"                    \
"  dt.print_error(\"the exit export script requires at least darktable version 1.7.0\")\n"              \
"  return\n"                                                                                            \
"else\n"                                                                                                \
"  dt.print(\"closing darktable will export the image and make image editor load it\")\n"               \
"end\n"                                                                                                 \
"\n"                                                                                                    \
"local export_filename = dt.preferences.read(\"export_on_exit\", \"export_filename\", \"string\")\n"    \
"\n"                                                                                                    \
"dt.register_event(\"exit\", function()\n"                                                              \
"  -- safegurad against someone using this with their library containing 50k images\n"                  \
"  if #dt.database > 1 then\n"                                                                          \
"    dt.print_error(\"too many images, only exporting the first\")\n"                                   \
"  -- return\n"                                                                                         \
"  end\n"                                                                                               \
"\n"                                                                                                    \
"  -- change the view first to force writing of the history stack\n"                                    \
"  dt.gui.current_view(dt.gui.views.lighttable)\n"                                                      \
"  -- now export\n"                                                                                     \
"  local format = dt.new_format(\"png\")\n"                                                             \
"  format.max_width = 0\n"                                                                              \
"  format.max_height = 0\n"                                                                             \
"  -- lets have the export in a loop so we could easily support > 1 images\n"                           \
"  for _, image in ipairs(dt.database) do\n"                                                            \
"    dt.print_error(\"exporting `\"..tostring(image)..\"' to `\"..export_filename..\"'\")\n"            \
"    format:write_image(image, export_filename)\n"                                                      \
"    break -- only export one image. see above for the reason\n"                                        \
"  end\n"                                                                                               \
"end)\n"                                                                                                \
"");

DarkTableRawImportPlugin::DarkTableRawImportPlugin(QObject* const parent)
    : DPluginRawImport(parent),
      m_darktable(nullptr),
      m_tempFile(nullptr)
{
    m_luaFile.open();
    QTextStream stream(&m_luaFile);
    stream << s_luaScriptData;
    stream.flush();
}

DarkTableRawImportPlugin::~DarkTableRawImportPlugin()
{
}

QString DarkTableRawImportPlugin::name() const
{
    return QString::fromUtf8("Raw Import using DarkTable");
}

QString DarkTableRawImportPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DarkTableRawImportPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString DarkTableRawImportPlugin::description() const
{
    return QString::fromUtf8("A RAW import plugin based on DarkTable");
}

QString DarkTableRawImportPlugin::details() const
{
    return QString::fromUtf8("<p>This RAW Import plugin use DarkTable tool to pre-process file in Image Editor.</p>");
}

QList<DPluginAuthor> DarkTableRawImportPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void DarkTableRawImportPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

bool DarkTableRawImportPlugin::run(const QString& filePath, const DRawDecoding& /*def*/)
{
    m_fileInfo = QFileInfo(filePath);
    m_props    = LoadingDescription(m_fileInfo.filePath(), LoadingDescription::ConvertForEditor);
    m_decoded  = DImg();

    delete m_tempFile;

    m_tempFile = new QTemporaryFile();
    m_tempFile->open();

    m_darktable    = new QProcess(this);
    m_darktable->setProcessChannelMode(QProcess::MergedChannels);
    m_darktable->setWorkingDirectory(m_fileInfo.path());

    connect(m_darktable, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(slotErrorOccurred(QProcess::ProcessError)));

    connect(m_darktable, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)));

    connect(m_darktable, SIGNAL(readyRead()),
            this, SLOT(slotProcessReadyRead()));

    // --------

    m_fileInfo = QFileInfo(filePath);

    m_darktable->setProgram(QLatin1String("darktable"));
    m_darktable->setArguments(QStringList() << QLatin1String("--library")
                                            << QLatin1String(":memory:")                                    // Run DarkTable to process only one file
                                            << QLatin1String("--luacmd")
                                            << QString::fromUtf8("dofile('%1')")
                                               .arg(m_luaFile.fileName())                                   // LUA script to run in DarkTable
                                            << QLatin1String("--conf")
                                            << QLatin1String("plugins/lighttable/export/icctype=3")         // Output color-space
                                            << QLatin1String("--conf")
                                            << QString::fromUtf8("lua/export_on_exit/export_filename=%1")
                                               .arg(m_tempFile->fileName())                                 // Ouput file
                                            << filePath);                                                   // Input file

    qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable arguments:" << m_darktable->arguments();

    m_darktable->start();

    return true;
}

void DarkTableRawImportPlugin::slotErrorOccurred(QProcess::ProcessError error)
{
    switch (error)
    {
        case QProcess::FailedToStart:
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process has failed to start";
            break;
        case QProcess::Crashed:
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process has crashed";
            break;
        case QProcess::Timedout:
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process time-out";
            break;
        case QProcess::WriteError:
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process write error";
            break;
        case QProcess::ReadError:
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process read error";
            break;
        default:
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process error unknown";
            break;
    }
}

void DarkTableRawImportPlugin::slotProcessFinished(int code, QProcess::ExitStatus status)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: return code:" << code << ":: Exit status:" << status;

    m_decoded = DImg(m_tempFile->fileName());

    if (m_decoded.isNull())
    {
        QString message = i18n("Error to import RAW image with DarkTable\nClose this dialog to load RAW image with native import tool");
        QMessageBox::information(0, qApp->applicationName(), message);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is null! Load with Native tool...";
        qCDebug(DIGIKAM_GENERAL_LOG) << m_props.filePath;
        emit signalLoadRaw(m_props);
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is not null...";
        qCDebug(DIGIKAM_GENERAL_LOG) << m_props.filePath;
        m_props = LoadingDescription(m_tempFile->fileName(), LoadingDescription::ConvertForEditor);
        emit signalDecodedImage(m_props, m_decoded);
    }

    delete m_tempFile;
    m_tempFile = nullptr;
}

void DarkTableRawImportPlugin::slotProcessReadyRead()
{
    QByteArray data   = m_darktable->readAllStandardError();
    QStringList lines = QString::fromUtf8(data).split(QLatin1Char('\n'), QString::SkipEmptyParts);

    foreach (const QString& one, lines)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable ::" << one;
    }
}

} // namespace DigikamRawImportDarkTablePlugin
