/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2007, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  * 
 * it under the terms of the GNU General Public License as published by  * 
 * the Free Software Foundation; either version 2 of the License, or     * 
 * (at your option) any later version.                                   * 
 *                                                                       * 
 * This program is distributed in the hope that it will be useful,       * 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 * GNU General Public License for more details.                          * 
 *                                                                       * 
 * You should have received a copy of the GNU General Public License     * 
 * along with this program; if not, write to the                         * 
 * Free Software Foundation, Inc.,                                       * 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 *                                                                       * 
 *************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc ,char *argv[])
{
    FILE *infile, *outfile;	
    char infname[1024];
    char outfname[1024];
    char *rt28xxdir;
    char *chipset;
    int i=0;//,n=0;
    unsigned char c;
   
    memset(infname,0,1024);
    memset(outfname,0,1024);
    
    rt28xxdir = (char *)getenv("RT28xx_DIR");
    chipset = (char *)getenv("CHIPSET");
    if(!rt28xxdir)
    {
         printf("Environment value \"RT28xx_DIR\" not export \n");
	 return -1;
    }
    if(!chipset)
    {
	 printf("Environment value \"CHIPSET\" not export \n");
	 return -1;
    }	    
	if (strlen(rt28xxdir) > (sizeof(infname)-100))
	{
		printf("Environment value \"RT28xx_DIR\" is too long!\n");
		return -1;
	}

    strcat(infname,rt28xxdir);
    if(strncmp(chipset, "2860",4)==0)
	    strcat(infname,"/common/rt2860.bin");
    else if(strncmp(chipset, "2870",4)==0)
	    strcat(infname,"/common/rt2870.bin");
	else if(strncmp(chipset, "3090",4)==0)
	    strcat(infname,"/common/rt2860.bin");
	else if(strncmp(chipset, "2070",4)==0)
	    strcat(infname,"/common/rt2870.bin");
	else if(strncmp(chipset, "3070",4)==0)
	    strcat(infname,"/common/rt2870.bin");
	else if(strncmp(chipset, "3572",4)==0)
	    strcat(infname,"/common/rt2870.bin");
	else if(strncmp(chipset, "3370",4)==0)
            strcat(infname,"/common/rt2870.bin");
	else if(strncmp(chipset, "USB",3)==0)
	    strcat(infname,"/common/rt2870.bin");
	else if(strncmp(chipset, "PCI",3)==0)
	    strcat(infname,"/common/rt2860.bin");
    else
    	strcat(infname,"/common/rt2860.bin");
	    
    strcat(outfname,rt28xxdir);
    strcat(outfname,"/include/firmware.h");
     
    infile = fopen(infname,"r");
    if (infile == (FILE *) NULL)
    {
         printf("Can't read file %s \n",infname);
	 return -1;
    }
    outfile = fopen(outfname,"w");
    
    if (outfile == (FILE *) NULL)
    {
         printf("Can't open write file %s \n",outfname);
        return -1;
    }
    
    fputs("/*\n",outfile); 
    fputs(" Copyright (c) 2007, Ralink Technology Corporation \n",outfile); 
    fputs(" All rights reserved. \n\n",outfile);

    fputs(" Redistribution.  Redistribution and use in binary form, without \n",outfile); 
    fputs(" modification, are permitted provided that the following conditions are \n",outfile); 
    fputs(" met: \n\n",outfile);

    fputs(" \t* Redistributions must reproduce the above copyright notice and the \n",outfile);
    fputs(" \tfollowing disclaimer in the documentation and/or other materials \n",outfile);
    fputs(" \tprovided with the distribution. \n",outfile);
    fputs(" \t* Neither the name of Ralink Technology Corporation nor the names of its \n",outfile);
    fputs(" \tsuppliers may be used to endorse or promote products derived from this \n",outfile);
    fputs(" \tsoftware without specific prior written permission. \n",outfile);
    fputs(" \t* No reverse engineering, decompilation, or disassembly of this software \n",outfile);
    fputs(" \tis permitted. \n\n",outfile);

    fputs(" Limited patent license. Ralink Technology Corporation grants a world-wide, \n",outfile);
    fputs(" royalty-free, non-exclusive license under patents it now or hereafter \n",outfile);
    fputs(" owns or controls to make, have made, use, import, offer to sell and \n",outfile);
    fputs(" sell (\"Utilize\") this software, but solely to the extent that any \n",outfile);
    fputs(" such patent is necessary to Utilize the software alone, or in \n",outfile);
    fputs(" combination with an operating system licensed under an approved Open \n",outfile);
    fputs(" Source license as listed by the Open Source Initiative at \n",outfile);
    fputs(" http://opensource.org/licenses.  The patent license shall not apply to \n",outfile);
    fputs(" any other combinations which include this software.  No hardware per \n",outfile);
    fputs(" se is licensed hereunder. \n\n",outfile);

    fputs(" DISCLAIMER.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND \n",outfile);
    fputs(" CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, \n",outfile);
    fputs(" BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND \n",outfile);
    fputs(" FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE \n",outfile);
    fputs(" COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, \n",outfile);
    fputs(" INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, \n",outfile);
    fputs(" BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS \n",outfile);
    fputs(" OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND \n",outfile);
    fputs(" ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR \n",outfile);
    fputs(" TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE \n",outfile);
    fputs(" USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH \n",outfile);
    fputs(" DAMAGE. \n",outfile);
    fputs("*/ \n",outfile);

    fputs("/* AUTO GEN PLEASE DO NOT MODIFY IT */ \n",outfile);
    fputs("/* AUTO GEN PLEASE DO NOT MODIFY IT */ \n",outfile);
    fputs("\n",outfile);
    fputs("\n",outfile);
    fputs("UCHAR FirmwareImage [] = { \n",outfile);
    while(1)
    {
	char cc[3];    

	c = getc(infile);
	
	if (feof(infile))
	    break;
	
	memset(cc,0,2);
	
	if (i>=16)
	{	
	    fputs("\n", outfile);	
	    i = 0;
	}    
	fputs("0x", outfile); 
	sprintf(cc,"%02x",c);
	fputs(cc, outfile);
	fputs(", ", outfile);
	i++;
    } 
    
    fputs("} ;\n", outfile);
    fclose(infile);
    fclose(outfile);
    exit(0);
}	
