#ifndef _TCPSERV_INCLUDED
#define _TCPSERV_INCLUDED

#define TCPSERV_BUFSIZE		2048
#define TCPSERV_HOSTNLEN         128

int     tcpserv_init();
void	tcpserv_shutdown();

int	tcpserv_getsocket();

int     tcpserv_connect(char *host, unsigned int port);
int     tcpserv_listen_and_accept(unsigned int port);

char*	tcpserv_gets(char *buf, int size);
int		tcpserv_read_to_buf();				// call this
char*	tcpserv_getline();					// before this

void	tcpserv_send(char *text);
void	tcpserv_puts(char *text);

void	tcpserv_dumpbuf(char *fname); // debug
int		tcpserv_fail(char *msg);

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

/* that's a printf function */
#ifndef _NO_VA_ARGS
char	tcpserv_buf_pf[TCPSERV_BUFSIZE];
#define		tcpserv_pf(format, ...)	sprintf(tcpserv_buf_pf, format, __VA_ARGS__); tcpserv_send((char*)&tcpserv_buf_pf)
#endif

#endif


