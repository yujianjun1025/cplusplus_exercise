/* strset.cxx
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
// strset.cxx - strings set class
// Created: 8 May 1998 (split from cstring.h)
// Author:  Theppitak Karoonboonyanan
//

#include "strset.h"
#include "strstrm.h"  // #include <strstream.h>
#include <string.h>
#include <ctype.h>


//////////////// Implemantation of StringSet class ////////////////
const StringSet& StringSet::operator=(const StringSet& aSet)
{
    clear();
    for (list<AutoString>::const_iterator i = aSet.begin();
         i != aSet.end(); ++i)
    {
        add(*i);
    }
    return *this;
}

bool operator==(const StringSet& set1, const StringSet& set2)
{
    list<AutoString>::const_iterator i1 = set1.begin();
    list<AutoString>::const_iterator i2 = set2.begin();
    while (i1 != set1.end() && i2 != set2.end() && (*i1 == *i2)) {
        ++i1; ++i2;
    }
    return (i1 == set1.end()) && (i2 == set2.end());
}

void StringSet::init(const char* strList, char delim)
{
    clear();
    const char* pBeg = strList;
    for (;;) {
        // skip white spaces
        while (*pBeg && isascii(*pBeg) && isspace(*pBeg)) { pBeg++; }
        if (*pBeg == '\0')  { break; }

        // locate next delimitor and mark it
        const char* pEnd = strchr(pBeg, delim);
        if (pEnd != 0)  {
            add(AutoString(pBeg, pEnd - pBeg));
            pBeg = pEnd + 1;  // skip the delimitor
        } else {
            add(pBeg);
            break;
        }
    }
}

bool StringSet::contains(const char* aStr) const
{
    for (list<AutoString>::const_iterator i = begin(); i != end(); ++i) {
        if (strcmp(*i, aStr) == 0) { return true; }
    }
    return false;
}

int StringSet::count() const
{
    int cnt = 0;
    for (const_iterator i = begin(); i != end(); ++i) { ++cnt; }
    return cnt;
}

void StringSet::add(const char* aStr)
{
    push_back(AutoString(aStr));
}

void StringSet::add(const StringSet& aStrSet)
{
    for (list<AutoString>::const_iterator i = aStrSet.begin();
         i != aStrSet.end();
         ++i)
    {
        add(*i);
    }
}

void StringSet::addUnique(const char* aStr)
{
    if (!contains(aStr)) {
        add(aStr);
    }
}

AutoString StringSet::listOut(const char* delimitor) const
{
    ostrstream buff;
    if (!empty()) {
        list<AutoString>::const_iterator i = begin();
        buff << *i;
        while (++i != end()) {
            buff << delimitor << *i;
        }
    }
    buff << ends;
    AutoString res(buff.str());
    buff.rdbuf()->freeze(false);
    return res;
}

