/* ============================================================
 *
 * Date        : 2008-02-10
 * Description : a tool to fix automatically camera lens aberrations
 *
 * SPDX-FileCopyrightText: 2008      by Adrian Schroeter <adrian at suse dot de>
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "lensfunsettings.h"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN LensFunSettings::Private
{
public:

    Private() = default;

    const QString configCCAEntry        = QLatin1String("CCA");
    const QString configVignettingEntry = QLatin1String("Vignetting");
    const QString configDistortionEntry = QLatin1String("Distortion");
    const QString configGeometryEntry   = QLatin1String("Geometry");

    QCheckBox*           filterCCA      = nullptr;
    QCheckBox*           filterVIG      = nullptr;
    QCheckBox*           filterDST      = nullptr;
    QCheckBox*           filterGEO      = nullptr;
};

// --------------------------------------------------------

LensFunSettings::LensFunSettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing = layoutSpacing();


    QGridLayout* const grid = new QGridLayout(this);

    QLabel* const title     = new QLabel(i18n("Lens Corrections to Apply:"));
    d->filterCCA            = new QCheckBox(i18n("Chromatic Aberration"));
    d->filterCCA->setWhatsThis(i18n("Chromatic aberration is easily recognized as color fringes "
                                    "towards the image corners. CA is due to a varying lens focus "
                                    "for different colors."));
    d->filterVIG  = new QCheckBox(i18n("Vignetting"));
    d->filterVIG->setWhatsThis(i18n("Vignetting refers to an image darkening, mostly in the corners. "
                                    "Optical and natural vignetting can be canceled out with this option, "
                                    "whereas mechanical vignetting will not be cured."));
    d->filterDST = new QCheckBox(i18n("Distortion"));
    d->filterDST->setWhatsThis(i18n("Distortion refers to an image deformation, which is most pronounced "
                                    "towards the corners. These Seidel aberrations are known as pincushion "
                                    "and barrel distortions."));
    d->filterGEO = new QCheckBox(i18n("Geometry"));
    d->filterGEO->setWhatsThis(i18n("Four geometries are handled here: Rectilinear (99 percent of all lenses), "
                                    "Fisheye, Cylindrical, Equirectangular."));
    QLabel* const note = new QLabel(i18n("<b>Note: lens correction options depend of filters available in LensFun library. "
                                         "See <a href='http://lensfun.sourceforge.net'>LensFun project web site</a> "       // krazy:exclude=insecurenet
                                         "for more information.</b>"), this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    grid->addWidget(title,        0, 0, 1, 2);
    grid->addWidget(d->filterCCA, 1, 0, 1, 2);
    grid->addWidget(d->filterVIG, 2, 0, 1, 2);
    grid->addWidget(d->filterDST, 4, 0, 1, 2);
    grid->addWidget(d->filterGEO, 5, 0, 1, 2);
    grid->addWidget(note,         6, 0, 1, 2);
    grid->setRowStretch(7, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    connect(d->filterCCA, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->filterVIG, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->filterDST, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->filterGEO, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));
}

LensFunSettings::~LensFunSettings()
{
    delete d;
}

void LensFunSettings::setEnabledCCA(bool b)
{
    d->filterCCA->setEnabled(b);
}

void LensFunSettings::setEnabledVig(bool b)
{
    d->filterVIG->setEnabled(b);
}

void LensFunSettings::setEnabledDist(bool b)
{
    d->filterDST->setEnabled(b);
}

void LensFunSettings::setEnabledGeom(bool b)
{
    d->filterGEO->setEnabled(b);
}

void LensFunSettings::assignFilterSettings(LensFunContainer& prm)
{
    prm.filterCCA = (d->filterCCA->isChecked() && d->filterCCA->isEnabled());
    prm.filterVIG = (d->filterVIG->isChecked() && d->filterVIG->isEnabled());
    prm.filterDST = (d->filterDST->isChecked() && d->filterDST->isEnabled());
    prm.filterGEO = (d->filterGEO->isChecked() && d->filterGEO->isEnabled());
}

void LensFunSettings::setFilterSettings(const LensFunContainer& settings)
{
    blockSignals(true);
    d->filterCCA->setChecked(settings.filterCCA);
    d->filterVIG->setChecked(settings.filterVIG);
    d->filterDST->setChecked(settings.filterDST);
    d->filterGEO->setChecked(settings.filterGEO);
    blockSignals(false);
}

void LensFunSettings::resetToDefault()
{
    setFilterSettings(LensFunContainer());
}

LensFunContainer LensFunSettings::defaultSettings() const
{
    LensFunContainer prm;
    return prm;
}

LensFunContainer LensFunSettings::settings() const
{
    LensFunContainer prm;
    prm.filterCCA = d->filterCCA->isChecked();
    prm.filterVIG = d->filterVIG->isChecked();
    prm.filterDST = d->filterDST->isChecked();
    prm.filterGEO = d->filterGEO->isChecked();
    return prm;
}

void LensFunSettings::readSettings(const KConfigGroup& group)
{
    LensFunContainer prm;
    LensFunContainer defaultPrm = defaultSettings();
    prm.filterCCA = group.readEntry(d->configCCAEntry,        defaultPrm.filterCCA);
    prm.filterVIG = group.readEntry(d->configVignettingEntry, defaultPrm.filterVIG);
    prm.filterDST = group.readEntry(d->configDistortionEntry, defaultPrm.filterDST);
    prm.filterGEO = group.readEntry(d->configGeometryEntry,   defaultPrm.filterGEO);
    setFilterSettings(prm);
}

void LensFunSettings::writeSettings(KConfigGroup& group)
{
    LensFunContainer prm;
    assignFilterSettings(prm);

    if (d->filterCCA->isEnabled())
    {
        group.writeEntry(d->configCCAEntry, (prm.filterCCA));
    }

    if (d->filterVIG->isEnabled())
    {
        group.writeEntry(d->configVignettingEntry, (prm.filterVIG));
    }

    if (d->filterDST->isEnabled())
    {
        group.writeEntry(d->configDistortionEntry, (prm.filterDST));
    }

    if (d->filterGEO->isEnabled())
    {
        group.writeEntry(d->configGeometryEntry, (prm.filterGEO));
    }
}

} // namespace Digikam

#include "moc_lensfunsettings.cpp"
