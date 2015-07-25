/* hashtable.h
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
// hashtable.h - associative memory class template
// Created: 27 Apr 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_HASHTABLE_H
#define MIDA_UTILS_HASHTABLE_H

#include <mida/utils/hasher.h>
#include <assert.h>

// Note: assumes long is at least 32 bits.
static const int NumPrimes_ = 28;
static const unsigned long Primes_[NumPrimes_] =
{
    53,         97,         193,       389,       769,
    1543,       3079,       6151,      12289,     24593,
    49157,      98317,      196613,    393241,    786433,
    1572869,    3145739,    6291469,   12582917,  25165843,
    50331653,   100663319,  201326611, 402653189, 805306457, 
    1610612741, 3221225473UL, 4294967291UL
};

// find lowest i : Primes_[i] >= n
inline unsigned long NextPrime_(unsigned long n)
{
    int l = 0, h = NumPrimes_;
    while (l < h - 1) {
        int i = (l + h)/2;
        if (Primes_[i] == n) { return n; }
        else if (Primes_[i] < n) { l = i; }
        else { h = i; }

        // check loop invariant
        assert(Primes_[l] <= Primes_[h]);
        assert(n <= Primes_[h]);
    }
    return Primes_[h];
}

template <class Key, class Val>
class hash_table {
public:  // public type definitions
    typedef Key key_type;
    typedef Val value_type;

public:  // public member functions
    hash_table()
        : table_size_(Primes_[0]), n_data_(0)
        { p_hash_cells_ = new t_hash_cell_[table_size_]; }
    hash_table(int table_size)
        : table_size_(NextPrime_(table_size)), n_data_(0)
        { p_hash_cells_ = new t_hash_cell_[table_size_]; }
    ~hash_table()  { delete[] p_hash_cells_; }

    const value_type* operator[](const key_type& k) const
    {
        int index = lookup_(k);
        return (index == -1) ? 0 : &p_hash_cells_[index].value;
    }

    value_type* operator[](const key_type& k)
    {
        int index = lookup_(k);
        return (index == -1) ? 0 : &p_hash_cells_[index].value;
    }

    bool add(const key_type& k, const value_type& v)
    {
        resize(n_data_ + 1);
        return add_noresize_(k, v);
    }
    bool remove(const key_type& k)
    {
        int index = lookup_(k);
        if (index < 0) { return false; }

        p_hash_cells_[index].flag = STF_DELETED;

        --n_data_;

        return true;
    }
    void resize(int size_hint)
    {
        if (size_hint <= table_size_) { return; }
        int new_size = NextPrime_(size_hint);
        if (new_size > table_size_) {
            t_hash_cell_* old_table = p_hash_cells_;
            p_hash_cells_ = new t_hash_cell_[new_size];
            int old_table_size = table_size_;
            table_size_ = new_size;
            n_data_ = 0;
            for (int i = 0; i < old_table_size; ++i) {
                if (old_table[i].flag == STF_USED) {
                    add_noresize_(old_table[i].key, old_table[i].value);
                }
            }
            delete[] old_table;
        }
    }

private:  // private constants & typedefs
    enum t_flag_ {
        STF_UNUSED,
        STF_DELETED,
        STF_USED
    };

    struct t_hash_cell_ {
        t_flag_     flag;
        key_type    key;
        value_type  value;

    public:
        t_hash_cell_() : flag(STF_UNUSED) {}
    };

private:  // private functions
    // lookup_()
    // returns the cell for the key k
    //         -1 if not found
    int lookup_(const key_type& k) const
    {
       int h = key_hash<key_type>(k, table_size_);
       int i = h, d = 1;
       while (
           (p_hash_cells_[i].flag == STF_USED && p_hash_cells_[i].key != k)
           || p_hash_cells_[i].flag == STF_DELETED
       ) {
           i = (i + d) % table_size_;
           if (i == h) { return -1; }
           d += 2;
       }
       return (p_hash_cells_[i].flag == STF_USED) ? i : -1;
    }
    // find_free_cell_()
    // returns free cell index
    //         -1 if table full
    //         -2 if k already exists
    int find_free_cell_(const key_type& k) const
    {
       int h = key_hash<key_type>(k, table_size_);
       int i = h, d = 1;
       while (p_hash_cells_[i].flag == STF_USED) {
           if (p_hash_cells_[i].key == k) { return -2; }
           i = (i + d) % table_size_;
           if (i == h) { return -1; }
           d += 2;
       }
       return i;
    }

    bool add_noresize_(const key_type& k, const value_type& v)
    {
        int index = find_free_cell_(k);
        if (index < 0) { return false; }

        p_hash_cells_[index].flag  = STF_USED;
        p_hash_cells_[index].key   = k;
        p_hash_cells_[index].value = v;

        ++n_data_;

        return true;
    }

private:  // private data
    int           table_size_;
    int           n_data_;
    t_hash_cell_* p_hash_cells_;
};

#endif  // MIDA_UTILS_HASHTABLE_H

