/* 
   mida - multi-index double-array trie manipulator utility

   Copyright (C) 2001 Theppitak Karoonboonyanan

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <getopt.h>
#include <iostream.h>
#include <strstream.h>
#include <string.h>
#include <mida/midatrie.h>
#include <mida/utils/cstring.h>
#include <mida/utils/list.h>
#include <mida/utils/autoptr.h>

#define _(text) text

#define EXIT_FAILURE 1

extern "C" {
  char *xstrdup (char *p);
}

static void usage (int status);

/* The name the program was run with, stripped of any leading path. */
char *program_name;

/* Option flags and variables */
class ITrieCommand {
public:
  virtual ~ITrieCommand() {}
  virtual bool execute(IMiTrie* pTrie) = 0;
};

class AddWordCommand : public ITrieCommand {
public:
  AddWordCommand(const char* aWord, int indexNo = 0, TrIndex data = 0)
    : word_(aWord), indexNo_(indexNo), data_(data) {}

  bool execute(IMiTrie* pTrie) {
    return pTrie->insertKey(indexNo_, (const TrChar*)(const char*)word_, data_);
  }

private:
  AutoString word_;
  int        indexNo_;
  TrIndex    data_;
};

class DeleteWordCommand : public ITrieCommand {
public:
  DeleteWordCommand(const char* aWord, int indexNo = 0)
    : word_(aWord), indexNo_(indexNo) {}

  bool execute(IMiTrie* pTrie)
    { return pTrie->deleteKey(indexNo_, (const TrChar*)(const char*)word_); }

private:
  AutoString word_;
  int        indexNo_;
};

class AddWordListCommand : public ITrieCommand {
public:
  AddWordListCommand(const char* listFile, int indexNo = 0)
    : listFile_(listFile), indexNo_(indexNo) {}

  bool execute(IMiTrie* pTrie);

private:
  AutoString listFile_;
  int        indexNo_;
};

bool AddWordListCommand::execute(IMiTrie* pTrie)
{
  ifstream listFile;
  if (strcmp(listFile_, "-") == 0) {
    listFile.attach(0);
  } else {
    listFile.open(listFile_);
  }
  if (!listFile.rdbuf()->is_open()) { return false; }
  char buff[256];
  while (listFile.getline(buff, sizeof buff)) {
    char* p = strchr(buff, '\t');
    TrIndex data = 0;
    if (p) {
      *p++ = '\0';
      while (isspace(*p)) { p++; }
      data = atol(p);
    }
    if (!pTrie->insertKey(indexNo_, (const TrChar*)buff, data)) {
      return false;
    }
  }
  return true;
}

class DeleteWordListCommand : public ITrieCommand {
public:
  DeleteWordListCommand(const char* listFile, int indexNo = 0)
    : listFile_(listFile), indexNo_(indexNo) {}

  bool execute(IMiTrie* pTrie);

private:
  AutoString listFile_;
  int        indexNo_;
};

bool DeleteWordListCommand::execute(IMiTrie* pTrie)
{
  ifstream listFile;
  if (strcmp(listFile_, "-") == 0) {
    listFile.attach(0);
  } else {
    listFile.open(listFile_);
  }
  if (!listFile.rdbuf()->is_open()) { return false; }
  char buff[256];
  while (listFile.getline(buff, sizeof buff)) {
    char* p = strchr(buff, '\t');
    if (p) { *p = '\0'; }
    if (!pTrie->deleteKey(indexNo_, (const TrChar*)buff)) {
      return false;
    }
  }
  return true;
}

class RetrieveWordCommand : public ITrieCommand {
public:
  RetrieveWordCommand(const char* word, int indexNo = 0)
    : word_(word), indexNo_(indexNo) {}

  bool execute(IMiTrie* pTrie);

private:
  AutoString word_;
  int        indexNo_;
};

bool RetrieveWordCommand::execute(IMiTrie* pTrie)
{
  TrIndex data = pTrie->retrieve(indexNo_, (const TrChar*)(const char*)word_);
  if (data != TrErrorIndex) {
    cout << data << endl;
    return true;
  }
  return false;
}

class EnumerateCommand : public ITrieCommand {
public:
  EnumerateCommand(int indexNo = 0) : indexNo_(indexNo) {}

  bool execute(IMiTrie* pTrie);

private:
  static bool listFunc(const TrChar* key, TrIndex data);

private:
  int  indexNo_;
};

bool EnumerateCommand::listFunc(const TrChar* key, TrIndex data)
{
  cout << key << '\t' << data << endl;
  return true;
}

bool EnumerateCommand::execute(IMiTrie* pTrie)
{
  auto_ptr<ITrieState> s = pTrie->startState(indexNo_);
  s->enumerate(listFunc);
  return true;
}

class ProgramEnv {
public:
  ProgramEnv();

  void setPath(const char* path);
  void setTrieName(const char* trieName);
  void setNIndices(int nIndices);
  void setCurIndexNo(int indexNo);
  void setCurData(TrIndex curData);

  void addWordCommand(const char* theWord);
  void addWordListCommand(const char* listName);
  void deleteWordCommand(const char* theWord);
  void deleteWordListCommand(const char* listName);
  void retrieveWordCommand(const char* theWord);
  void enumerateCommand();

  bool execute();

private:
  list< auto_ptr<ITrieCommand> > commands_;
  AutoString  path_;
  AutoString  trieName_;
  int         nIndices_;
  int         curIndexNo_;
  TrIndex     curData_;
};

ProgramEnv::ProgramEnv() : nIndices_(1), curIndexNo_(0), curData_(0) {}

void ProgramEnv::setPath(const char* path) { path_ = path; }
void ProgramEnv::setTrieName(const char* trieName) { trieName_ = trieName; }
void ProgramEnv::setNIndices(int nIndices) { nIndices_ = nIndices; }
void ProgramEnv::setCurIndexNo(int indexNo) { curIndexNo_ = indexNo; }
void ProgramEnv::setCurData(TrIndex curData) { curData_ = curData; }

void ProgramEnv::addWordCommand(const char* theWord)
{
  commands_.push_back(
    auto_ptr<ITrieCommand>(new AddWordCommand(theWord, curIndexNo_, curData_))
  );
}
void ProgramEnv::addWordListCommand(const char* listName)
{
  commands_.push_back(
    auto_ptr<ITrieCommand>(new AddWordListCommand(listName, curIndexNo_))
  );
}
void ProgramEnv::deleteWordCommand(const char* theWord)
{
  commands_.push_back(
    auto_ptr<ITrieCommand>(new DeleteWordCommand(theWord, curIndexNo_))
  );
}
void ProgramEnv::deleteWordListCommand(const char* listName)
{
  commands_.push_back(
    auto_ptr<ITrieCommand>(new DeleteWordListCommand(listName, curIndexNo_))
  );
}
void ProgramEnv::retrieveWordCommand(const char* theWord)
{
  commands_.push_back(
    auto_ptr<ITrieCommand>(new RetrieveWordCommand(theWord, curIndexNo_))
  );
}
void ProgramEnv::enumerateCommand()
{
  commands_.push_back(
    auto_ptr<ITrieCommand>(new EnumerateCommand(curIndexNo_))
  );
}

bool ProgramEnv::execute()
{
  MidaTrie theTrie;
  if (theTrie.open(path_, trieName_, ios::in|ios::out) != MidaTrie::NoError) {
    if (theTrie.create(nIndices_, path_, trieName_) != MidaTrie::NoError) {
      cerr << _("Cannot create trie") << " '" << trieName_ << "' " << flush;
      perror("");
      return false;
    }
  }

  list< auto_ptr<ITrieCommand> >::iterator c;
  for (c = commands_.begin(); c != commands_.end(); ++c) {
    if (!(*c)->execute(&theTrie)) { return false; }
  }
  return true;
}

static struct option const long_options[] =
{
  {"index", required_argument, 0, 'i'},
  {"data", required_argument, 0, 'x'},
  {"add", required_argument, 0, 'a'},
  {"add-list", required_argument, 0, 'A'},
  {"delete", required_argument, 0, 'd'},
  {"delete-list", required_argument, 0, 'D'},
  {"retrieve", required_argument, 0, 'r'},
  {"list", no_argument, 0, 'l'},
  {"path", required_argument, 0, 'p'},
  {"nindices", required_argument, 0, 'n'},
  {"help", no_argument, 0, 'h'},
  {"version", no_argument, 0, 'V'},
  {NULL, 0, NULL, 0}
};

static int decode_switches (int argc, char **argv, ProgramEnv& progEnv);

int
main (int argc, char **argv)
{
  ProgramEnv progEnv;
  int i;

  program_name = argv[0];

  i = decode_switches (argc, argv, progEnv);

  /* do the work */
  progEnv.setTrieName(argv[i]);
  if (!progEnv.execute()) { exit(EXIT_FAILURE); }
  exit (0);
}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.  */

static int
decode_switches (int argc, char **argv, ProgramEnv& progEnv)
{
  int c;


  while ((c = getopt_long (argc, argv, 
                           "i:"        /* index */
                           "x:"        /* data */
                           "a:"        /* add */
                           "A:"        /* add-list */
                           "d:"        /* delete */
                           "D:"        /* delete-list */
                           "r:"        /* retrieve */
                           "l"         /* list */
                           "p:"        /* path */
                           "n:"        /* nindices */
                           "h"         /* help */
                           "V",        /* version */
                           long_options, (int *) 0)) != EOF)
    {
      switch (c) {
        case 'i':
          progEnv.setCurIndexNo(atoi(optarg));
          break;

        case 'x':
          progEnv.setCurData(atol(optarg));
          break;

        case 'a':
          progEnv.addWordCommand(optarg);
          break;

        case 'A':
          progEnv.addWordListCommand(optarg);
          break;

        case 'd':
          progEnv.deleteWordCommand(optarg);
          break;

        case 'D':
          progEnv.deleteWordListCommand(optarg);
          break;

        case 'r':
          progEnv.retrieveWordCommand(optarg);
          break;

        case 'l':
          progEnv.enumerateCommand();
          break;

        case 'p':
          progEnv.setPath(optarg);
          break;

        case 'n':
          progEnv.setNIndices(atoi(optarg));
          break;

        case 'V':
          printf ("prog %s\n", VERSION);
          exit (0);

        case 'h':
          usage (0);

        default:
          usage (EXIT_FAILURE);
        }
    }

    if (argc - optind != 1) {
        usage (EXIT_FAILURE);
    }
  return optind;
}


static void
usage (int status)
{
  printf (_("%s - \
multi-index double-array trie manipulator\n"), program_name);
  printf (_("Usage: %s [OPTION]... TRIE\n"), program_name);
  printf (_("\
Options:
  -i, --index IDXNO          set current index number\n\
  -x, --data DATA            set current data for adding\n\
  -a, --add WORD             add WORD to trie\n\
  -A, --add-list LISTFILE    add word list from LISTFILE to trie\n\
  -d, --delete WORD          delete WORD from trie\n\
  -D, --delete-list LISTFILE delete words listed in LISTFILE from trie\n\
  -r, --retrieve WORD        retrieve WORD data from trie\n\
  -l, --list                 list all words in trie \n\
  -p, --path DIR             define trie directory to DIR [default='.']\n\
  -n, --nindices N           define total indices for a new trie\n\
                             ignored if trie already exists [default=1]\n\
  -h, --help                 display this help and exit\n\
  -V, --version              output version information and exit\n\
"));
  exit (status);
}
