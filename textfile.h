// textfile.h: interface for reading and writing text files
// www.lighthouse3d.com
//
// You may use these functions freely.
// they are provided as is, and no warranties, either implicit,
// or explicit are given
//////////////////////////////////////////////////////////////////////

#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <stdlib.h>
#include <string>

using namespace std;

char *textFileRead(string fn);

int textFileWrite(char *fn, char *s);

#endif
