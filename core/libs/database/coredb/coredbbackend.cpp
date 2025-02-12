/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-04-15
 * Description : Core database abstract backend.
 *
 * SPDX-FileCopyrightText: 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Local includes

#include "coredbbackend.h"
#include "coredbbackend_p.h"
#include "coredbschemaupdater.h"
#include "coredbwatch.h"

namespace Digikam
{

CoreDbBackend::CoreDbBackend(DbEngineLocking* const locking, const QString& backendName)
    : BdEngineBackend(backendName, locking, *new CoreDbBackendPrivate(this))
{
}

CoreDbBackend::~CoreDbBackend()
{
}

void CoreDbBackend::setCoreDbWatch(CoreDbWatch* watch)
{
    Q_D(CoreDbBackend);

    d->watch = watch;
}

bool CoreDbBackend::initSchema(CoreDbSchemaUpdater* updater)
{
    Q_D(CoreDbBackend);

    if (d->status == OpenSchemaChecked)
    {
        return true;
    }

    if (d->status == Unavailable)
    {
        return false;
    }

    if (updater->update())
    {
        d->status = OpenSchemaChecked;

        return true;
    }

    return false;
}

void CoreDbBackend::recordChangeset(const ImageChangeset& changeset)
{
    Q_D(CoreDbBackend);

    // if we want to do compression of changesets, think about doing this here

    d->imageChangesetContainer.recordChangeset(changeset);
}

void CoreDbBackend::recordChangeset(const ImageTagChangeset& changeset)
{
    Q_D(CoreDbBackend);

    d->imageTagChangesetContainer.recordChangeset(changeset);
}

void CoreDbBackend::recordChangeset(const CollectionImageChangeset& changeset)
{
    Q_D(CoreDbBackend);

    d->collectionImageChangesetContainer.recordChangeset(changeset);
}

void CoreDbBackend::recordChangeset(const AlbumChangeset& changeset)
{
    Q_D(CoreDbBackend);

    d->albumChangesetContainer.recordChangeset(changeset);
}

void CoreDbBackend::recordChangeset(const TagChangeset& changeset)
{
    Q_D(CoreDbBackend);

    d->tagChangesetContainer.recordChangeset(changeset);
}

void CoreDbBackend::recordChangeset(const AlbumRootChangeset& changeset)
{
    Q_D(CoreDbBackend);

    d->albumRootChangesetContainer.recordChangeset(changeset);
}

void CoreDbBackend::recordChangeset(const SearchChangeset& changeset)
{
    Q_D(CoreDbBackend);

    d->searchChangesetContainer.recordChangeset(changeset);
}

} // namespace Digikam

#include "moc_coredbbackend.cpp"
