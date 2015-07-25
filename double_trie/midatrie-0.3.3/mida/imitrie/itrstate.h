/* itrstate.h
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
// itrstate.h - trie state interface
// Created: 6 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_IMITRIE_ITRSTATE_H
#define MIDA_IMITRIE_ITRSTATE_H

#include <mida/imitrie/triedefs.h>

class IMiTrie;

//
// ITrieState
// ------------
//
class ITrieState {
public: // public typedefs
    typedef bool (*EnumFunc)(const TrChar* key, TrIndex data);

public: // public functions
    virtual ~ITrieState();

    virtual ITrieState* clone() const = 0;

    // Walking & Retrieval
    virtual void walk(TrChar c, TrWalkResult& rRes) = 0;
    // walk with Terminator
    void terminate(TrWalkResult& rRes);
    virtual void walk(const TrChar* key, TrWalkResult& rRes) = 0;
    virtual TrIndex getKeyData() const = 0;

    // For Enumeration
    virtual int         outDegree() const = 0;
    virtual TrSymbolSet outputSet() const = 0;
    virtual void        enumerate(EnumFunc fn) = 0;
};

inline ITrieState::~ITrieState() {}

inline void ITrieState::terminate(TrWalkResult& rRes)
  { walk(TrTerminator, rRes); }

#endif // MIDA_IMITRIE_ITRSTATE_H

