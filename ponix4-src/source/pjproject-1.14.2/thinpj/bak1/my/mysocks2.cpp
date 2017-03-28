
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#include "mysocks2.h"

#include "mylist.h"
#include "logit.h"
#include "readst3.h"

static struct timeval select_time;

void NetStartup()
{
        WSADATA ws;

        int rc = WSAStartup (MAKEWORD(2, 1), &ws);

        if (rc)
           {
                CRITICAL("NetStartup: unable to initialize winsock 2.1");
                return;
           }

//        INFO(St("NetStartup ok. ") + St(ws.szDescription));
//		INFO(ws.szSystemStatus);

}

void NetShutdown()
{
        WSACleanup();
//        INFO("NetShutdown ok.");
}


SOCKET initSocket()
{
        int x = 1;

        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

        if (s == INVALID_SOCKET)
           {
                CRITICAL("initSocket: can't create socket!");
                return INVALID_SOCKET;
           }

        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x)) < 0 )
           {
                CRITICAL("initSocket: error on setsockopt");
                return INVALID_SOCKET;
           }

        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x)) < 0 )
           {
                CRITICAL("initSocket: error on setsockopt");
                return INVALID_SOCKET;
           }


        return s;
}

TNETClient :: TNETClient()
{
        socket = INVALID_SOCKET;
        connected = 0;
        gotdata = 0;

        memset(&our_addr,0,sizeof(our_addr));
        memset(&remote_addr,0,sizeof(remote_addr));

        name = NULL;

        buf = new char[NETIOBUFFERSIZE]; sbuf = buf;

		if (buf==NULL)
               CRITICAL("TNETClient: can't allocate memory!");

};

void TNETClient :: SetPeekTime(long s, long ms)
{
	peek_time.tv_sec = s;
	peek_time.tv_usec = ms;
}

void TNETClient :: RetrieveOurAddr()
{
	int x = sizeof(sockaddr);
	getsockname(socket,(sockaddr*)&our_addr,&x);
}

unsigned short int TNETClient :: GetOurPort()
{
	return htons(our_addr.sin_port);
};

unsigned short int TNETClient :: GetRemotePort()
{
	return htons(remote_addr.sin_port);
};

int TNETClient :: GetCurrentRcvBufSize()
{
	int value;
	int size=4;

	if (getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *) &value, &size) < 0 )
	{
		CRITICAL(St("GetCurrentRcvBufSize failed: ")+St(WSAGetLastError()));
		return 0;
	}

	return value;
}

void TNETClient :: SetRcvBufSize(int value)
{
	if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *) &value, sizeof(value)) < 0 )
	{
		CRITICAL(St("SetRcvBufSize failed: ")+St(WSAGetLastError()));
		return;
	}

}


TNETClient :: ~TNETClient()
{
        if (socket != INVALID_SOCKET)
			closesocket(socket);

        delete sbuf;

		if (name!=NULL)
			delete name;
};

char TNETClient :: Peek()
{

    if (!connected)
       {
            WARN("TNETClient/peek: not connected");
            return NETERROR;
       }

    fd_set fd_in, fd_out, fd_err;

    FD_ZERO( &fd_in  );
    FD_ZERO( &fd_out );
    FD_ZERO( &fd_err );

    FD_SET (socket, &fd_in);

    if (select(0, &fd_in, &fd_out, &fd_err, &peek_time)<0)
       {
            CRITICAL("TNETClient/peek: select error");
            return NETERROR;
       }

    if (FD_ISSET(socket, &fd_in))
       {
			return Read();	
       }

    return NETNODATA;

};

char TNETClient :: ConnectTo(char *host, unsigned int port)
{
		PHOSTENT phe;

		if (socket!=INVALID_SOCKET)
		{
                CRITICAL("TNETClient/connectto: allready connected!");
                return NETERROR;
		}
	
		phe = gethostbyname(host);

        if (phe == NULL)
           {
                CRITICAL(St("TNETClient/connectto: can't resolve host ")+St(host));
                return NETERROR;
           }

		memcpy((char FAR *)&(remote_addr.sin_addr ), phe->h_addr, phe->h_length);

		remote_addr.sin_family = AF_INET;
		remote_addr.sin_port = htons(port);

        socket = initSocket();

		if (socket==INVALID_SOCKET)
			return NETERROR;

        if (connect(socket, (PSOCKADDR)&remote_addr, sizeof(remote_addr)) < 0)
           {
                CRITICAL(St("TNETClient/connectto: can't connect to host ")+St(host));
                return NETERROR;
           }


        name = strdup(host);

//		INFO(St("TNETClient/connectto: established connection with ")+St(host));
        connected = 1;

        return NETOK;
}


char TNETClient :: AcceptIt(SOCKET s)
{
        int asize = sizeof(remote_addr);

        socket = accept (s, (LPSOCKADDR)&remote_addr, (int FAR *)&asize);

        if (socket == INVALID_SOCKET)
           {
                CRITICAL("TNETClient/accept: invalid socket on accept");
                return NETERROR;
           }

        name = strdup(inet_ntoa(remote_addr.sin_addr));

//		INFO(St("TNETClient/accept: ")+St(name)+St(" connected to socket ")+St((int)socket));

        connected = 1;
        return NETOK;
};


char TNETClient :: SendTxt(char *t)
{
        int rc = send(socket, (char FAR *)t, lstrlen(t), 0);

        if (rc == SOCKET_ERROR)
           {
                CRITICAL("TNETClient/sendtxt: error on write");
                return NETERROR;
           }

        return NETOK;
};


char TNETClient :: Read()
{
        int rc = recv(socket, (char FAR *)buf, NETIOBUFFERSIZE-(buf-sbuf)-1, 0);

        if (rc != 0)
           {
                if (rc == SOCKET_ERROR)
                   {
                        CRITICAL("TNETClient/read: error on read");

						if (socket!=INVALID_SOCKET)
							closesocket(socket);

						connected=0;

                        return NETERROR;
                   }

                buf+=rc;
                gotdata = 1;

				if ((buf-sbuf)>=NETIOBUFFERSIZE-1)
				{
                    CRITICAL("TNETClient/read: data didn't fit into internal buffer");
					return NETNOFIT;
				}

           }
        else
           {
//                INFO(St("TNETClient/read: connection with ")+St(name)+St(" dropped."));

				connected=0;
				closesocket(socket);
				socket = INVALID_SOCKET;

				return NETDROPPED;
           }


        return NETOK;
};

char *TNETClient :: FlushLine(char *r)
{
        if (buf == sbuf) return NULL;

        char *p = sbuf;

        while (p <= buf)
              {
                if (*p=='\0')
                   {
                        strcpy(r,sbuf);

                        while (*p=='\0') p++;

                        if (p<buf)
                           {
                                memmove(sbuf, p, buf - p);
                                buf -= (p - sbuf);
                           } else { buf = sbuf; gotdata = 0; };

                        return r;
                   }
                p++;
              }

        gotdata = 0;
        return NULL;
};

char *TNETClient :: FlushAll(char *r)
{
        if (buf == sbuf)
			return NULL;
		*buf = '\0';
		strcpy(r,sbuf);
		buf = sbuf;
		gotdata=0;
        return r;
};


// TNETClient --------

unsigned short int TNETServer :: GetListeningPort()
{
	int x = sizeof(sockaddr);
	getsockname(sListening,(sockaddr*)&laddr,&x);
	return htons(laddr.sin_port);
};

TNETServer :: TNETServer(unsigned int p)
{

    clients = new TML();
    sListening = initSocket();

    if (sListening == INVALID_SOCKET)
       {
			CRITICAL("TNETServer: invalid listening socket");
			return;
       }

    laddr.sin_addr.s_addr = INADDR_ANY;
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(p);

    int rc = bind (sListening, (LPSOCKADDR)&laddr, sizeof(laddr));

    if (rc == SOCKET_ERROR)
       {
            CRITICAL("TNETServer: can't bind socket (listen)");
            return;
       }

    if (listen(sListening,1) == SOCKET_ERROR)
       {
            CRITICAL("TNETServer: listen error");
			return;
		}


    FD_ZERO( &fd_in );
    FD_ZERO( &fd_out );
    FD_ZERO( &fd_err );

  //INFO(St("TNETServer: listening on port ")+St(p));

};

void TNETServer :: DropAllClients()
{
    TMLSearch *z = new TMLSearch();
    TNETClient *cc;

    z->init(clients->list());

    while (z->more())
          {
            cc = (TNETClient*)z->next();
            clients->remove(cc);
            delete cc;
          }

    delete z;
};

TNETServer :: ~TNETServer ()
{
	DropAllClients();
	delete clients;
    closesocket(sListening);
}

char TNETServer :: NetCycle()
{

    FD_ZERO( &fd_in  );
    FD_SET (sListening, &fd_in);

    acceptedClient = NULL;

    TMLSearch z;
    TNETClient *cc;

    z.init(clients->list());

    while (z.more())
          {
            cc = (TNETClient*)z.next();
            FD_SET (cc->GetSocket(), &fd_in);
          }

    if (select(0, &fd_in, &fd_out, &fd_err, &select_time)<0)
       {
            CRITICAL("NetCycle: select error");
            return NETERROR;
       }

    if (FD_ISSET(sListening, &fd_in))
       {
            acceptedClient = new TNETClient();
            if (acceptedClient->AcceptIt(sListening)>=NETERROR)
				CRITICAL("NetCycle: client wasn't accepted");
			else
				clients->reg(acceptedClient);
       }

    z.init(clients->list());

    while (z.more())
          {
            cc = (TNETClient*)z.next();

            if (FD_ISSET(cc->GetSocket(), &fd_in))
               {
                    if (cc->Read() != NETOK) // dropped/error
                       {
                            clients->remove(cc);
                            cc->connected = 0;
                       }

               }

          }

    return NETOK;
};



