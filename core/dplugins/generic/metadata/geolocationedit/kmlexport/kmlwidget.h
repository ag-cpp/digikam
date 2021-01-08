/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_KML_WIDGET_H
#define DIGIKAM_KML_WIDGET_H

// Qt includes

#include <QWidget>
#include <QList>
#include <QUrl>

// Local includes

#include "kmlexport.h"
#include "dfileselector.h"
#include "dcolorselector.h"
#include "dinfointerface.h"
#include "geolocationedit.h"
#include "gpsitemmodel.h"

class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpacerItem;
class QComboBox;
class QCloseEvent;
class QSpinBox;
class QPushButton;

using namespace Digikam;

namespace DigikamGenericGeolocationEditPlugin
{

class KmlWidget : public QWidget
{
    Q_OBJECT

public:

    explicit KmlWidget(GeolocationEdit* const dlg,
                       GPSItemModel* const imageModel,
                       DInfoInterface* const iface);
    ~KmlWidget() override;

public:

    QLabel*           ImageSizeLabel;
    QLabel*           IconSizeLabel;
    QLabel*           destinationDirectoryLabel_;
    QLabel*           FileNameLabel_;
    QLabel*           DestinationUrlLabel_;
    QLabel*           GPXFileLabel_;
    QLabel*           timeZoneLabel_;
    QLabel*           GPXLineWidthLabel_;
    QLabel*           GPXColorLabel_;
    QLabel*           GPXAltitudeLabel_;
    QLabel*           GPXTracksOpacityLabel_;

    QGroupBox*        TargetPreferenceGroupBox;
    QGroupBox*        TargetTypeGroupBox;

    QButtonGroup*     buttonGroupTargetType;

    QRadioButton*     LocalTargetRadioButton_;
    QRadioButton*     GoogleMapTargetRadioButton_;

    QLineEdit*        DestinationUrl_;
    QLineEdit*        FileName_;

    QCheckBox*        GPXTracksCheckBox_;

    QComboBox*        AltitudeCB_;
    QComboBox*        timeZoneCB;
    QComboBox*        GPXAltitudeCB_;

    DColorSelector*   GPXTrackColor_;

    DFileSelector*    DestinationDirectory_;
    DFileSelector*    GPXFileUrlRequester_;
    QPushButton*      m_geneBtn;

    QSpinBox*         ImageSizeInput_;
    QSpinBox*         IconSizeInput_;
    QSpinBox*         GPXTracksOpacityInput_;
    QSpinBox*         GPXLineWidthInput_;

    GPSItemModel*     m_model;
    GeolocationEdit*  m_dlg;

Q_SIGNALS:

    void signalSetUIEnabled(const bool enabledState);
    void signalProgressSetup(const int maxProgress, const QString& progressText);

public Q_SLOTS:

    void slotGoogleMapTargetRadioButtonToggled(bool);
    void slotKMLTracksCheckButtonToggled(bool);

protected:

    void saveSettings();
    void readSettings();

protected Q_SLOTS:

    void slotKMLGenerate();

protected:

    QGridLayout* KMLExportConfigLayout;
    QGridLayout* SizeGroupBoxLayout;
    QGridLayout* TargetPreferenceGroupBoxLayout;
    QGridLayout* buttonGroupTargetTypeLayout;

    KmlExport    m_kmlExport;
};

} // namespace DigikamGenericGeolocationEditPlugin

#endif // DIGIKAM_KML_WIDGET_H
