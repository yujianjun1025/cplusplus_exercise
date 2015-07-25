/* test-atrie.cxx
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
// test-atrie.cxx - test program for ApproxTrie class
// Created: 17 Mar 2001
// Author:  Theppitak Karoonboonyanan
//

#include <mida/atrie.h>
#include <mida/midatrie.h>

static void DumpStrings_(const StringSet& aSet)
{
    for (StringSet::const_iterator p = aSet.begin(); p != aSet.end(); ++p) {
        cout << (const char*)*p << endl;
    }
}

static bool TestSampleWord_()
{
    MidaTrie aTrie(1, 0, "testatrie");

    aTrie.insertKey(0, (const TrChar*)"��",   1);
    aTrie.insertKey(0, (const TrChar*)"��",   2);
    aTrie.insertKey(0, (const TrChar*)"�ô",  3);
    aTrie.insertKey(0, (const TrChar*)"��á", 4);
    aTrie.insertKey(0, (const TrChar*)"��ҡ", 5);
    aTrie.insertKey(0, (const TrChar*)"�ҡ",  6);
    aTrie.insertKey(0, (const TrChar*)"á",   7);
    aTrie.insertKey(0, (const TrChar*)"á�",  8);
    aTrie.insertKey(0, (const TrChar*)"�á", 9);
    aTrie.insertKey(0, (const TrChar*)"�á", 10);

    StringSet answer;
    answer.add("��");
    answer.add("��");
    answer.add("�ô");
    answer.add("��á");
    answer.add("��ҡ");
    answer.add("�ҡ");
    answer.add("á");
    answer.add("á�");
    answer.add("�á");
    answer.add("�á");

    ApproxTrie approxTrie(aTrie);
    StringSet words;
    int resCnt = approxTrie.approxSearch(0, (const TrChar*)"�á", &words);
    cout << "words found approximately �á: " << words.listOut("; ") << endl;
    cout << "correct answer: " << answer.listOut("; ") << endl;
    // check equality of 'words' and 'answer'
    StringSet::const_iterator i;
    for (i = words.begin(); i != words.end(); ++i) {
        if (!answer.contains(*i)) { return false; }
    }
    for (i = answer.begin(); i != answer.end(); ++i) {
        if (!words.contains(*i)) { return false; }
    }
    return true;
}

void TestLexDict_()
{
    MidaTrie aTrie(1, 0, "lexdct");

    StringSet words;
    for (;;) {
        int  indexNo;
        TrChar key[TrMaxKeyLen+1];

        cout << "> " << flush;
        cin >> indexNo;
        if (indexNo == -1)  break;
        cin >> key;
        ApproxTrie approxTrie(aTrie);
        int resCnt = approxTrie.approxSearch(indexNo, key, &words);
        cout << resCnt << " words" << endl;
        DumpStrings_(words);
    }
}

int main()
{
    if (!TestSampleWord_()) { return 1; }
    // TestLexDct_();
    return 0;
}


