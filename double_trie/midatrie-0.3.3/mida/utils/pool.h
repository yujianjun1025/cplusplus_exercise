/* pool.h
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
// pool.h - pool of object, described by descriptor (int)
// Created: 27 Dec 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_POOL_H
#define MIDA_UTILS_POOL_H

template <class T, int poolSize>
class pool {
public:
    pool() : lastID_(0) {}

    int alloc(T t) {
        int allocElement = -1;
        if (lastID_ < poolSize) {
            allocElement = lastID_++;
        } else {
            for (int i = 0; i < poolSize; ++i) {
                if (!pool_[i].isUsed) {
                    allocElement = i;
                    break;
                }
            }
        }
        if (allocElement != -1) {
            pool_[allocElement].isUsed = true;
            pool_[allocElement].elm = t;
        }
        return allocElement;
    }

    bool free(int id) {
        if (id < 0 || poolSize <= id || !pool_[id].isUsed) { return false; }
        pool_[id].isUsed = false;
        if (id == lastID_ - 1) { --lastID_; }
        return true;
    }

    T* get(int id) {
        if (id < 0 || poolSize <= id || !pool_[id].isUsed) { return 0; }
        return &pool_[id].elm;
    }

private:  // private data types
    struct Element_ {
        bool isUsed;
        T    elm;

    public:
        Element_() : isUsed(false) {}
    };

private:
    int        lastID_;
    Element_   pool_[poolSize];
};

#endif  // MIDA_UTILS_POOL_H


