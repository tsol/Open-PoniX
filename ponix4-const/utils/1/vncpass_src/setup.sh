gcc -c d3des.c vncauth.c
ar cq librfb.a d3des.o vncauth.o
gcc vncpasswd.c -o ../vncpasswd librfb.a
