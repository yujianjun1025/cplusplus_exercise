/* iter.h
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
// iter.h - iterator abstract class template
// Created: 26 Jan 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_ITER_H
#define MIDA_UTILS_ITER_H

//
// Iterator
//

template <class Item>
class Iterator {
public:
    virtual ~Iterator() {}

    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;

    virtual Item currentItem() const = 0;

protected:
    Iterator() {}
};

#endif  // MIDA_UTILS_ITER_H

