/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-12
 * Description : metadata Settings Container.
 *
 * Copyright (C) 2015      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * Copyright (C) 2015-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dmetadatasettingscontainer.h"

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "dmetadatasettings.h"
#include "digikam_debug.h"

namespace Digikam
{

QString NamespaceEntry::DM_TAG_CONTAINER()
{
    return QString::fromUtf8(I18N_NOOP("Tags"));
}

QString NamespaceEntry::DM_RATING_CONTAINER()
{
    return QString::fromUtf8(I18N_NOOP("Rating"));
}

QString NamespaceEntry::DM_COMMENT_CONTAINER()
{
    return QString::fromUtf8(I18N_NOOP("Caption"));
}

QString NamespaceEntry::DM_COLORLABEL_CONTAINER()
{
    return QString::fromUtf8(I18N_NOOP("Color label"));
}

// ------------------------------------------------------------

bool s_dmcompare(const NamespaceEntry& e1, const NamespaceEntry& e2)
{
    return (e1.index < e2.index);
}

QDebug operator<<(QDebug dbg, const NamespaceEntry& inf)
{
    dbg.nospace() << "[NamespaceEntry] nsType("
                  << inf.nsType << "), ";
    dbg.nospace() << "subspace("
                  << inf.subspace << "), ";
    dbg.nospace() << "isDefault("
                  << inf.isDefault << "), ";
    dbg.nospace() << "isDisabled("
                  << inf.isDisabled << "), ";
    dbg.nospace() << "index("
                  << inf.index << "), ";
    dbg.nospace() << "namespaceName("
                  << inf.namespaceName << "), ";
    dbg.nospace() << "alternativeName("
                  << inf.alternativeName << "), ";
    dbg.nospace() << "tagPaths("
                  << inf.tagPaths << "), ";
    dbg.nospace() << "separator("
                  << inf.separator << "), ";
    dbg.nospace() << "convertRatio("
                  << inf.convertRatio << "), ";
    dbg.nospace() << "specialOpts("
                  << inf.specialOpts << "), ";
    dbg.nospace() << "secondNameOpts("
                  << inf.secondNameOpts << ")";

    return dbg.space();
}

// -------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DMetadataSettingsContainer::Private
{
public:

    explicit Private()
      : unifyReadWrite(false)
    {
    }

public:

    QMap<QString, QList<NamespaceEntry> > readMappings;
    QMap<QString, QList<NamespaceEntry> > writeMappings;
    bool                                  unifyReadWrite;
};

// -------------------------------------------------------------------------------------------------

DMetadataSettingsContainer::DMetadataSettingsContainer()
    : d(new Private)
{
    addMapping(NamespaceEntry::DM_TAG_CONTAINER());
    addMapping(NamespaceEntry::DM_RATING_CONTAINER());
    addMapping(NamespaceEntry::DM_COMMENT_CONTAINER());
    addMapping(NamespaceEntry::DM_COLORLABEL_CONTAINER());
}

DMetadataSettingsContainer::DMetadataSettingsContainer(const DMetadataSettingsContainer& other)
    : d(new Private)
{
    *d = *other.d;
}

DMetadataSettingsContainer& DMetadataSettingsContainer::operator=(const DMetadataSettingsContainer& other)
{
    *d = *other.d;

    return *this;
}

DMetadataSettingsContainer::~DMetadataSettingsContainer()
{
    delete d;
}

bool DMetadataSettingsContainer::unifyReadWrite() const
{
    return d->unifyReadWrite;
}

void DMetadataSettingsContainer::setUnifyReadWrite(bool b)
{
    d->unifyReadWrite = b;
}

void DMetadataSettingsContainer::readFromConfig(KConfigGroup& group)
{
    bool valid                   = true;
    const QString readNameSpace  = QLatin1String("read%1Namespaces");
    const QString writeNameSpace = QLatin1String("write%1Namespaces");

    foreach (const QString& str, mappingKeys())
    {
        if (!group.hasGroup(readNameSpace.arg(str)))
        {
            valid = false;
            qCDebug(DIGIKAM_GENERAL_LOG) << "Does not contain " << str << " Namespace";
            break;
        }

        if (!group.hasGroup(writeNameSpace.arg(str)))
        {
            valid = false;
            qCDebug(DIGIKAM_GENERAL_LOG) << "Does not contain " << str << " Namespace";
            break;
        }
    }

    if (valid)
    {
        foreach (const QString& str, mappingKeys())
        {
            readOneGroup(group, readNameSpace.arg(str), getReadMapping(str));
            readOneGroup(group, writeNameSpace.arg(str), getWriteMapping(str));
        }
    }
    else
    {
        defaultValues();
    }

    d->unifyReadWrite = group.readEntry(QLatin1String("unifyReadWrite"), true);
}

void DMetadataSettingsContainer::writeToConfig(KConfigGroup& group) const
{
   const QString readNameSpace  = QLatin1String("read%1Namespaces");
   const QString writeNameSpace = QLatin1String("write%1Namespaces");

    foreach (const QString& str, mappingKeys())
    {
        // Remove all old group elements.

        group.group(readNameSpace.arg(str)).deleteGroup();
        group.group(writeNameSpace.arg(str)).deleteGroup();

        writeOneGroup(group, readNameSpace.arg(str), getReadMapping(str));
        writeOneGroup(group, writeNameSpace.arg(str), getWriteMapping(str));
    }

    group.writeEntry(QLatin1String("unifyReadWrite"), d->unifyReadWrite);
    group.sync();
}

void DMetadataSettingsContainer::defaultValues()
{
    d->unifyReadWrite = true;
    d->writeMappings.clear();
    d->readMappings.clear();

    defaultTagValues();
    defaultRatingValues();
    defaultCommentValues();
    defaultColorLabelValues();
}

void DMetadataSettingsContainer::addMapping(const QString& key)
{
    d->readMappings[key]  = QList<NamespaceEntry>();
    d->writeMappings[key] = QList<NamespaceEntry>();
}

QList<NamespaceEntry>& DMetadataSettingsContainer::getReadMapping(const QString& key) const
{
    return d->readMappings[key];
}

QList<NamespaceEntry>& DMetadataSettingsContainer::getWriteMapping(const QString& key) const
{
    return d->writeMappings[key];
}

QList<QString> DMetadataSettingsContainer::mappingKeys() const
{
    return d->readMappings.keys();
}

void DMetadataSettingsContainer::defaultTagValues()
{
    // Default tag namespaces

    NamespaceEntry tagNs1;
    tagNs1.namespaceName    = QLatin1String("Xmp.digiKam.TagsList");
    tagNs1.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs1.separator        = QLatin1Char('/');
    tagNs1.nsType           = NamespaceEntry::TAGS;
    tagNs1.index            = 0;
    tagNs1.specialOpts      = NamespaceEntry::TAG_XMPSEQ;
    tagNs1.subspace         = NamespaceEntry::XMP;

    NamespaceEntry tagNs2;
    tagNs2.namespaceName    = QLatin1String("Xmp.MicrosoftPhoto.LastKeywordXMP");
    tagNs2.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs2.separator        = QLatin1Char('/');
    tagNs2.nsType           = NamespaceEntry::TAGS;
    tagNs2.index            = 1;
    tagNs2.specialOpts      = NamespaceEntry::TAG_XMPBAG;
    tagNs2.subspace         = NamespaceEntry::XMP;

    NamespaceEntry tagNs3;
    tagNs3.namespaceName    = QLatin1String("Xmp.lr.hierarchicalSubject");
    tagNs3.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs3.separator        = QLatin1Char('|');
    tagNs3.nsType           = NamespaceEntry::TAGS;
    tagNs3.index            = 2;
    tagNs3.specialOpts      = NamespaceEntry::TAG_XMPBAG;
    tagNs3.subspace         = NamespaceEntry::XMP;
    tagNs3.alternativeName  = QLatin1String("Xmp.lr.HierarchicalSubject");
    tagNs3.secondNameOpts   = NamespaceEntry::TAG_XMPSEQ;

    NamespaceEntry tagNs4;
    tagNs4.namespaceName    = QLatin1String("Xmp.mediapro.CatalogSets");
    tagNs4.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs4.separator        = QLatin1Char('|');
    tagNs4.nsType           = NamespaceEntry::TAGS;
    tagNs4.index            = 3;
    tagNs4.specialOpts      = NamespaceEntry::TAG_XMPBAG;
    tagNs4.subspace         = NamespaceEntry::XMP;

    NamespaceEntry tagNs5;
    tagNs5.namespaceName    = QLatin1String("Xmp.acdsee.categories");
    tagNs5.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs5.separator        = QLatin1Char('/');
    tagNs5.nsType           = NamespaceEntry::TAGS;
    tagNs5.index            = 4;
    tagNs5.specialOpts      = NamespaceEntry::TAG_ACDSEE;
    tagNs5.subspace         = NamespaceEntry::XMP;

    NamespaceEntry tagNs6;
    tagNs6.namespaceName    = QLatin1String("Xmp.dc.subject");
    tagNs6.tagPaths         = NamespaceEntry::TAG;
    tagNs6.separator        = QLatin1Char('/');
    tagNs6.nsType           = NamespaceEntry::TAGS;
    tagNs6.index            = 5;
    tagNs6.specialOpts      = NamespaceEntry::TAG_XMPBAG;
    tagNs6.subspace         = NamespaceEntry::XMP;

    NamespaceEntry tagNs7;
    tagNs7.namespaceName    = QLatin1String("Iptc.Application2.Keywords");
    tagNs7.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs7.separator        = QLatin1Char(',');
    tagNs7.nsType           = NamespaceEntry::TAGS;
    tagNs7.index            = 6;
    tagNs7.subspace         = NamespaceEntry::IPTC;

    NamespaceEntry tagNs8;
    tagNs8.namespaceName    = QLatin1String("Exif.Image.XPKeywords");
    tagNs8.tagPaths         = NamespaceEntry::TAGPATH;
    tagNs8.separator        = QLatin1Char(';');
    tagNs8.nsType           = NamespaceEntry::TAGS;
    tagNs8.index            = 7;
    tagNs8.subspace         = NamespaceEntry::EXIF;

    getReadMapping(NamespaceEntry::DM_TAG_CONTAINER()) << tagNs1
                                                       << tagNs2
                                                       << tagNs3
                                                       << tagNs4
                                                       << tagNs5
                                                       << tagNs6
                                                       << tagNs7
                                                       << tagNs8;

    d->writeMappings[NamespaceEntry::DM_TAG_CONTAINER()]
        = QList<NamespaceEntry>(getReadMapping(NamespaceEntry::DM_TAG_CONTAINER()));
}

void DMetadataSettingsContainer::defaultRatingValues()
{
    QList<int> defaultVal;
    QList<int> microsoftMappings;
    QList<int> iptcMappings;

    defaultVal        << 0 << 1 << 2  << 3  << 4  << 5;
    microsoftMappings << 0 << 1 << 25 << 50 << 75 << 99;
    iptcMappings      << 8 << 6 << 5  << 4  << 2  << 1;

    NamespaceEntry ratingNs1;
    ratingNs1.namespaceName = QLatin1String("Xmp.xmp.Rating");
    ratingNs1.convertRatio  = defaultVal;
    ratingNs1.nsType        = NamespaceEntry::RATING;
    ratingNs1.index         = 0;
    ratingNs1.subspace      = NamespaceEntry::XMP;

    NamespaceEntry ratingNs2;
    ratingNs2.namespaceName = QLatin1String("Xmp.acdsee.rating");
    ratingNs2.convertRatio  = defaultVal;
    ratingNs2.nsType        = NamespaceEntry::RATING;
    ratingNs2.index         = 1;
    ratingNs2.subspace      = NamespaceEntry::XMP;

    NamespaceEntry ratingNs3;
    ratingNs3.namespaceName = QLatin1String("Xmp.MicrosoftPhoto.Rating");
    ratingNs3.convertRatio  = microsoftMappings;
    ratingNs3.nsType        = NamespaceEntry::RATING;
    ratingNs3.index         = 2;
    ratingNs3.subspace      = NamespaceEntry::XMP;

    NamespaceEntry ratingNs4;
    ratingNs4.namespaceName = QLatin1String("Exif.Image.Rating");
    ratingNs4.convertRatio  = defaultVal;
    ratingNs4.nsType        = NamespaceEntry::RATING;
    ratingNs4.index         = 3;
    ratingNs4.subspace      = NamespaceEntry::EXIF;

    NamespaceEntry ratingNs5;
    ratingNs5.namespaceName = QLatin1String("Exif.Image.RatingPercent");
    ratingNs5.convertRatio  = microsoftMappings;
    ratingNs5.nsType        = NamespaceEntry::RATING;
    ratingNs5.index         = 4;
    ratingNs5.subspace      = NamespaceEntry::EXIF;

    NamespaceEntry ratingNs6;
    ratingNs6.namespaceName = QLatin1String("Iptc.Application2.Urgency");
    ratingNs6.convertRatio  = iptcMappings;
    ratingNs6.nsType        = NamespaceEntry::RATING;
    ratingNs6.index         = 5;
    ratingNs6.subspace      = NamespaceEntry::IPTC;

    getReadMapping(NamespaceEntry::DM_RATING_CONTAINER()) << ratingNs1
                                                          << ratingNs2
                                                          << ratingNs3
                                                          << ratingNs4
                                                          << ratingNs5
                                                          << ratingNs6;

    d->writeMappings[NamespaceEntry::DM_RATING_CONTAINER()]
        = QList<NamespaceEntry>(getReadMapping(NamespaceEntry::DM_RATING_CONTAINER()));
}

void DMetadataSettingsContainer::defaultCommentValues()
{
    NamespaceEntry commNs1;
    commNs1.namespaceName   = QLatin1String("Xmp.dc.description");
    commNs1.nsType          = NamespaceEntry::COMMENT;
    commNs1.specialOpts     = NamespaceEntry::COMMENT_ATLLANGLIST;
    commNs1.index           = 0;
    commNs1.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs2;
    commNs2.namespaceName   = QLatin1String("Xmp.exif.UserComment");
    commNs2.nsType          = NamespaceEntry::COMMENT;
    commNs2.specialOpts     = NamespaceEntry::COMMENT_ALTLANG;
    commNs2.index           = 1;
    commNs2.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs3;
    commNs3.namespaceName   = QLatin1String("Xmp.tiff.ImageDescription");
    commNs3.nsType          = NamespaceEntry::COMMENT;
    commNs3.specialOpts     = NamespaceEntry::COMMENT_ALTLANG;
    commNs3.index           = 2;
    commNs3.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs4;
    commNs4.namespaceName   = QLatin1String("Xmp.acdsee.notes");
    commNs4.nsType          = NamespaceEntry::COMMENT;
    commNs4.specialOpts     = NamespaceEntry::COMMENT_XMP;
    commNs4.index           = 3;
    commNs4.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs5;
    commNs5.namespaceName   = QLatin1String("JPEG/TIFF Comments");
    commNs5.nsType          = NamespaceEntry::COMMENT;
    commNs5.specialOpts     = NamespaceEntry::COMMENT_JPEG;
    commNs5.index           = 4;
    commNs5.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs6;
    commNs6.namespaceName   = QLatin1String("Exif.Image.ImageDescription");
    commNs6.nsType          = NamespaceEntry::COMMENT;
    commNs6.specialOpts     = NamespaceEntry::NO_OPTS;
    commNs6.index           = 5;
    commNs6.alternativeName = QLatin1String("Exif.Photo.UserComment");
    commNs6.subspace        = NamespaceEntry::EXIF;

    NamespaceEntry commNs7;
    commNs7.namespaceName   = QLatin1String("Iptc.Application2.Caption");
    commNs7.nsType          = NamespaceEntry::COMMENT;
    commNs7.specialOpts     = NamespaceEntry::NO_OPTS;
    commNs7.index           = 6;
    commNs7.subspace        = NamespaceEntry::IPTC;


     getReadMapping(NamespaceEntry::DM_COMMENT_CONTAINER()) << commNs1
                                                            << commNs2
                                                            << commNs3
                                                            << commNs4
                                                            << commNs5
                                                            << commNs6
                                                            << commNs7;

    d->writeMappings[NamespaceEntry::DM_COMMENT_CONTAINER()]
        = QList<NamespaceEntry>(getReadMapping(NamespaceEntry::DM_COMMENT_CONTAINER()));
}

void DMetadataSettingsContainer::defaultColorLabelValues()
{
    NamespaceEntry commNs1;
    commNs1.namespaceName   = QLatin1String("Xmp.digiKam.ColorLabel");
    commNs1.nsType          = NamespaceEntry::COLORLABEL;
    commNs1.specialOpts     = NamespaceEntry::NO_OPTS;
    commNs1.index           = 0;
    commNs1.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs2;
    commNs2.namespaceName   = QLatin1String("Xmp.xmp.Label");
    commNs2.nsType          = NamespaceEntry::COLORLABEL;
    commNs2.specialOpts     = NamespaceEntry::NO_OPTS;
    commNs2.index           = 1;
    commNs2.subspace        = NamespaceEntry::XMP;

    NamespaceEntry commNs3;
    commNs3.namespaceName   = QLatin1String("Xmp.photoshop.Urgency");
    commNs3.nsType          = NamespaceEntry::COLORLABEL;
    commNs3.specialOpts     = NamespaceEntry::NO_OPTS;
    commNs3.index           = 2;
    commNs3.subspace        = NamespaceEntry::XMP;

     getReadMapping(NamespaceEntry::DM_COLORLABEL_CONTAINER()) << commNs1
                                                               << commNs2
                                                               << commNs3;

    d->writeMappings[NamespaceEntry::DM_COLORLABEL_CONTAINER()]
        = QList<NamespaceEntry>(getReadMapping(NamespaceEntry::DM_COLORLABEL_CONTAINER()));
}

void DMetadataSettingsContainer::readOneGroup(KConfigGroup& group, const QString& name, QList<NamespaceEntry>& container)
{
    KConfigGroup myItems = group.group(name);

    foreach (const QString& element, myItems.groupList())
    {
        KConfigGroup gr      = myItems.group(element);
        NamespaceEntry ns;

        if (element.startsWith(QLatin1Char('#')))
        {
            ns.namespaceName = gr.readEntry("namespaceName");
        }
        else
        {
            ns.namespaceName = element;
        }

        ns.tagPaths          = (NamespaceEntry::TagType)gr.readEntry("tagPaths").toInt();
        ns.separator         = gr.readEntry("separator");
        ns.nsType            = (NamespaceEntry::NamespaceType)gr.readEntry("nsType").toInt();
        ns.index             = gr.readEntry("index").toInt();
        ns.subspace          = (NamespaceEntry::NsSubspace)gr.readEntry("subspace").toInt();
        ns.alternativeName   = gr.readEntry("alternativeName");
        ns.specialOpts       = (NamespaceEntry::SpecialOptions)gr.readEntry("specialOpts").toInt();
        ns.secondNameOpts    = (NamespaceEntry::SpecialOptions)gr.readEntry("secondNameOpts").toInt();
        ns.isDefault         = gr.readEntry(QLatin1String("isDefault"), QVariant(true)).toBool();
        ns.isDisabled        = gr.readEntry(QLatin1String("isDisabled"), QVariant(false)).toBool();
        QString conversion   = gr.readEntry("convertRatio");

        foreach (const QString& str, conversion.split(QLatin1String(",")))
        {
            ns.convertRatio.append(str.toInt());
        }

        container.append(ns);
    }

    std::sort(container.begin(), container.end(), Digikam::s_dmcompare);
}

void DMetadataSettingsContainer::writeOneGroup(KConfigGroup& group, const QString& name, QList<NamespaceEntry>& container) const
{
    KConfigGroup namespacesGroup = group.group(name);
    int index                    = 0;

    foreach (const NamespaceEntry& e, container)
    {
        QString groupNumber = QString::fromLatin1("#%1")
                              .arg(index++, 4, 10, QLatin1Char('0'));

        KConfigGroup tmp = namespacesGroup.group(groupNumber);
        tmp.writeEntry("namespaceName",   e.namespaceName);
        tmp.writeEntry("alternativeName", e.alternativeName);
        tmp.writeEntry("subspace",        (int)e.subspace);
        tmp.writeEntry("tagPaths",        (int)e.tagPaths);
        tmp.writeEntry("separator",       e.separator);
        tmp.writeEntry("nsType",          (int)e.nsType);
        tmp.writeEntry("convertRatio",    e.convertRatio);
        tmp.writeEntry("specialOpts",     (int)e.specialOpts);
        tmp.writeEntry("secondNameOpts",  (int)e.secondNameOpts);
        tmp.writeEntry("index",           e.index);
        tmp.writeEntry("isDisabled",      e.isDisabled);
        tmp.writeEntry("isDefault",       e.isDefault);
    }
}

QDebug operator<<(QDebug dbg, const DMetadataSettingsContainer& inf)
{
    dbg.nospace() << "[DMetadataSettingsContainer] readMappings(";

    foreach (const QString& str, inf.mappingKeys())
    {
        dbg.nospace() << inf.getReadMapping(str) << "), ";
    }

    dbg.nospace() << "writeMappings(";

    foreach (const QString& str, inf.mappingKeys())
    {
        dbg.nospace() << inf.getWriteMapping(str) << "), ";
    }

    dbg.nospace() << "unifyReadWrite("
                  << inf.unifyReadWrite() << ")";

    return dbg.space();
}

} // namespace Digikam
