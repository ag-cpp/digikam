/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-10-18
 * Description : XMP content settings page.
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_XMP_CONTENT_H
#define DIGIKAM_XMP_CONTENT_H

// Qt includes

#include <QWidget>
#include <QByteArray>

namespace DigikamGenericMetadataEditPlugin
{

class XMPContent : public QWidget
{
    Q_OBJECT

public:

    explicit XMPContent(QWidget* const parent);
    ~XMPContent() override;

    void applyMetadata(QByteArray& exifData, QByteArray& xmpData);
    void readMetadata(QByteArray& xmpData);

    bool syncJFIFCommentIsChecked()   const;
    bool syncEXIFCommentIsChecked()   const;
    bool syncEXIFCopyrightIsChecked() const;

    void setCheckedSyncJFIFComment(bool c);
    void setCheckedSyncEXIFComment(bool c);
    void setCheckedSyncEXIFCopyright(bool c);

    QString getXMPCaption()           const;
    QString getXMPCopyright()         const;

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotSyncCaptionOptionsEnabled(bool);
    void slotSyncCopyrightOptionsEnabled(bool);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_XMP_CONTENT_H
