/* heaplist.h
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
// heaplist.h - linked list on VirtualMem
// Created: 31 Jul 1996 (splitted from dict.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_VMEM_HEAPLIST_H
#define MIDA_VMEM_HEAPLIST_H

#include <mida/utils/hash.h>
#include <mida/vmem/dataheap.h>

////////////////
//  Contents  //
////////////////

class LinkHeap;
class HeapedList;


//
// LinkHeap
// --------
//
typedef uint32 LinkPtr;

class LinkHeap : public VirtualMem {
public:
    LinkHeap(const char* swapFileName, int iosModes);

    LinkPtr  newCell(HashType hash, DataPtr data);
    void     deleteCell(LinkPtr p);

    // link node access
    LinkPtr  next(LinkPtr n);
    HashType hashVal(LinkPtr n);
    DataPtr  data(LinkPtr n);
    void     setNext(LinkPtr n, LinkPtr newVal);
    void     setHashVal(LinkPtr n, HashType newVal);
    void     setData(LinkPtr n, DataPtr newVal);

private:  // private types & enums
    struct LinkCell_ {
        LinkPtr  next;
        HashType hash;
        DataPtr  data;
    };

    enum {
        NextOffset = 0,
        HashOffset = NextOffset + sizeof(LinkPtr),
        DataOffset = HashOffset + sizeof(HashType)
    };

private:  // private functions
    // address calculation aid
    static  VirtualMem::Pointer CellBegin_(LinkPtr n);

    // free list manipulations
    static  int  CellsPerPage_();
    LinkPtr freeListHead_();
    void    setFreeListHead_(LinkPtr newVal);
    void    extend_();

private:  // private data
    LinkPtr totalCells_;
};

inline LinkPtr LinkHeap::next(LinkPtr n)
{
    ASSERT(sizeof(LinkPtr) == 4);
    return getUInt32(CellBegin_(n) + NextOffset);
}
inline HashType LinkHeap::hashVal(LinkPtr n)
{
    ASSERT(sizeof(HashType) == 4);
    return getUInt32(CellBegin_(n) + HashOffset);
}
inline DataPtr LinkHeap::data(LinkPtr n)
{
    ASSERT(sizeof(DataPtr) == 4);
    return getUInt32(CellBegin_(n) + DataOffset);
}

inline void LinkHeap::setNext(LinkPtr n, LinkPtr newVal)
{
    ASSERT(sizeof(LinkPtr) == 4);
    setUInt32(CellBegin_(n) + NextOffset, newVal);
}
inline void LinkHeap::setHashVal(LinkPtr n, HashType newVal)
{
    ASSERT(sizeof(HashType) == 4);
    setUInt32(CellBegin_(n) + HashOffset, newVal);
}
inline void LinkHeap::setData(LinkPtr n, DataPtr newVal)
{
    ASSERT(sizeof(DataPtr) == 4);
    setUInt32(CellBegin_(n) + DataOffset, newVal);
}

inline int LinkHeap::CellsPerPage_()
{
    return VirtualMem::PageSize() / sizeof(LinkCell_);
}
inline LinkPtr  LinkHeap::freeListHead_()              { return next(0); }
inline void LinkHeap::setFreeListHead_(LinkPtr newVal) { setNext(0, newVal); }


//
// HeapedList (Circular List)
// ----------
// Implemented as circular list, HeapedList is a class to manipulate lists
// stored in heap. There are two relevant heaps: DataHeap and LinkHeap.
// Information stored in DataHeap is the list heading information, such as
// head, tail, etc. (in this case, only tail is stored), while that in
// LinkHeap is the linked-list nodes, of which size are fixed.
//
// Data Structure:
//
//                   +--------+---+
//                   | data 1 |   |<--+
//                   +--------+-|-+   |
//                              V     |
//                   +--------+---+   |
//                   | data 2 |   |   |
//                   +--------+-|-+   |
//                              V     |
//    +------+       +--------+---+   |
//    | tail |------>| data 3 |  -+---+
//    +------+       +--------+---+
//  (in DataHeap)    (in LinkHeap)
//
class HeapedList : public HeapAssociated {
public:  // public constants
    enum {
        ENTRYSIZE  = 4,
        TAILOFFSET = 0   // LinkPtr tail;
    };

public:  // public functions
    // Default c-tor
    HeapedList(LinkHeap& linkHeap, DataHeap& dataHeap);
    // Creation Function (in place of creating c-tor)
    void createNew();
    // Associating c-tor
    HeapedList(
        LinkHeap& linkHeap, DataHeap& dataHeap, VirtualMem::Pointer aEntry
    );

    LinkHeap& getLinkHeap() const;
    DataHeap& getDataHeap() const;

    LinkPtr head();
    LinkPtr tail();
    bool    isEmpty();
    int     elementCount();

    // relocation
    void    shallowCopyTo(VirtualMem::Pointer pNewLocation);

    // for circular iteration
    LinkPtr circularNext(LinkPtr p);
    LinkPtr circularBefore(LinkPtr p);

    // for linear iteration
    LinkPtr linearNext(LinkPtr p);
    LinkPtr linearBefore(LinkPtr p);

    // cell allocation
    LinkPtr newCell(HashType hash, DataPtr d);

    // list updatings
    void    addHead(LinkPtr p);
    void    addTail(LinkPtr p);
    void    insertAfter(LinkPtr p, LinkPtr q);
    LinkPtr removeHead();
    LinkPtr removeTail();
    LinkPtr removeCell(LinkPtr p);
    void    empty();
    void    deleteAll();

    // cell accessing
    DataPtr data(LinkPtr p);
    void    setData(LinkPtr l, DataPtr d);

private:  // private functions
    // member access functions
    LinkPtr head_();
    LinkPtr tail_();
    void    setHead_(LinkPtr h);
    void    setTail_(LinkPtr t);

    // circular list insertion
    void insertAfterTail_(LinkPtr p);

private:  // private data
    LinkHeap& linkHeap_;
};

// Default c-tor
inline HeapedList::HeapedList(LinkHeap& linkHeap, DataHeap& dataHeap)
: HeapAssociated(dataHeap), linkHeap_(linkHeap)  {}

// Associating c-tor
inline HeapedList::HeapedList(
    LinkHeap& linkHeap, DataHeap& dataHeap, VirtualMem::Pointer aEntry
) : HeapAssociated(dataHeap, aEntry), linkHeap_(linkHeap)  {}

inline LinkPtr HeapedList::newCell(HashType hash, DataPtr d)
{
    return linkHeap_.newCell(hash, d);
}

inline LinkHeap& HeapedList::getLinkHeap() const
{
    return linkHeap_;
}
inline DataHeap& HeapedList::getDataHeap() const
{
    return HeapAssociated::getDataHeap();
}

inline LinkPtr HeapedList::head()  { return head_(); }
inline LinkPtr HeapedList::tail()  { return tail_(); }

inline bool HeapedList::isEmpty()  { return tail_() != 0; }
inline void HeapedList::empty()    { setTail_(0); }

inline DataPtr HeapedList::data(LinkPtr l)
{
    return linkHeap_.data(l);
}
inline void    HeapedList::setData(LinkPtr l, DataPtr d)
{
    linkHeap_.setData(l, d);
}


#endif  // MIDA_VMEM_HEAPLIST_H

