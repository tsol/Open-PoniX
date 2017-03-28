#include "tcpserv.h"

main()
{
	tcpserv_init_and_accept();
	char *t = tcpserv_read();

	tcpserv_send("thanx for that!");
	tcpserv_send(t);

	tcpserv_pf("Love you %d times!",10);
	
	tcpserv_shutdown();	
}

