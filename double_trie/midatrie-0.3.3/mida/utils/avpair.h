/* avpair.h
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
// avpair.h - Attribute-Value pair
// Created: 20 Jan 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_AVPAIR_H
#define MIDA_UTILS_AVPAIR_H

#include <mida/utils/cstring.h>
#include <mida/utils/list.h>

//
// AV Pair
//

class AVPair {
public:
    AVPair();
    AVPair(const char* attrib);
    AVPair(const char* attrib, const char* val);
    AVPair(const AVPair& aPair);

    const char* getAttrib() const;
    const char* getVal() const;

    void setAttrib(const char* aAttrib);
    void setVal(const char* aVal);

private:
    AutoString attrib_;
    AutoString val_;
};

inline AVPair::AVPair() {}
inline AVPair::AVPair(const char* attrib) : attrib_(attrib), val_() {}
inline AVPair::AVPair(const char* attrib, const char* val)
    : attrib_(attrib), val_(val) {}
inline AVPair::AVPair(const AVPair& aPair)
    : attrib_(aPair.attrib_), val_(aPair.val_) {}

inline const char* AVPair::getAttrib() const { return attrib_; }
inline const char* AVPair::getVal()    const { return val_; }

inline void AVPair::setAttrib(const char* aAttrib) { attrib_ = aAttrib; }
inline void AVPair::setVal(const char* aVal) { val_ = aVal; }


//
// AVList
//
class AVList : public list<AVPair> {
public:
    AVList();
    AVList(const list<AVPair>& avlist);
    AVList(const AVList& avlist);

    const char* getAttrib(const char* attrib) const;
    void        setAttrib(const char* attrib, const char* val);
};

inline AVList::AVList() {}
inline AVList::AVList(const list<AVPair>& avlist) : list<AVPair>(avlist) {}
inline AVList::AVList(const AVList& avlist) : list<AVPair>(avlist) {}

#endif  // MIDA_UTILS_AVPAIR_H

