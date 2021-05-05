/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : image editor plugin to print an image
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "printplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "imageiface.h"
#include "editorwindow.h"
#include "printhelper.h"

namespace DigikamEditorPrintToolPlugin
{

PrintToolPlugin::PrintToolPlugin(QObject* const parent)
    : DPluginEditor(parent)
{
}

PrintToolPlugin::~PrintToolPlugin()
{
}

QString PrintToolPlugin::name() const
{
    return i18nc("@title", "Print Image");
}

QString PrintToolPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon PrintToolPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("document-print-frame"));
}

QString PrintToolPlugin::description() const
{
    return i18nc("@info", "A tool to print an image");
}

QString PrintToolPlugin::details() const
{
    return i18nc("@info", "This Image Editor tool can print an image.");
}

QList<DPluginAuthor> PrintToolPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Angelo Naselli"),
                             QString::fromUtf8("anaselli at linux dot it"),
                             QString::fromUtf8("(C) 2009"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2009-2021"))
            ;
}

void PrintToolPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Print Image..."));
    ac->setObjectName(QLatin1String("editorwindow_print"));
    ac->setShortcut(Qt::CTRL + Qt::Key_P);
    ac->setActionCategory(DPluginAction::EditorFile);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotPrint()));

    addAction(ac);
}

void PrintToolPlugin::slotPrint()
{
    EditorWindow* const editor = dynamic_cast<EditorWindow*>(sender()->parent());

    if (editor)
    {
        ImageIface iface;
        DImg* const image = iface.original();

        if (!image || image->isNull())
        {
            return;
        }

        PrintHelper printHelp(editor);
        printHelp.print(*image);
    }
}

} // namespace DigikamEditorPrintToolPlugin
