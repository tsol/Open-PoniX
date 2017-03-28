#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Main routine */
#define PADCHAR		'_'
#define PADLEN		14
#define BUF		256

int my_strlen_utf8_c(char *s) {
   int i = 0, j = 0;
   while (s[i]) {
   	if ((s[i] & 0xc0) != 0x80) j++;
   	i++;
   }
   return j;
}

int main(int argc, char *argv[])
{
  int len = 0;
 
  if (argc < 2) {
    printf("Usage: upadd [string:value] <padlen:default=14> <padchar:default='_'>\n");
    exit(0); 
  }

/*
  int padlen = PADLEN;
  
  if (padlen <= 0) { padlen = PAD; } 
*/
  int padlen = PADLEN;

  if ( argc >= 3 ) {
     padlen = atoi(argv[2]);  
  }

  char padchar = PADCHAR;
  
  if (argc >= 4) {
     padchar = argv[3][0];  
  }


  char tmp[BUF];
  strcpy(tmp,argv[1]);
  
  int i = 0;
  
  while (tmp[i]) {

    if ((tmp[i] & 0xc0) != 0x80) {       
       if (tmp[i] == ':')
       {
             if (len < padlen) {
                 if ( len < padlen-1) { putchar(' '); len++; }
                 for (; len < padlen-1; len++) { putchar(padchar); }
                 if ( len < padlen) { putchar(' '); }
                 putchar(':');
                 goto next;
             }
          
       }

       len++;
    }    

    putchar(tmp[i]);
    next:
    i++;
  }
                          
  exit(0);

}
