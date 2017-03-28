#!/usr/bin/perl

while(<>){

	s/puts\s*?\("/tcpserv_puts\("/g;
	s/printf\("/tcpserv_pf\("/g;
	print;
}
