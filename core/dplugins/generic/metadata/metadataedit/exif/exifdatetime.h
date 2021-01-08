/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-10-18
 * Description : EXIF date and time settings page.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EXIF_DATETIME_H
#define DIGIKAM_EXIF_DATETIME_H

// Qt includes

#include <QWidget>
#include <QByteArray>
#include <QDateTime>

namespace DigikamGenericMetadataEditPlugin
{

class EXIFDateTime : public QWidget
{
    Q_OBJECT

public:

    explicit EXIFDateTime(QWidget* const parent);
    ~EXIFDateTime() override;

    void applyMetadata(QByteArray& exifData, QByteArray& iptcData, QByteArray& xmpData);
    void readMetadata(QByteArray& exifData);

    bool syncXMPDateIsChecked() const;
    bool syncIPTCDateIsChecked() const;

    void setCheckedSyncXMPDate(bool c);
    void setCheckedSyncIPTCDate(bool c);

    QDateTime getEXIFCreationDate() const;

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotSetTodayCreated();
    void slotSetTodayOriginal();
    void slotSetTodayDigitalized();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_EXIF_DATETIME_H
