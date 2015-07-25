/* strex.h
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
// strex.h - string function extension
// Created: 1 Oct 1998
// Author:  Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_STREX_H
#define MIDA_UTILS_STREX_H

#include <ctype.h>
#include <string.h>

inline int stricmp(const char* s1, const char* s2)
{
    while (toupper(*s1) == toupper(*s2) && *s1 != 0) { ++s1; ++s2; }
    return toupper(*s1) - toupper(*s2);
}

#endif  // MIDA_UTILS_STREX_H

