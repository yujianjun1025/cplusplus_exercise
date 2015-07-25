/* hasher.h
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
// hasher.h - templates for safe hashing
// Created: 26 Apr 1999
// Author:  Theppitak Karoonboonyanan
// Reference:
//   Michael P. Lindner, "Designing for Safe Hashing", C++ Report,
//       November-December 1996.
//

#ifndef MIDA_UTILS_HASHER_H
#define MIDA_UTILS_HASHER_H

typedef unsigned long hash_t;

class hash_stream {
public:  // public functions
    hash_stream(hash_t modulo) : h_(0), modulo_(modulo) {}

    hash_stream& operator<<(char c)
    {
        append_hash_key_((unsigned char&)c);  return *this;
    }
    hash_stream& operator<<(unsigned char c)
    {
        append_hash_key_(c);  return *this;
    }
    hash_stream& operator<<(int i)
    {
        for (int b = sizeof(int); b > 0; --b) {
            append_hash_key_((unsigned char)(i & 0xff));
            i >>= 8;
        }
        return *this;
    }
    hash_stream& operator<<(unsigned int i)
    {
        for (int b = sizeof(unsigned int); b > 0; --b) {
            append_hash_key_((unsigned char)(i & 0xff));
            i >>= 8;
        }
        return *this;
    }
    hash_stream& operator<<(short i)
    {
        for (int b = sizeof(short); b > 0; --b) {
            append_hash_key_((unsigned char)(i & 0xff));
            i >>= 8;
        }
        return *this;
    }
    hash_stream& operator<<(unsigned short i)
    {
        for (int b = sizeof(unsigned short); b > 0; --b) {
            append_hash_key_((unsigned char)(i & 0xff));
            i >>= 8;
        }
        return *this;
    }
    hash_stream& operator<<(long l)
    {
        for (int b = sizeof(long); b > 0; --b) {
            append_hash_key_((unsigned char)(l & 0xff));
            l >>= 8;
        }
        return *this;
    }
    hash_stream& operator<<(unsigned long l)
    {
        for (int b = sizeof(unsigned long); b > 0; --b) {
            append_hash_key_((unsigned char)(l & 0xff));
            l >>= 8;
        }
        return *this;
    }
    hash_stream& operator<<(const char* s)
    {
        for ( ; *s; ++s) {
            append_hash_key_(*(const unsigned char*)s);
        }
        return *this;
    }
    hash_stream& operator<<(const unsigned char* s)
    {
        for ( ; *s; ++s) {
            append_hash_key_(*s);
        }
        return *this;
    }

    hash_t hash_val() const { return h_; }

private:  // private functions
    void append_hash_key_(unsigned char c) { h_ = (h_*3 + c) % modulo_; }

private:  // private data
    hash_t h_;
    hash_t modulo_;
};


template <class Key>
inline hash_t key_hash(const Key& k, hash_t modulo)
{
    hash_stream h(modulo);
    h << k;
    return h.hash_val();
}


#endif  // MIDA_UTILS_HASHER_H

