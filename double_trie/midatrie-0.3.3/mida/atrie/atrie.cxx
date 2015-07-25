/* atrie.cxx
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
// atrie.cxx - the Approximate Trie class
// Created: 24 Oct 1996
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include "atrie.h"
#include <mida/utils/cstring.h>
#include <mida/utils/strset.h>
#include <mida/utils/autoptr.h>

//////////////// Implementation of ApproxTrie class ////////////////
int ApproxTrie::approxSearch(
    int indexNo, const TrChar* key, StringSet* pResults /* default = 0 */
)
{
    auto_ptr<ITrieState> s = miTrie_.startState(indexNo);
    TrWalkResult r;

    int keyLen = strlen((const char*)key);
    TrChar* walkableKey = new TrChar[keyLen+2];
    strcpy((char*)walkableKey, (const char*)AppendTerminator(key));
    const TrChar* p = walkableKey;

    char* approx = new char[keyLen+3];
    char* pApprox = approx;

    int   restKeyLen = strlen((const char*)p);
    int   resCnt = 0;

    while (restKeyLen > 0) {
        //
        // I. Assume *p is the error position
        //

        // 1. *p is an extra char  [try skipping *p]
        if (restKeyLen > 1) {
            auto_ptr<ITrieState> t = s->clone();
            t->walk(p+1, r);
            if (r.getWalkResult() == Tr_TERMINAL) {
                strcpy(pApprox, (const char*)(p+1));
                if (pResults) {
                    pResults->addUnique(TruncateTerminator(approx));
                }
                resCnt++;
            }
        }

        // 2. missing char before *p  &  3. *p is a wrong char
        TrSymbolSet outputs = s->outputSet();
        for (TrSymbolSet::SymbolIterator i = outputs.first();
             !i.isDone();
             i.next())
        {
            auto_ptr<ITrieState> t = s->clone();
            t->walk(i.currentItem(), r);
            ASSERT(r.getWalkResult() != Tr_CRASH);
            if (r.getWalkResult() == Tr_INTERNAL) {
                *pApprox = i.currentItem();

                // 2. missing char before *p  [*pSym was assumed inserted]
                auto_ptr<ITrieState>(t->clone())->walk(p, r);
                if (r.getWalkResult() == Tr_TERMINAL) {
                    strcpy(pApprox+1, (const char*)p);
                    if (pResults) {
                        pResults->addUnique(TruncateTerminator(approx));
                    }
                    resCnt++;
                }

                // 3. *p is a wrong char  [*pSym was assumed replacing *p]
                if (restKeyLen > 1) {
                    auto_ptr<ITrieState>(t->clone())->walk(p+1, r);
                    if (r.getWalkResult() == Tr_TERMINAL) {
                        strcpy(pApprox+1, (const char*)(p+1));
                        if (pResults) {
                            pResults->addUnique(TruncateTerminator(approx));
                        }
                        resCnt++;
                    }
                }
            }
        }

        // 4. *p and *(p+1) is misplaced   [try swapping *p and *(p+1)]
        if (restKeyLen > 2) {
            auto_ptr<ITrieState> t = s->clone();
            t->walk(*(p+1), r);
            if (r.getWalkResult() == Tr_INTERNAL) {
                *pApprox = *(p+1);
                t->walk(*p, r);
                if (r.getWalkResult() == Tr_INTERNAL) {
                    *(pApprox+1) = *p;
                    t->walk(p+2, r);
                    if (r.getWalkResult() == Tr_TERMINAL) {
                        strcpy(pApprox+2, (const char*)(p+2));
                        if (pResults) {
                            pResults->addUnique(TruncateTerminator(approx));
                        }
                        resCnt++;
                    }
                }
            }
        }

        //
        // II. Then, continue if *p is a correct char
        //

        s->walk(*p, r);
        if (r.getWalkResult() == Tr_CRASH)  { break; }
        *pApprox++ = *p++;
        restKeyLen--;
        if (r.getWalkResult() == Tr_TERMINAL) {
            ASSERT(restKeyLen == 0);
            // indeed, key is included in the trie!
            // for now, just add the key to the list and continue searching
            *pApprox = '\0';
            if (pResults) {
                pResults->addUnique(TruncateTerminator(approx));
            }
            resCnt++;
        }
    }

    delete approx;
    delete walkableKey;

    return resCnt;
}

