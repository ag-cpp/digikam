/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-24
 * Description : Batch Tools Factory.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_BATCH_TOOLS_FACTORY_H
#define DIGIKAM_BQM_BATCH_TOOLS_FACTORY_H

// Qt includes

#include <QObject>

// Local includes

#include "batchtool.h"
#include "batchtoolutils.h"

namespace Digikam
{

class BatchToolsFactory : public QObject
{
    Q_OBJECT

public:

    static BatchToolsFactory* instance();

public:

    void           registerTool(BatchTool* const tool);
    BatchTool*     findTool(const QString& name, BatchTool::BatchToolGroup group) const;
    BatchToolsList toolsList()                                                    const;

private:

    // Disable
    BatchToolsFactory();
    ~BatchToolsFactory() override;

    explicit BatchToolsFactory(QObject*) = delete;

private:

    class Private;
    Private* const d;

    friend class BatchToolsFactoryCreator;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_BATCH_TOOLS_FACTORY_H
