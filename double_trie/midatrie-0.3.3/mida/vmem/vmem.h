/* vmem.h
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
// vmem.h - declaration of a class for virtual memory
// Created: 23 May 1996
// Author:  Theppitak Karoonboonyanan
//
// Modification log:
//   * 22 Jul 1996 Theppitak Karoonboonyanan
//     - add class VirtualHeap
//   * 25 Jan 1999 Ananlada Chotimongkol
//     - implement heap pooling
//   * 28 Jun 2000 Theppitak Karoonboonyanan
//     - split VirtualHeap class to vheap.h
//

#ifndef MIDA_VMEM_VMEM_H
#define MIDA_VMEM_VMEM_H

#include <fstream.h>
#include <mida/utils/typedefs.h>

//////////////
// Contents //
//////////////
class VirtualMem;

//////////////////
// External Ref //
//////////////////
class PageMap;

//
// Virtual Memory
// --------------
//
// [History]: This class was originally for speeding up disk access operations;
// that is, it should have been named DiskCache or so on. But, for the sake of
// syntactic convenience in using it, it was then reconceptualized as a virtual
// memory. The idea was just in reverse: instead of viewing it as data
// retrieval from the disk, we can imagine a large block of memory we wish to
// use, which is then stored in the disk file, but all its parts are always
// ready for us to access, by some magic. And by setting some parameters, disk
// caching is its side-effect.
//
// The organization of the class is similar to that of virtual memory in
// operating systems (See Deitel, Operating Systems, Addison-Wesley, 1990)
//
// An implementation detail is that, to avoid accessing blocks which overlap
// the boundary between any two pages, page size must be multiple of doubleword
// size, and when accessing words or doublewords, their address must be
// word-aligned and doubleword-aligned respectively. And, when accessing a
// large block, it must not lie in more than one page.
//
// Another enhancement is for data portability. Since there are two different
// systems of storing integers: high-endian (such as Motorola 68xxx, Sun Sparc)
// and low-endian (such as Intel 80x86 and Pentium), we must select one to
// use with our swap files, so that they can be transferred between systems.
// In this implementation, we choose low-endian because the program is intended
// to be run upon Intel family microprocessors.
//

class VirtualMem {
public: // public typedefs and consts
    typedef uint32 Pointer;
    typedef uint16 PageNumber;
    typedef uint16 PageOffset;

    enum { NoPage  = PageNumber(~0) };

public:  // public functions
    VirtualMem();
    VirtualMem(const char* swapFileName, int iosModes);
    ~VirtualMem();

    // Swap file status (same meaning as the corresponding ios funcs)
    bool        openSwapFile(const char* swapFileName, int iosModes);
    bool        closeSwapFile();
    const char* swapFileName() const;
    bool        isSwapFileOpen() const;
    bool        isSwapFileGood() const;
    bool        isSwapFileBad()  const;
    bool        isSwapFileFail() const;

    // Parameters
    static int PageSize();

    // total pages being used
    PageNumber totalPages() const;

    // Memory Writing
    void setByte  (Pointer p, byte   b);
    void setWord  (Pointer p, word   w);
    void setDWord (Pointer p, dword  d);
    void setInt8  (Pointer p, int8   n);
    void setInt16 (Pointer p, int16  n);
    void setInt32 (Pointer p, int32  n);
    void setUInt8 (Pointer p, uint8  u);
    void setUInt16(Pointer p, uint16 u);
    void setUInt32(Pointer p, uint32 u);
    void setPtr   (Pointer p, Pointer ptr);

    // Memory Reading
    byte    getByte(Pointer p) const;
    word    getWord  (Pointer p) const;
    dword   getDWord (Pointer p) const;
    int8    getInt8  (Pointer p) const;
    int16   getInt16 (Pointer p) const;
    int32   getInt32 (Pointer p) const;
    uint8   getUInt8 (Pointer p) const;
    uint16  getUInt16(Pointer p) const;
    uint32  getUInt32(Pointer p) const;
    Pointer getPtr   (Pointer p) const;

    // Block Address Calculation
    Pointer blockStartingAt(Pointer p, int nSize) const;

    // Block Access
    int blockRead(Pointer p, char buf[], int nBytes) const;
    int blockWrite(Pointer p, const char buf[], int nBytes);

public:
    // address alignments
    static bool    IsWordBound(Pointer p);
    static bool    IsDWordBound(Pointer p);
    static Pointer WordBound(Pointer p);
    static Pointer DWordBound(Pointer p);

    // address conversions
    static PageNumber PageNo(Pointer p);
    static PageOffset Offset(Pointer p);
    static Pointer    MakePointer(PageNumber pgNo, PageOffset offset);

private:  // private types
    enum {
        // offset extractions
        OffsetBits_ = 12,    // 4KB per page
        OffsetMask_ = ~((~0L)<<OffsetBits_),
        PageSize_   = (1L)<<OffsetBits_,

        // offset alignments
        WordBoundMask_  = (~0L)<<1,
        DWordBoundMask_ = (~0L)<<2
    };

private:  // private functions
    // binary data storage bottle-neck
    static uint16 EncodeUInt16_(uint16 n);
    static uint32 EncodeUInt32_(uint32 n);
    static int16  EncodeInt16_(int16 n);
    static int32  EncodeInt32_(int32 n);

    static uint16 DecodeUInt16_(uint16 n);
    static uint32 DecodeUInt32_(uint32 n);
    static int16  DecodeInt16_(int16 n);
    static int32  DecodeInt32_(int32 n);

    // page preparations for memory access
    const char* pageFrame_(PageNumber pageNo) const;
    char*       pageFrameRef_(PageNumber pageNo);

    // virtual memory block preparations
    const char* block_(Pointer p, int nSize) const;
    char*       blockRef_(Pointer p, int nSize);

private:  // private data
    PageMap*  pPgMap_;
};

// parameters
inline int VirtualMem::PageSize()  { return PageSize_; }

// pointer calculations
inline VirtualMem::PageNumber VirtualMem::PageNo(Pointer p)
{
    return PageNumber(p >> OffsetBits_);
}
inline VirtualMem::PageOffset VirtualMem::Offset(Pointer p)
{
    return PageOffset(p & OffsetMask_);
}
inline VirtualMem::Pointer VirtualMem::MakePointer(
    VirtualMem::PageNumber pgNo, VirtualMem::PageOffset offset)
{
    return Pointer((pgNo << OffsetBits_) | (offset & OffsetMask_));
}

// returns next word-aligned address
inline bool VirtualMem::IsWordBound(Pointer p)
    { return (p & ~WordBoundMask_) == 0; }
inline bool VirtualMem::IsDWordBound(Pointer p)
    { return (p & ~DWordBoundMask_) == 0; }
inline VirtualMem::Pointer VirtualMem::WordBound(VirtualMem::Pointer p)
    { return (p + sizeof(word) - 1) & WordBoundMask_; }
// returns next doubleword-aligned address
inline VirtualMem::Pointer VirtualMem::DWordBound(VirtualMem::Pointer p)
    { return (p + sizeof(dword) - 1) & DWordBoundMask_; }


#endif // MIDA_VMEM_VMEM_H
