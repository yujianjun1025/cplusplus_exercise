/* dataheap.cxx
 *
 * Copyright (C) 2001 Theppitak Karoonboonyanan,
 *   National Electronics and Computer Technology Center
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.  
 */

//
// dataheap.cxx - variable-sized data heap on VirtualMem
// Created: 31 Jul 1996 (splitted from dict.cpp)
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include "dataheap.h"

//////////////// Implementation of DataHeap class ////////////////

//////////////// Implementation of HeapAssociated class ////////////////
void HeapAssociated::create(int size)
{
    destroy();
    entry_ = dataHeap_.newBlock(size);
}

void HeapAssociated::destroy()
{
    if (entry_ != 0) {
        dataHeap_.deleteBlock(entry_);
        entry_ = 0;
    }
}

//////////////// Implementation of CharBlock class ////////////////

// size precalculation (static function)
int CharBlock::PrecalcSize(const char* str)
{
//    return strlen(str) + 1;
    return strlen(str) + 2;  // +2 for string length word
}


// Default c-tor
CharBlock::CharBlock(DataHeap& aHeap)
: HeapAssociated(aHeap)
{
}

// Creating c-tor
CharBlock::CharBlock(DataHeap& aHeap, const char* str)
: HeapAssociated(aHeap)
{
    create(PrecalcSize(str));
    setString(str);
}

// Associating c-tor
CharBlock::CharBlock(DataHeap& aHeap, VirtualMem::Pointer aEntry)
: HeapAssociated(aHeap, aEntry)
{
}

int CharBlock::blockSize()
{
    ASSERT(getEntryPtr() != 0);
//    return int(GetDataHeap().Byte(GetEntryPtr())) + 1;
    // read the first word for string length
    return int(getDataHeap().getUInt16(getEntryPtr())) + 2;
}

int CharBlock::stringLen()
{
    return int(getDataHeap().getUInt16(getEntryPtr()));
}

const char* CharBlock::getString(char buff[], int buffSize)
{
    VirtualMem::Pointer entry_ = getEntryPtr();
    ASSERT(entry_ != 0);

#if 0
    int len = int(GetDataHeap().Byte(entry));
    if (buffSize > len+2) {
        buffSize = len+2; // 1 length byte + len chars + '\0'
    }
#endif

    int len = int(getDataHeap().getUInt16(entry_));
    if (buffSize > len+3) {
        buffSize = len+3; // 2 length bytes + |len| chars + '\0'
    }

    getDataHeap().blockRead(entry_, buff, buffSize-1);
    buff[buffSize-1] = '\0';

#if 0
    // eliminate the leading length byte
    memmove(buff, buff+1, buffSize-1);
#endif

    // eliminate the leading length byte
    memmove(buff, buff+2, buffSize-1);

    return buff;
}

void CharBlock::setString(const char* str)
{
    ASSERT(getEntryPtr() != 0);

    int    len = strlen(str);
    uint8* buffer = new uint8[len+2];

    // prepare data block
    buffer[0] = 0;                        // first 2-byte is string length,
    buffer[1] = 0;                        //  (left blank first)
    strncpy((char*)(buffer+2), str, len); // then the string itself

    // write it down to virtual mem
    getDataHeap().blockWrite(getEntryPtr(), (char*)buffer, len+2);
    // then set the length word
    getDataHeap().setUInt16(getEntryPtr(), uint16(len));

    delete buffer;

#if 0
    // prepare data block
    buffer[0] = uint8(len);               // first byte is string length,
    strncpy((char*)(buffer+1), str, len); // then the string itself

    // write it down to virtual mem
    GetDataHeap().BlockWrite(GetEntryPtr(), (char*)buffer, len+1);

    delete buffer;
#endif
}
