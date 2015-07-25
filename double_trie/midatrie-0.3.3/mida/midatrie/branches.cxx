/* branches.cxx
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
// branches.cxx - branching part of the Trie structure
// Created: 12 Feb 2001 (split from trie.cxx)
// Author:  Theppitak Karoonboonyanan
//

#include "branches.h"

//////////////// Implementation of DoubleArray class ////////////////

//////////////// Implementation of Branches class ////////////////
bool Branches::open(const char* pathName, int openModes)
{
    if (!da_.open(pathName, openModes)) { return false; }
    return load_();
}

bool Branches::create(int nIndices, const char* pathName)
{
    if (!da_.create(pathName)) { return false; }
    return bless_(nIndices);
}

bool Branches::close()
{
    return da_.close();
}

bool Branches::bless_(int nIndices)
{
    ASSERT(da_.isOpen());
    setFileSignature_();
    setNIndices_(nIndices);

    // init free list header cell
    da_.setBase(FreeListHead_, -FreeListHead_);
    da_.setCheck(FreeListHead_, -FreeListHead_);
    // init root states
    for (int i = 0; i < nIndices; i++) {
        da_.setBase(RootState(i), RootState(i) + nIndices);
        da_.setCheck(RootState(i), 0);
    }
    // so, totalCells is now...
    totalCells_ = RootState(nIndices);

    extendDAPool_();
#ifdef TRIE_DEBUG
    assertValid();
#endif  // TRIE_DEBUG
    return true;
}

bool Branches::load_()
{
    ASSERT(da_.isOpen());
    if (getFileSignature_() != DaSignature_) { return false; }
    totalCells_ = (da_.totalPages() * da_.PageSize()) / sizeof(DACell);
    if (totalCells_ == 0) {
        bless_(DefaultNIndices_);
    }
#ifdef TRIE_DEBUG
    assertValid();
#endif  // TRIE_DEBUG
    return true;
}

// return number of edges that point out from node s
// i.e. number of c such that CHECK[BASE[s] + c] = s
// [Aoe's N(SET_LIST(s))]
int Branches::outDegree(MidaState s) const
{
    int   outDegree = 0;
    MidaState b = da_.base(s);
    ASSERT(b > 0);
    for (int c = 0; c < TrAlphabetSize; c++) {
        if (da_.check(b + c) == s)  { outDegree++; }
    }
    return outDegree;
}

// return the set of those symbols on the output edges of the node s
// [Aoe's SET_LIST(s)]
TrSymbolSet Branches::outputSet(MidaState s) const
{
    TrSymbolSet theSet;
    MidaState b = da_.base(s);
    ASSERT(b > 0);
    for (int c = 1; c < TrAlphabetSize; c++) {
        if (da_.check(b + c) == s)  { theSet += c; }
    }
    return theSet;
}

TrChar Branches::inputSymbol(MidaState s) const
{
    return s - da_.base(parentOf(s));
}

// return:
//    s is separate node  -> the equivalent TAIL state of s
//    otherwise           -> s
MidaState Branches::tailState(MidaState s) const
{
    MidaState t = da_.base(s);
    return (t < 0) ? t : s;
}

// walking & retrieval
// behavior on walkResult:
//    CRASH    - returns the state before crashing
//    TERMINAL - returns the terminal state
//    INTERNAL - returns the entered state
MidaState Branches::walk(MidaState s, TrChar c, TrWalkResult& rRes) const
{
    ASSERT(s > 0);
    MidaState t = da_.base(s) + c;
    if (t < totalCells_ && da_.check(t) == s) {
        rRes.setWalkResult((c == TrTerminator) ? Tr_TERMINAL : Tr_INTERNAL);
        rRes.setNWalked(1);
        return t;
    } else {
        rRes.setWalkResult(Tr_CRASH);
        rRes.setNWalked(0);
        return s;
    }
}

// extend the double-array pool for a page, with free-list initialized
void Branches::extendDAPool_()
{
    const int CellsPerPage = da_.PageSize() / sizeof(DACell);
    ASSERT(da_.PageSize() % sizeof(DACell) == 0);
    // add cells up to page end
    int cellsToAdd = int(CellsPerPage - (totalCells_ % CellsPerPage));

    // init new free-list
    MidaState i;
    for (i = totalCells_; i < totalCells_ + cellsToAdd - 1; i++) {
        da_.setCheck(i, -(i+1));
        da_.setBase(i+1, -i);
    }

    // merge the new circular list to the old
    da_.setCheck(-da_.base(FreeListHead_), -totalCells_);
    da_.setBase(totalCells_, da_.base(FreeListHead_));
    da_.setCheck(i, -FreeListHead_);
    da_.setBase(FreeListHead_, -i);

    totalCells_ += cellsToAdd;
}

void Branches::removeFreeCell_(MidaState s)
{
    ASSERT(s > 0);

    // ensure existence of the free cell
    while (totalCells_ <= s) {
        extendDAPool_();
    }

    ASSERT(da_.check(s) < 0);
    ASSERT(da_.base(s) < 0);
    da_.setCheck(-da_.base(s), da_.check(s));
    da_.setBase(-da_.check(s), da_.base(s));
}

void Branches::insertFreeCell_(MidaState s)
{
    ASSERT(s > 0);
    if (s < totalCells_) {
        // let i be the first free cell which i > s
        MidaState i = -da_.check(FreeListHead_);
        ASSERT(i > 0);
        while (i != FreeListHead_ && i < s) {
            i = -da_.check(i);
            ASSERT(i >= FreeListHead_);
        }

        // insert s before i
        da_.setCheck(s, -i);
        da_.setBase(s, da_.base(i));
        da_.setCheck(-da_.base(i), -s);
        da_.setBase(i, -s);
    }
}

// check if the place beginning from s is vacant for placing
// all the symbols in aSet
bool Branches::isVacantFor_(const TrSymbolSet& aSet, MidaState aBase) const
{
    for (TrSymbolSet::SymbolIterator i = aSet.first(); !i.isDone(); i.next()) {
        MidaState to = aBase + i.currentItem();
        if (to >= totalCells() || isUsedNode(to))  { return false; }
    }
    return true;
}

// search for a BASE beginning index such that all the cells to be occupied
// by the symbols in aSet are free
// Note: For optimal space usage, the first symbol in aSet should be
//       the smallest value.
// [Aoe's X_CHECK(LIST)]
MidaState Branches::findFreeBase_(const TrSymbolSet& aSet)
{
    TrChar  firstSymbol = aSet.first().currentItem();
    for (;;) {  // retry loop
        MidaState s;
        // s := first free cell such that s > first symbol
        for (;;) {  // retry loop
            s = -da_.check(FreeListHead_);
            ASSERT(s >= FreeListHead_);
            while (s != FreeListHead_ && s <= MidaState(firstSymbol)) {
                s = -da_.check(s);
                ASSERT(s >= FreeListHead_);
            }
            if (s != FreeListHead_)  { break; }
            extendDAPool_();
        }

        // search for next free cell in which aSet fits
        while (s != FreeListHead_ && !isVacantFor_(aSet, s - firstSymbol)) {
            s = -da_.check(s);
            ASSERT(s == FreeListHead_ || s >= MidaState(firstSymbol));
        }
        if (s != FreeListHead_)  { return s - firstSymbol; }
        extendDAPool_();
    }
}

void Branches::relocateBase_(MidaState s, MidaState newBase,
                             MidaState* pInspectedState)
{
    MidaState oldBase = da_.base(s);
    ASSERT(oldBase > 0);
    TrSymbolSet outputs(outputSet(s));
    ASSERT(isVacantFor_(outputs, newBase));

    for (TrSymbolSet::SymbolIterator i = outputs.first();
         !i.isDone(); i.next())
    {
        MidaState oldNext = oldBase + i.currentItem();
        MidaState newNext = newBase + i.currentItem();

        // give newNext node to s and copy BASE
        ASSERT(newNext >= totalCells_ || da_.check(newNext) <= 0);
        removeFreeCell_(newNext);
        da_.setCheck(newNext, s);  // mark owner
        da_.setBase(newNext, da_.base(oldNext)); // copy value

        // oldNext node is now moved to newNext
        // so, all cells belonging to oldNext
        // must be given to newNext
        MidaState nextBase = da_.base(oldNext);
        // preventing cases of TAIL pointer
        if (nextBase > 0) {
            for (int c = 0; c < TrAlphabetSize; c++) {
                if (da_.check(nextBase + c) == oldNext) {
                    da_.setCheck(nextBase + c, newNext);
                }
            }
        }
        // And, if inspected state is oldNext, change it to newNext
        if (pInspectedState && *pInspectedState == oldNext) {
            *pInspectedState = newNext;
        }

        // free oldNext node
        insertFreeCell_(oldNext);
    }

    // finally, make BASE[s] point to newBase
    da_.setBase(s, newBase);
}

#ifdef TRIE_DEBUG
void Branches::assertValid() const
{
    // file header checking
    ASSERT(getFileSignature_() == DaSignature_);
    int nIndices = getNIndices_();

    // check circular joining
    ASSERT(-da_.base(FreeListHead_) < totalCells_);
    ASSERT(da_.check(-da_.base(FreeListHead_)) == FreeListHead_);

    MidaState s = FreeListHead_;
    do {
        MidaState t = -da_.check(s);
        ASSERT(t > 0);
        ASSERT(t < totalCells_);
        // Check back pointer
        ASSERT(da_.base(t) == -s);
        // The cells in between must not be free
        for (MidaState u = s + 1; u < t; u++) {
            ASSERT(da_.check(u) >= 0);
            ASSERT(da_.check(u) < totalCells);
            if (!isSeparateNode(u)) {
                ASSERT(da_.base(u) < totalCells_);
            }
            // detect no-branch node (whose BASE may be incorrect)
            if (u > RootState(nIndices) && !isSeparateNode(u))
            {
                ASSERT(outDegree(u) > 0);
            }
        }
        s = t;
    } while (s != FreeListHead_);
}
#endif // TRIE_DEBUG

MidaState Branches::insertBranch(MidaState from, TrChar c)
{
    ASSERT(da_.check(from) >= 0);
    MidaState to;
    if (da_.base(from) == UnusedBase_ || isSeparateNode(from))
    {
        // BASE[from] isn't pointing to a BASE, so just find a brand-new one
        MidaState newBase = findFreeBase_(TrSymbolSet(1, c));
        da_.setBase(from, newBase);
        to = newBase + c;
        ASSERT(da_.check(to) < 0);
    } else {
        to = da_.base(from) + c;
        // for existing branch, just return it
        if (to < totalCells() && da_.check(to) == from) { return to; }
        // the cell is free for use?
        if (isUsedNode(to)) {
            // no, prepare place for 'to'
            // determine work amount and minimize it
            MidaState overlapped = parentOf(to);
            ASSERT(da_.check(overlapped) >= 0);
            ASSERT(da_.base(overlapped) > 0);
            if (outDegree(from) < outDegree(overlapped)) {
                // relocate 'from'
                TrSymbolSet outputs(outputSet(from));
                outputs += c;
                MidaState newBase = findFreeBase_(outputs);
                relocateBase_(from, newBase);
                // let 'to' be the new cell reallocated
                to = newBase + c;
                ASSERT(da_.check(to) != 0);
                ASSERT(da_.check(to) < 0);
            } else {
                // relocate 'overlapped' so that 'to' is free
                // Note that since the relocated state is not 'from',
                // so it can possibly be changed, and thus must be inspected
                MidaState newBase = findFreeBase_(outputSet(overlapped));
                relocateBase_(overlapped, newBase, &from);
                ASSERT(da_.check(to) != 0);
                ASSERT(da_.check(to) < 0);
            }
        }
    }

    // take and occupy it
    ASSERT(da_.check(to) != 0);
    ASSERT(da_.check(to) < 0);
    removeFreeCell_(to);
    da_.setBase(to, UnusedBase_);
    da_.setCheck(to, from);

    return to;
}

void Branches::deleteNode(MidaState s)
{
    insertFreeCell_(s);
}



