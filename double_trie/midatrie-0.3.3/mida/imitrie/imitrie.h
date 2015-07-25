/* imitrie.h
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
// imitrie.h - multi-index trie interface
// Created: 6 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_IMITRIE_IMITRIE_H
#define MIDA_IMITRIE_IMITRIE_H

#include <mida/imitrie/triedefs.h>

class ITrieState;

//
// IMiTrie (Multi-Index Trie Interface)
// -------
//
class IMiTrie {
public:
    typedef bool (*EnumFunc)(const TrChar* key, TrIndex data);

public:
    virtual ~IMiTrie();

    // File Status
    virtual bool isOpen() const = 0;

    virtual int  getNIndices() const = 0;

    // General Operations
    virtual bool insertKey(int indexNo, const TrChar* key,
                           TrIndex dataIdx = TrDummyIndex) = 0;
    virtual bool deleteKey(int indexNo, const TrChar* key) = 0;
    virtual TrIndex retrieve(int indexNo, const TrChar* key) const = 0;

    // Walking & Retrieval
    virtual ITrieState* startState(int indexNo) const = 0;
};

inline IMiTrie::~IMiTrie() {}

#endif // MIDA_IMITRIE_IMITRIE_H

