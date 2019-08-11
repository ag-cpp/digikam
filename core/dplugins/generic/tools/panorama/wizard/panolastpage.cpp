/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a tool to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending tool
 *
 * Copyright (C) 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "panolastpage.h"

// Qt includes

#include <QUrl>
#include <QFile>
#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QStandardPaths>
#include <QLineEdit>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <kconfig.h>

// Local includes

#include "digikam_debug.h"
#include "panomanager.h"
#include "panoactionthread.h"
#include "dlayoutbox.h"

namespace DigikamGenericPanoramaPlugin
{

class Q_DECL_HIDDEN PanoLastPage::Private
{
public:

    explicit Private()
      : copyDone(false),
        title(nullptr),
        saveSettingsGroupBox(nullptr),
        fileTemplateQLineEdit(nullptr),
        savePtoCheckBox(nullptr),
        warningLabel(nullptr),
        errorLabel(nullptr),
        mngr(nullptr)
    {
    }

    bool         copyDone;

    QLabel*      title;

    QGroupBox*   saveSettingsGroupBox;
    QLineEdit*   fileTemplateQLineEdit;
    QCheckBox*   savePtoCheckBox;
    QLabel*      warningLabel;
    QLabel*      errorLabel;

    PanoManager* mngr;
};

PanoLastPage::PanoLastPage(PanoManager* const mngr, QWizard* const dlg)
     : DWizardPage(dlg, i18nc("@title:window", "<b>Panorama Stitched</b>")),
       d(new Private)
{
    KConfig config;
    KConfigGroup group        = config.group("Panorama Settings");

    d->mngr                   = mngr;

    DVBox* const vbox         = new DVBox(this);

    d->title                  = new QLabel(vbox);
    d->title->setOpenExternalLinks(true);
    d->title->setWordWrap(true);

    QVBoxLayout* const formatVBox = new QVBoxLayout();

    d->saveSettingsGroupBox   = new QGroupBox(i18nc("@title:group", "Save Settings"), vbox);
    d->saveSettingsGroupBox->setLayout(formatVBox);
    formatVBox->addStretch(1);

    QLabel* const fileTemplateLabel = new QLabel(i18nc("@label:textbox", "File name template:"), d->saveSettingsGroupBox);
    formatVBox->addWidget(fileTemplateLabel);

    d->fileTemplateQLineEdit  = new QLineEdit(QLatin1String("panorama"), d->saveSettingsGroupBox);
    d->fileTemplateQLineEdit->setToolTip(i18nc("@info:tooltip", "Name of the panorama file (without its extension)."));
    d->fileTemplateQLineEdit->setWhatsThis(i18nc("@info:whatsthis", "<b>File name template</b>: Set here the base name of the files that "
                                                "will be saved. For example, if your template is <i>panorama</i> and if "
                                                "you chose a JPEG output, then your panorama will be saved with the "
                                                "name <i>panorama.jpg</i>. If you choose to save also the project file, "
                                                "it will have the name <i>panorama.pto</i>."));
    formatVBox->addWidget(d->fileTemplateQLineEdit);

    d->savePtoCheckBox        = new QCheckBox(i18nc("@option:check", "Save project file"), d->saveSettingsGroupBox);
    d->savePtoCheckBox->setChecked(group.readEntry("Save PTO", false));
    d->savePtoCheckBox->setToolTip(i18nc("@info:tooltip", "Save the project file for further processing within Hugin GUI."));
    d->savePtoCheckBox->setWhatsThis(i18nc("@info:whatsthis", "<b>Save project file</b>: You can keep the project file generated to stitch "
                                          "your panorama for further tweaking within "
                                          "<a href=\"http://hugin.sourceforge.net/\">Hugin</a> by checking this. "
                                          "This is useful if you want a different projection, modify the horizon or "
                                          "the center of the panorama, or modify the control points to get better results."));
    formatVBox->addWidget(d->savePtoCheckBox);

    d->warningLabel = new QLabel(d->saveSettingsGroupBox);
    d->warningLabel->hide();
    formatVBox->addWidget(d->warningLabel);

    d->errorLabel = new QLabel(d->saveSettingsGroupBox);
    d->errorLabel->hide();
    formatVBox->addWidget(d->errorLabel);

    vbox->setStretchFactor(new QWidget(vbox), 2);

    setPageWidget(vbox);

    QPixmap leftPix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/assistant-hugin.png")));
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->fileTemplateQLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotTemplateChanged(QString)));

    connect(d->savePtoCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotPtoCheckBoxChanged(int)));
}

PanoLastPage::~PanoLastPage()
{
    KConfig config;
    KConfigGroup group = config.group("Panorama Settings");
    group.writeEntry("Save PTO", d->savePtoCheckBox->isChecked());
    config.sync();

    delete d;
}

void PanoLastPage::copyFiles()
{
    connect(d->mngr->thread(), SIGNAL(jobCollectionFinished(DigikamGenericPanoramaPlugin::PanoActionData)),
            this, SLOT(slotPanoAction(DigikamGenericPanoramaPlugin::PanoActionData)));

    QUrl panoUrl = d->mngr->preProcessedMap().begin().key().adjusted(QUrl::RemoveFilename);
    panoUrl.setPath(panoUrl.path() + panoFileName(d->fileTemplateQLineEdit->text()));

    d->mngr->thread()->copyFiles(d->mngr->panoPtoUrl(),
                                 d->mngr->panoUrl(),
                                 panoUrl,
                                 d->mngr->preProcessedMap(),
                                 d->savePtoCheckBox->isChecked(),
                                 d->mngr->gPano()
                                );

    emit d->mngr->updateHostApp(panoUrl);
}

QString PanoLastPage::panoFileName(const QString& fileTemplate) const
{
    switch (d->mngr->format())
    {
        default:
        case JPEG:
            return fileTemplate + QLatin1String(".jpg");
        case TIFF:
            return fileTemplate + QLatin1String(".tif");
    }
}

void PanoLastPage::checkFiles()
{
    QString dir = d->mngr->preProcessedMap().begin().key().toString(QUrl::RemoveFilename);
    QUrl panoUrl(dir + panoFileName(d->fileTemplateQLineEdit->text()));
    QUrl ptoUrl(dir + d->fileTemplateQLineEdit->text() + QLatin1String(".pto"));
    QFile panoFile(panoUrl.toString(QUrl::PreferLocalFile));
    QFile ptoFile(ptoUrl.toString(QUrl::PreferLocalFile));

    bool rawsOk = true;

    if (d->savePtoCheckBox->isChecked())
    {
        for (auto& input : d->mngr->preProcessedMap().keys())
        {
            if (input != d->mngr->preProcessedMap()[input].preprocessedUrl)
            {
                QString dir2 = input.toString(QUrl::RemoveFilename);
                QUrl derawUrl(dir2 + d->mngr->preProcessedMap()[input].preprocessedUrl.fileName());
                QFile derawFile(derawUrl.toString(QUrl::PreferLocalFile));
                rawsOk &= !derawFile.exists();
            }
        }
    }

    if (panoFile.exists() || (d->savePtoCheckBox->isChecked() && ptoFile.exists()))
    {
        setComplete(false);
        emit completeChanged();
        d->warningLabel->setText(i18n("<qt><p><font color=\"red\"><b>Warning:</b> "
                                      "This file already exists.</font></p></qt>"));
        d->warningLabel->show();
    }
    else if (!rawsOk)
    {
        setComplete(true);
        emit completeChanged();
        d->warningLabel->setText(i18n("<qt><p><font color=\"orange\"><b>Warning:</b> "
                                      "One or more converted raw files already exists (they will be skipped during the copying process).</font></p></qt>"));
        d->warningLabel->show();
    }
    else
    {
        setComplete(true);
        emit completeChanged();
        d->warningLabel->hide();
    }
}

void PanoLastPage::initializePage()
{
    QString first = d->mngr->itemsList().first().fileName();
    QString last  = d->mngr->itemsList().last().fileName();
    QString file  = QString::fromLatin1("%1-%2")
        .arg(first.left(first.lastIndexOf(QLatin1Char('.'))))
        .arg(last.left(last.lastIndexOf(QLatin1Char('.'))));
    d->fileTemplateQLineEdit->setText(file);

    checkFiles();
}

bool PanoLastPage::validatePage()
{
    if (d->copyDone)
        return true;

    setComplete(false);
    copyFiles();

    return false;
}

void PanoLastPage::slotTemplateChanged(const QString&)
{
    d->title->setText(i18n("<qt>"
                           "<p><h1><b>Panorama Stitching is Done</b></h1></p>"
                           "<p>Congratulations. Your images are stitched into a panorama.</p>"
                           "<p>Your panorama will be created to the directory:<br />"
                           "<b>%1</b><br />"
                           "once you press the <i>Finish</i> button, with the name set below.</p>"
                           "<p>If you choose to save the project file, and "
                           "if your images were raw images then the converted images used during "
                           "the stitching process will be copied at the same time (those are "
                           "TIFF files that can be big).</p>"
                           "</qt>",
                           QDir::toNativeSeparators(d->mngr->preProcessedMap().begin().key().toString(QUrl::RemoveFilename | QUrl::PreferLocalFile))
                          ));
    checkFiles();
}

void PanoLastPage::slotPtoCheckBoxChanged(int)
{
    checkFiles();
}

void PanoLastPage::slotPanoAction(const DigikamGenericPanoramaPlugin::PanoActionData& ad)
{
    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "SlotPanoAction (lastPage)";
    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "starting, success, action:" << ad.starting << ad.success << ad.action;

    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case PANO_COPY:
                {
                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(DigikamGenericPanoramaPlugin::PanoActionData)),
                               this, SLOT(slotPanoAction(DigikamGenericPanoramaPlugin::PanoActionData)));

                    d->errorLabel->setText(i18n("<qt><p><font color=\"red\"><b>Error:</b> "
                                                 "%1</font></p></qt>", ad.message));
                    d->errorLabel->show();
                    break;
                }
                default:
                {
                    qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Unknown action (last) " << ad.action;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case PANO_COPY:
                {
                    disconnect(d->mngr->thread(), SIGNAL(jobCollectionFinished(DigikamGenericPanoramaPlugin::PanoActionData)),
                               this, SLOT(slotPanoAction(DigikamGenericPanoramaPlugin::PanoActionData)));

                    d->copyDone = true;
                    emit signalCopyFinished();
                    break;
                }
                default:
                {
                    qCWarning(DIGIKAM_DPLUGIN_GENERIC_LOG) << "Unknown action (last) " << ad.action;
                    break;
                }
            }
        }
    }
}

} // namespace DigikamGenericPanoramaPlugin
