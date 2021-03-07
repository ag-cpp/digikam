/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-08-05
 * Description : Plasma file indexer interface.
 *
 * Copyright (C) 2014 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#include "baloowrap.h"

// Qt includes

#include <QUrl>

// KDE includes

#include <klocalizedstring.h>
#include <kfilemetadata/usermetadata.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN BalooWrap::Private
{
public:

    explicit Private()
      : syncFromDigikamToBaloo(false),
        syncFromBalooToDigikam(false)
    {
    }

    bool syncFromDigikamToBaloo;
    bool syncFromBalooToDigikam;
};

QPointer<BalooWrap> BalooWrap::internalPtr = QPointer<BalooWrap>();

BalooWrap::BalooWrap()
    : d(new Private)
{
}

BalooWrap::~BalooWrap()
{
    delete d;
}

bool BalooWrap::isCreated()
{
    return (!internalPtr.isNull());
}

BalooWrap* BalooWrap::instance()
{
    if (BalooWrap::internalPtr.isNull())
    {
        BalooWrap::internalPtr = new BalooWrap();
    }

    return BalooWrap::internalPtr;
}

void BalooWrap::setSemanticInfo(const QUrl& url, const BalooInfo& bInfo)
{
    if (!d->syncFromDigikamToBaloo)
    {
        return;
    }

    KFileMetaData::UserMetaData md(url.toLocalFile());

    md.setTags(bInfo.tags);
    md.setUserComment(bInfo.comment);

    // digiKam store rating as value form 0 to 5
    // while baloo store it as value from 0 to 10

    md.setRating((bInfo.rating == -1) ? 0 : bInfo.rating * 2);
}

BalooInfo BalooWrap::getSemanticInfo(const QUrl& url) const
{
    if (!d->syncFromBalooToDigikam)
    {
        return BalooInfo();
    }

    KFileMetaData::UserMetaData md(url.toLocalFile());

    BalooInfo bInfo;

    // Baloo have rating from 0 to 10, while digiKam have only from 0 to 5

    bInfo.comment = md.userComment();
    bInfo.rating  = md.rating() / 2;
    bInfo.tags    = md.tags();

    return bInfo;
}

void BalooWrap::setSyncToBaloo(bool value)
{
    d->syncFromDigikamToBaloo = value;
}

bool BalooWrap::getSyncToBaloo() const
{
    return d->syncFromDigikamToBaloo;
}

bool BalooWrap::getSyncToDigikam() const
{
    return d->syncFromBalooToDigikam;
}

void BalooWrap::setSyncToDigikam(bool value)
{
    d->syncFromBalooToDigikam = value;
}

} // namespace Digikam
