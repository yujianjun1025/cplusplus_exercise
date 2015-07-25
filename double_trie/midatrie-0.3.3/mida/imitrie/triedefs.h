/* triedefs.h
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
// triedefs.h - common definitions for Trie classes
// Created: 12 Feb 2001 (split from trie.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_IMITRIE_TRIEDEFS_H
#define MIDA_IMITRIE_TRIEDEFS_H

#include <mida/utils/typedefs.h>
#include <mida/utils/iter.h>
#include <string.h>
#include <stdarg.h>

typedef int32 TrIndex;  // index into data associating to keys

const TrIndex TrErrorIndex = -1;
const TrIndex TrDummyIndex = 0x7fffffff;

// Alphabet set
typedef unsigned char TrChar;
const int    TrAlphabetSize = 256;
const TrChar TrTerminator   = 255;

const int TrMaxKeyLen = 1024;

//
// TrSymbolSet
// -----------
// class of trie symbol set
//
class TrSymbolSet {
public:
    class SymbolIterator : public Iterator<TrChar> {
    public:
        SymbolIterator(const TrChar* pFirst) : pCur_(pFirst) {}
        SymbolIterator(const SymbolIterator& aIter) : pCur_(aIter.pCur_) {}

        virtual void first() {}
        virtual void next() { if (*pCur_) ++pCur_; }
        virtual bool isDone() const { return *pCur_ == '\0'; }

        virtual TrChar currentItem() const { return *pCur_; }

    private:
        const TrChar* pCur_;
    };

public:
    // Constructors
    TrSymbolSet();
    TrSymbolSet(int nElements, ...);

    // Symbol List
    SymbolIterator first() const { return SymbolIterator(symbols_); }

    // Basic Operations
    int  nElements() const        { return nSymbols_; }
    bool contains(TrChar c) const { return strchr((char*)symbols_, c) != 0; }
    void makeNull()               { symbols_[0] = 0; nSymbols_ = 0; }

    // Binary Set Operators
    const TrSymbolSet& operator+=(TrChar c);
    const TrSymbolSet& operator-=(TrChar c);
/*
    const TrSymbolSet& operator+=(const TrSymbolSet& aSet);
    const TrSymbolSet& operator-=(const TrSymbolSet& aSet);
    const TrSymbolSet& operator*=(const TrSymbolSet& aSet);

    friend TrSymbolSet operator+(const TrSymbolSet& set1, TrChar c);
    friend TrSymbolSet operator-(const TrSymbolSet& set1, TrChar c);
    friend TrSymbolSet operator+(const TrSymbolSet& set1, const TrSymbolSet& set2);
    friend TrSymbolSet operator-(const TrSymbolSet& set1, const TrSymbolSet& set2);
    friend TrSymbolSet operator*(const TrSymbolSet& set1, const TrSymbolSet& set2);
*/

private:  // private data
    TrChar symbols_[TrAlphabetSize+1];
    int    nSymbols_;
};

//
// TrWalkResult
// ------------
// class for keeping trie walk result
//
enum TrEWalkResult {
    Tr_INTERNAL, Tr_TERMINAL, Tr_CRASH
};
class TrWalkResult {
public:
    TrWalkResult() : result_(Tr_INTERNAL), nWalked_(0) {}
    TrWalkResult(TrEWalkResult result, int nWalked)
        : result_(result), nWalked_(nWalked) {}

    TrEWalkResult getWalkResult() const { return result_; }
    int           getNWalked()    const { return nWalked_; }

    void          setWalkResult(TrEWalkResult result) { result_ = result; }
    void          setNWalked(int nWalked) { nWalked_ = nWalked; }

private:
    TrEWalkResult result_;
    int           nWalked_;
};

// helper functions
const TrChar* AppendTerminator(const TrChar* key);
const char*   TruncateTerminator(const char* key);


#endif  // MIDA_IMITRIE_TRIEDEFS_H

