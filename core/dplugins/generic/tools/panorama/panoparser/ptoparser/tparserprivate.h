/*============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Description : Helper functions for the Hugin API
 *
 * Copyright (C) 2007 by Daniel M German <dmgerman at uvic doooot ca>
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * ============================================================ */

#ifndef DIGIKAM_T_PARSER_PRIVATE_H
#define DIGIKAM_T_PARSER_PRIVATE_H

// C includes

#include <ctype.h>
#include <stdlib.h>

// Local includes

#include "tparser.h"

/*
void TokenBegin(char *t);
*/

int  panoScriptDataReset(void);
int  panoScriptParserInit(const char* const filename);
void panoScriptParserClose(void);

int  panoScriptScannerGetNextChar(char* b, int maxBuffer);
void panoScriptScannerTokenBegin(char* t);

#ifndef _MSC_VER   // krazy:exclude=cpp

void panoScriptParserError(char const* errorstring, ...) __attribute__ ((format (printf, 1, 2)));

#else

void panoScriptParserError(char const* errorstring, ...);

#endif

void  yyerror(char const* st);
void* panoScriptReAlloc(void** ptr, size_t size, int* count);

#endif /* DIGIKAM_T_PARSER_PRIVATE_H */
