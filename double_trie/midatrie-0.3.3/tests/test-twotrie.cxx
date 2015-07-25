/* test-twotrie.cxx
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
// test-twotrie.cxx - test program for TwoTrie class
// Created: 16 Apr 2001
// Author:  Theppitak Karoonboonyanan
//

#include <mida/twotrie.h>
#include <mida/utils/autoptr.h>
#include "idx1.h"
#include "idx2.h"
#include "idx3.h"
#include "idx4.h"
#include <iomanip.h>

static bool TestInsertDelete_(TwoTrie& twoTrie)
{
    twoTrie.insertKey(0, (TrChar*)"progress", 1);
    twoTrie.insertKey(0, (TrChar*)"programme", 2);
    twoTrie.insertKey(0, (TrChar*)"produce", 3);
    twoTrie.insertKey(0, (TrChar*)"producer", 4);
    twoTrie.insertKey(0, (TrChar*)"pro", 5);

    if (twoTrie.retrieve(0, (TrChar*)"progress") != 1) { return false; }
    if (twoTrie.retrieve(0, (TrChar*)"programme") != 2) { return false; }
    if (twoTrie.retrieve(0, (TrChar*)"produce") != 3) { return false; }
    if (twoTrie.retrieve(0, (TrChar*)"producer") != 4) { return false; }
    if (twoTrie.retrieve(0, (TrChar*)"pro") != 5) { return false; }

    twoTrie.deleteKey(0, (TrChar*)"producer");
    twoTrie.deleteKey(0, (TrChar*)"produce");
    twoTrie.deleteKey(0, (TrChar*)"progress");    

    twoTrie.insertKey(0, (TrChar*)"pass", 6);

    if (twoTrie.retrieve(0, (TrChar*)"progress") != TrErrorIndex) {
        return false;
    }
    if (twoTrie.retrieve(0, (TrChar*)"programme") != 2) { return false; }
    if (twoTrie.retrieve(0, (TrChar*)"produce") != TrErrorIndex) {
        return false;
    }
    if (twoTrie.retrieve(0, (TrChar*)"producer") != TrErrorIndex) {
        return false;
    }
    if (twoTrie.retrieve(0, (TrChar*)"pro") != 5) { return false; }
    if (twoTrie.retrieve(0, (TrChar*)"pass") != 6) { return false; }

    return true;
}

static bool ShowKey_(const TrChar* key, TrIndex data)
{
    cout << '(' << (const char*)key << ',' << data << ')' << endl;
    return true;
}

static bool TestQuery_(TwoTrie& twoTrie)
{
    char    buff1[256];
    TrIndex data;

    for (;;) {
        cout << "> " << flush;
        cin >> buff1;
        if (strcmp(buff1, "i") == 0) {
            cin >> buff1 >> data;
            if (twoTrie.insertKey(0, (TrChar*)buff1, data)) {
                cout << '(' << buff1 << ',' << data << ") inserted." << endl;
            } else {
                cout << "Can't insert (" << buff1 << ',' << data << ")" << endl;
            }
        } else if (strcmp(buff1, "d") == 0) {
            cin >> buff1;
            if (twoTrie.deleteKey(0, (TrChar*)buff1)) {
                cout << '(' << buff1 << ") deleted." << endl;
            } else {
                cout << "Can't delete (" << buff1 << ")" << endl;
            }
        } else if (strcmp(buff1, "r") == 0) {
            cin >> buff1;
            data = twoTrie.retrieve(0, (TrChar*)buff1);
            if (data != TrErrorIndex) {
                cout << data << endl;
            } else {
                cout << "Can't retrieve (" << buff1 << ")" << endl;
            }
        } else if (strcmp(buff1, "l") == 0) {
            auto_ptr<ITrieState> s = twoTrie.startState(0);
            s->enumerate(ShowKey_);
//        } else if (strcmp(buff1, "c") == 0) {
//            twoTrie.compact();
        } else if (strcmp(buff1, "q") == 0) {
            break;
        } else {
            cout << "Unknown command" << endl;
        }
        cin.getline(buff1, 256);
    }
    return true;
}

static bool TestLargeDict_(TwoTrie& twoTrie, const char* srcFileName)
{
    ifstream source(srcFileName);
    char buff[256];
    TrIndex idx = 0;
    while (source.getline(buff, sizeof buff)) {
        cout << buff << endl;
        if (!twoTrie.insertKey(0, (const TrChar*)buff, idx)) {
            return false;
        }
        idx++;
    }
    return true;
}

const int NIndices = 4;
static const char* const * const Sources_[NIndices] = {
    idx1, idx2, idx3, idx4
};

static bool TestMultiIndex_(TwoTrie& twoTrie)
{
    // insert keys
    int j = 0;
    for (j = 0; ; j++) {
        for (int i = 0; i < NIndices; i++) {
            if (!Sources_[i][j])  { goto finish_insert; }
            if (twoTrie.retrieve(i, (const TrChar*)Sources_[i][j])
                    == TrErrorIndex)
            {
                cout << setw(3) << i+1 << ": ";
                cout << setw(i+1) << " " << Sources_[i][j] << endl;
                if (!twoTrie.insertKey(i, (const TrChar*)Sources_[i][j],
                                            TrIndex(j)))
                {
                    return false;
                }
            }
        }
    }

finish_insert:
    for (--j ; j >= 0; j--) {
        for (int i = 0; i < NIndices; i++) {
            if (twoTrie.retrieve(i, (const TrChar*)Sources_[i][j])
                    == TrErrorIndex)
            {
                return false;
            }
        }
    }
    return true;
}

int main()
{
    TwoTrie aTrie(NIndices, 0, "testtrie");

    cout << "Testing inserting & deleting... " << flush;
    if (!TestInsertDelete_(aTrie)) { cout << "fail" << endl; return 1; }
    cout << "ok" << endl;

//    TestQuery_(pTwoTrie);
//    if (!TestLargeDict_(pTwoTrie, "aa.1")) { return 1; }

    cout << "Testing reopenning trie... " << flush;
    aTrie.close();
    aTrie.open(0, "testtrie", ios::in|ios::out);
    if (!aTrie.isOpen()) { cout << "fail" << endl; return 1; }
    cout << "ok" << endl;

    cout << "Testing multiindex... " << flush;
    if (!TestMultiIndex_(aTrie)) { cout << "fail" << endl; return 1; }
    cout << "ok" << endl;

    return 0;
}

