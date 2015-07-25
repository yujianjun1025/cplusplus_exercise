/* vmem.cxx
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
// vmem.cxx - implementation of a class for virtual memory
// Created: 24 May 1996
// Author:  Theppitak Karoonboonyanan
//

#include <limits.h>
#include "vmem.h"
#include "pagemap.h"
#include <mida/utils/porting.h>

//////////////// implementation of VirtualMem class ////////////////
VirtualMem::VirtualMem() : pPgMap_(0) {}
VirtualMem::VirtualMem(const char* swapFileName, int iosModes)
    : pPgMap_(0) { openSwapFile(swapFileName, iosModes); }
VirtualMem::~VirtualMem() { delete pPgMap_; }

// swap file status
bool VirtualMem::openSwapFile(const char* swapFileName, int iosModes)
{
    delete pPgMap_; pPgMap_ = new PageMap;
    return pPgMap_ ? pPgMap_->openSwapFile(swapFileName, iosModes) : false;
}
bool VirtualMem::closeSwapFile()
{
    return pPgMap_ ? pPgMap_->closeSwapFile() : false;
}
const char* VirtualMem::swapFileName() const
{
    return pPgMap_ ? pPgMap_->swapFileName() : false;
}
bool VirtualMem::isSwapFileOpen() const
{
    return pPgMap_ ? pPgMap_->isSwapFileOpen() : false;
}
bool VirtualMem::isSwapFileGood() const
{
    return pPgMap_ ? pPgMap_->isSwapFileGood() : false;
}
bool VirtualMem::isSwapFileBad()  const
{
    return pPgMap_ ? pPgMap_->isSwapFileBad() : false;
}
bool VirtualMem::isSwapFileFail() const
{
    return pPgMap_ ? pPgMap_->isSwapFileFail() : false;
}

// total pages being used
VirtualMem::PageNumber VirtualMem::totalPages() const
{
    return pPgMap_ ? pPgMap_->totalPages() : 0;
}

// page frame preparations
inline const char* VirtualMem::pageFrame_(PageNumber pageNo) const
{
    return pPgMap_ ? pPgMap_->pageFrame(pageNo) : false;
}
inline char* VirtualMem::pageFrameRef_(PageNumber pageNo)
{
    return pPgMap_ ? pPgMap_->pageFrameRef(pageNo) : false;
}

//
// binary data storage bottle-neck
//
uint16 VirtualMem::EncodeUInt16_(uint16 n)
{
    return IsBigEndianSystem() ? ReverseBytes16(n) : n;
}

uint32 VirtualMem::EncodeUInt32_(uint32 n)
{
    return IsBigEndianSystem() ? ReverseBytes32(n) : n;
}

int16  VirtualMem::EncodeInt16_(int16 n)
{
    return IsBigEndianSystem() ? int16(ReverseBytes16(uint16(n))) : n;
}

int32  VirtualMem::EncodeInt32_(int32 n)
{
    return IsBigEndianSystem() ? int32(ReverseBytes32(uint32(n))) : n;
}


uint16 VirtualMem::DecodeUInt16_(uint16 n)
{
    return IsBigEndianSystem() ? ReverseBytes16(n) : n;
}

uint32 VirtualMem::DecodeUInt32_(uint32 n)
{
    return IsBigEndianSystem() ? ReverseBytes32(n) : n;
}

int16  VirtualMem::DecodeInt16_(int16 n)
{
    return IsBigEndianSystem() ? int16(ReverseBytes16(uint16(n))) : n;
}

int32  VirtualMem::DecodeInt32_(int32 n)
{
    return IsBigEndianSystem() ? int32(ReverseBytes32(uint32(n))) : n;
}


//
// Memory Writings
//
void VirtualMem::setByte(Pointer p, byte b)
{
    *(byte*)(pageFrameRef_(PageNo(p)) + Offset(p)) = b;
}

void VirtualMem::setWord(Pointer p, word w)
{
    ASSERT(IsWordBound(p));
    // p = wordBound(p);
    w = EncodeUInt16_(w);
    *(word*)(pageFrameRef_(PageNo(p)) + Offset(p)) = w;
}

void VirtualMem::setDWord(Pointer p, dword d)
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    d = EncodeUInt32_(d);
    *(dword*)(pageFrameRef_(PageNo(p)) + Offset(p)) = d;
}

void VirtualMem::setInt8(Pointer p, int8 n)
{
    *(int8*)(pageFrameRef_(PageNo(p)) + Offset(p)) = n;
}

void VirtualMem::setInt16(Pointer p, int16 n)
{
    ASSERT(IsWordBound(p));
    // p = wordBound(p);
    n = EncodeInt16_(n);
    *(int16*)(pageFrameRef_(PageNo(p)) + Offset(p)) = n;
}

void VirtualMem::setInt32(Pointer p, int32 n)
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    n = EncodeInt32_(n);
    *(int32*)(pageFrameRef_(PageNo(p)) + Offset(p)) = n;
}

void VirtualMem::setUInt8(Pointer p, uint8 u)
{
    *(uint8*)(pageFrameRef_(PageNo(p)) + Offset(p)) = u;
}

void VirtualMem::setUInt16(Pointer p, uint16 u)
{
    ASSERT(IsWordBound(p));
    // p = wordBound(p);
    u = EncodeUInt16_(u);
    *(uint16*)(pageFrameRef_(PageNo(p)) + Offset(p)) = u;
}

void VirtualMem::setUInt32(Pointer p, uint32 u)
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    u = EncodeUInt32_(u);
    *(uint32*)(pageFrameRef_(PageNo(p)) + Offset(p)) = u;
}

void VirtualMem::setPtr(Pointer p, Pointer ptr)
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    ptr = EncodeUInt32_(ptr);
    *(uint32*)(pageFrameRef_(PageNo(p)) + Offset(p)) = ptr;
}

//
// Memory Readings
//
byte VirtualMem::getByte(Pointer p) const
{
    return *(const byte*)(pageFrame_(PageNo(p)) + Offset(p));
}

word VirtualMem::getWord(Pointer p) const
{
    ASSERT(IsWordBound(p));
    // p = wordBound(p);
    return DecodeUInt16_(*(const word*)(pageFrame_(PageNo(p)) + Offset(p)));
}

dword VirtualMem::getDWord(Pointer p) const
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    return DecodeUInt32_(*(const dword*)(pageFrame_(PageNo(p)) + Offset(p)));
}

int8 VirtualMem::getInt8(Pointer p) const
{
    return *(const int8*)(pageFrame_(PageNo(p)) + Offset(p));
}

int16 VirtualMem::getInt16(Pointer p) const
{
    ASSERT(IsWordBound(p));
    // p = wordBound(p);
    return DecodeInt16_(*(const int16*)(pageFrame_(PageNo(p)) + Offset(p)));
}

int32 VirtualMem::getInt32(Pointer p) const
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    return DecodeInt32_(*(const int32*)(pageFrame_(PageNo(p)) + Offset(p)));
}

uint8 VirtualMem::getUInt8(Pointer p) const
{
    return *(const uint8*)(pageFrame_(PageNo(p)) + Offset(p));
}

uint16 VirtualMem::getUInt16(Pointer p) const
{
    ASSERT(IsWordBound(p));
    // p = wordBound(p);
    return DecodeUInt16_(*(const uint16*)(pageFrame_(PageNo(p)) + Offset(p)));
}

uint32 VirtualMem::getUInt32(Pointer p) const
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    return DecodeUInt32_(*(const uint32*)(pageFrame_(PageNo(p)) + Offset(p)));
}

VirtualMem::Pointer VirtualMem::getPtr(Pointer p) const
{
    ASSERT(IsDWordBound(p));
    // p = dwordBound(p);
    return DecodeUInt32_(*(const uint32*)(pageFrame_(PageNo(p)) + Offset(p)));
}


// return the next doubleword-bound block after the place pointed by p
// which occupies a single page
VirtualMem::Pointer VirtualMem::blockStartingAt(
    VirtualMem::Pointer p, int nSize
) const
{
    ASSERT(nSize < PageSize_);
    p = DWordBound(p);
    PageNumber pgNo     = PageNo(p);
    PageOffset pgOffset = Offset(p);
    if ((pgOffset + nSize) & (~OffsetMask_)) {  // page overflow
        // skip to next page
        pgNo++;
        pgOffset = 0;
    }
    return MakePointer(pgNo, pgOffset);
}

// restriction: block must be within a single page (otherwise NULL is returned)
const char* VirtualMem::block_(Pointer p, int nSize) const
{
    p = blockStartingAt(p, nSize);
    return pageFrame_(PageNo(p)) + Offset(p);
}

// similar to VirtualMem::block(), but the reference
char* VirtualMem::blockRef_(Pointer p, int nSize)
{
    p = blockStartingAt(p, nSize);
    return pageFrameRef_(PageNo(p)) + Offset(p);
}

int VirtualMem::blockRead(Pointer p, char buff[], int nBytes) const
{
    memcpy(buff, block_(p, nBytes), nBytes);
    return nBytes;
}

int VirtualMem::blockWrite(Pointer p, const char buff[], int nBytes)
{
    memcpy(blockRef_(p, nBytes), buff, nBytes);
    return nBytes;
}


