/* pagemap.cxx
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
// pagemap.cxx - page map component for VirtualMem implementation
// Created: 9 Feb 2001 (split from vmem.h)
// Author:  Theppitak Karoonboonyanan
//
// Modification log:
//   * 9 Feb 2001 Theppitak Karoonboonyanan
//     - split from vmem.cxx
//

#include <string.h>
#include "pagemap.h"

static long FileLength_(fstream& fs)
{
    streampos prevPos = fs.tellg();
    fs.seekg(0L, ios::end);
    long len = fs.tellg();
    fs.seekg(prevPos, ios::beg);
    return len;
}

//////////////// implementation of PageMap class ////////////////
PageMap::PageMap()
: totalPages_(0)
{
    // initialize page & frame tables
    resetTables_();
}

PageMap::PageMap(const char* swapFileName, int iosModes)
{
    if (!openSwapFile(swapFileName, iosModes)) {
        totalPages_ = 0;
        resetTables_();
    }
}

PageMap::~PageMap()
{
    if (isSwapFileOpen()) {
        flushSwapFile_();
    }
}

void PageMap::resetTables_()
{
    for (VirtualMem::PageNumber p = 0; p < MaxPages; p++) {
        pages_[p].frameNo  = NoFrame;
        pages_[p].hitCount = 0;
    }
    for (FrameNumber f = 0; f < MaxFrames; f++) {
        frames_[f].pageNo    = VirtualMem::NoPage;
        frames_[f].frame     = (char*)0;
        frames_[f].isChanged = false;
    }
}

bool PageMap::openSwapFile(const char* swapFileName, int iosModes)
{
    if (swapStream_.rdbuf()->is_open()) {
        if (!closeSwapFile()) { return false; }
    }

    swapStream_.open(swapFileName, iosModes | ios::binary);

    // get initial total pages
    if (swapStream_.rdbuf()->is_open()) {
        swapFileName_ = swapFileName;
        totalPages_ = VirtualMem::PageNumber(
            (FileLength_(swapStream_) + VirtualMem::PageSize() - 1)
                / VirtualMem::PageSize()
        );
        resetTables_();
        return true;
    }
    return false;
}

bool PageMap::closeSwapFile()
{
    flushSwapFile_();
    swapStream_.close();
    return !swapStream_.fail();
}

void PageMap::flushSwapFile_()
{
    // flush all buffered frames and free all allocated blocks
    for (FrameNumber f = 0; f < MaxFrames; f++) {
        if (frames_[f].pageNo != VirtualMem::NoPage && frames_[f].isChanged) {
            flushPage_(frames_[f].pageNo, frames_[f].frame);
        }
        delete frames_[f].frame;
    }

    resetTables_();
}

// read the given page from swap file into frameBuf
void PageMap::retrievePage_(VirtualMem::PageNumber pageNo, char* frameBuf)
{
    ASSERT(pageNo < MaxPages);
    swapStream_.clear();
    swapStream_.seekg(long(pageNo)*VirtualMem::PageSize());
    swapStream_.read(frameBuf, VirtualMem::PageSize());
    // fill the remainder (if any) of the frame with zeros
    int nRead = swapStream_.gcount();
    memset(frameBuf + nRead, 0, VirtualMem::PageSize() - nRead);
}

// flush the given page from frameBuf out to swap file
void PageMap::flushPage_(VirtualMem::PageNumber pageNo, const char* frameBuf)
{
    ASSERT(pageNo < MaxPages);
    swapStream_.clear();
    swapStream_.seekp(long(pageNo) * VirtualMem::PageSize());
    swapStream_.write(frameBuf, VirtualMem::PageSize());
}

// reserve a free frame, swapping out some page if needed
PageMap::FrameNumber PageMap::findFreeFrame_()
{
    FrameNumber f;
    // find if there is a frame f such that frames[f].PageNo == NoPage
    for (f = 0; f < MaxFrames; f++) {
        if (frames_[f].pageNo == VirtualMem::NoPage) {
            ASSERT(frames_[f].frame == (char*)0);
            ASSERT(frames_[f].isChanged == false);
            frames_[f].frame = new char[VirtualMem::PageSize()];
            return f;
        }
    }

    // no such frame, try swapping out
    // f := the frame with least HitCount
    unsigned minHit = UINT_MAX;
    for (FrameNumber i = 0; i < MaxFrames; i++) {
        unsigned h = pages_[frames_[i].pageNo].hitCount;
        if (h < minHit) {
            minHit = h;  f = i;
        }
    }
    // if the frame was changed, flush it
    if (frames_[f].isChanged) {
        flushPage_(frames_[f].pageNo, frames_[f].frame);
    }
    // cut links and make it a new frame
    pages_[frames_[f].pageNo].frameNo = NoFrame;
    frames_[f].pageNo    = VirtualMem::NoPage;
    frames_[f].isChanged = false;

    return f;
}

// swap the page into a frame and return the frame swapped
PageMap::FrameNumber PageMap::swapPageIn_(VirtualMem::PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    FrameNumber f = findFreeFrame_();
    pages_[pageNo].frameNo = f;
    frames_[f].pageNo = pageNo;
    retrievePage_(pageNo, frames_[f].frame);

    // update total pages
    if (pageNo > totalPages_)  totalPages_ = pageNo;

    return f;
}

// have the page been buffered in a frame and return the frame number
PageMap::FrameNumber PageMap::validFrameNo_(VirtualMem::PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    FrameNumber f = pages_[pageNo].frameNo;
    return (f == NoFrame) ? swapPageIn_(pageNo) : f;
}

// have the page been present in memory and return the pointer to it
const char* PageMap::pageFrame(VirtualMem::PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    pages_[pageNo].hitCount++;
    return frames_[validFrameNo_(pageNo)].frame;
}

// have the page been present in memory and return the pointer to it
char* PageMap::pageFrameRef(VirtualMem::PageNumber pageNo)
{
    ASSERT(pageNo < MaxPages);
    pages_[pageNo].hitCount++;
    FrameNumber f = validFrameNo_(pageNo);
    frames_[f].isChanged = true;
    return frames_[f].frame;
}


