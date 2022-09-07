/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-11-06
 * Description : list of RAW file extensions supported by libraw
 *
 * Copyright (C) 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "drawfiles.h"

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

QString s_rawFileExtensions()
{
    QList<QString> list = s_rawFileExtensionsdWithDesc().keys();
    QString ret;

    Q_FOREACH (const QString& mime, list)
    {
        ret.append(QString::fromLatin1("*.%1 ").arg(mime));
    }

    return ret;
}

QMap<QString, QString> s_rawFileExtensionsdWithDesc()
{
    QMap<QString, QString> map;

    /**
     * NOTE: extension list Version 1 and 2 are taken from www.cybercom.net/~dcoffin/dcraw/rawphoto.c
     *
     *                        Ext           Descriptions From
     *                                      www.file-extensions.org
     *                                      en.wikipedia.org/wiki/RAW_file_format
     *                                      filext.com
     */

    /**
     * NOTE: VERSION 1
     */
    map.insert(QLatin1String("bay"),  i18n("Casio Digital Camera Raw File Format"));
    map.insert(QLatin1String("bmq"),  i18n("NuCore Raw Image File"));

    /**
     * These images are based on the TIFF image standard.
     */
    map.insert(QLatin1String("cr2"),  i18n("Canon Digital Camera RAW Image Format version 2.0"));

    map.insert(QLatin1String("crw"),  i18n("Canon Digital Camera RAW Image Format version 1.0"));
    map.insert(QLatin1String("cs1"),  i18n("Capture Shop Raw Image File"));
    map.insert(QLatin1String("dc2"),  i18n("Kodak DC25 Digital Camera File"));

    /**
     * For these models: Kodak DSC Pro SLR/c, Kodak DSC Pro SLR/n, Kodak DSC Pro 14N, Kodak DSC PRO 14nx.
     */
    map.insert(QLatin1String("dcr"),  i18n("Kodak Digital Camera Raw Image Format"));

    /**
     * DNG is publicly available archival format for the raw files generated by digital cameras.
     * By addressing the lack of an open standard for the raw files created by individual camera models,
     * DNG helps ensure that photographers will be able to access their files in the future.
     */
    map.insert(QLatin1String("dng"),  i18n("Adobe Digital Negative"));

    map.insert(QLatin1String("erf"),  i18n("Epson Digital Camera Raw Image Format"));
    map.insert(QLatin1String("fff"),  i18n("Imacon Digital Camera Raw Image Format"));
    map.insert(QLatin1String("hdr"),  i18n("Leaf Raw Image File"));
    map.insert(QLatin1String("k25"),  i18n("Kodak DC25 Digital Camera Raw Image Format"));
    map.insert(QLatin1String("kdc"),  i18n("Kodak Digital Camera Raw Image Format"));
    map.insert(QLatin1String("mdc"),  i18n("Minolta RD175 Digital Camera Raw Image Format"));
    map.insert(QLatin1String("mos"),  i18n("Mamiya Digital Camera Raw Image Format"));
    map.insert(QLatin1String("mrw"),  i18n("Minolta Dimage Digital Camera Raw Image Format"));
    map.insert(QLatin1String("nef"),  i18n("Nikon Digital Camera Raw Image Format"));
    map.insert(QLatin1String("orf"),  i18n("Olympus Digital Camera Raw Image Format"));
    map.insert(QLatin1String("pef"),  i18n("Pentax Digital Camera Raw Image Format"));
    map.insert(QLatin1String("pxn"),  i18n("Logitech Digital Camera Raw Image Format"));
    map.insert(QLatin1String("raf"),  i18n("Fuji Digital Camera Raw Image Format"));
    map.insert(QLatin1String("raw"),  i18n("Panasonic Digital Camera Image Format"));
    map.insert(QLatin1String("rdc"),  i18n("Digital Foto Maker Raw Image File"));
    map.insert(QLatin1String("sr2"),  i18n("Sony Digital Camera Raw Image Format"));

    /**
     * For DSC-F828 8 megapixel digital camera or Sony DSC-R1.
     */
    map.insert(QLatin1String("srf"),  i18n("Sony Digital Camera Raw Image Format"));

    /**
     * For devices based on Foveon X3 direct image sensor.
     */
    map.insert(QLatin1String("x3f"),  i18n("Sigma Digital Camera Raw Image Format"));

    /**
     * For Alpha devices.
     */
    map.insert(QLatin1String("arw"),  i18n("Sony Digital Camera Raw Image Format"));

    /**
     * NOTE: VERSION 2
     */

    map.insert(QLatin1String("3fr"),  i18n("Hasselblad Digital Camera Raw Image Format"));
    map.insert(QLatin1String("cine"), i18n("Phantom Software Raw Image File"));
    map.insert(QLatin1String("ia"),   i18n("Sinar Raw Image File"));
    map.insert(QLatin1String("kc2"),  i18n("Kodak DCS200 Digital Camera Raw Image Format"));
    map.insert(QLatin1String("mef"),  i18n("Mamiya Digital Camera Raw Image Format"));
    map.insert(QLatin1String("nrw"),  i18n("Nikon Digital Camera Raw Image Format"));
    map.insert(QLatin1String("qtk"),  i18n("Apple Quicktake 100/150 Digital Camera Raw Image Format"));
    map.insert(QLatin1String("rw2"),  i18n("Panasonic LX3 Digital Camera Raw Image Format"));
    map.insert(QLatin1String("sti"),  i18n("Sinar Capture Shop Raw Image File"));

    /**
     * NOTE: VERSION 3
     */

    map.insert(QLatin1String("rwl"),  i18n("Leica Digital Camera Raw Image Format"));

    /**
     * NOTE: VERSION 4
     */

    map.insert(QLatin1String("srw"),  i18n("Samsung Raw Image Format"));

    /**
     * NOTE: VERSION 5
     */
    map.insert(QLatin1String("cr3"),  i18n("Canon Digital Camera RAW Image Format version 3.0"));

    /**
     * NOTE: VERSION 6
     */
    map.insert(QLatin1String("ori"),  i18n("Olympus Digital Camera Raw Image Format"));

    /**
     * NOTE: VERSION 7
     */
    map.insert(QLatin1String("iiq"),  i18n("Phase One Digital Camera Raw Image Format"));

    /**
     * NOTE: VERSION 8
     */
    map.insert(QLatin1String("ari"),  i18n("Arriflex Camera Raw Image Format"));
    map.insert(QLatin1String("cap"),  i18n("Phase One Digital Camera Raw Image Format"));
    map.insert(QLatin1String("fff"),  i18n("Hasselblad Digital Camera Raw Image Format"));
    map.insert(QLatin1String("cin"),  i18n("Phantom Software Raw Image File"));

/*
    TODO: check if these format are supported

    map.insert(QLatin1String("drf"),  i18n("Kodak Digital Camera Raw Image Format"));
    map.insert(QLatin1String("dsc"),  i18n("Kodak Digital Camera Raw Image Format"));
    map.insert(QLatin1String("ptx"),  i18n("Pentax Digital Camera Raw Image Format"));
    map.insert(QLatin1String("rwz"),  i18n("Rawzor Digital Camera Raw Image Format"));
*/

    return map;
}

int s_rawFileExtensionsVersion()
{
    /**
     * NOTE: increment this number whenever you change the above strings
     */
    return 8;
}

} // namespace Digikam
