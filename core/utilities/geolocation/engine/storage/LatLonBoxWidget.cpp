/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *               (c) 2007-2022 Marble Team
 *               https://invent.kde.org/education/marble/-/raw/master/data/credits_authors.html
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "LatLonBoxWidget.h"

// Local includes

#include "GeoDataLatLonBox.h"
#include "ui_LatLonBoxWidget.h"

namespace Marble
{

class Q_DECL_HIDDEN LatLonBoxWidget::Private
{
public:

    explicit Private(QWidget* const parent);

    Ui::LatLonBoxWidget m_ui;
};

LatLonBoxWidget::Private::Private(QWidget* const parent)
{
    m_ui.setupUi(parent);
}

LatLonBoxWidget::LatLonBoxWidget(QWidget* const parent, Qt::WindowFlags const f)
    : QWidget(parent, f),
      d(new Private(this))
{
    // used for updating tiles count in DownloadRegionDialog

    connect(d->m_ui.northSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    connect(d->m_ui.southSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    connect(d->m_ui.eastSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    connect(d->m_ui.westSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));

    // used for adjusting single step values

    connect(d->m_ui.northSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLatSingleStep()));
    connect(d->m_ui.southSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLatSingleStep()));
    connect(d->m_ui.eastSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLonSingleStep()));
    connect(d->m_ui.westSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLonSingleStep()));
}

LatLonBoxWidget::~LatLonBoxWidget()
{
    delete d;
}

GeoDataLatLonBox LatLonBoxWidget::latLonBox() const
{
    return GeoDataLatLonBox(d->m_ui.northSpinBox->value(), d->m_ui.southSpinBox->value(),
                            d->m_ui.eastSpinBox->value(), d->m_ui.westSpinBox->value(),
                            GeoDataCoordinates::Degree);
}

void LatLonBoxWidget::setLatLonBox(GeoDataLatLonBox const& latLonBox)
{
    d->m_ui.northSpinBox->setValue(latLonBox.north(GeoDataCoordinates::Degree));
    d->m_ui.southSpinBox->setValue(latLonBox.south(GeoDataCoordinates::Degree));
    d->m_ui.eastSpinBox->setValue(latLonBox.east(GeoDataCoordinates::Degree));
    d->m_ui.westSpinBox->setValue(latLonBox.west(GeoDataCoordinates::Degree));
}

void LatLonBoxWidget::updateLatSingleStep()
{
    double const singleStep =
        qAbs(d->m_ui.northSpinBox->value() - d->m_ui.southSpinBox->value()) / 10.0;
    d->m_ui.northSpinBox->setSingleStep(singleStep);
    d->m_ui.southSpinBox->setSingleStep(singleStep);
}

void LatLonBoxWidget::updateLonSingleStep()
{
    double const singleStep =
        qAbs(d->m_ui.eastSpinBox->value() - d->m_ui.westSpinBox->value()) / 10.0;
    d->m_ui.eastSpinBox->setSingleStep(singleStep);
    d->m_ui.westSpinBox->setSingleStep(singleStep);
}

} // namespace Marble

#include "moc_LatLonBoxWidget.cpp"
