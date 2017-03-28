#include <stdio.h>
#include <stdlib.h>

#include "readst3.h"
#include "logit.h"
#include "string.h"

// ***************************
/* freebsd doesn't know this function
char *itoa(int value,char* string,int radix)
{
	return "dummy";
};
*/


char *BreakList = DEFAULT_BREAKLIST;


void setBreakList(char *bl)
{
	BreakList = bl;
};

char St :: buf[STRSIZE];


int St :: len()
{
	return strlen(text);
};


void St :: setChar(int pos, char ch)
{
	text[pos]=ch;
};

int St :: toInt()
{
	return atoi(text);
};

void St :: operator= (char *s)
{
	strcpy(text,s);
};

St St :: operator+ (St s)
{
	strcpy(buf,text);
	strcat(buf,s.ch());
	return St(buf);
};

St :: operator char* () const
{
	return (char*)&text;
};

St :: St(char *s)
{
	strcpy(text,s);
};

St :: St(int i)
{
	sprintf(text, "%i", i);
};


char* St :: ch()
{
	return (char*)&text;
};

St :: St()
{
	strcpy(text,"\0");
};

St :: ~St()
{
};

bool parse_break(char ch)
{
	char *br = BreakList;

	while (*br!='\0')
	{
		if (*br==ch)
			return true;

		br++;
	}

	return false;
};


char* seekToNextToken(char *st)
{
	char *s=st;

    while (parse_break(*s) && (*s!=0))
	{
		if(*s==';')
		{
			while ((*s!='\n')&&(*s!=0)) s++;
		}
		else
		s++;
	}

	return s;
};

// ****************************
char* readst(char **st, char *dest)
{
    char *s = *st;
    char *d = dest;

	s=seekToNextToken(s);

    bool quote=false;

    if (*s!=0)
	while ((!parse_break(*s) || quote) && (*s!=0))
    {
		if (*s==QUOTECHAR)
		{
			s++;
			if (quote)
				break;

			quote=true;
		}
		else
		{
			*d = *s; d++; s++;

			if ((int)(d-dest) == STRSIZE)
				break;
		}

    };

    *d = 0;
    *st = s;
    return dest;
};

char *readfile_s(char *n, long *fsize, char *filemode)
{
    FILE *f;

	if ((n==NULL)||(!strcmp(n,"")))
	{
		return NULL;
	}
    
	if ((f=fopen(n,filemode))==NULL)
       {
			return NULL;
       }

    fseek(f,0,SEEK_END); *fsize = ftell(f); fseek(f,0,SEEK_SET);

    char *s = new char[*fsize+1];

	memset(s,0,*fsize);

    fread(s,1,*fsize,f);
    fclose(f);

    char *c = s + *fsize; *c = '\0';

    return s;

};

char *readfile(char *n)
{
	long fs; return readfile_s(n,&fs,"rb");
};

char *readtextfile(char *n)
{
	long fs; return readfile_s(n,&fs,"r");
};


char* cutpath(char *s)
{
                char *z = s + strlen(s);;
                while ((*z!='/')&&(z>=s)) z--;
                z++;
                return z;
};



