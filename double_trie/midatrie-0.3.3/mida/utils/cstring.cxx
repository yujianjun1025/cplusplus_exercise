/* cstring.cxx
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
// cstring.cxx - a dynamic string class
// Created: 20 Aug. 1996
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include "cstring.h"

//////////////// implementation of String class ////////////////
const AutoString& AutoString::operator=(const AutoString& aString)
{
    return *this = aString.theString_;
}

const AutoString& AutoString::operator=(const char* aStr)
{
    //ASSERT(aStr != theString);
    char* newStr = 0;
    if (aStr != 0) {
        newStr = new char[strlen(aStr)+1];
        strcpy(newStr, aStr);
    }
    delete theString_;
    theString_ = newStr;

    return *this;
}

void AutoString::setChars(const char* aStr, int len)
{
    char* newStr = new char[len+1];
    strncpy(newStr, aStr, len);
    newStr[len] = '\0';
    delete theString_;
    theString_ = newStr;
}

int AutoString::length() const
{
    return theString_ ? strlen(theString_) : 0;
}

bool operator == (const AutoString& str1, const AutoString& str2)
{
    if (str1.theString_ == 0) {
        return str2.theString_ == 0 || str2.theString_[0] == 0;
    } else if (str2.theString_ == 0) {
        return str1.theString_[0] == 0;
    }
    return strcmp(str1.theString_, str2.theString_) == 0;
}

bool operator < (const AutoString& str1, const AutoString& str2)
{
    if (str2.theString_ == 0) {
        return false;
    } else if (str1.theString_ == 0) {
        return str2.theString_[0] != 0;
    }
    return strcmp(str1.theString_, str2.theString_) < 0;
}


//////////////// Test Bed ////////////////
#ifdef CSTRING_TEST

#include <iostream.h>

int main()
{
    {
        AutoString str1;
        str1 = "This is String #1";
        cout << "str1 = [" << str1 << "]" << endl;
    }
    {
        AutoString str2("And this is String #2");
        cout << "str2 = [" << str2 << "]" << endl;
    }
    {
        AutoString str3 = "Ha! String #3!";
        cout << "str3 = [" << str3 << "]" << endl;
    }
    {
        AutoString name1 = "Albert";
        AutoString name2 = "Michael";
        AutoString name3 = "Steven";
        AutoString nullName1;
        AutoString nullName2;

        if (name1 < name2) {
            cout << name1 << " < " << name2 << endl;
        } else {
            cout << name1 << " !< " << name2 << endl;
        }
        if (name3 <= name1) {
            cout << name3 << " <= " << name1 << endl;
        } else {
            cout << name3 << " !<= " << name1 << endl;
        }
        if (nullName1 <= name1) {
            cout << nullName1 << " <= " << name1 << endl;
        } else {
            cout << nullName1 << " !<= " << name1 << endl;
        }
        if (name1 > nullName1) {
            cout << name1 << " > " << nullName1 << endl;
        } else {
            cout << name1 << " !> " << nullName1 << endl;
        }
        if (name1 == nullName1) {
            cout << name1 << " == " << nullName1 << endl;
        } else {
            cout << name1 << " !== " << nullName1 << endl;
        }
        if (name1 == name2) {
            cout << name1 << " == " << name2 << endl;
        } else {
            cout << name1 << " !== " << name2 << endl;
        }
        if (nullName1 == nullName2) {
            cout << nullName1 << " == " << nullName2 << endl;
        } else {
            cout << nullName1 << " !== " << nullName2 << endl;
        }
    }
    return 0;
}

#endif


