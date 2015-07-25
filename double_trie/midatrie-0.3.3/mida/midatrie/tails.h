/* tails.h
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
// tails.h - tail part of the Trie structure
// Created: 12 Feb 2001 (split from trie.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_MIDATRIE_TAILS_H
#define MIDA_MIDATRIE_TAILS_H

#include <mida/imitrie/triedefs.h>
#include <mida/midatrie/midadefs.h>
#include <mida/vmem/vheap.h>

//
// TailBlock
// ---------
// A data structure to transfer each TAIL element as a whole block.
// The block is composed of :
//    - suffix : string of remaining part of the keys in double-array structure
//    - data   : the data associating with each key
//
class TailBlock {
public:   // public functions
    // Constructors & Destructor
    TailBlock();
    TailBlock(const TrChar* suffix, TrIndex data);
    ~TailBlock();

    // Information Accessing
    const TrChar* suffix() const;
    TrIndex       data()   const;
    void          setSuffix(const TrChar* aSuffix);
    void          setData(TrIndex aData);

private:  // private functions
    // never make a copy
    TailBlock(const TailBlock&);
    const TailBlock& operator=(const TailBlock&);

private:  // private data
    TrChar*  suffix_;
    TrIndex  data_;
};

inline const TrChar* TailBlock::suffix() const { return suffix_; }
inline TrIndex       TailBlock::data()   const { return data_;   }

//
// Tails
// -----
// The suffix part of the keys distinguished by the Branches structure.
// Blocks, each of which contains suffix and associating data, can be allocated,
// but, at present, cannot be freed. It needs further storage compaction stage.
// (See the reason for this in my document.)
//
// The single-step and multiple-step walk on the suffixes are also provided.
// To do so, the TAIL's State is defined as follows:
//
//    lower  8 bits - contains character offset in the suffix string
//    upper 24 bits - contains pointer to the beginning of the block
//
// thus completely describes a certain position in a certain suffix.
//
class Tails {
public:
    // Constructor
    Tails();
    Tails(const char* pathName, int openModes);

    // Swap file status
    bool        open(const char* pathName, int iosModes);
    bool        create(const char* pathName);
    bool        close();
    const char* swapFileName() const;
    bool        isSwapFileOpen() const;
    bool        isSwapFileGood() const;
    bool        isSwapFileBad()  const;
    bool        isSwapFileFail() const;

    // Key data access
    TrIndex getKeyData(MidaState t) const;
    void    setKeyData(MidaState t, TrIndex data);

    // TAIL Block Access
    void  getBlock(MidaState t, TailBlock* pBlock) const;
    void  setBlock(MidaState t, const TailBlock& aBlock);

    // Walking & Retrieval
    TrChar    nextSymbol(MidaState t) const;
    MidaState walk(MidaState t, TrChar c, TrWalkResult& rRes) const;
    MidaState walk(MidaState t, const TrChar* key, TrWalkResult& rRes) const;
    bool      isTerminal(MidaState t) const;

    // Insertion & Deletion
    MidaState allocString(const TrChar* str, TrIndex data);
    void      changeString(MidaState t, const TrChar* str, TrIndex data);
    void      freeBlock(MidaState t);

private:  // private constants
    // TAIL state representation
    enum {
        OffsetBits_ = 8,
        OffsetMask_ = ~((~0L) << OffsetBits_)
    };

    // TAIL block representation
    enum {
        DataOffset_   = 0,
        SuffixOffset_ = 4
    };

private:  // private functions
    // State decoding
    static VirtualMem::Pointer blockBegin_(MidaState t);
    static int     charOffset_(MidaState t);

    // State encoding
    static MidaState makeTailState_(VirtualMem::Pointer blockBegin, int offset);

    // Block representation
    static VirtualMem::Pointer dataBegin_(MidaState t);
    static VirtualMem::Pointer suffixBegin_(MidaState t);

private:  // private data
    VirtualHeap dataHeap_;
};

inline bool Tails::open(const char* pathName, int iosModes)
    { return dataHeap_.open(pathName, iosModes); }
inline bool Tails::create(const char* pathName)
    { return dataHeap_.create(pathName); }
inline bool Tails::close()                { return dataHeap_.close(); }
inline const char* Tails::swapFileName() const
    { return dataHeap_.swapFileName(); }
inline bool Tails::isSwapFileOpen() const { return dataHeap_.isSwapFileOpen(); }
inline bool Tails::isSwapFileGood() const { return dataHeap_.isSwapFileGood(); }
inline bool Tails::isSwapFileBad()  const { return dataHeap_.isSwapFileBad();  }
inline bool Tails::isSwapFileFail() const { return dataHeap_.isSwapFileFail(); }

inline VirtualMem::Pointer Tails::blockBegin_(MidaState t)
    { return t >> OffsetBits_; }
inline int     Tails::charOffset_(MidaState t) { return int(t & OffsetMask_); }

inline MidaState Tails::makeTailState_(VirtualMem::Pointer blockBegin,
                                       int offset)
    { return (blockBegin << OffsetBits_) | (offset & OffsetMask_); }

inline VirtualMem::Pointer Tails::dataBegin_(MidaState t)
    { return blockBegin_(t) + DataOffset_; }
inline VirtualMem::Pointer Tails::suffixBegin_(MidaState t)
    { return blockBegin_(t) + SuffixOffset_; }

// Key data access
inline TrIndex Tails::getKeyData(MidaState t) const
    { return dataHeap_.getUInt32(dataBegin_(t)); }
inline void  Tails::setKeyData(MidaState t, TrIndex data)
    { dataHeap_.setUInt32(dataBegin_(t), data); }

inline TrChar Tails::nextSymbol(MidaState t) const
    { return dataHeap_.getByte(suffixBegin_(t) + charOffset_(t)); }

inline bool Tails::isTerminal(MidaState t) const
{
    return dataHeap_.getByte(suffixBegin_(t)+charOffset_(t)-1) == TrTerminator;
}

#endif  // MIDA_MIDATRIE_TAILS_H

