#ifndef __LOGIT
#define __LOGIT

// todo: totally rewrite. static array, buffered logging

#include "mylist.h"

enum SysConStringType {SC_INFO,SC_WARNING,SC_CRITICAL};

#define SC_LOG_LEVEL	SC_INFO	// log to file all that is greater that this (or equal)

#define IS_CRITICAL		sc_wascritical()
#define CRITICAL(p)		sc_add(SC_CRITICAL,p)
#define WARN(p)			sc_add(SC_WARNING,p)
#define INFO(p)			sc_add(SC_INFO,p)

void LOG(char *s);

class TSysConString : public Listable
{
public:
	SysConStringType	Type;
	char				*Text;

	TSysConString();
	~TSysConString();

};

bool	sc_changed();
bool	sc_wascritical();
void	sc_resetcritical();
void	sc_add(SysConStringType t, char* s);
char*	sc_get(SysConStringType t, char *s);

void	LogIt(char *f, char *s);
void	LogIt(bool bTime, char *fname, char *s);

void	SetDefaultLogFile(char *f);
void	SetIncludeTimeInLogs(bool b);

void	sc_flush2screen();
void	sc_flush();
 
#endif
