/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// C++ includes

#include <cstdint>
#include <cstdio>

#define O5MREADER_RET_OK 1
#define O5MREADER_RET_ERR 0

#define O5MREADER_ITERATE_RET_DONE 0
#define O5MREADER_ITERATE_RET_ERR -1
#define O5MREADER_ITERATE_RET_NEXT 1

#define O5MREADER_DS_END 0xfe
#define O5MREADER_DS_NODE 0x10
#define O5MREADER_DS_WAY 0x11
#define O5MREADER_DS_REL 0x12
#define O5MREADER_DS_BBOX 0xdb
#define O5MREADER_DS_TSTAMP 0xdc
#define O5MREADER_DS_HEADER 0xe0
#define O5MREADER_DS_SYNC 0xee
#define O5MREADER_DS_JUMP 0xef
#define O5MREADER_DS_RESET 0xff

#define O5MREADER_ERR_CODE_OK 0
#define O5MREADER_ERR_CODE_FILE_HAS_WRONG_START 1
#define O5MREADER_ERR_CODE_UNEXPECTED_END_OF_FILE 2
#define O5MREADER_ERR_CODE_MEMORY_ERROR 3
#define O5MREADER_ERR_CODE_CAN_NOT_ITERATE_TAGS_HERE 4
#define O5MREADER_ERR_CODE_CAN_NOT_ITERATE_NDS_HERE 5
#define O5MREADER_ERR_CODE_CAN_NOT_ITERATE_REFS_HERE 6

namespace Marble
{

typedef int O5mreaderRet;
typedef int O5mreaderIterateRet;

typedef struct
{
    int errCode;
    char* errMsg;
    FILE* f;
    uint64_t offset;
    uint64_t offsetNd;
    uint64_t offsetRf;
    uint64_t current;
    char* tagPair;
    int64_t nodeId;
    int64_t wayId;
    int64_t wayNodeId;
    int64_t relId;
    int64_t nodeRefId;
    int64_t wayRefId;
    int64_t relRefId;
    int32_t lon;
    int32_t lat;
    uint8_t canIterateTags;
    uint8_t canIterateNds;
    uint8_t canIterateRefs;
    char** strPairTable;
} O5mreader;

typedef struct
{
    uint8_t type;
    uint64_t id;
    uint32_t version;
    uint8_t isEmpty;
    int32_t lon;
    int32_t lat;
} O5mreaderDataset;

void o5mreader_setError(O5mreader* pReader, int code, const char* message);
void o5mreader_setNoError(O5mreader* pReader);
O5mreaderIterateRet o5mreader_skipTags(O5mreader* pReader);
O5mreaderIterateRet o5mreader_readNode(O5mreader* pReader, O5mreaderDataset* ds);
O5mreaderIterateRet o5mreader_readWay(O5mreader* pReader, O5mreaderDataset* ds);
O5mreaderIterateRet o5mreader_readRel(O5mreader* pReader, O5mreaderDataset* ds);
O5mreaderIterateRet o5mreader_skipRefs(O5mreader* pReader);
O5mreaderIterateRet o5mreader_skipNds(O5mreader* pReader);
O5mreaderRet o5mreader_readInt(O5mreader* pReader, uint64_t* ret);

O5mreaderRet o5mreader_open(O5mreader** ppReader, FILE* f);

void o5mreader_close(O5mreader* pReader);

const char* o5mreader_strerror(int errCode);

O5mreaderIterateRet o5mreader_iterateDataSet(O5mreader* pReader, O5mreaderDataset* ds);

O5mreaderIterateRet o5mreader_iterateTags(O5mreader* pReader, char** pKey, char** pVal);

O5mreaderIterateRet o5mreader_iterateNds(O5mreader* pReader, uint64_t* nodeId);

O5mreaderIterateRet o5mreader_iterateRefs(O5mreader* pReader, uint64_t* refId, uint8_t* type, char** pRole);

} // namespace Marble
