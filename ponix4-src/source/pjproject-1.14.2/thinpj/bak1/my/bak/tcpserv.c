#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "tcpserv.h"

int tcpserv_fail(char *msg)
{
	printf("tcpserv_fail: %s\n", msg);
	tcpserv_shutdown();
	tcpserv_init_and_accept();
}

int tcpserv_init_and_accept()
{
	tcpserv_socket = -1;
	memset((char*)&tcpserv_buf,0,TCPSERV_BUFSIZE);

	int 	 sd, sd_current, cc, fromlen, tolen;
	int 	 addrlen;
	struct   sockaddr_in sin;
	struct   sockaddr_in pin;

	/* get an internet domain socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(TCPSERV_PORT);

	int val = 1;
 	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	/* bind the socket to the port number */
	if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
		perror("bind");
		exit(1);
	}

	/* show that we are willing to listen */
	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}

	/* wait for a client to talk to us */
        addrlen = sizeof(pin); 
	if ((sd_current = accept(sd, (struct sockaddr *)  &pin, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}

	close(sd);

/*
	printf("Hi there, from  %s#\n",inet_ntoa(pin.sin_addr));
	printf("Coming from port %d\n",ntohs(pin.sin_port));
*/
	tcpserv_socket = sd_current;
	return tcpserv_socket;
}


char *tcpserv_gets(char buf[], int size) 
{
	if (tcpserv_socket==-1)
		{ return; }

	/* get a message from the client */

	if (recv(tcpserv_socket, buf, size, 0) <= 0) {
		tcpserv_fail("recv");
		return (char*)"\0";
	}

	return (char*)&tcpserv_buf;
}

char *tcpserv_read()
{
	return tcpserv_gets(tcpserv_buf, TCPSERV_BUFSIZE);
}


void tcpserv_send(char *text)
{
	if (tcpserv_socket==-1)
		{ return; }

	if (send(tcpserv_socket, text, strlen(text), 0) == -1) {
		tcpserv_fail("send");
	}
}

void tcpserv_puts(char *text)
{
	tcpserv_send(text);
	tcpserv_send((char*)"\n\r");
}

void tcpserv_shutdown()
{
	close(tcpserv_socket);
	tcpserv_socket = -1;
        
        /* give client a chance to properly shutdown */
        sleep(3);
}


