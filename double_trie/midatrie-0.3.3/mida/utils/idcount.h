/* idcount.h
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
// idcount.h - class ID counter
// Created: 1 Jun 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_IDCOUNT_H
#define MIDA_UTILS_IDCOUNT_H

//
// IdCounter - generates class ID serial numbers
// ---------
// To assign class ID to a class, declare a IdCounter static member
//
class IdCounter {
public:
    IdCounter() : id_(++LastID_) {}
    int getID() const { return id_; }

private:  // private auto data
    int    id_;

private:  // private static data
    static int LastID_;
};

#endif  // MIDA_UTILS_IDCOUNT_H

