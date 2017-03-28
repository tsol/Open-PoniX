#ifndef __THASH
#define __THASH

#include "readst3.h"

/*
** My Texthash. Used mainly for .ini file reading
**
*/

#define TH_MAXPARAMS		20
#define TH_MAXPLEN		32
#define TH_MAXVLEN		80

class Thash
{
	private:

        char empty[1];

	char params[TH_MAXPARAMS][TH_MAXPLEN];
	char values[TH_MAXPARAMS][TH_MAXVLEN];

 public:

        Thash();
        ~Thash();

	char *get(char *param);			// returns ref to empty on fail
	char set(char *param, char *value);	// returns 1 - updated, 2-new, 0-fail
	char del(char *param);		// returns 1/0
	char cmp(char *param, char *value); // compares hash value with given value

	void clear();
	char loadFile(char *filename);		// returns 1/0
	char parseString(char *st);     // parses string of the form
                                        // "param1='value1' param2='value2'..."
	St toSt();			// save to St string
		

};

#endif
