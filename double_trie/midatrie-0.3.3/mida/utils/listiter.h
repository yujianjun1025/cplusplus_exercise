/* listiter.h
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
// list iter.h - iterator class template for list<> template
// Created: 26 Jan 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_LISTITER_H
#define MIDA_UTILS_LISTITER_H

#include <mida/utils/list.h>
#include <mida/utils/iter.h>
#include <mida/utils/booltype.h>

//
// ListIterator
//

template <class Item>
class ListIterator : public Iterator<Item> {
public:
    ListIterator(const list<Item>& lst) : lst_(lst) {}

    virtual void first()        { current_ = lst_.begin(); }
    virtual void next()         { ++current_; }
    virtual bool isDone() const { return current_ != lst_.end(); }

    virtual Item currentItem() const { return *current_; }

private:
    const list<Item>& lst_;
    list<Item>::iterator current_;
};


#endif  // MIDA_UTILS_LISTITER_H

