#ifndef __MYSOCKS2
#define __MYSOCKS2

#include <winsock2.h>
#include "mylist.h"

//#define  NETIOBUFFERSIZE   1048576	// 1M
#define  NETIOBUFFERSIZE   32768
#define  NETPORT           5000

#define  NETOK          0
#define  NETDROPPED     1
#define  NETNODATA      2
#define  NETNOFIT       3

#define  NETERROR       64


void NetStartup();
void NetShutdown();

SOCKET initSocket();


class TNETClient : public Listable
{
	private:
	friend class TNETServer;

	SOCKET		socket;
        sockaddr_in	our_addr, remote_addr;

        char    *buf, *sbuf;

        char    *name;
        char    connected, gotdata;

	struct	timeval peek_time;

 public:

        TNETClient();
        ~TNETClient();
		
	void			RetrieveOurAddr();	// run this after ConnectTo to find out
											// our port etc.
	unsigned short int	GetOurPort();
	unsigned short int	GetRemotePort();

	int			GetCurrentRcvBufSize();
	void		SetRcvBufSize(int value);

	char*		Name() { return name; };

	SOCKET		GetSocket() { return socket; };

	bool		IsConnected() { return connected!=0; };
	bool		IsGotData() { return gotdata!=0; };

	void		SetPeekTime(long s, long ms); // slows all select calls

        char      	Peek();	// check if incoming data is avail. read() it
							// use it only in client applications when no TServer is used
							// (TServer->NetCycle performs such IO for all of its clients)

        char      ConnectTo(char* host, unsigned int port); // connect to host
        char      AcceptIt(SOCKET s);						// accept from listening socket

        char      SendTxt(char* t);             // send string (w\o \0)
        char      Read();                       // read data to internal buf

        char*     FlushLine(char* r);           // get \0 terminated string from buf (NULL and no flush if string is incomplete)
	char*	  FlushAll(char *r);			// get ALL contents of the buf
};


class TNETServer
{
        TML			*clients;

        SOCKET		sListening;

        sockaddr_in	laddr;
        fd_set		fd_in, fd_out, fd_err;

        TNETClient		*acceptedClient;

 public:

        TNETServer(unsigned int p);
        ~TNETServer();

	TNETClient	*GetAcceptedClient() { return acceptedClient; };
	void		DropAllClients();
        char		NetCycle();

	unsigned short int GetListeningPort();

};


#endif
