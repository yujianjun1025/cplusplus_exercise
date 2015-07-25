/* test-vemem.cxx
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
// test-vmem.cxx - test program for VirtualMem class
// Created: 17 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#include <mida/vmem/vmem.h>
#include <iostream.h>

//static VirtualMem vm("vm1.vm", ios::in|ios::out);

int main()
{
    VirtualMem vm("vm1.vm", ios::in|ios::out);

    vm.setUInt16(0,      0xabcd);
    vm.setUInt32(16000,  0xcdba9876);
    vm.setUInt16(6000,   0xcafe);
    vm.setUInt16(10000,  0xface);
    vm.setUInt16(6002,   0xfeed);
    vm.setUInt32(4,      0xbad0beef);
    vm.setUInt32(16020,  0xdeaddeaf);

    if (
        vm.getUInt16(0)     == 0xabcd      &&
        vm.getUInt32(16000) == 0xcdba9876  &&
        vm.getUInt16(6000)  == 0xcafe      &&
        vm.getUInt16(10000) == 0xface      &&
        vm.getUInt16(6002)  == 0xfeed      &&
        vm.getUInt32(4)     == 0xbad0beef  &&
        vm.getUInt32(16020) == 0xdeaddeaf
    ) {
        cout << "passed" << endl;
        return 0;
    }

    return 1;
}

