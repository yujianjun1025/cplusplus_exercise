/* atrie.h
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
// atrie.h - the Approximate Trie class
// Created: 24 Oct 1996
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_ATRIE_ATRIE_H
#define MIDA_ATRIE_ATRIE_H

#include <mida/imitrie/imitrie.h>
#include <mida/imitrie/itrstate.h>
#include <mida/utils/strset.h>

////////////////
//  Contents  //
////////////////
class ApproxTrie;


//
// Approx Trie
// -----------
//
class ApproxTrie {
public:
    ApproxTrie(const IMiTrie& miTrie);

    // returns total number of approximate words found
    int approxSearch(int indexNo, const TrChar* key, StringSet* pResults = 0);

private:
    const IMiTrie& miTrie_;
};

inline ApproxTrie::ApproxTrie(const IMiTrie& miTrie) : miTrie_(miTrie) {}

#endif  // MIDA_ATRIE_ATRIE_H

