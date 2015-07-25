/* vheap.cxx
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
// vheap.cxx - implementation of a class for virtual heap
// Created: 28 Jun 2000
// Author:  Theppitak Karoonboonyanan
//
// Modification log:
//   * 22 Jul 1996 Theppitak Karoonboonyanan
//     - add class VirtualHeap
//   * 25 Jan 1999 Ananlada Chotimongkol
//     - implement heap pooling
//   * 28 Jun 2000 Theppitak Karoonboonyanan
//     - split VirtualHeap class from vmem.cxx
//

#include "vheap.h"

//////////////// BlockHeader implementation class ////////////////
//modified by Moss 25 Jan 1999
//revised by Thep 29 Jun 2000

class BlockHeader {
public:
    BlockHeader(VirtualMem& vm, VirtualMem::Pointer p) : vm_(vm), p_(p) {}

    static uint32 HeaderSize();  // dword boundary assured

    // header info accessing
    uint32  blockSize() const;
    void    setBlockSize(uint32 size);

    VirtualMem::Pointer nextAvail() const;
    void    setNextAvail(VirtualMem::Pointer p);

    bool    isUsed() const;
    void    setUsed(bool u);

private:  // private consts
    enum {
        UseMask_  = 0x80000000,
        SizeMask_ = 0x7fffffff
    };

private:  // private data
    VirtualMem&          vm_;
    VirtualMem::Pointer  p_;
};

//
// Block Header Structure :
//
// +-----------------------------------------+
// | usss ssss ssss ssss ssss ssss ssss ssss | (UInt32)
// +-----------------------------------------+
// | nnnn nnnn nnnn nnnn nnnn nnnn nnnn nnnn | (Ptr)
// +-----------------------------------------+
// u      : 0 = free, 1 = used
// ss..ss : block size (unsigned)
// nn..nn : next available node
//
inline uint32  BlockHeader::HeaderSize() {
    return VirtualMem::DWordBound(sizeof(uint32) + sizeof (uint32));
}
inline uint32  BlockHeader::blockSize() const {
    return vm_.getUInt32(p_) & SizeMask_;
}
inline void    BlockHeader::setBlockSize(uint32 size) {
    vm_.setUInt32(p_, (vm_.getUInt32(p_) & UseMask_) | (size & SizeMask_));
}

inline VirtualMem::Pointer BlockHeader::nextAvail() const {
    return vm_.getUInt32(p_+4);
}
inline void    BlockHeader::setNextAvail(VirtualMem::Pointer p) {
    vm_.setUInt32(p_+4, p);
}

inline bool    BlockHeader::isUsed() const {
    return (vm_.getUInt32(p_) & UseMask_) != 0;
}
inline void    BlockHeader::setUsed(bool u) {
    vm_.setUInt32(
        p_, u ? vm_.getUInt32(p_) | UseMask_ : vm_.getUInt32(p_) & ~UseMask_
    );
}

//////////////// implementation of VirtualHeap class ////////////////
VirtualHeap::VirtualHeap()
: VirtualMem()
{
}

VirtualHeap::VirtualHeap(const char* swapFileName, int iosModes)
: VirtualMem()
{
    if (!open(swapFileName, iosModes|ios::nocreate)) {
        create(swapFileName);
    }
}

bool VirtualHeap::open(const char* swapFileName, int iosModes)
{
    return VirtualMem::openSwapFile(swapFileName, iosModes);
}

bool VirtualHeap::create(const char* swapFileName)
{
    if (VirtualMem::openSwapFile(swapFileName, ios::in|ios::out|ios::trunc)) {
        initHeaders_();
        return true;
    }
    return false;
}

bool VirtualHeap::close()
{
    return VirtualMem::closeSwapFile();
}

void VirtualHeap::initHeaders_()
{
    if (VirtualMem::totalPages() == 0) {
        // init total size to 12 bytes (the total size itself + pointer to
        // available list + pointer to last block)
        setTotalSize_(12);
        setAvail_(0);
        setLast_(0);
    }
}

VirtualMem::Pointer VirtualHeap::newBlock(int size)
{
    // plus header, adjust to dword boundary
    size = VirtualMem::DWordBound(size + BlockHeader::HeaderSize());
    VirtualMem::Pointer p = getFreeBlock_(size);
    if (p == 0) {
        //allocate new block of data
        p = blockStartingAt(VirtualMem::Pointer(getTotalSize_()), size);
        Pointer last = getLast_();

        if (p != VirtualMem::Pointer(getTotalSize_())) {
            // p was moved to double word boundary or page boundary
            // adjust size of the previous block in order that pointer to
            // the previous block + its size = pointer to p
            // But if p is first block no adjustment have to be done
            if (last != 0) {
                BlockHeader lastBlock(*this, last);
                //lastBlock.SetBlockSize(lastBlock.BlockSize() + p - totalSize());
                lastBlock.setBlockSize(p - last);
            }
        }

        setTotalSize_(p + size);
        setLast_(p);

        BlockHeader newBlock(*this, p);
        newBlock.setUsed(true);
        newBlock.setBlockSize(size);
        newBlock.setNextAvail(0);
    } else {
        BlockHeader pBlock(*this, p);
        uint32 pBlockSize = pBlock.blockSize();
        ASSERT(VirtualMem::IsDWordBound(pBlockSize));
        if (pBlockSize - size > BlockHeader::HeaderSize()) {
            // use first part of the block
            pBlock.setBlockSize(size);

            // make the rest part a free block
            VirtualMem::Pointer newp = p + size;
            ASSERT(VirtualMem::IsDWordBound(newp));
            BlockHeader newBlock(*this, newp);
            newBlock.setUsed(false);
            newBlock.setBlockSize(pBlockSize - size);
            newBlock.setNextAvail(0);
            insertToAvailList_(newp);
            if (getLast_() == p) {
                setLast_(newp);
            }
        }
#if 0  // Moss' strategy: use last part of the block
        Pointer newp = p + pBlockSize - size;
        if (!isDWordBound(newp)) {
            newp = VirtualMem::dwordBound(newp) - sizeof(dword);
        }
        if (newp - p > BlockHeader::HeaderSize()) {
            // use last part of block
            BlockHeader newBlock(*this, newp);
            newBlock.SetUsed(true);
            newBlock.SetSize(p + pBlockSize - newp);
            newBlock.SetNextAvail(0);

            pBlock.SetBlockSize(newp - p);
            insertToAvailList(p);  //move remaining part to available list
            p = newp;
        }
#endif
   }

   return p + BlockHeader::HeaderSize();
}

void VirtualHeap::deleteBlock(VirtualMem::Pointer p)
{
    p -= BlockHeader::HeaderSize();
    VirtualMem::Pointer prev = insertToAvailList_(p);
    BlockHeader pBlock(*this, p);

    // merge with left and right blocks which are in the same page
    // merge right block
    if (p != getLast_()) {  // check if p is right most block
        VirtualMem::Pointer right = p + pBlock.blockSize();
        BlockHeader rightBlock(*this, right);
        if (!rightBlock.isUsed()
            && VirtualMem::PageNo(p) == VirtualMem::PageNo(right))
        {
            mergeFreeBlock_(p, right);
        }
    }

    // merge left block
    if (prev != 0) {	 // check if p is left most block
        BlockHeader prevBlock(*this, prev);
        if (!prevBlock.isUsed() && (prev + prevBlock.blockSize()) == p
            && VirtualMem::PageNo(prev) == VirtualMem::PageNo(p))
        {
            mergeFreeBlock_(prev, p);
        }
    }
}

VirtualMem::Pointer VirtualHeap::getFreeBlock_(int size)
{
    // FindFreeBlock by using First-Fit strategy
    // and remove it from available list

    VirtualMem::Pointer p = getAvail_();
    VirtualMem::Pointer prev = 0;
    while (p != 0) {
        BlockHeader pBlock(*this, p);
        if (pBlock.blockSize() >= size) {
            delFromAvailList_(prev, p);
            return p;
        }
        prev = p;
        p = pBlock.nextAvail();
    }
    return 0;
}

void VirtualHeap::delFromAvailList_(
    VirtualMem::Pointer prev, VirtualMem::Pointer p
)
{
    BlockHeader pBlock(*this, p);
    if (prev == 0) {
        // remove first block
        setAvail_(pBlock.nextAvail());
    } else {
        BlockHeader prevBlock(*this, prev);
        prevBlock.setNextAvail(pBlock.nextAvail());
    }

    // update pBlock
    pBlock.setUsed(true);
    pBlock.setNextAvail(0);
}

VirtualMem::Pointer VirtualHeap::insertToAvailList_(VirtualMem::Pointer p)
{
    // insert block pointed by p to sorted available list
    // return pointer to previous block of p after the insertion

    BlockHeader pBlock(*this, p);
    VirtualMem::Pointer q = getAvail_();
    VirtualMem::Pointer prev = 0;
    while (q != 0 && q < p) {
        BlockHeader qBlock(*this, q);
        prev = q;
        q = qBlock.nextAvail();
    }
    if (prev == 0) {
        // p will be first block in list
        pBlock.setNextAvail(getAvail_());
        setAvail_(p);
    } else {
        BlockHeader prevBlock(*this, prev);
        pBlock.setNextAvail(prevBlock.nextAvail());
        prevBlock.setNextAvail(p);
    }
    pBlock.setUsed(false);
    return prev;
}

void VirtualHeap::mergeFreeBlock_(
    VirtualMem::Pointer left, VirtualMem::Pointer right
)
{
    BlockHeader leftBlock(*this, left);
    BlockHeader rightBlock(*this, right);
    leftBlock.setBlockSize(leftBlock.blockSize() + rightBlock.blockSize());
    delFromAvailList_(left, right);  // remove right block
    if (getLast_() == right) {
        setLast_(left);
    }
}

void VirtualHeap::printMemoryBlock()
{
    VirtualMem::Pointer p = 12;
    VirtualMem::Pointer last = getLast_();
    for (;;) {
        BlockHeader pBlock(*this, p);
        cerr << (pBlock.isUsed() ? "{" : "[");
        cerr << p << "|" << pBlock.blockSize() << "|" << pBlock.isUsed() << "|"
             << pBlock.nextAvail();
        cerr << (pBlock.isUsed() ? "}" : "]") << endl;;
        if (p == last) { break; }
        p = p + pBlock.blockSize();
    }

    //print avail list
    cerr << "available list" << endl;
    p = getAvail_();
    while (p != 0) {
        BlockHeader pBlock(*this, p);
        cerr << p << "|" << pBlock.blockSize() << "|" << pBlock.isUsed() << "|"
             << pBlock.nextAvail() << endl;
        p = pBlock.nextAvail();
    }
}

