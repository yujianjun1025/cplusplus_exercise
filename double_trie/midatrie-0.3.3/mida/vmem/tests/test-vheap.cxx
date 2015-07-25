/* test-vheap.cxx
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
// test-vheap.cxx - test program for VirtualHeap class
// Created: 17 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#include <mida/vmem/vheap.h>
#include <iostream.h>

int main_interactive()
{
    VirtualHeap vh("vh1.vm", ios::in|ios::out);

    char c;
    VirtualMem::Pointer p1 = vh.newBlock(50); vh.printMemoryBlock(); cin.get(c);
    VirtualMem::Pointer p2 = vh.newBlock(50); vh.printMemoryBlock(); cin.get(c);
    VirtualMem::Pointer p3 = vh.newBlock(50); vh.printMemoryBlock(); cin.get(c);
    VirtualMem::Pointer p4 = vh.newBlock(100);vh.printMemoryBlock(); cin.get(c);
    VirtualMem::Pointer p5 = vh.newBlock(30); vh.printMemoryBlock(); cin.get(c);
    VirtualMem::Pointer p6 = vh.newBlock(60); vh.printMemoryBlock(); cin.get(c);
    vh.deleteBlock(p5);            vh.printMemoryBlock(); cin.get(c);
    p5 = vh.newBlock(20);          vh.printMemoryBlock(); cin.get(c);
    vh.deleteBlock(p6);            vh.printMemoryBlock(); cin.get(c);
    vh.deleteBlock(p2);            vh.printMemoryBlock(); cin.get(c);
    p6 = vh.newBlock(10);          vh.printMemoryBlock(); cin.get(c);
    p2 = vh.newBlock(30);          vh.printMemoryBlock(); cin.get(c);
    VirtualMem::Pointer p7 = vh.newBlock(30); vh.printMemoryBlock(); cin.get(c);

    return 0;
}

static void FillBlock(
    VirtualMem& vm, VirtualMem::Pointer p, char filler, int nFill
)
{
    char* pBuff = new char[nFill];
    for (int i = 0; i < nFill; i++) {
        pBuff[i] = filler;
    }
    vm.blockWrite(p, pBuff, nFill);
    delete pBuff;
}

static bool CheckFilledBlock(
    VirtualMem& vm, VirtualMem::Pointer p, char filler, int nFill
)
{
    char* pBuff = new char[nFill];
    int nRead = vm.blockRead(p, pBuff, nFill);
    if (nRead != nFill) { delete pBuff; return false; }
    for (int i = 0; i < nFill; i++) {
        if (pBuff[i] != filler) { delete pBuff; return false; }
    }
    delete pBuff;
    return true;
}

int main()
{
    VirtualHeap vh("vh1.vm", ios::in|ios::out);

    VirtualMem::Pointer p1 = vh.newBlock(50); FillBlock(vh, p1, 'a', 50);
    VirtualMem::Pointer p2 = vh.newBlock(50); FillBlock(vh, p2, 'b', 50);
    VirtualMem::Pointer p3 = vh.newBlock(50); FillBlock(vh, p3, 'c', 50);
    VirtualMem::Pointer p4 = vh.newBlock(100); FillBlock(vh, p4, 'd', 100);
    VirtualMem::Pointer p5 = vh.newBlock(30); FillBlock(vh, p5, 'e', 30);
    VirtualMem::Pointer p6 = vh.newBlock(60); FillBlock(vh, p6, 'f', 60);

    if (!CheckFilledBlock(vh, p1, 'a', 50)) { return 1; }
    if (!CheckFilledBlock(vh, p2, 'b', 50)) { return 1; }
    if (!CheckFilledBlock(vh, p3, 'c', 50)) { return 1; }
    if (!CheckFilledBlock(vh, p4, 'd', 100)) { return 1; }
    if (!CheckFilledBlock(vh, p5, 'e', 30)) { return 1; }
    if (!CheckFilledBlock(vh, p6, 'f', 60)) { return 1; }

    vh.deleteBlock(p5);
    p5 = vh.newBlock(20); FillBlock(vh, p5, 'g', 20);
    vh.deleteBlock(p6);
    vh.deleteBlock(p2);
    p6 = vh.newBlock(10); FillBlock(vh, p6, 'h', 10);
    p2 = vh.newBlock(30); FillBlock(vh, p2, 'i', 30);
    VirtualMem::Pointer p7 = vh.newBlock(30); FillBlock(vh, p7, 'j', 30);

    if (!CheckFilledBlock(vh, p1, 'a', 50)) { return 1; }
    if (!CheckFilledBlock(vh, p2, 'i', 30)) { return 1; }
    if (!CheckFilledBlock(vh, p3, 'c', 50)) { return 1; }
    if (!CheckFilledBlock(vh, p4, 'd', 100)) { return 1; }
    if (!CheckFilledBlock(vh, p5, 'g', 20)) { return 1; }
    if (!CheckFilledBlock(vh, p6, 'h', 10)) { return 1; }
    if (!CheckFilledBlock(vh, p7, 'j', 30)) { return 1; }

    cout << "passed" << endl;
    return 0;
}

