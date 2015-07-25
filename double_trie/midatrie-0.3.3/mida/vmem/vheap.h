/* vhecp.h
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
// vheap.h - declaration of a class for virtual heap
// Created: 28 Jun 2000
// Author:  Theppitak Karoonboonyanan
//
// Modification log:
//   * 22 Jul 1996 Theppitak Karoonboonyanan
//     - add class VirtualHeap
//   * 25 Jan 1999 Ananlada Chotimongkol
//     - implement heap pooling
//   * 28 Jun 2000 Theppitak Karoonboonyanan
//     - split VirtualHeap class from vmem.h
//

#ifndef MIDA_VMEM_VHEAP_H
#define MIDA_VMEM_VHEAP_H

#include <mida/vmem/vmem.h>

//////////////
// Contents //
//////////////
class VirtualHeap;

//
// Virtual Heap
// ------------
// Provides a heap of variable-sized blocks upon VirtualMem
// Blocks can be allocated and recycled.
//
// The first 4 bytes of the heap is reserved for keeping the total size of the
// heap being used.
//

class VirtualHeap : public VirtualMem {
public:
    VirtualHeap();
    VirtualHeap(const char* swapFileName, int iosModes);

    bool open(const char* swapFileName, int iosModes);
    bool create(const char* swapFileName);
    bool close();

    Pointer newBlock(int size);
    void    deleteBlock(Pointer p);
    void    printMemoryBlock();

private:
    uint32  getTotalSize_();
    void    setTotalSize_(uint32 s);

//add by moss 25 Jan 99
    void    initHeaders_();

    Pointer getAvail_();
    void    setAvail_(Pointer p);

    Pointer getLast_();
    void    setLast_(Pointer p);

    Pointer getFreeBlock_(int size);    //return pointer to header of block
    void    delFromAvailList_(Pointer prev, Pointer p);
    Pointer insertToAvailList_(Pointer p);
    void    mergeFreeBlock_(Pointer left, Pointer right);
//
};

inline uint32 VirtualHeap::getTotalSize_()           { return getUInt32(0); }
inline void   VirtualHeap::setTotalSize_(uint32 s)       { setUInt32(0, s); }

inline VirtualMem::Pointer VirtualHeap::getAvail_()     { return getPtr(4); }
inline void   VirtualHeap::setAvail_(VirtualMem::Pointer p) { setPtr(4, p); }

inline VirtualMem::Pointer VirtualHeap::getLast_()      { return getPtr(8); }
inline void   VirtualHeap::setLast_(VirtualMem::Pointer p)  { setPtr(8, p); }


#endif // MIDA_VMEM_VHEAP_H

