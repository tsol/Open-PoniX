
#define TCPSERV_PORT 		9998
#define TCPSERV_BUFSIZE		256

char	tcpserv_buf[TCPSERV_BUFSIZE];
int	tcpserv_socket;

int tcpserv_init_and_accept();
char *tcpserv_read();
char *tcpserv_gets(char buf[], int size);
void tcpserv_send(char *text);
void tcpserv_puts(char *text);
void tcpserv_shutdown();

/* that's a printf function */
#define		tcpserv_pf(format,...)	sprintf(tcpserv_buf, format, ## __VA_ARGS__); \
					tcpserv_send((char*)&tcpserv_buf)
