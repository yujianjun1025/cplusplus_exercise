/* autoptr.h
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
// autoptr.h - auto-destructed pointer
// Created: 15 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_AUTOPTR_H
#define MIDA_UTILS_AUTOPTR_H

template <class P>
class auto_ptr {
private:
    class ref_cnt {
    public:
        ref_cnt(P* p) : ref_(1), p_(p) {}
        ~ref_cnt() { delete p_; }
 
        ref_cnt<P>* ref()   { ++ref_; return this; }
        void        deref() { if (--ref_ == 0) { delete this; } }

        P& operator*() const { return *p_; }
        P* operator->() const { return p_; }
 
    private:
        int ref_;
        P*  p_;
    };

public:
    auto_ptr() : p_(new ref_cnt<P>(0)) {}
    auto_ptr(P* p) : p_(new ref_cnt<P>(p)) {}
    auto_ptr(const auto_ptr<P>& ap) : p_(ap.p_->ref()) {}
    ~auto_ptr() { p_->deref(); }

    P& operator*() const { return **p_; }
    P* operator->() const { return p_->operator->(); }
    const auto_ptr<P>& operator=(P* p) {
        p_->deref();
        p_ = new ref_cnt<P>(p);
        return *this;
    }
    const auto_ptr<P>& operator=(const auto_ptr<P>& ap) {
        if (&ap != this) { p_ = ap.p_->ref(); }
        return *this;
    }

private:
    ref_cnt* p_;
};

#endif // MIDA_UTILS_AUTOPTR_H

