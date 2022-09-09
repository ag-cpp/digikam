/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR settings widgets
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "textconvertersettings.h"

// Qt includes

#include <QGridLayout>
#include <QCheckBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dcombobox.h"
#include "dprogresswdg.h"
#include "dexpanderbox.h"
#include "dnuminput.h"
#include "textconverterlist.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class Q_DECL_HIDDEN TextConverterSettings::Private
{
public:

    explicit Private()
      : ocrTesseractLanguageMode   (nullptr),
        ocrTesseractPSMMode        (nullptr),
        ocrTesseractOEMMode        (nullptr),
        ocrTesseractDpi            (nullptr),
        saveTextFile               (nullptr),
        saveXMP                    (nullptr)
    {
    }

    // Tesseract options

    DComboBox*          ocrTesseractLanguageMode;

    DComboBox*          ocrTesseractPSMMode;

    DComboBox*          ocrTesseractOEMMode;

    DIntNumInput*       ocrTesseractDpi;

    QCheckBox*          saveTextFile;

    QCheckBox*          saveXMP;
};

TextConverterSettings::TextConverterSettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QLabel* const ocrTesseractLanguageLabel   = new QLabel(i18nc("@label", "Languages:"));
    d->ocrTesseractLanguageMode               = new DComboBox(this);

    d->ocrTesseractLanguageMode->insertItem(int(OcrOptions::Languages::DEFAULT),
                                            i18nc("@option:default Tesseract mode", "Default"));

    d->ocrTesseractLanguageMode->setDefaultIndex(int(OcrOptions::Languages::DEFAULT));
    d->ocrTesseractLanguageMode->setToolTip(i18nc("@info", "Specify language(s) used for OCR. "
                                                           "In the Default mode of Language settings for digital text with multiple languages, \n"
                                                           "Tesseract can automatically recognize languages using Latin alphabets such as English or French, \n"
                                                           "but is not compatible with languages using hieroglyphs such as Chinese, Japanese.\n"
                                                           "OSD mode use the Orientation and Script detection Module."));

    // ------------

    QLabel* const ocrTesseractPSMLabel  = new QLabel(i18nc("@label", "Segmentation mode:"));
    d->ocrTesseractPSMMode              = new DComboBox(this);

    QMap<OcrOptions::PageSegmentationModes, QPair<QString, QString> >                psmMap = OcrOptions::psmNames();
    QMap<OcrOptions::PageSegmentationModes, QPair<QString, QString> >::const_iterator it1   = psmMap.constBegin();

    while (it1 != psmMap.constEnd())
    {
        d->ocrTesseractPSMMode->addItem(it1.value().first, (int)it1.key());
        d->ocrTesseractPSMMode->combo()->setItemData((int)it1.key(), it1.value().second, Qt::ToolTipRole);
        ++it1;
    }

    d->ocrTesseractPSMMode->setDefaultIndex(int(OcrOptions::PageSegmentationModes::DEFAULT));
    d->ocrTesseractPSMMode->setToolTip(i18nc("@info", "Specify page segmentation mode."));

    // ------------

    QLabel* const ocrTesseractOEMLabel  = new QLabel(i18nc("@label", "Engine mode:"));
    d->ocrTesseractOEMMode              = new DComboBox(this);

    QMap<OcrOptions::EngineModes, QPair<QString, QString> >                oemMap  = OcrOptions::oemNames();
    QMap<OcrOptions::EngineModes, QPair<QString, QString> >::const_iterator it2    = oemMap.constBegin();

    while (it2 !=  oemMap.constEnd())
    {
        d->ocrTesseractOEMMode->addItem(it2.value().first, (int)it2.key());
        d->ocrTesseractOEMMode->combo()->setItemData((int)it2.key(), it2.value().second, Qt::ToolTipRole);
        ++it2;
    }

    d->ocrTesseractOEMMode->setDefaultIndex(int(OcrOptions::EngineModes::DEFAULT));
    d->ocrTesseractOEMMode->setToolTip(i18nc("@info", "Specify OCR engine mode."));

    // ------------

    QLabel* const ocrTesseractDpiLabel  = new QLabel(i18nc("@label", "Resolution Dpi:"));
    d->ocrTesseractDpi                  = new DIntNumInput(this);
    d->ocrTesseractDpi->setRange(70, 2400, 1);
    d->ocrTesseractDpi->setToolTip(i18nc("@info", "Specify DPI for input image."));
    d->ocrTesseractDpi->setDefaultValue(300);
    ocrTesseractDpiLabel->setBuddy(d->ocrTesseractDpi);

    // ------------

    QLabel* const saveOcrResultLabel = new QLabel(i18nc("@label", "Store result in : "));
    d->saveTextFile = new QCheckBox(i18nc("@option:check", "Text file"), this);
    d->saveTextFile->setToolTip(i18nc("@info", "Store OCR result in separated text file"));
    d->saveTextFile->setChecked(true);

    d->saveXMP = new QCheckBox(i18nc("@option:check", "Metadata"), this);
    d->saveXMP->setToolTip(i18nc("@info", "Store OCR result in XMP metadata"));
    d->saveXMP->setChecked(true);

    // ------------

    QGridLayout* const settingsBoxLayout = new QGridLayout(this);
    settingsBoxLayout->addWidget(ocrTesseractLanguageLabel,        0, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractLanguageMode,      0, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractPSMLabel,             1, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractPSMMode,           1, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractOEMLabel,             2, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractOEMMode,           2, 1, 1, 1);
    settingsBoxLayout->addWidget(ocrTesseractDpiLabel,             3, 0, 1, 1);
    settingsBoxLayout->addWidget(d->ocrTesseractDpi,               3, 1, 1, 1);
    settingsBoxLayout->addWidget(saveOcrResultLabel,               4, 0, 1, 1);
    settingsBoxLayout->addWidget(d->saveTextFile,                  5, 0, 1, 1);
    settingsBoxLayout->addWidget(d->saveXMP,                       5, 1, 1, 1);


    settingsBoxLayout->setRowStretch(9, 10);
    settingsBoxLayout->setContentsMargins(QMargins());

    // ------------------------------------------------------------------------

    connect(d->ocrTesseractLanguageMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->ocrTesseractPSMMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->ocrTesseractOEMMode, SIGNAL(activated(int)),
            this, SIGNAL(signalSettingsChanged()));
}

TextConverterSettings::~TextConverterSettings()
{
    delete d;
}

void TextConverterSettings::setDefaultSettings()
{
    d->ocrTesseractLanguageMode->slotReset();
    d->ocrTesseractOEMMode->slotReset();
    d->ocrTesseractPSMMode->slotReset();
    d->ocrTesseractDpi->slotReset();
    d->saveTextFile->setChecked(true);
}

void TextConverterSettings::setOcrOptions(const OcrOptions& opt)
{
    QString lg = opt.language;

    if (lg.isEmpty())
    {
        d->ocrTesseractLanguageMode->setCurrentIndex((int)OcrOptions::Languages::DEFAULT);
    }
    else
    {
        d->ocrTesseractLanguageMode->combo()->setCurrentText(lg);
    }

    d->ocrTesseractPSMMode->setCurrentIndex(opt.psm);
    d->ocrTesseractOEMMode->setCurrentIndex(opt.oem);
    d->ocrTesseractDpi->setValue(opt.dpi);
    d->saveTextFile->setChecked(opt.isSaveTextFile);
    d->saveXMP->setChecked(opt.isSaveXMP);
}

OcrOptions TextConverterSettings::ocrOptions() const
{
    OcrOptions opt;

    if (d->ocrTesseractLanguageMode->currentIndex() == (int)OcrOptions::Languages::DEFAULT)
    {
        opt.language = QString();
    }
    else
    {
        opt.language = d->ocrTesseractLanguageMode->combo()->currentText();
    }

    opt.psm            = d->ocrTesseractPSMMode->currentIndex();
    opt.oem            = d->ocrTesseractOEMMode->currentIndex();
    opt.dpi            = d->ocrTesseractDpi->value();
    opt.isSaveTextFile = d->saveTextFile->isChecked();
    opt.isSaveXMP      = d->saveXMP->isChecked();

    return opt;
}

void TextConverterSettings::populateLanguagesMode(const QStringList& langs)
{
    Q_FOREACH (const QString& lg, langs)
    {
        d->ocrTesseractLanguageMode->addItem(lg);
    }
}

} // namespace DigikamGenericTextConverterPlugin
