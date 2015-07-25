/* cstring.h
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
// cstring.h - a dynamic string class
// Created: 20 Aug. 1996
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_CSTRING_H
#define MIDA_UTILS_CSTRING_H

#include <mida/utils/booltype.h>

//
// AutoString
// ----------
// shields up C null-terminated string, with constructor and destructor
// to automatically create and destroy the string array.
//
// Implementation Note:
//    The conditions
//       theString == 0
//    and
//       theString != 0 && theString[0] == '\0'
//    are equivalent and both represent a zero-length string.
//
class AutoString {
public:
    AutoString();
    AutoString(const char* aStr);
    AutoString(const char* aStr, int len);
    AutoString(const AutoString& aString);
    ~AutoString();

    const AutoString& operator=(const AutoString& aString);
    const AutoString& operator=(const char* aStr);
    void  setChars(const char* aStr, int len);

    int   length() const;
    operator const char*() const;
    // operator const unsigned char*() const;

    friend bool operator==(const AutoString& str1, const AutoString& str2);
    friend bool operator!=(const AutoString& str1, const AutoString& str2);
    friend bool operator< (const AutoString& str1, const AutoString& str2);
    friend bool operator> (const AutoString& str1, const AutoString& str2);
    friend bool operator<=(const AutoString& str1, const AutoString& str2);
    friend bool operator>=(const AutoString& str1, const AutoString& str2);

private:
    char* theString_;
};

inline AutoString::AutoString() : theString_(0) {}
inline AutoString::AutoString(const char* aStr) : theString_(0)
    { *this = aStr; }
inline AutoString::AutoString(const char* aStr, int len) : theString_(0)
    { setChars(aStr, len); }
inline AutoString::AutoString(const AutoString& aString) : theString_(0)
    { *this = aString; }
inline AutoString::~AutoString()  { delete theString_; }

inline AutoString::operator const char*() const
    { return theString_ ? theString_ : ""; }
/*
inline AutoString::operator const unsigned char*() const
    { return (const unsigned char*)(theString ? theString : ""); }
*/

inline bool operator!=(const AutoString& str1, const AutoString& str2)
    { return !(str1 == str2); }
inline bool operator>(const AutoString& str1, const AutoString& str2)
    { return str2 < str1; }
inline bool operator<=(const AutoString& str1, const AutoString& str2)
    { return !(str1 > str2); }
inline bool operator>=(const AutoString& str1, const AutoString& str2)
    { return !(str1 < str2); }


#endif  // MIDA_UTILS_CSTRING_H

