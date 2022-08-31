/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-30
 * Description : translate metadata batch tool.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "translate.h"

// Qt includes

#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QScopedPointer>
#include <QEventLoop>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "donlinetranslator.h"
#include "localizesettings.h"
#include "altlangstredit.h"

namespace DigikamBqmTranslatePlugin
{

class Q_DECL_HIDDEN Translate::Private
{

public:

    explicit Private()
      : titleCB       (nullptr),
        captionCB     (nullptr),
        copyrightsCB  (nullptr),
        usageTermsCB  (nullptr),
        trLabel       (nullptr),
        tagsLabel     (nullptr),
        trComboBox    (nullptr),
        changeSettings(true)
    {
    }

    QCheckBox* titleCB;
    QCheckBox* captionCB;
    QCheckBox* copyrightsCB;
    QCheckBox* usageTermsCB;

    QLabel*    trLabel;
    QLabel*    tagsLabel;

    QComboBox* trComboBox;

    bool       changeSettings;
};

Translate::Translate(QObject* const parent)
    : BatchTool(QLatin1String("Translate"), MetadataTool, parent),
      d        (new Private)
{
}

Translate::~Translate()
{
    delete d;
}

BatchTool* Translate::clone(QObject* const parent) const
{
    return new Translate(parent);
}

void Translate::registerSettingsWidget()
{
    QWidget* const panel     = new QWidget;
    QGridLayout* const grid  = new QGridLayout(panel);

    d->tagsLabel             = new QLabel(i18nc("@label", "Tags to Translate:"), panel);
    d->titleCB               = new QCheckBox(i18nc("@title", "Title"),           panel);
    d->captionCB             = new QCheckBox(i18nc("@title", "Caption"),         panel);
    d->copyrightsCB          = new QCheckBox(i18nc("@title", "Copyrights"),      panel);
    d->usageTermsCB          = new QCheckBox(i18nc("@title", "Usage Terms"),     panel);

    d->trLabel              = new QLabel(i18nc("@label", "Translate to:"),      panel);
    d->trComboBox            = new QComboBox(panel);

    slotLocalizeChanged();

    grid->addWidget(d->tagsLabel,    0, 0, 1, 1);
    grid->addWidget(d->titleCB,      1, 0, 1, 1);
    grid->addWidget(d->captionCB,    2, 0, 1, 1);
    grid->addWidget(d->copyrightsCB, 3, 0, 1, 1);
    grid->addWidget(d->usageTermsCB, 4, 0, 1, 1);
    grid->addWidget(d->trLabel,      5, 0, 1, 1);
    grid->addWidget(d->trComboBox,   6, 0, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(7, 10);

    m_settingsWidget = panel;

    connect(d->titleCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->captionCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->copyrightsCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->usageTermsCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->trComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(LocalizeSettings::instance(), SIGNAL(signalSettingsChanged()),
            this, SLOT(slotLocalizeChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings Translate::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("Title"),      false);
    settings.insert(QLatin1String("Caption"),    false);
    settings.insert(QLatin1String("Copyrights"), false);
    settings.insert(QLatin1String("UsageTerms"), false);
    settings.insert(QLatin1String("TrLang"),     QLatin1String("en-US"));

    return settings;
}

void Translate::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    d->titleCB->setChecked(settings()[QLatin1String("Title")].toBool());
    d->captionCB->setChecked(settings()[QLatin1String("Caption")].toBool());
    d->copyrightsCB->setChecked(settings()[QLatin1String("Copyrights")].toBool());
    d->usageTermsCB->setChecked(settings()[QLatin1String("UsageTerms")].toBool());

    QString lang = settings()[QLatin1String("TrLang")].toString();
    d->trComboBox->setCurrentIndex(d->trComboBox->findText(lang));

    d->changeSettings = true;
}

void Translate::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("Title"),       d->titleCB->isChecked());
        settings.insert(QLatin1String("Caption"),     d->captionCB->isChecked());
        settings.insert(QLatin1String("Copyrights"),  d->copyrightsCB->isChecked());
        settings.insert(QLatin1String("UsageTerms"),  d->usageTermsCB->isChecked());
        settings.insert(QLatin1String("trLang"),      d->trComboBox->currentText());

        BatchTool::slotSettingsChanged(settings);
    }
}

bool Translate::toolOperations()
{
    bool ret = true;
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret = QFile::copy(inputUrl().toLocalFile(), outputUrl().toLocalFile());

        if (!ret || !meta->load(outputUrl().toLocalFile()))
        {
            return ret;
        }
    }
    else
    {
        ret = savefromDImg();
        meta->setData(image().getMetadata());
    }

    bool titleStage      = settings()[QLatin1String("Title")].toBool();
    bool captionStage    = settings()[QLatin1String("Caption")].toBool();
    bool copyrightsStage = settings()[QLatin1String("Copyrights")].toBool();
    bool usageTermsStage = settings()[QLatin1String("UsageTerms")].toBool();
    QString trLang       = settings()[QLatin1String("TrLang")].toString();

    if (titleStage)
    {
        ret = insertTranslation(QLatin1String("Xmp.dc.title"), trLang, meta.data());
    }

    if (captionStage)
    {
        ret = insertTranslation(QLatin1String("Xmp.dc.description"), trLang, meta.data());
    }

    if (copyrightsStage)
    {
        ret = insertTranslation(QLatin1String("Xmp.dc.rights"), trLang, meta.data());
    }

    if (usageTermsStage)
    {
        ret = insertTranslation(QLatin1String("Xmp.xmpRights.UsageTerms"), trLang, meta.data());
    }

    if (ret && (titleStage || captionStage || copyrightsStage || usageTermsStage))
    {
        ret = meta->save(outputUrl().toLocalFile());
    }

    return ret;
}

bool Translate::insertTranslation(const QString& tagName, const QString& trLang, DMetadata* const meta) const
{
    bool ret                  = false;
    DMetadata::AltLangMap map = meta->getXmpTagStringListLangAlt(tagName.toLatin1().constData(), false);

    if (!map.isEmpty() && map.contains(QLatin1String("x-default")))
    {
        QString sc = map.value(QLatin1String("x-default"));

        if (!sc.isEmpty())
        {
            QString tr;
            ret = translateString(sc, trLang, tr);

            if (ret)
            {
                map[trLang] = tr;
                meta->setXmpTagStringListLangAlt(tagName.toLatin1().constData(), map);
            }
        }
    }

    return ret;
}

bool Translate::translateString(const QString& text, const QString& trCode, QString& tr) const
{
    QScopedPointer<DOnlineTranslator> trengine(new DOnlineTranslator);
    QScopedPointer<QEventLoop> waitingLoop(new QEventLoop);

    DOnlineTranslator::Language srcLang = DOnlineTranslator::Auto;
    DOnlineTranslator::Language trLang  = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(LocalizeSettings::instance()->settings().translatorEngine, trCode));

    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Request to translate with Web-service:";
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Text to translate        :" << text;
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "To target language       :" << trLang;
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "With source language     :" << srcLang;

    connect(trengine.data(), &DOnlineTranslator::signalFinished,
            waitingLoop.data(), &QEventLoop::quit);

    trengine->translate(text,                                                            // String to translate
                        LocalizeSettings::instance()->settings().translatorEngine,       // Web service
                        trLang,                                                          // Target language
                        srcLang,                                                         // Source langage
                        DOnlineTranslator::Auto);

    waitingLoop->exec(QEventLoop::ExcludeUserInputEvents);

    if (trengine->error() == DOnlineTranslator::NoError)
    {
        tr = trengine->translation();
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Text translated          :" << tr;

        return true;
    }
    else
    {
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Translation Error       :" << trengine->error();
    }

    return false;
}

void Translate::slotLocalizeChanged()
{
    d->trComboBox->setToolTip(i18nc("@info", "Select language to translate with %1",
                              DOnlineTranslator::engineName(LocalizeSettings::instance()->settings().translatorEngine)));

    d->trComboBox->clear();

    QStringList allRFC3066  = DOnlineTranslator::supportedRFC3066(LocalizeSettings::instance()->settings().translatorEngine);
    LocalizeContainer set   = LocalizeSettings::instance()->settings();

    Q_FOREACH (const QString& lg, set.translatorLang)
    {
        d->trComboBox->addItem(lg);
        d->trComboBox->setItemData(d->trComboBox->findText(lg), i18nc("@info", "Translate to %1", AltLangStrEdit::languageNameRFC3066(lg)), Qt::ToolTipRole);
    }
}

} // namespace DigikamBqmTranslatePlugin
