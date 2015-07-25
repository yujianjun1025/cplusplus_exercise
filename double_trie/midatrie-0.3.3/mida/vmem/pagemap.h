/* pagemap.h
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
// pagemap.h - page map component for VirtualMem implementation
// Created: 9 Feb 2001 (split from vmem.h)
// Author:  Theppitak Karoonboonyanan
//
// Modification log:
//   * 9 Feb 2001 Theppitak Karoonboonyanan
//     - split from vmem.h
//

#ifndef MIDA_VMEM_PAGEMAP_H
#define MIDA_VMEM_PAGEMEP_H

#include "vmem.h"
#include <mida/utils/cstring.h>

// Page Map Table
// --------------
// For quick access to the memory block allocated for a page, the page number
// is used to index into a "page map table" to check if the page has already
// been swapped in. If so, the block is immediately used. If not, the page
// must be swapped in first.
//
// To manage the blocks efficiently, an intermediate "frame map table" is
// defined so that all blocks are registered in this table, and the "pointers"
// from the page map table will indirectly point to their block through
// entries in this frame map table.
//
//         page table                frame table
//      +--------------+          +-------+-------+
//      |              |   +----->|       |       |         page block
//      |              |   |  +-------    |    --------->+--------------+
//      +------+-------+   |  |   +-------+-------+      |              |
//      |Other |    -------+  |   |               |      |              |
//      |Attr. |       |<-----+   |               |      :              :
//      +------+-------+          +---------------+      |              |
//      |              |          |               |      |              |
//      :              :          :               :      +--------------+
//
//

class PageMap {
public:  // public types & constants
    typedef int    FrameNumber;

    enum { NoFrame = FrameNumber(-1) };

    enum {
        MaxPages  = 65535,
        MaxFrames = 200 // 3 // 16
    };

public:  // public functions
    PageMap();
    PageMap(const char* swapFileName, int iosModes);
    ~PageMap();

    // swap file status & operations (same meaning as corresponding ios funcs)
    bool        openSwapFile(const char* swapFileName, int iosModes);
    bool        closeSwapFile();
    const char* swapFileName() const;
    bool        isSwapFileOpen() const;
    bool        isSwapFileGood() const;
    bool        isSwapFileBad()  const;
    bool        isSwapFileFail() const;

    // total pages being used
    VirtualMem::PageNumber  totalPages() const;

    // page access & preparation (returns pointer to page beginning)
    const char* pageFrame(VirtualMem::PageNumber pageNo);
    char*       pageFrameRef(VirtualMem::PageNumber pageNo);

private:  // private types & consts
    struct PageMapEntry_ {
        FrameNumber frameNo;
        unsigned    hitCount;
    };

    struct FrameMapEntry_ {
        VirtualMem::PageNumber pageNo;
        char*                  frame;
        bool                   isChanged;
    };

private:  // private functions
    void        resetTables_();
    FrameNumber validFrameNo_(VirtualMem::PageNumber pageNo); //prep valid frame
    void        retrievePage_(VirtualMem::PageNumber pageNo, char* frameBuff);
    void        flushPage_(VirtualMem::PageNumber pageNo, const char* frameBuff);
    void        flushSwapFile_();
    FrameNumber findFreeFrame_();
    FrameNumber swapPageIn_(VirtualMem::PageNumber pageNo);

private:  // private data
    AutoString             swapFileName_;
    fstream                swapStream_;
    VirtualMem::PageNumber totalPages_;
    PageMapEntry_          pages_[MaxPages];
    FrameMapEntry_         frames_[MaxFrames];
};

// swap file status & operations
inline const char* PageMap::swapFileName() const  { return swapFileName_; }
inline bool PageMap::isSwapFileOpen() const
    { return swapStream_.rdbuf()->is_open() != 0; }
inline bool PageMap::isSwapFileGood() const { return swapStream_.good() != 0; }
inline bool PageMap::isSwapFileBad()  const { return swapStream_.bad()  != 0; }
inline bool PageMap::isSwapFileFail() const { return swapStream_.fail() != 0; }

// total pages being used
inline VirtualMem::PageNumber PageMap::totalPages() const
    { return totalPages_; }

#endif  // MIDA_VMEM_PAGEMAP_H

