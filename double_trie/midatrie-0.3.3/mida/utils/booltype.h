/* booltype.h
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
// booltype.h - to simulate ISO C++ bool type
//              if not provided by the C++ compiler
// Created: 17 Oct 1998
// Author: Theppitak Karoonboonyanan
//

#ifndef MIDA_UTILS_BOOLTYPE_H
#define MIDA_UTILS_BOOLTYPE_H

# if defined(_BOOL)
#   define __HAS_BOOL
# elif defined(__BOOL)
#   define __HAS_BOOL
# elif defined(__GNUC__)
#   if (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
#     define __HAS_BOOL
#   endif
# elif defined(_MSC_VER) && _MSC_VER > 1000
#   define __HAS_BOOL
# elif defined(__BORLANDC__) && __BORLANDC__ >= 0x500
#   define __HAS_BOOL
# endif

#ifndef __HAS_BOOL
enum { false = 0, true = 1 };
typedef int bool;
#endif // __HAS_BOOL

#endif  // MIDA_UTILS_BOOLTYPE_H

