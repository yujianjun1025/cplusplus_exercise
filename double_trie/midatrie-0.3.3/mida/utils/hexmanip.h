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
// hexmanip.h - hexadecimal stream manipulators
// Created: 13 Aug 1999
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_HEXMANIP_H
#define MIDA_UTILS_HEXMANIP_H

#include <iomanip.h>

//
// HexByte(unsigned char b)
// : ostream manipulator
//
inline ostream& HexByte_(ostream& os, unsigned char c)
{
    return os << hex << setw(2) << setfill('0')
              << (unsigned int)c << setfill(' ') << dec;
}

inline omanip<unsigned char> HexByte(unsigned char c)
{
    return omanip<unsigned char>(HexByte_, c);
}


//
// HexStream(const unsigned char* pData, int nData)
// : ostream manipulator
//
struct BytesData_ {
    int                  nData;
    const unsigned char* pData;
public:
    BytesData_(int nData, const unsigned char* pData)
        : nData(nData), pData(pData) {}
};

ostream& HexStream_(ostream& os, BytesData_ bd);

inline omanip<BytesData_> HexStream(int nData, const unsigned char* pData)
{
    return omanip<BytesData_>(HexStream_, BytesData_(nData, pData));
}


#endif  // MIDA_UTILS_HEXMANIP_H


