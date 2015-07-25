/* branches.h
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
// branches.h - branching part of the Trie structure
// Created: 12 Feb 2001 (split from trie.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_MIDATRIE_BRANCHES_H
#define MIDA_MIDATRIE_BRANCHES_H

#include <mida/imitrie/triedefs.h>
#include <mida/midatrie/midadefs.h>
#include <mida/vmem/vmem.h>

// Cells in Double-Array Structure
struct DACell {
    MidaState base;
    MidaState check;
};
enum {
    BaseOffset = 0,
    CheckOffset = BaseOffset + sizeof(MidaState)
};

//
// Double-Array Pool  (derived from VirtualMem class --See vmem.h)
// -----------------
// This class just shields up the more general VirtualMem class. It represents
// a virtual, dynamic-sized BASE and CHECK arrays.
//
class DoubleArray : public VirtualMem {
public:  // public functions
    DoubleArray();
    DoubleArray(const char* pathName, int openModes);

    bool open(const char* pathName, int openModes);
    bool create(const char* pathName);
    bool close();
    bool isOpen() const;

    // BASE & CHECK referencings
    void setBase(MidaState s, MidaState b);
    void setCheck(MidaState s, MidaState c);

    // BASE & CHECK Read-Only Access
    MidaState base(MidaState s) const;
    MidaState check(MidaState s) const;
};

inline DoubleArray::DoubleArray() {}
inline DoubleArray::DoubleArray(const char* pathName, int openModes)
    { open(pathName, openModes); }

inline bool DoubleArray::open(const char* pathName, int openModes)
    { return VirtualMem::openSwapFile(pathName, openModes|ios::nocreate); }
inline bool DoubleArray::create(const char* pathName)
    { return VirtualMem::openSwapFile(pathName, ios::in|ios::out|ios::trunc); }
inline bool DoubleArray::close()
    { return VirtualMem::closeSwapFile(); }
inline bool DoubleArray::isOpen() const
    { return VirtualMem::isSwapFileOpen(); }

inline void DoubleArray::setBase(MidaState s, MidaState b)
    { setInt32(Pointer(s*sizeof(DACell) + BaseOffset), b); }
inline void DoubleArray::setCheck(MidaState s, MidaState c)
    { setInt32(Pointer(s*sizeof(DACell) + CheckOffset), c); }

inline MidaState DoubleArray::base(MidaState s) const
    { return getInt32(Pointer(s*sizeof(DACell) + BaseOffset)); }
inline MidaState DoubleArray::check(MidaState s) const
    { return getInt32(Pointer(s*sizeof(DACell) + CheckOffset)); }


//
// Branches
// --------
// A tree structure, represented in double-array structure, of the branching
// part of the trie. There are some improvements from Aoe's :
//
// 1. Terminal-nodes are allowed in the Branches structure. Terminal-nodes are
//    all nodes with incoming edge labeled Terminator, that is, a node s of
//    Branches is terminal if:
//
//          BASE[CHECK[s]] + Terminator = s
//
//    This can reduce some steps in retrievals: for words which are prefixes
//    of some other words, their last branches will be labeled Terminator.
//    Thus, pointers to TAIL blocks are no longer needed. We can immediately
//    store the associating data in such nodes.
//
// 2. The free-space list (G-link) is adapted to circular doubly-linked list.
//    This is accomplished by letting BASE of a free cell points to the
//    previous free cell (while CHECK points to next). Doing so can reduce the
//    time used to manipulate the list.
//
//    The entry point to the list is stored in BASE[0] and CHECK[0], and the
//    first cell available for use is BASE[1] and CHECK[1], which stores the
//    root node.
//
class Branches {
public:
    Branches();
    Branches(const char* pathName, int openModes);
    Branches(int nIndices, const char* pathName);

    bool open(const char* pathName, int openModes);
    bool create(int nIndices, const char* pathName);
    bool close();

    // File Status
    bool      isSwapFileOpen() const;
    bool      isSwapFileGood() const;
    bool      isSwapFileBad()  const;
    bool      isSwapFileFail() const;

    // index info
    int       getNIndices() const;

    // Node info
    MidaState   parentOf(MidaState s) const;
    int         outDegree(MidaState s) const;
    TrSymbolSet outputSet(MidaState s) const;
    TrChar      inputSymbol(MidaState s) const;
    bool        isUsedNode(MidaState s) const;
    bool        isSeparateNode(MidaState s) const;
    MidaState   tailState(MidaState s) const;

    // Key's data access
    TrIndex getKeyData(MidaState aSepNode) const;
    void    setKeyData(MidaState aSepNode, TrIndex data);

    static MidaState RootState(int indexNo);

    // Walking & Retrieval
    MidaState walk(MidaState s, TrChar c, TrWalkResult& rRes) const;

    // Insertion & Deletion
    MidaState insertBranch(MidaState from, TrChar c);
    void      deleteNode(MidaState s);

    // For Storage Enumeration
    MidaState totalCells() const;

#ifdef TRIE_DEBUG
    void  assertValid() const;
#endif  // TRIE_DEBUG

private:  // private const
    enum {
        DaSignature_     = 0xDAFC,  // double-array file signature
        DefaultNIndices_ = 1        // default #indices, if unspecified
    };
    enum {  // file format
        HeaderOffset_    = 0,       // (DaSignature_, nIndices)
        FreeListHead_    = 1,       // (before, next)
        RootStateBase_   = 2        // first cell of first root state
    };
    enum {
        UnusedBase_ = 0
    };

private:  // private functions
    // initialization
    bool bless_(int nIndices);
    bool load_();

    // header operation
    uint16 getFileSignature_() const;
    void   setFileSignature_();
    int    getNIndices_() const;
    void   setNIndices_(int nIndices);

    // free-list operations
    void  extendDAPool_();
    void  removeFreeCell_(MidaState s);
    void  insertFreeCell_(MidaState s);

    bool      isVacantFor_(const TrSymbolSet& aSet, MidaState aBase) const;
    MidaState findFreeBase_(const TrSymbolSet& aSet);    // Aoe's X_CHECK
    void      relocateBase_(MidaState s, MidaState newBase,
                            MidaState* pInspectedState = 0);

private:  // private data
    // the double-array structure
    DoubleArray da_;
    MidaState   totalCells_;
};

inline Branches::Branches() : totalCells_(0) {}
inline Branches::Branches(const char* pathName, int openModes)
    { open(pathName, openModes); }
inline Branches::Branches(int nIndices, const char* pathName)
    { create(nIndices, pathName); }

// File Status
inline bool Branches::isSwapFileOpen() const { return da_.isSwapFileOpen(); }
inline bool Branches::isSwapFileGood() const { return da_.isSwapFileGood(); }
inline bool Branches::isSwapFileBad()  const { return da_.isSwapFileBad();  }
inline bool Branches::isSwapFileFail() const { return da_.isSwapFileFail(); }

inline int Branches::getNIndices() const { return getNIndices_(); }

inline MidaState Branches::parentOf(MidaState s) const { return da_.check(s); }
inline bool Branches::isUsedNode(MidaState s) const
    { return s < totalCells() && da_.check(s) >= 0; }
inline bool Branches::isSeparateNode(MidaState s) const
    { return isUsedNode(s) && da_.base(s) < 0; }

inline TrIndex Branches::getKeyData(MidaState aSepNode) const
    { return -da_.base(aSepNode); }
inline void  Branches::setKeyData(MidaState aSepNode, TrIndex data)
    { da_.setBase(aSepNode, -data); }

inline MidaState Branches::RootState(int indexNo)
    { return RootStateBase_ + indexNo; }

inline MidaState Branches::totalCells() const { return totalCells_; }

inline uint16 Branches::getFileSignature_() const
{
    ASSERT(da_.isOpen());
    return da_.base(HeaderOffset_);
}
inline void Branches::setFileSignature_()
{
    ASSERT(da_.isOpen());
    da_.setBase(HeaderOffset_, DaSignature_);
}
inline int Branches::getNIndices_() const
{
    ASSERT(da_.isOpen());
    return da_.check(HeaderOffset_);
}
inline void Branches::setNIndices_(int nIndices)
{
    ASSERT(da_.isOpen());
    da_.setCheck(HeaderOffset_, nIndices);
}

#endif  // MIDA_MIDATRIE_BRANCHES_H

