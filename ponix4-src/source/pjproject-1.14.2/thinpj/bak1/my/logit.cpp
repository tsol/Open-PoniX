#include "logit.h"
#include "mylist.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

bool	bSysConChanged=false;
bool	bWasCritical=false;

TML		lSysConStrings;

char	defaultLogFile[256] = {"main.log"};
bool	bIncludeTime=true;


void	SetDefaultLogFile(char *f)
{
	strcpy(defaultLogFile,f);	
};

void	SetIncludeTimeInLogs(bool b)
{
	bIncludeTime=b;
};

void LOG(char *s)
{
	LogIt(defaultLogFile,s);
}

// --- System Console Functions

void sc_resetcritical()
{
	bWasCritical=false;
}

bool sc_wascritical()
{
	return bWasCritical;
};

bool sc_changed()
{
	return bSysConChanged;
}

TSysConString :: TSysConString()
{
	Type = SC_INFO;
	Text = NULL;
};

TSysConString :: ~TSysConString()
{
	if (Text!=NULL)
		delete Text;
}

void	sc_add(SysConStringType t, char* s)
{
	TSysConString *cs = new TSysConString();
	cs->Text = strdup(s);
	cs->Type = t;

	if (t==SC_CRITICAL)
		bWasCritical=true;

	if (t>=SC_LOG_LEVEL)
		LOG(s);

	lSysConStrings.add(cs);
	bSysConChanged=true;
}

/*
bool	sc_has(SysConStringType t)
{
	TSysConString *cs;
	TMLSearch b; b.init(lSysConStrings.list());

	while (b.more())
	{
		cs = (TSysConString*)b.next();
		if (cs->Type>=t)
			return true;
	}

	return false;
};
*/

void	sc_flush2screen()
{
	TSysConString *cs;
	TMLSearch b; b.init(lSysConStrings.list());

	while (b.more())
	{
		cs = (TSysConString*)b.next();
		printf("%s\n",cs->Text);
		lSysConStrings.remove(cs);
		delete cs;
	}

	bSysConChanged=false;

};

void	sc_flush()
{
	TSysConString *cs;
	TMLSearch b; b.init(lSysConStrings.list());

	while (b.more())
	{
		cs = (TSysConString*)b.next();
		lSysConStrings.remove(cs);
		delete cs;
	}

	bSysConChanged=false;

};


char*	sc_get(SysConStringType t, char *s)
{
	bSysConChanged=false;

	TSysConString *cs;
	TMLSearch b; b.init(lSysConStrings.list());

	while (b.more())
	{
		cs = (TSysConString*)b.next();
		if (cs->Type>=t)
		{
			strcpy(s,cs->Text);
			lSysConStrings.remove(cs);
			delete cs;
			return s;
		}

	}

	return NULL;	
};

// --- LOG functions

void LogIt(bool bTime, char *fname, char *s)
{
	FILE *f;

	char *lf = "\n";

	if ((f=fopen(fname,"ab"))==NULL)
		return;

    fseek(f,0,SEEK_END);
	//long fsize = ftell(f); fseek(f,0,SEEK_SET);

	char tbuffer [9];
  
	if (bTime && (strlen(s)>0) )
	{
		_strdate(tbuffer);
		fwrite(tbuffer,1,strlen(tbuffer),f);
		fwrite(" ",1,1,f);
		_strtime(tbuffer);
		fwrite(tbuffer,1,strlen(tbuffer),f);
		fwrite(" ",1,1,f);
	}

	fwrite(s,1,strlen(s),f);
	fwrite(lf,1,strlen(lf),f);

    fclose(f);
};

void LogIt(char *fname, char *s)
{
	LogIt(bIncludeTime,fname,s);
};
