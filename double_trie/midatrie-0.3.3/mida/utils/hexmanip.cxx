/* hexmanip.h
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
// hexmanip.cxx - hexadecimal stream manipulators
// Created: 13 Aug 1999
// Author:  Theppitak Karoonboonyanan
//

#include "hexmanip.h"

//////////////// HexStream() manipulator implementation ////////////////
ostream& HexStream_(ostream& os, BytesData_ bd)
{
    os << hex << setfill('0');

    for (int i = 0; i < bd.nData; ++i) {
        os << setw(2) << (unsigned)(bd.pData[i]);
    }

    os << setfill(' ') << dec;
    return os;
}

#ifdef TEST

#include <iostream.h>

int main()
{
    cout << HexByte('A') << endl;
    cout << HexStream(8, (unsigned char*)"ABCDEFGH") << endl;
    return 0;
}

#endif
