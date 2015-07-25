/* list.h
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
// list.h - to simulate STL's <list>
// Created: 30 Sep 1998
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_LIST_H
#define MIDA_UTILS_LIST_H

typedef int size_type;

template <class T>
class list {
private:
    class listElm {
    public:
        listElm(const T& t) : t(t), pPrev(this), pNext(this) {}
        listElm(const T& t, listElm* pPrev, listElm* pNext)
            : t(t), pPrev(pPrev), pNext(pNext) {}

        listElm*  prev() const { return pPrev; }
        listElm*  next() const { return pNext; }
        void      setPrev(listElm* e) { pPrev = e; }
        void      setNext(listElm* e) { pNext = e; }

        const T&  val()   const { return t; }
        const T*  pVal()  const { return &t; }
        T&        val()         { return t; }
        T*        pVal()        { return &t; }

    protected:
        listElm*  pPrev;
        listElm*  pNext;
        T         t;
    };

public:
    class iterator {
    public:
        iterator() : e(0) {}
        iterator(listElm* e) : e(e) {}
        iterator(const iterator& i) : e(i.e) {}
        T&       operator*()        { return e->val(); }
        T*       operator->()       { return e->pVal(); }
        const T& operator*()  const { return e->val(); }
        const T* operator->() const { return e->pVal(); }
        iterator& operator++()    { e = e->next(); return *this; }
        iterator  operator++(int)
            { iterator t = *this; ++*this; return t; }
        iterator& operator--()    { e = e->prev(); return *this; }
        iterator  operator--(int)
            { iterator t = *this; --*this; return t; }
        bool operator==(const iterator& x) const { return e == x.e; }
        bool operator!=(const iterator& x) const { return !(*this==x); }
    public:
        listElm* node() { return e; }
    protected:
        listElm* e;
    };
    class reverse_iterator : public iterator {
    public:
        reverse_iterator(listElm* e) : iterator(e) {}
        reverse_iterator(const iterator& i) : iterator(i) {}
    };
    class const_iterator : public iterator {
    public:
        const_iterator() {}
        const_iterator(listElm* e) : iterator(e) {}
        const_iterator(const iterator& i) : iterator(i) {}
        const T& operator*()  const { return e->val(); }
        const T* operator->() const { return e->pVal(); }
        const_iterator& operator++()    { e = e->next(); return *this; }
        const_iterator  operator++(int) {
            const_iterator t = *this; ++*this; return t;
        }
        const_iterator& operator--()    { e = e->prev(); return *this; }
        const_iterator  operator--(int) {
            const_iterator t = *this; --*this; return t;
        }
        bool operator==(const const_iterator& x) const { return e == x.e; }
        bool operator!=(const const_iterator& x) const { return !(*this==x); }
    };
    class const_reverse_iterator : public const_iterator {
    public:
        const_reverse_iterator(listElm* e) : const_iterator(e) {}
        const_reverse_iterator(const iterator& i)
            : const_iterator(i) {}
    };

public:
    list() : pHead(new listElm(T())), sz(0) {}
    list(size_type n, const T& t) : pHead(new listElm(T())), sz(0)
        { insert(begin(), n, t); }
    list(const list<T>& x) : pHead(new listElm(T())), sz(0)
        { insert(begin(), x.begin(), x.end()); }
    list(const_iterator f, const_iterator l) : pHead(new listElm(T())), sz(0)
        { insert(begin(), f, l); }
    ~list() { erase(begin(), end()); delete pHead; }

    iterator       begin() { return iterator(pHead->next()); }
    iterator       end()   { return iterator(pHead); }
    const_iterator begin() const { return const_iterator(pHead->next()); }
    const_iterator end()   const { return const_iterator(pHead); }

    reverse_iterator       rbegin() { return reverse_iterator(pHead->prev()); }
    reverse_iterator       rend()   { return reverse_iterator(pHead); }
    const_reverse_iterator rbegin() const
        { return const_reverse_iterator(pHead->prev()); }
    const_reverse_iterator rend()   const
        { return const_reverse_iterator(pHead); }

    list<T>& operator=(const list<T>& x)
    {
        if (this != &x) { assign(x.begin(), x.end()); }
        return *this;
    }
    size_type size()  const { return sz; }
    bool      empty() const { return size() == 0; }

    T&       front()       { return *begin(); }
    const T& front() const { return *begin(); }
    T&       back()        { return *--end(); }
    const T& back()  const { return *--end(); }
    void     push_front(const T& t) { insert(begin(), t); }
    void     pop_front()   { erase(begin()); }
    void     push_back(const T& t)  { insert(end(), t); }
    void     pop_back()    { erase(--end()); }
    void     assign(const_iterator f, const_iterator l)
    {
        erase(begin(), end());
        insert(begin(), f, l);
    }
    void     assign(size_type n, const T& t)
    {
        erase(begin(), end());
        insert(begin(), n, t);
    }
    iterator insert(iterator p, const T& t)
    {
        listElm* node = p.node();
        listElm* befo = node->prev();
        listElm* newNode = new listElm(t, befo, node);
        node->setPrev(newNode);
        befo->setNext(newNode);
        ++sz;
        return iterator(newNode);
    }
    void     insert(iterator p, size_type n, const T& t)
    {
        for ( ; 0 < n; --n) {
            insert(p, t);
        }
    }
    void     insert(iterator p, const T* f, const T* l)
    {
        for ( ; f != l; ++f) {
            insert(p, *f);
        }
    }
    void     insert(iterator p, const_iterator f, const_iterator l)
    {
        for ( ; f != l; ++f) {
            insert(p, *f);
        }
    }
    iterator erase(iterator p)
    {
        listElm* node = p.node();
        node->prev()->setNext(node->next());
        node->next()->setPrev(node->prev());
        delete node;
        --sz;
        return p;
    }
    iterator erase(iterator f, iterator l)
    {
        while (f != l) {
            erase(f++);
        }
        return f;
    }
    void     clear()  { erase(begin(), end()); }

private:
    listElm*   pHead;
    size_type  sz;
};

#endif  // MIDA_UTILS_LIST_H

/*
 * Copyright (c) 1998 by Theppitak Karoonboonyanan. All rights reserved.
 */

/*
 * Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 */

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Hewlett-Packard Company makes no representations about the
 * suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 */


