/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-22
 * Description : a parser for the AdvancedRename utility used for importing images,
 *               excluding the database options
 *
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_IMPORT_RENAME_PARSER_H
#define DIGIKAM_IMPORT_RENAME_PARSER_H

// Local includes

#include "parser.h"

namespace Digikam
{

class ImportRenameParser : public Parser
{

public:

    explicit ImportRenameParser();

private:

    // Disable
    ImportRenameParser(const ImportRenameParser&)            = delete;
    ImportRenameParser& operator=(const ImportRenameParser&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_RENAME_PARSER_H
