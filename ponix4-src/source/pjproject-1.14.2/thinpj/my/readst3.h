#ifndef __READST3
#define __READST3

#define QUOTECHAR '"'
#define STRSIZE 255		// maximum St size
#define FNAME_STRSIZE 255	// maximum file name size

#define DEFAULT_BREAKLIST " \t\r\n;,@()\0"

void setBreakList(char *bl);

class St
{
	static	char buf[STRSIZE]; // buffer
	char	text[STRSIZE];
public:
	St();
	St(char *s);
	St(int i);

	~St();

	char* ch();
	int toInt();

	int len();
	void setChar(int pos, char ch);

	operator char* () const;
	St operator+ (St s);	
	void operator= (char *s);
};

char* seekToNextToken(char *st);
char *readst(char **st, char *dest);
        // get next token starting from *st
        // write it to dest and return
        // position *st to the end of the token
        // (prepare for next call)

char *readfile_s(char *n, long *fsize, char *filemode);
char *readfile(char *n);
char *readtextfile(char *n);

char* cutpath(char *s);

#endif
