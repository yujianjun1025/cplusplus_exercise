/* dataheap.h
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
// dataheap.h - variable-sized data heap on VirtualMem
// Created: 31 Jul 1996 (splitted from dict.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_VMEM_DATAHEAP_H
#define MIDA_VMEM_DATAHEAP_H

#include <mida/vmem/vheap.h>

////////////////
//  Contents  //
////////////////

class DataHeap;
class HeapAssociated;
class   CharBlock;


//
// DataHeap
// --------
//
typedef uint32 DataPtr;

class DataHeap : public VirtualHeap {
public:
    DataHeap(const char* swapFileName, int iosModes);
};

inline DataHeap::DataHeap(const char* swapFileName, int iosModes)
: VirtualHeap(swapFileName, iosModes) {}


//
// HeapAssociated
// --------------
//
class HeapAssociated {
public:
    DataHeap& getDataHeap() const { return dataHeap_; }
    VirtualMem::Pointer  getEntryPtr() const { return entry_; }

    void      attach(VirtualMem::Pointer aEntry);
    VirtualMem::Pointer  detach();

    void      destroy();        // delete the object in heap

protected:
    // never directly instantiate this class
    HeapAssociated(DataHeap& aHeap);   // Default c-tor
    // Associating c-tor
    HeapAssociated(DataHeap& aHeap, VirtualMem::Pointer aEntry);

    void      create(int size);  // to be called by derived class' creating func

private:
    DataHeap&            dataHeap_;
    VirtualMem::Pointer  entry_;
};

inline HeapAssociated::HeapAssociated(DataHeap& aHeap)
: dataHeap_(aHeap), entry_(0)  {}

inline HeapAssociated::HeapAssociated(
    DataHeap& aHeap, VirtualMem::Pointer aEntry
) : dataHeap_(aHeap), entry_(aEntry)  {}

inline void HeapAssociated::attach(VirtualMem::Pointer aEntry)
{
    entry_ = aEntry;
}
inline VirtualMem::Pointer HeapAssociated::detach()
{
    VirtualMem::Pointer prvEntry = entry_;
    entry_ = 0;
    return prvEntry;
}


//
// CharBlock
// ---------
//

class CharBlock : public HeapAssociated {
public:
    static int PrecalcSize(const char* str);

public:
    CharBlock(DataHeap& aHeap);                   // default c-tor
    CharBlock(DataHeap& aHeap, const char* str);  // creating c-tor
    CharBlock(DataHeap& aHeap, VirtualMem::Pointer aEntry); // associating c-tor

    int  blockSize();
    int  stringLen();

    const char* getString(char buff[], int buffSize);
    void        setString(const char* str);
};


#endif  // MIDA_VMEM_DATAHEAP_H
