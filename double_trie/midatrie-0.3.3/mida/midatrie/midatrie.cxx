/* midatrie.cxx
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
// midatrie.cxx - multi-index double-array trie implementation
// Created: 30 May 1996
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "midatrie.h"
#include <mida/utils/fileutil.h>


//////////////// Implementation of MidaTrie class ////////////////
MidaTrie::ErrorCode
MidaTrie::open(const char* dir, const char* trieName, int openModes)
{
    if (!branches_.open(FullPath(dir, trieName, "br"), openModes)) {
        return BranchesError;
    }
    if (!tails_.open(FullPath(dir, trieName, "tl"), openModes)) {
        return TailsError;
    }
    return NoError;
}

MidaTrie::ErrorCode
MidaTrie::create(int nIndices, const char* dir, const char* trieName)
{
    if (!branches_.create(nIndices, FullPath(dir, trieName, "br"))) {
        return BranchesError;
    }
    if (!tails_.create(FullPath(dir, trieName, "tl"))) {
        return TailsError;
    }
    return NoError;
}

MidaTrie::ErrorCode
MidaTrie::close()
{
    if (!branches_.close()) {
        return BranchesError;
    }
    if (!tails_.close()) {
        return TailsError;
    }
    return NoError;
}

bool MidaTrie::isOpen() const
{
    return branches_.isSwapFileOpen() && tails_.isSwapFileOpen();
}

int MidaTrie::getNIndices() const
{
    return branches_.getNIndices();
}

ITrieState* MidaTrie::startState(int indexNo) const
{
    return new MidaTrieState(*this, indexNo);
}

MidaState MidaTrie::startStateImpl(int indexNo) const
{
    return (indexNo >= getNIndices()) ?
               TrErrorIndex : Branches::RootState(indexNo);
}

int MidaTrie::outDegree(MidaState s) const
{
    if (isTerminal_(s)) { return 0; }

    if (s > 0) {
        // if s is separate node, it's equivalent to a TAIL state
        return branches_.isSeparateNode(s) ? 1 : branches_.outDegree(s);
    }
    return (s < 0) ? 1 : 0;
}

TrSymbolSet MidaTrie::outputSet(MidaState s) const
{
    if (isTerminal_(s)) { return TrSymbolSet(); }

    // if s is separate node, jump to TAIL first
    if (isSeparate_(s)) {
        s = branches_.tailState(s);
    }

    if (s > 0) {
        return branches_.outputSet(s);
    } else if (s < 0) {
        return TrSymbolSet(1, tails_.nextSymbol(-s));
    } else {
        return TrSymbolSet();
    }
}

//
// MidaTrie::walk() with a transition character
//
MidaState MidaTrie::walk(MidaState s, TrChar c, TrWalkResult& rRes) const
{
#ifndef NOTERMINATOR_OPT
    if (isTerminal_(s)) {
        // crash
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
        return s;
    }
#endif  // NOTERMINATOR_OPT

    // if s is separate node, jump to TAIL first
    if (isSeparate_(s)) {
        s = branches_.tailState(s);
    }

    // walk according to state type
    if (s > 0) {
        s = branches_.walk(s, c, rRes);
    } else if (s < 0) {
        s = -tails_.walk(-s, c, rRes);
    } else {
        // crash
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
    }

    return s;
}

//
// MidaTrie::walk() with a key as string
// returns the farthest state that the key can reach
// behavior on walkResult: (similar to Branches::walk() and Tails::walk())
//    CRASH    - returns the state before crashing
//    TERMINAL - returns the terminal state
//    INTERNAL - impossible
//
MidaState MidaTrie::walk(MidaState s, const TrChar* key, TrWalkResult& rRes) const
{
    int nWalked = 0;
    const TrChar* p = key;

#ifndef NOTERMINATOR_OPT
    if (isTerminal_(s)) {
        // crash
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
        return s;
    }
#endif  // NOTERMINATOR_OPT

    // walk through multi-nodes
    if (s > 0) {
        while (!branches_.isSeparateNode(s)) {
            s = branches_.walk(s, *p, rRes);
            if (rRes.getWalkResult() == Tr_CRASH)  return s;
            p++; rRes.setNWalked(++nWalked);
            if (rRes.getWalkResult() == Tr_TERMINAL)  return s;
        }
        s = branches_.tailState(s);
    }

    // walk through single-nodes
    if (s < 0) {
        TrWalkResult r;
        s = -tails_.walk(-s, p, r);
        rRes.setWalkResult(r.getWalkResult());
        rRes.setNWalked(nWalked += r.getNWalked());
    } else {
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
    }

    return s;
}

TrIndex MidaTrie::getKeyData(MidaState s) const
{
#ifndef NOTERMINATOR_OPT
    if (isTerminal_(s)) {
        TrIndex ret = (s > 0) ? branches_.getKeyData(s) : tails_.getKeyData(-s);
        return ret;
//        return (s > 0) ? branches_.getKeyData(s) : tails_.getKeyData(-s);
    }
#else
    if (isSeparate_(s)) {
        s = branches_.tailState(s);
    }

    if (s < 0 && tails_.isTerminal(-s)) {
        return tails_.getKeyData(-s);
    } else {
        return TrErrorIndex;
    }
#endif // NOTERMINATOR_OPT
}

// Aoe's A_INSERT
void MidaTrie::branchWithinBranches_(
    MidaState from, const TrChar* suffix, TrIndex dataIdx
)
{
    MidaState newState = branches_.insertBranch(from, *suffix);
#ifndef NOTERMINATOR_OPT
    if (*suffix == TrTerminator) {
        // it's an immediate terminal node
        branches_.setKeyData(newState, dataIdx);
    } else {
        // link it to a TAIL block
        branches_.setKeyData(newState, tails_.allocString(suffix+1, dataIdx));
    }
#else
    branches_.setKeyData(newState, tails_.allocString(suffix+1, dataIdx));
#endif  // NOTERMINATOR_OPT

#ifdef TRIE_DEBUG
    branches_.AssertValid();
#endif  // TRIE_DEBUG
}

// Aoe's B_INSERT
void MidaTrie::branchWithinTail_(
    MidaState sepNode, const TrChar* suffix, TrIndex dataIdx
)
{
    ASSERT(isSeparate_(sepNode));

    // get previous TAIL info
    MidaState oldTail = -branches_.tailState(sepNode);
    TailBlock oldTailBlock;
    tails_.getBlock(oldTail, &oldTailBlock);

    // insert chain of common nodes
    const TrChar* p = oldTailBlock.suffix();
    MidaState s = sepNode;
    while (*p == *suffix) {
        s = branches_.insertBranch(s, *p);
        p++;  suffix++;
    }

    // insert back the remaining of old TAIL block
#ifndef NOTERMINATOR_OPT
    if (*p == TrTerminator) {
        // free old tail block
        tails_.freeBlock(oldTail);
        // insert immediate terminal node in branches
        MidaState t = branches_.insertBranch(s, TrTerminator);
        branches_.setKeyData(t, oldTailBlock.data());
    } else {
        // cut remaining TAIL and insert back
        MidaState t = branches_.insertBranch(s, *p);
        tails_.changeString(oldTail, p+1, oldTailBlock.data());
        branches_.setKeyData(t, oldTail);
    }
#else
    // cut remaining TAIL and insert back
    MidaState t = branches_.insertBranch(s, *p);
    tails_.changeString(oldTail, p+1, oldTailBlock.data());
    branches_.setKeyData(t, oldTail);
#endif  // NOTERMINATOR_OPT

    // then, insert the new branch
    branchWithinBranches_(s, suffix, dataIdx);
}

bool MidaTrie::insertKey(int indexNo, const TrChar* key, TrIndex dataIdx)
{
    if (indexNo >= getNIndices()) { return false; }

    MidaState s = startStateImpl(indexNo);
    TrWalkResult r;
    const TrChar* p = AppendTerminator(key);

    // walk through multi-nodes
    do {
        s = branches_.walk(s, *p, r);
        switch (r.getWalkResult()) {
            case Tr_CRASH: {
                // A_INSERT(s, p);
                branchWithinBranches_(s, p, dataIdx);
                return true;
            }

            case Tr_TERMINAL: {
                ASSERT(*p == TrTerminator);
                //duplicated key, overwrite data index
#ifndef NOTERMINATOR_OPT
                branches_.setKeyData(s, dataIdx);
#else
                ASSERT(isSeparate_(s));
                tails_.setKeyData(-branches_.tailState(s), dataIdx);
#endif  // NOTERMINATOR_OPT
                return true;
            }
        }
        p++;
    } while (!isSeparate_(s));

    // continue walking on single-nodes
    MidaState t = -tails_.walk(-branches_.tailState(s), p, r);
    switch (r.getWalkResult()) {
        case Tr_CRASH: {
            // B_INSERT(s, p);
            branchWithinTail_(s, p, dataIdx);
            return true;
        }

        case Tr_TERMINAL: {
            ASSERT(p[r.getNWalked()-1] == TrTerminator);
            // duplicated key, overwrite data index
            tails_.setKeyData(-t, dataIdx);
            return true;
        }
    }

    ASSERT(false);
    return false;
}

bool MidaTrie::deleteKey(int indexNo, const TrChar* key)
{
    if (indexNo >= getNIndices()) { return false; }

    MidaState s = startStateImpl(indexNo);
    TrWalkResult r;
    const TrChar* p = AppendTerminator(key);

    // walk to separate or terminal node
    do {
        s = branches_.walk(s, *p, r);
        switch (r.getWalkResult()) {
            case Tr_CRASH:    return false;
#ifndef NOTERMINATOR_OPT
            case Tr_TERMINAL: goto   DELETE_BRANCHES;
#else
            case Tr_TERMINAL:
                tails_.freeBlock(-branches_.tailState(s));
                goto DELETE_BRANCHES;
#endif  // NOTERMINATOR_OPT
        }
        p++;
    } while (!isSeparate_(s));

    // for separate node, delete TAIL block if key exists
    {
        MidaState t = -branches_.tailState(s);
        tails_.walk(t, p, r);
        switch (r.getWalkResult()) {
            case Tr_CRASH:    return false;
            case Tr_TERMINAL: tails_.freeBlock(t); break;
            default:       ASSERT(false);
        }
    }

DELETE_BRANCHES:
    // delete relevant branches
    do {
        MidaState parent = branches_.parentOf(s);
        branches_.deleteNode(s);
        s = parent;
    } while (s != startStateImpl(indexNo) && branches_.outDegree(s) == 0);

    return true;
}

TrIndex MidaTrie::retrieve(int indexNo, const TrChar* key) const
{
    if (indexNo >= getNIndices()) { return TrErrorIndex; }

    TrWalkResult r;
    const TrChar* p = AppendTerminator(key);
    MidaState s = walk(startStateImpl(indexNo), p, r);
    return (r.getWalkResult() == Tr_TERMINAL) ? getKeyData(s) : TrErrorIndex;
}

bool MidaTrie::enumKeys_(MidaState s, int level, IMiTrie::EnumFunc fn) const
{
    static TrChar theWord[256];
    TrSymbolSet outputs(outputSet(s));
    TrWalkResult r;
    for (TrSymbolSet::SymbolIterator i = outputs.first();
         !i.isDone(); i.next())
    {
        theWord[level] = i.currentItem();
        MidaState t = walk(s, i.currentItem(), r);
        ASSERT(r.getWalkResult() != Tr_CRASH);
        if (r.getWalkResult() == Tr_TERMINAL) {
            theWord[level] = '\0';
            return fn && (*fn)(theWord, getKeyData(t));
        } else {
            if (!enumKeys_(t, level+1, fn)) {
                return false;
            }
        }
    }
    return true;
}

void MidaTrie::enumerate(MidaState s, IMiTrie::EnumFunc fn) const
{
    enumKeys_(s, 0, fn);
}

// reallocate all TAIL  blocks to remove garbage in TAIL heap
void MidaTrie::reallocTails_()
{
    const char* oldTailsName = tails_.swapFileName();
    const char* tmpTailsName = tmpnam(NULL);

    Tails* pNewTails = new Tails(tmpTailsName, ios::in | ios::out);

    // read all TAIL blocks and allocate them in the new TAIL heap
    for (MidaState s = startStateImpl(0); s < branches_.totalCells(); s++) {
#ifndef NOTERMINATOR_OPT
        if (isSeparate_(s) && !isTerminal_(s)) {
#else
        if (isSeparate_(s)) {
#endif  // NOTERMINATOR_OPT
            TailBlock aTailBlock;
            tails_.getBlock(-branches_.tailState(s), &aTailBlock);
            MidaState t = pNewTails->allocString(
                aTailBlock.suffix(), aTailBlock.data()
            );
            branches_.setKeyData(s, t);
        }
    }

    tails_.close();
    pNewTails->close();
    delete pNewTails;

    // replace the old file with the temp file
    remove(oldTailsName);
    MoveFile(tmpTailsName, oldTailsName);
    tails_.open(oldTailsName, ios::in | ios::out);
}

// Storage Compaction
void MidaTrie::compact()
{
    reallocTails_();
    //branches_.removeExtraFreeCells();
}

