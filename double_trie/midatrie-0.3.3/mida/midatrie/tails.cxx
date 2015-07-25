/* tails.cxx
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
// tails.cxx - tail part of the Trie structure
// Created: 12 Feb 2001 (split from trie.cxx)
// Author:  Theppitak Karoonboonyanan
//

#include "tails.h"

//////////////// Implementation of TailBlock class ////////////////
TailBlock::TailBlock()
{
    suffix_ = (TrChar*)0;
    data_   = 0;
}

TailBlock::TailBlock(const TrChar* aSuffix, TrIndex aData)
{
    suffix_ = (TrChar*)0;
    setSuffix(aSuffix);
    setData(aData);
}

TailBlock::~TailBlock()
{
    delete suffix_;
}

void TailBlock::setSuffix(const TrChar* aSuffix)
{
    delete suffix_;
    suffix_ = new TrChar[strlen((const char*)aSuffix)+1];
    strcpy((char*)suffix_, (const char*)aSuffix);
}

void TailBlock::setData(TrIndex aData)
{
    data_ = aData;
}


//////////////// Implementation of Tails class ////////////////
// constructor
Tails::Tails()
: dataHeap_()
{
}

Tails::Tails(const char* fileName, int openModes)
: dataHeap_(fileName, openModes)
{
}

// TAIL Block Access
void Tails::getBlock(MidaState t, TailBlock* pBlock) const
{
    pBlock->setData(getKeyData(t));

    TrChar  buffer[TrMaxKeyLen];
    TrChar* pBuf = buffer;
    VirtualMem::Pointer p = suffixBegin_(t);
    for (TrChar c = dataHeap_.getByte(p); c != 0; c = dataHeap_.getByte(++p)) {
        *pBuf++ = c;
    }
    *pBuf = 0;
    pBlock->setSuffix(buffer);
}

void Tails::setBlock(MidaState t, const TailBlock& aBlock)
{
    setKeyData(t, aBlock.data());

    const TrChar* suffix = aBlock.suffix();
    dataHeap_.blockWrite(
        suffixBegin_(t), (const char*)suffix, strlen((const char*)suffix) + 1
    );
}

// Walking & Retrieval
MidaState Tails::walk(MidaState t, TrChar c, TrWalkResult& rRes) const
{
    int offset = charOffset_(t);
    if (dataHeap_.getByte(suffixBegin_(t) + offset) == c) {
        rRes.setWalkResult((c == TrTerminator) ? Tr_TERMINAL : Tr_INTERNAL);
        rRes.setNWalked(1);
        return makeTailState_(blockBegin_(t), ++offset);
    } else {
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
        return t;
    }
}

// key must be ended with a Terminator
// on return: WalkResult will equal either TERMINAL or CRASH only
MidaState Tails::walk(MidaState t, const TrChar* key, TrWalkResult& rRes) const
{
    int nWalked = 0;
    rRes.setWalkResult(Tr_CRASH);  // if not set to TERMINAL, it CRASHes

    VirtualMem::Pointer pSuffix = suffixBegin_(t);
    int offset = charOffset_(t);
    while (dataHeap_.getByte(pSuffix + offset) == *key) {
        offset++; nWalked++;
        if (*key == TrTerminator) {
            rRes.setWalkResult(Tr_TERMINAL);
            break;
        }
        key++;
    }
    rRes.setNWalked(nWalked);

    return makeTailState_(blockBegin_(t), offset);
}

// Insertion & Deletion
MidaState Tails::allocString(const TrChar* str, TrIndex data)
{
    int blockSize = SuffixOffset_ + strlen((const char*)str) + 1;
    VirtualMem::Pointer p = dataHeap_.newBlock(blockSize);

    // fill out the block
    dataHeap_.setUInt32(p, data);
    dataHeap_.blockWrite(
        p + SuffixOffset_, (const char*)str, strlen((const char*)str) + 1
    );

    return makeTailState_(p, 0);
}

void Tails::changeString(MidaState t, const TrChar* str, TrIndex data)
{
    setKeyData(t, data);
    dataHeap_.blockWrite(
        suffixBegin_(t), (const char*)str, strlen((const char*)str) + 1
    );
}

void Tails::freeBlock(MidaState t)
{
    dataHeap_.deleteBlock(blockBegin_(t));
}



