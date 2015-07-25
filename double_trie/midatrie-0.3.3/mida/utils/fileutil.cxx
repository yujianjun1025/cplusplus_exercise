/* fileutil.cxx
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
// fileutil.cxx - file utility functions
// Created : 11 Jul 1996
// Author:  Theppitak Karoonboonyanan
//

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fstream.h>
#include "fileutil.h"
#include "strstrm.h"  // #include <strstream.h>

#if defined(_WIN32) || defined(DOS)
#  include <direct.h>
#elif defined(unix)
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

#define FILEBUFFSIZE 1024

static char buf[FILEBUFFSIZE];

AutoString FullPath(const char* dir, const char* file, const char* ext)
{
    ostrstream path(buf, sizeof buf);
    if (dir) {
        path << dir;
        if (dir[strlen(dir)-1] != DIR_DELIM) { path << DIR_DELIM; }
    }
    path << file;
    if (ext) {
        path << '.' << ext;
    }
    path << ends;
    return buf;
}

int CopyFile(const char* from, const char* to)
{
    ifstream fFrom(from, ios::binary);
    ofstream fTo(to, ios::trunc|ios::binary);

    if (!fFrom) { return -1; }
    if (!fTo)   { return -1; }

    while (fFrom.read(buf, sizeof buf)) {
        if (!fTo.write(buf, fFrom.gcount())) { return -1; }
    }

    return 0;
}

int MoveFile(const char* from, const char* to)
{
    /* try renaming first, if fail, make a copy */
    if (rename(from, to) == 0) {
        return 0;
    } else {
        if (CopyFile(from, to) == 0) {
            remove(from);
            return 0;
        }
    }

    return -1;
}

// make dir if not exists
bool PrepareDir(const char* dir)
{
#if defined(_WIN32) || defined(DOS)
    if (_mkdir(dir) == 0 || errno == EEXIST) { return true; }
// #elif defined(SYSV)
#else
    if (mkdir(dir, 0777) == 0 || errno == EEXIST) { return true; }
#endif

    return false;
}
