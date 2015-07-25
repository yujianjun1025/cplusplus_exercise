/* triedefs.cxx
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
// triedefs.cxx - common definitions for Trie classes
// Created: 12 Feb 2001 (split from trie.cxx)
// Author:  Theppitak Karoonboonyanan
//

#include "triedefs.h"

//////////////// Implementation of TrSymbolSet class ////////////////
TrSymbolSet::TrSymbolSet()
{
    makeNull();
}

TrSymbolSet::TrSymbolSet(int nElements, ...)
{
    ASSERT(nElements <= TrAlphabetSize);
    va_list args;
    va_start(args, nElements);

    int i;
    for (i = 0; i < nElements; i++) {
        symbols_[i] = TrChar(va_arg(args, int));
    }
    symbols_[i] = 0;

    va_end(args);

    nSymbols_ = nElements;
}

const TrSymbolSet& TrSymbolSet::operator+=(TrChar c)
{
    symbols_[nSymbols_++] = c;
    symbols_[nSymbols_] = 0;
    return *this;
}

const TrSymbolSet& TrSymbolSet::operator-=(TrChar c)
{
    TrChar* p = symbols_;
    while ((p = (TrChar*)strchr((char*)p, (char)c)) != 0) {
        // shift the following chars
        TrChar* q = p;
        while (*q) {
            *q = *(q+1);
            q++;
        }
        nSymbols_--;
    }
    return *this;
}

//////////////// Implementation of helping functions ////////////////
// A helping function: append a terminator char to the key
// and return pointer to an internal static buffer containing the result
const TrChar* AppendTerminator(const TrChar* key)
{
    static TrChar  buff[TrMaxKeyLen+2];

    strncpy((char*)buff, (const char*)key, sizeof(buff)-2);
    int len = strlen((char*)buff);
    buff[len++] = TrTerminator;
    buff[len] = 0;

    return buff;
}

const char* TruncateTerminator(const char* key)
{
    static char  buff[TrMaxKeyLen+1];

    strncpy(buff, key, sizeof(buff)-2);
    if (buff[0]) {
        buff[strlen(buff)-1] = '\0';
    }
    return buff;
}


