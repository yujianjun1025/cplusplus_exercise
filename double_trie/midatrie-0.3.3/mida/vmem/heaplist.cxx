/* heaplist.cxx
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
// heaplist.cxx - linked list on VirtualMem
// Created: 31 Jul 1996 (splitted from dict.cpp)
// Author:  Theppitak Karoonboonyanan
//

#include "heaplist.h"

//////////////// Implementation of LinkHeap class ////////////////
VirtualMem::Pointer LinkHeap::CellBegin_(LinkPtr n)
{
    uint32 pageNo = n / CellsPerPage_();
    uint32 elmNo  = n % CellsPerPage_();
    return pageNo * VirtualMem::PageSize() + elmNo * sizeof(LinkCell_);
}

LinkHeap::LinkHeap(const char* swapFileName, int iosModes)
: VirtualMem(swapFileName, iosModes)
{
    totalCells_ = VirtualMem::totalPages() * CellsPerPage_();
    if (totalCells_ == 0) {
        // free-space list head
        setFreeListHead_(0);
        //SetNext(0, 0);
        setData(0, 0);
        // so, totalCells is now...
        totalCells_ = 1;

        extend_();
    }
}

void LinkHeap::extend_()
{
    // ASSERT(PageSize() % sizeof(LinkCell) == 0);
    int cellsToAdd = int(CellsPerPage_() - (totalCells_ % CellsPerPage_()));

    // init new free-list
    LinkPtr i;
    for (i = totalCells_; i < totalCells_ + cellsToAdd - 1; i++) {
        setNext(i, i+1);
        setHashVal(i, 0);
        setData(i, 0);
    }

    // add it to the top of free-list
    setNext(i, freeListHead_());
    setFreeListHead_(totalCells_);

    // count the cells
    totalCells_ += cellsToAdd;
}

LinkPtr LinkHeap::newCell(HashType hash, DataPtr data)
{
    // try to get a free cell from the free-list
    LinkPtr theCell = freeListHead_();
    if (theCell == 0) {
        // free-list is empty, extend the heap
        extend_();
        theCell = freeListHead_();
    }
    ASSERT(theCell != 0);

    // remove and return it
    setFreeListHead_(next(theCell));

    setNext(theCell, 0);
    setHashVal(theCell, hash);
    setData(theCell, data);

    return theCell;
}

void LinkHeap::deleteCell(LinkPtr p)
{
    // push the cell into the free-list
    setNext(p, freeListHead_());
    setFreeListHead_(p);
}


//////////////// Implementation of HeapedList class ////////////////

// New list creator
void HeapedList::createNew()
{
    HeapAssociated::create(ENTRYSIZE);
    empty();
}

// just copy the list pointer
void HeapedList::shallowCopyTo(VirtualMem::Pointer pNewLocation)
{
    getDataHeap().setUInt32(pNewLocation + TAILOFFSET, tail());
}

LinkPtr HeapedList::tail_()
{
    return getDataHeap().getUInt32(getEntryPtr() + TAILOFFSET);
}

void HeapedList::setTail_(LinkPtr t)
{
    getDataHeap().setUInt32(getEntryPtr() + TAILOFFSET, t);
}

LinkPtr HeapedList::head_()
{
    LinkPtr t = tail_();
    return (t == 0) ? 0 : linkHeap_.next(t);
}

void HeapedList::setHead_(LinkPtr h)
{
    LinkPtr t = tail_();
    ASSERT(t != 0);
    linkHeap_.setNext(t, h);
}

LinkPtr HeapedList::circularNext(LinkPtr p)
{
    ASSERT(p != 0);
    return linkHeap_.next(p);
}

LinkPtr HeapedList::circularBefore(LinkPtr p)
{
    ASSERT(p != 0);
    LinkPtr b = p;
    while (circularNext(b) != p) {
        b = circularNext(b);
    }
    return b;
}

LinkPtr HeapedList::linearNext(LinkPtr p)
{
    ASSERT(p != 0);
    return (p == tail_()) ? 0 : circularNext(p);
}

LinkPtr HeapedList::linearBefore(LinkPtr p)
{
    ASSERT(p != 0);
    LinkPtr b = head_();
    ASSERT(b != 0);
    if (b == p)  { return 0; }  // none is before head

    while (b != 0 && linearNext(b) != p) {
        b = linearNext(b);
    }

    return b;
}

void HeapedList::insertAfterTail_(LinkPtr p)
{
    LinkPtr theTail = tail_();
    if (theTail == 0) {
        linkHeap_.setNext(p, p);
        setTail_(p);
    } else {
        linkHeap_.setNext(p, circularNext(theTail));
        linkHeap_.setNext(theTail, p);
    }
}

void HeapedList::addHead(LinkPtr p)
{
    // the node next to tail is head
    insertAfterTail_(p);
}

void HeapedList::addTail(LinkPtr p)
{
    insertAfterTail_(p);
    setTail_(p);
}

void HeapedList::insertAfter(LinkPtr p, LinkPtr q)
{
    linkHeap_.setNext(q, circularNext(p));
    linkHeap_.setNext(p, q);
    if (p == tail_()) {
        setTail_(q);
    }
}

LinkPtr HeapedList::removeHead()
{
    LinkPtr h = head_();
    if (h != 0) {
        LinkPtr t = tail_();
        if (t == h) {  // it's the last cell left
            setTail_(0);
        } else {
            linkHeap_.setNext(t, circularNext(h));
        }
        linkHeap_.setNext(h, 0);
    }
    return h;
}

LinkPtr HeapedList::removeTail()
{
    LinkPtr t = tail_();
    if (t != 0) {
        LinkPtr b = linearBefore(t);
        if (b == 0) {  // it's the last cell left
            setTail_(0);
        } else {
            linkHeap_.setNext(b, circularNext(t));
            setTail_(b);
        }
        linkHeap_.setNext(t, 0);
    }
    return t;
}

LinkPtr HeapedList::removeCell(LinkPtr p)
{
    if (p == tail_()) {
        return removeTail();
    } else {
        LinkPtr b = circularBefore(p);
        ASSERT(b != p);  // b == p  iff.  p == tail_()
        linkHeap_.setNext(b, linkHeap_.next(p));
        linkHeap_.setNext(p, 0);
        return p;
    }
}

void HeapedList::deleteAll()
{
    for (LinkPtr p = head(); p != 0; ) {
        LinkPtr q = linearNext(p);
        linkHeap_.deleteCell(p);
        p = q;
    }
    setTail_(0);
}

int HeapedList::elementCount()
{
    int count = 0;
    for (LinkPtr p = head(); p != 0; p = linearNext(p)) {
        count++;
    }
    return count;
}

