/* twotrie.cxx
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
// twotrie.cxx - double trie structure
// Created: 25 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#include "twotrie.h"
#include <mida/utils/fileutil.h>
#include <mida/utils/autoptr.h>

//////////////// Implementation of TwoTrie class ////////////////
TwoTrie::ErrorCode TwoTrie::open(
    const char* dir, const char* trieName, int openModes
)
{
    if (!branches_.open(FullPath(dir, trieName, "tr2"), openModes)) {
        return BranchesError;
    }
    return NoError;
}

TwoTrie::ErrorCode TwoTrie::create(
    int nIndices, const char* dir, const char* trieName
)
{
    if (!branches_.create(nIndices*2, FullPath(dir, trieName, "tr2"))) {
        return BranchesError;
    }
    return NoError;
}

TwoTrie::ErrorCode TwoTrie::close()
{
    if (!branches_.close()) {
        return BranchesError;
    }
    return NoError;
}

bool TwoTrie::isOpen() const
{
    return branches_.isSwapFileOpen();
}

int TwoTrie::getNIndices() const
{
    return branches_.getNIndices()/2;
}

ITrieState* TwoTrie::startState(int indexNo) const
{
    return new TwoTrieState(*this, indexNo);
}

MidaState TwoTrie::startStateImpl(int indexNo) const
{
    return (indexNo >= getNIndices()) ?
               TrErrorIndex : Branches::RootState(indexNo*2);
}

MidaState TwoTrie::finalStateImpl(int indexNo) const
{
    return (indexNo >= getNIndices()) ?
               TrErrorIndex : Branches::RootState(indexNo*2+1);
}

bool TwoTrie::isSeparate_(MidaState s) const
{
    TrWalkResult res;
    branches_.walk(s, ChRightLink_, res);
    return (res.getWalkResult() != Tr_CRASH);
}

bool TwoTrie::isTerminal_(MidaState s) const
{
    if (s > 0) {
        return (branches_.inputSymbol(s) == TrTerminator);
    } else {
        TrWalkResult res;
        branches_.walk(-s, TrTerminator, res);
        return (res.getWalkResult() != Tr_CRASH);
    }
}

MidaState TwoTrie::getRightLink_(MidaState s) const
{
    ASSERT(isSeparate_(s));
    TrWalkResult res;
    return branches_.getKeyData(branches_.walk(s, ChRightLink_, res));
}

void TwoTrie::setRightLink_(MidaState s, MidaState r)
{
    branches_.setKeyData(branches_.insertBranch(s, ChRightLink_), r);
}

TrIndex TwoTrie::getDataIndex_(MidaState s) const
{
    ASSERT(s > 0);
#ifndef NOTERMINATOR_OPT
    ASSERT(isTerminal_(s) || isSeparate_(s));
    if (isTerminal_(s)) {
        return branches_.getKeyData(s);
    } else {
        TrWalkResult res;
        return branches_.getKeyData(branches_.walk(s, ChDataIndex_, res));
    }
#else
    ASSERT(isSeparate_(s));
    TrWalkResult res;
    return branches_.getKeyData(branches_.walk(s, ChDataIndex_, res));
#endif  // NOTERMINATOR_OPT
}

void TwoTrie::setDataIndex_(MidaState s, TrIndex d)
{
    ASSERT(s > 0);
#ifndef NOTERMINATOR_OPT
    ASSERT(isTerminal_(s) || isSeparate_(s));
    if (isTerminal_(s)) {
        branches_.setKeyData(s, d);
    } else {
        branches_.setKeyData(branches_.insertBranch(s, ChDataIndex_), d);
    }
#else
    ASSERT(isSeparate_(s));
    branches_.setKeyData(branches_.insertBranch(s, ChDataIndex_), d);
#endif  // NOTERMINATOR_OPT
}

void TwoTrie::deleteSepData_(MidaState s)
{
    ASSERT(isSeparate_(s));
    MidaState n;
    TrWalkResult res;
    n = branches_.walk(s, ChRightLink_, res);
    if (res.getWalkResult() != Tr_CRASH) {
        branches_.deleteNode(n);
    }
    n = branches_.walk(s, ChDataIndex_, res);
    if (res.getWalkResult() != Tr_CRASH) {
        branches_.deleteNode(n);
    }
}

int TwoTrie::outDegree(MidaState s) const
{
    if (isTerminal_(s)) { return 0; }

    if (s > 0) {
        // if s is separate node, it's equivalent to a TAIL state
        return isSeparate_(s) ? 1 : branches_.outDegree(s);
    }
    return (s < 0) ? 1 : 0;
}

TrSymbolSet TwoTrie::outputSet(MidaState s) const
{
    if (isTerminal_(s)) { return TrSymbolSet(); }

    // if s is separate node, jump to RIGHT first
    if (s > 0 && isSeparate_(s)) {
        s = -getRightLink_(s);
    }

    if (s > 0) {
        return branches_.outputSet(s);
    } else if (s < 0) {
        return TrSymbolSet(1, branches_.inputSymbol(-s));
    } else {
        return TrSymbolSet();
    }
}

MidaState TwoTrie::walk(
    MidaState s, TrChar c, TrWalkResult& rRes, TrIndex* pKeyData
) const
{
    // if s is separate node, jump to RIGHT first
    if (s > 0 && isSeparate_(s)) {
        // sneak the data before jumping to RIGHT
        if (pKeyData) { *pKeyData = getDataIndex_(s); }
        s = -getRightLink_(s);
    }

    // walk according to state type
    if (s > 0) {
        s = branches_.walk(s, c, rRes);
#ifndef NOTERMINATOR_OPT
        if (pKeyData && c == TrTerminator) {
            *pKeyData = getDataIndex_(s);
        }
#endif  // NOTERMINATOR_OPT
    } else if (s < 0 && branches_.inputSymbol(-s) == c) {
        s = -branches_.parentOf(-s);
        rRes.setWalkResult(c == TrTerminator ? Tr_TERMINAL : Tr_INTERNAL);
        rRes.setNWalked(1);
    } else {
        // crash
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
    }

    return s;
}

MidaState TwoTrie::walk(
    MidaState s, const TrChar* key, TrWalkResult& rRes, TrIndex* pKeyData
) const
{
    int nWalked = 0;
    const TrChar* p = key;

    // walk through multi-nodes
    if (s > 0) {
        while (!isSeparate_(s)) {
            s = branches_.walk(s, *p, rRes);
            if (rRes.getWalkResult() == Tr_CRASH)  return s;
            p++; rRes.setNWalked(++nWalked);
            if (rRes.getWalkResult() == Tr_TERMINAL) {
                if (pKeyData) { *pKeyData = getDataIndex_(s); }
                return s;
            }
        }
        // sneak the data before jumping to RIGHT
        if (pKeyData) { *pKeyData = getDataIndex_(s); }
        s = -getRightLink_(s);
    }

    // walk through single-nodes
    if (s < 0) {
        s = -s;
        while (*p) {
            if (branches_.inputSymbol(s) == *p) {
                s = branches_.parentOf(s);
                rRes.setNWalked(++nWalked);
                if (*p == TrTerminator) {
                    rRes.setWalkResult(Tr_TERMINAL);
                    break;
                } else {
                    rRes.setWalkResult(Tr_INTERNAL);
                }
            } else {
                rRes.setWalkResult(Tr_CRASH);
                break;
            }
            ++p;
        }
        s = -s;
    } else {
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
    }

    return s;
}

MidaState TwoTrie::insertSuffix_(int indexNo, const TrChar* suffix)
{
    // find terminator
    const TrChar* p = suffix;
    while (*p && *p != TrTerminator) { ++p; }
    if (!*p) { return TrErrorIndex; }

    MidaState r = finalStateImpl(indexNo);
    for ( ; p >= suffix; --p) {
        TrWalkResult res;
        r = branches_.walk(r, *p, res);
        if (res.getWalkResult() == Tr_CRASH) { break; }
    }
    for ( ; p >= suffix; --p) {
        r = branches_.insertBranch(r, *p);
    }
    return r;
}

void TwoTrie::branchWithinLeft_(
    int indexNo, MidaState from, const TrChar* suffix, TrIndex dataIdx
)
{
    MidaState newState = branches_.insertBranch(from, *suffix);
    if (*suffix != TrTerminator) {
        // link it to RIGHT trie
        setRightLink_(newState, insertSuffix_(indexNo, suffix+1));
#ifdef NOTERMINATOR_OPT
    } else {
        setRightLink_(newState, finalStateImpl(indexNo));
#endif // NOTERMINATOR_OPT
    }
    setDataIndex_(newState, dataIdx);

#ifdef TRIE_DEBUG
    branches_.AssertValid();
#endif  // TRIE_DEBUG
}

// Aoe's B_INSERT
void TwoTrie::branchWithinRight_(
    int indexNo, MidaState sepNode, const TrChar* suffix, TrIndex dataIdx
)
{
    ASSERT(isSeparate_(sepNode));

    TrIndex pathData = getDataIndex_(sepNode);
    MidaState t = -getRightLink_(sepNode);
    deleteSepData_(sepNode);
    MidaState s = sepNode;
    MidaState u;
    TrWalkResult r;
    bool isSinglePath = true;
    while (u = walk(t, *suffix, r), r.getWalkResult() != Tr_CRASH) {
        if (isSinglePath) {
            branches_.deleteNode(-t);
            if (branches_.outDegree(-u) > 0) { isSinglePath = false; }
        }
        s = branches_.insertBranch(s, *suffix);
        suffix++;
        t = u;
    }
    TrChar nextSymbol = branches_.inputSymbol(-t);
    u = branches_.insertBranch(s, nextSymbol);
#ifndef NOTERMINATOR_OPT
    if (nextSymbol != TrTerminator) {
        setRightLink_(u, branches_.parentOf(-t));
    }
#else
    setRightLink_(u, branches_.parentOf(-t));
#endif // NOTERMINATOR_OPT
    setDataIndex_(u, pathData);

    if (isSinglePath) {
        branches_.deleteNode(-t);
    }

    // then, insert the new branch
    branchWithinLeft_(indexNo, s, suffix, dataIdx);
}

bool TwoTrie::insertKey(int indexNo, const TrChar* key, TrIndex dataIdx)
{
    if (indexNo >= getNIndices()) { return false; }

    MidaState s = startStateImpl(indexNo);
    TrWalkResult r;
    const TrChar* p = AppendTerminator(key);

    // walk through LEFT trie
    do {
        s = branches_.walk(s, *p, r);
        switch (r.getWalkResult()) {
            case Tr_CRASH: {
                // A_INSERT(s, p);
                branchWithinLeft_(indexNo, s, p, dataIdx);
                return true;
            }

            case Tr_TERMINAL: {
                ASSERT(*p == TrTerminator);
                //duplicated key, overwrite data index
                setDataIndex_(s, dataIdx);
                return true;
            }
        }
        p++;
    } while (!isSeparate_(s));

    // continue walking on RIGHT trie
    MidaState t = walk(s, p, r);
    switch (r.getWalkResult()) {
        case Tr_CRASH: {
            // B_INSERT(s, p);
            branchWithinRight_(indexNo, s, p, dataIdx);
            return true;
        }

        case Tr_TERMINAL: {
            ASSERT(p[r.getNWalked()-1] == TrTerminator);
            // duplicated key, overwrite data index
            setDataIndex_(s, dataIdx);
            return true;
        }
    }

    ASSERT(false);
    return false;
}

bool TwoTrie::isInSingleChain_(MidaState s, MidaState* pSep) const
{
    TrSymbolSet symSet;
    TrWalkResult r;
    while (!isSeparate_(s) && (symSet = outputSet(s)).nElements() == 1) {
        s = branches_.walk(s, symSet.first().currentItem(), r);
        ASSERT(r.getWalkResult() != Tr_CRASH);
    }
    if (isSeparate_(s)) {
        if (pSep) { *pSep = s; }
        return true;
    }
    return false;
}

bool TwoTrie::deleteKey(int indexNo, const TrChar* key)
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
            case Tr_TERMINAL:
#ifdef NOTERMINATOR_OPT
                deleteSepData_(s);
#endif // NOTERMINATOR_OPT
                goto DELETE_LEFT_BRANCHES;
        }
        p++;
    } while (!isSeparate_(s));

    // for separate node, delete single chain in RIGHT if key exists
    {
        walk(s, p, r);
        switch (r.getWalkResult()) {
            case Tr_CRASH:    return false;
            case Tr_TERMINAL: {
                MidaState t = getRightLink_(s);
                int d;
                while (t != finalStateImpl(indexNo)
                       && branches_.outDegree(t) == 0)
                {
                    MidaState parent = branches_.parentOf(t);
                    branches_.deleteNode(t);
                    t = parent;
                }
                deleteSepData_(s);
                break;
            }
            default:          ASSERT(false);
        }
    }

DELETE_LEFT_BRANCHES:
    // delete relevant branches
    do {
        MidaState parent = branches_.parentOf(s);
        branches_.deleteNode(s);
        s = parent;
    } while (s != startStateImpl(indexNo) && branches_.outDegree(s) == 0);

    MidaState sep;
    if (isInSingleChain_(s, &sep)) {
        TrIndex keyData = getDataIndex_(sep);
        MidaState rightState = getRightLink_(sep);
        deleteSepData_(sep);
        do {
            MidaState parent = branches_.parentOf(sep);
            TrChar sym = branches_.inputSymbol(sep);
            rightState = branches_.walk(rightState, sym, r);
            if (r.getWalkResult() == Tr_CRASH) { break; }
            branches_.deleteNode(sep);
            sep = parent;
        } while (sep != startStateImpl(indexNo)
                 && branches_.outDegree(sep) == 0);
        setRightLink_(sep, rightState);
        setDataIndex_(sep, keyData);
    }

    return true;
}

TrIndex TwoTrie::retrieve(int indexNo, const TrChar* key) const
{
    if (indexNo >= getNIndices()) { return TrErrorIndex; }

    TrWalkResult r;
    TrIndex      keyData;
    const TrChar* p = AppendTerminator(key);
    walk(startStateImpl(indexNo), p, r, &keyData);
    return (r.getWalkResult() == Tr_TERMINAL) ? keyData : TrErrorIndex;
}

TrIndex TwoTrie::getKeyData(MidaState s) const
{
    if (s > 0) {
#ifndef NOTERMINATOR_OPT
        return (isTerminal_(s) || isSeparate_(s)) ?
               getDataIndex_(s) : TrErrorIndex;
#else
        return isSeparate_(s) ? getDataIndex_(s) : TrErrorIndex;
#endif  // NOTERMINATOR_OPT
    }
    return TrErrorIndex;
}

bool TwoTrie::enumKeys_(
    const TwoTrieState& s, int level, IMiTrie::EnumFunc f
) const
{
    static TrChar theWord[256];
    TrSymbolSet outputs(s.outputSet());
    TrWalkResult r;
    for (TrSymbolSet::SymbolIterator i = outputs.first();
         !i.isDone(); i.next())
    {
        theWord[level] = i.currentItem();
        auto_ptr<TwoTrieState> t = dynamic_cast<TwoTrieState*>(s.clone());
        t->walk(i.currentItem(), r);
        ASSERT(r.getWalkResult() != Tr_CRASH);
        if (r.getWalkResult() == Tr_TERMINAL) {
            theWord[level] = '\0';
            return f && (*f)(theWord, t->getKeyData());
        } else {
            if (!enumKeys_(*t, level+1, f)) {
                return false;
            }
        }
    }
    return true;
}

void TwoTrie::enumerate(const TwoTrieState& s, IMiTrie::EnumFunc f) const
{
    enumKeys_(s, 0, f);
}

