/* porting.h
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
// porting.h - definition of classes for code porting
// Created: 12 Jul 1996
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_PORTING_H
#define MIDA_UTILS_PORTING_H

#include <mida/utils/typedefs.h>

//
// IsBigEndianSystem() -  indicates if the running system is using
//    big-endian storage method,that is, most-significant byte of an
//    integer is stored at low address.
//
// For example, an 2-byte integer of value 0xff00 will be stored in:
//    Little-Endian:  00 FF
//    Big-Endian: FF 00
//
bool IsBigEndianSystem();

// Convertors between Low-Endian and High-Endian systems
uint16 ReverseBytes16(uint16 n);
uint32 ReverseBytes32(uint32 n);

inline uint16 ReverseBytes16(uint16 n)  { return (n<<8) | (n>>8); }

#endif  // MIDA_UTILS_PORTING_H
