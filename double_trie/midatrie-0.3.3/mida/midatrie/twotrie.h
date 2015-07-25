/* twotrie.h
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
// twotrie.h - double trie structure
// Created: 25 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_MIDATRIE_TWOTRIE_H
#define MIDA_MIDATRIE_TWOTRIE_H

#include <mida/imitrie/itrstate.h>
#include <mida/imitrie/imitrie.h>
#include <mida/midatrie/midadefs.h>
#include <mida/midatrie/branches.h>

class TwoTrieState;

//
// Two-Trie
// --------
// Morimoto and Aoe's two-trie structure
//
class TwoTrie : public IMiTrie {
public: // public typedefs & consts
    enum ErrorCode {
        NoError = 0,
        BranchesError
    };

public:
    // constuctor
    TwoTrie();
    TwoTrie(const char* dir, const char* trieName, int openModes);
    TwoTrie(int nIndices, const char* dir, const char* trieName);

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
    // Walking & Retrieval
    MidaState startStateImpl(int indexNo) const;
    MidaState finalStateImpl(int indexNo) const;
    MidaState walk(MidaState s, TrChar c, TrWalkResult& rRes,
                   TrIndex* pKeyData = 0) const;
    MidaState walk(MidaState s, const TrChar* key, TrWalkResult& rRes,
                   TrIndex* pKeyData = 0) const;
    TrIndex getKeyData(MidaState s) const;

    // For Enumeration
    int         outDegree(MidaState s) const;
    TrSymbolSet outputSet(MidaState s) const;
    void        enumerate(const TwoTrieState& s, IMiTrie::EnumFunc f) const;

private: // private consts and types
    enum {
        ChRightLink_ = 1,
        ChDataIndex_ = 2
    };

private: // private functions
    bool      isSeparate_(MidaState s) const;
#ifndef NOTERMINATOR_OPT
    bool      isTerminal_(MidaState s) const;
#endif  // NOTERMINATOR_OPT

    // operations at separate nodes
    MidaState getRightLink_(MidaState s) const;
    void      setRightLink_(MidaState s, MidaState r);
    TrIndex   getDataIndex_(MidaState s) const;
    void      setDataIndex_(MidaState s, TrIndex d);
    void      deleteSepData_(MidaState s);

    bool isInSingleChain_(MidaState s, MidaState* pSep = 0) const;

    MidaState insertSuffix_(int indexNo, const TrChar* suffix);
    void branchWithinLeft_(int indexNo, MidaState from, const TrChar* suffix,
                           TrIndex dataIdx);
    void branchWithinRight_(int indexNo, MidaState from, const TrChar* suffix,
                            TrIndex dataIdx);

    bool enumKeys_(const TwoTrieState& s, int level, IMiTrie::EnumFunc f) const;

private: // private data
    Branches branches_;
};

inline TwoTrie::TwoTrie() {}
inline TwoTrie::TwoTrie(const char* dir, const char* trieName, int openModes)
    { open(dir, trieName, openModes); }
inline TwoTrie::TwoTrie(int nIndices, const char* dir, const char* trieName)
    { create(nIndices, dir, trieName); }

//
// TwoTrieState
// -------------
//
class TwoTrieState : public ITrieState {
public:
    TwoTrieState(const TwoTrie& rTwoTrie, int indexNo);
    TwoTrieState(const TwoTrieState& aState);

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
    TwoTrieState(MidaState state, const TwoTrie& rMidaTrie);

private: // private data
    const TwoTrie&  rTwoTrie_;
    MidaState       state_;
    TrIndex         keyData_;
};

inline TwoTrieState::TwoTrieState(const TwoTrie& rTwoTrie, int indexNo)
    : rTwoTrie_(rTwoTrie), state_(rTwoTrie.startStateImpl(indexNo)),
      keyData_(TrErrorIndex) {}
inline TwoTrieState::TwoTrieState(const TwoTrieState& aState)
    : rTwoTrie_(aState.rTwoTrie_), state_(aState.state_),
      keyData_(TrErrorIndex) {}
inline TwoTrieState::TwoTrieState(MidaState state, const TwoTrie& rTwoTrie)
    : rTwoTrie_(rTwoTrie), state_(state),
      keyData_(TrErrorIndex) {}

inline ITrieState* TwoTrieState::clone() const
{
    return new TwoTrieState(*this);
}

inline void TwoTrieState::walk(TrChar c, TrWalkResult& rRes)
{
    state_ = rTwoTrie_.walk(state_, c, rRes, &keyData_);
}
inline void TwoTrieState::walk(const TrChar* key, TrWalkResult& rRes)
{
    state_ = rTwoTrie_.walk(state_, key, rRes, &keyData_);
}
inline TrIndex TwoTrieState::getKeyData() const
{
    return keyData_;
}

inline int TwoTrieState::outDegree() const
{
    return rTwoTrie_.outDegree(state_);
}
inline TrSymbolSet TwoTrieState::outputSet() const
{
    return rTwoTrie_.outputSet(state_);
}
inline void TwoTrieState::enumerate(IMiTrie::EnumFunc fn)
{
    return rTwoTrie_.enumerate(*this, fn);
}

#endif // MIDA_MIDATRIE_TWOTRIE_H

