/* midatrie.h
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
// midatrie.h - multi-index double-array trie
// Created: 23 May 1996
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_MIDATRIE_MIDATRIE_H
#define MIDA_MIDATRIE_MIDATRIE_H

#include <mida/imitrie/itrstate.h>
#include <mida/imitrie/imitrie.h>
#include <mida/midatrie/midadefs.h>
#include <mida/midatrie/branches.h>
#include <mida/midatrie/tails.h>

class MidaTrieState;

//
// MidaTrie (Multi-Index Double-Array Trie)
// --------
// This class gives a general view on tries. The class users can insert keys,
// delete keys, check key inclusion, and retrieve the data associating
// with the key. Furthermore, the users also can walk along the trie with
// a key character by character. To do this, the MidaState of the operation is
// defined so that the users can keep track of where they currently are.
//
// As named, the tries of this class can store more than one set of indices
// in the same storage. Each set is indicated by indexNo argument, of which
// value begins at zero.
//
// This class is implemented by using double-array structure with suffix
// compression. (See Aoe[1989] for the detail.) That is, the trie is composed
// of 2 structures:
//    - Branches : the branching part which distinguishes keys (it's a tree)
//    - Tails    : the remaining part of keys after the distinguishing position
//
// And, to indicate in which structure a MidaState is, we use its sign as follows:
//    + : represents multi-nodes (i.e. nodes in the Branches structure)
//    - : represents single-nodes (i.e. nodes in the Tails structure)
//    0 : undefined
//
// Terminology:
//    - multi-node    : internal node in the Branches structure, which
//                      contains a pointer to the beginning pos of the
//                      sparse table row containing their child nodes
//    - single-node   : every node in the Tails structure
//    - separate-node : leaf node in the Branches structure, which contains
//                      a pointer to its remaining part in the Tails
//                      structure. (This can be distinguished from the pointer
//                      to table row by its sign.)
//    - terminal-node : node with the incoming edge labeled TrTerminator
//                      A terminal-node can be in both Branches and Tails.
//
// There is an improvement from Aoe's: the deletion algorithm is more complete.
// Instead of just mark the separate node of a key as unused, the other
// relevant nodes are also deleted.
//
class MidaTrie : public IMiTrie {
public: // public typedefs & consts
    enum ErrorCode {
        NoError = 0,
        BranchesError,
        TailsError
    };

public: // public functions
    // constructor
    MidaTrie();
    MidaTrie(const char* dir, const char* trieName, int openModes);
    MidaTrie(int nIndices, const char* dir, const char* trieName);

    // File Status
    ErrorCode open(const char* dir, const char* trieName, int openModes);
    ErrorCode create(int nIndices, const char* dir, const char* trieName);
    ErrorCode close();

    //// IMiTrie interface overriding ////
    // File Status
    bool isOpen() const;

    // Index info
    int  getNIndices() const;

    // General Operations
    bool    insertKey(int indexNo, const TrChar* key,
                      TrIndex dataIdx = TrDummyIndex);
    bool    deleteKey(int indexNo, const TrChar* key);
    TrIndex retrieve(int indexNo, const TrChar* key) const;

    // Walking & Retrieval
    ITrieState* startState(int indexNo) const;

    //// Implementation functions ////
    // Storage Compaction
    void compact();

    // Walking & Retrieval
    MidaState startStateImpl(int indexNo) const;
    MidaState walk(MidaState s, TrChar c, TrWalkResult& rRes) const;
    MidaState walk(MidaState s, const TrChar* key, TrWalkResult& rRes) const;
    TrIndex getKeyData(MidaState s) const;

    // For Enumeration
    int         outDegree(MidaState s) const;
    TrSymbolSet outputSet(MidaState s) const;
    void        enumerate(MidaState s, IMiTrie::EnumFunc fn) const;

private:  // private functions
    bool isTerminal_(MidaState s) const;
    bool isSeparate_(MidaState s) const;

    // Aoe's A_INSERT & B_INSERT
    void branchWithinBranches_(MidaState from, const TrChar* suffix,
                               TrIndex dataIdx);
    void branchWithinTail_(MidaState sepNode, const TrChar* suffix,
                           TrIndex dataIdx);

    bool enumKeys_(MidaState s, int level, IMiTrie::EnumFunc fn) const;

    // storage compaction
    void reallocTails_();

private:  // private data
    // the trie structure
    Branches    branches_;
    Tails       tails_;
};

inline MidaTrie::MidaTrie() {}
inline MidaTrie::MidaTrie(const char* dir, const char* trieName, int openModes)
    { open(dir, trieName, openModes); }
inline MidaTrie::MidaTrie(int nIndices, const char* dir, const char* trieName)
    { create(nIndices, dir, trieName); }

inline bool MidaTrie::isTerminal_(MidaState s) const
{
    return (s > 0) ? (branches_.inputSymbol(s) == TrTerminator)
                     : tails_.isTerminal(-s);
}
inline bool MidaTrie::isSeparate_(MidaState s) const
{
    return (s > 0) && branches_.isSeparateNode(s);
}

//
// MidaTrieState
// -------------
//
class MidaTrieState : public ITrieState {
public:
    MidaTrieState(const MidaTrie& rMidaTrie, int indexNo);
    MidaTrieState(const MidaTrieState& aState);

    //// ITrieState interface overriding ////
    ITrieState* clone() const;

    // Walking & Retrieval
    void walk(TrChar c, TrWalkResult& rRes);
    void walk(const TrChar* key, TrWalkResult& rRes);
    TrIndex getKeyData() const;

    // For Enumeration
    int         outDegree() const;
    TrSymbolSet outputSet() const;
    void        enumerate(IMiTrie::EnumFunc fn);

private: // private functions
    MidaTrieState(MidaState state, const MidaTrie& rMidaTrie);

private: // private data
    const MidaTrie&  rMidaTrie_;
    MidaState        state_;
};

inline MidaTrieState::MidaTrieState(const MidaTrie& rMidaTrie, int indexNo)
    : rMidaTrie_(rMidaTrie), state_(rMidaTrie.startStateImpl(indexNo)) {}
inline MidaTrieState::MidaTrieState(const MidaTrieState& aState)
    : rMidaTrie_(aState.rMidaTrie_), state_(aState.state_) {}
inline MidaTrieState::MidaTrieState(MidaState state, const MidaTrie& rMidaTrie)
    : rMidaTrie_(rMidaTrie), state_(state) {}

inline ITrieState* MidaTrieState::clone() const
{
    return new MidaTrieState(*this);
}

inline void MidaTrieState::walk(TrChar c, TrWalkResult& rRes)
{
    state_ = rMidaTrie_.walk(state_, c, rRes);
}
inline void MidaTrieState::walk(const TrChar* key, TrWalkResult& rRes)
{
    state_ = rMidaTrie_.walk(state_, key, rRes);
}
inline TrIndex MidaTrieState::getKeyData() const
{
    return rMidaTrie_.getKeyData(state_);
}

inline int MidaTrieState::outDegree() const
{
    return rMidaTrie_.outDegree(state_);
}
inline TrSymbolSet MidaTrieState::outputSet() const
{
    return rMidaTrie_.outputSet(state_);
}
inline void MidaTrieState::enumerate(IMiTrie::EnumFunc fn)
{
    return rMidaTrie_.enumerate(state_, fn);
}

#endif // MIDA_MIDATRIE_MIDATRIE_H

