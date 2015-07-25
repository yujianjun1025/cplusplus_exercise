/* strset.h
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
// strset.h - strings set class
// Created: 8 May 1998 (split from cstring.h)
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_STRSET_H
#define MIDA_UTILS_STRSET_H

#include <iostream.h>
#include <mida/utils/cstring.h>
#include <mida/utils/list.h>

//
// String Set
// ----------
//
class StringSet : public list<AutoString> {
public:
    StringSet();
    StringSet(const StringSet& aSet);

    // deep copy
    const StringSet& operator=(const StringSet& aSet);

    // deep comparison
    friend bool operator==(const StringSet& set1, const StringSet& set2);
    friend bool operator!=(const StringSet& set1, const StringSet& set2);

    // initialize with strings listed in a single string
    void init(const char* strList, char delim);

    bool contains(const char* aStr) const;
    int  count() const;

    void add(const char* aStr);
    void add(const StringSet& aStrSet);
    void addUnique(const char* aStr);
    void clear();

    AutoString listOut(const char* delimitor) const;
};

inline StringSet::StringSet() : list<AutoString>() {}
inline StringSet::StringSet(const StringSet& aSet) :list<AutoString>(aSet) {}
inline bool operator!=(const StringSet& set1, const StringSet& set2)
    { return !(set1 == set2); }
inline void StringSet::clear() { list<AutoString>::clear(); }

inline ostream& operator<<(ostream& os, const StringSet& aStrSet)
    { return os << (const char*)aStrSet.listOut(","); }

#endif // MIDA_UTILS_STRSET_H

