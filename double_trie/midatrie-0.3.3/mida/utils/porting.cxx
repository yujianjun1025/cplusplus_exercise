/* porting.cxx
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
// porting.cxx - classes for code porting
// Created: 12 Jul 1996
// Author:  Theppitak Karoonboonyanan
//

#include "porting.h"

bool IsBigEndianSystem()
{
    static uint16 tester = ~0xff;
    return *(unsigned char*)&tester == 0xff;
}

uint32 ReverseBytes32(uint32 n)
{
    uint16* p = (uint16*)&n;
    *p = ReverseBytes16(*p);  p++;
    *p = ReverseBytes16(*p);
    return (n>>16) | (n<<16);
}

#ifdef PORTING_TEST

#include <iostream.h>

int main()
{
    if (IsHighEndianSystem()) {
        cout << "High-Endian!" << endl;
    } else {
        cout << "Low-Endian!" << endl;
    }

    uint16 i16 = 0xdeaf;
    cout << "ReverseBytes(" << hex << i16 << ") = ";
    cout << hex << ReverseBytes16(i16) << endl;

    uint32 i32 = 0xdeafcafe;
    cout << "ReverseBytes(" << hex << i32 << ") = ";
    cout << hex << ReverseBytes32(i32) << endl;

    return 0;
}

#endif // PORTING_TEST
