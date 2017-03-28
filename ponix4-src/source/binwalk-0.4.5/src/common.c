/* 
 * Copyright (c) 2010 Craig Heffner
 *
 * This software is provided under the MIT license. For the full text of this license, please see
 * the COPYING file included with this code, or visit http://www.opensource.org/licenses/MIT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "common.h"

#ifdef __linux
#include <linux/fs.h>
#endif

/* Convert decimal and hexadecimal strings to integers */
int str2int(char *str)
{
        int val = 0;
        int strsize = 0;
        int base = 10;
        char *strptr = NULL;
        char buffer[INT_MAX_STR_SIZE] = { 0 };

	if(str != NULL)
	{
        	strsize = strlen(str);

        	if(strsize < INT_MAX_STR_SIZE)
        	{

        		memcpy((void *) &buffer, str, strsize);
        		strptr = (char *) &buffer;
	
        		/* If the string starts with '0x' or '\x', or if it ends in 'h' or 'H', treat it as hex */
        		if(strsize >= 2)
        		{
        		        if(strptr[1] == 'x')
        		        {
        		                strptr += 2;
        		                base = 16;
        		        }
        		        else if(strptr[strsize-1] == 'h' || strptr[strsize-1] == 'H')
        		        {
        		                strptr[strsize-1] = '\0';
        		                base = 16;
        		        }
        		}
	
	        	val = strtol(strptr,NULL,base);
		}
	}

        return val;
}

/* Reads in and returns the contents and size of a given file */
const void *file_read(char *file, size_t *fsize)
{
        int fd = 0;
	size_t file_size = 0;
        struct stat _fstat = { 0 };
        const void *buffer = NULL;

	fd = open(file, O_RDONLY);
        if(!fd)
        {
                perror(file);
                goto end;
        }

        if(stat(file, &_fstat) == -1)
        {
		perror(file);
                goto end;
        }

        if(_fstat.st_size > 0)
        {
		file_size = _fstat.st_size;
        }
#ifdef __linux
	else
	{
		long long long_file_size = 0;

		/* Special files may report a zero size in stat(); must get their file size via an ioctl call */
		if(ioctl(fd, BLKGETSIZE64, &long_file_size) == -1)
		{
			perror("ioctl");
			goto end;
		}
		else
		{
			file_size = (size_t) long_file_size;
		}
	}
#endif

	if(file_size > 0)
	{
		buffer = mmap(NULL, file_size, PROT_READ, (MAP_SHARED | MAP_NORESERVE), fd, 0);
		if(buffer == MAP_FAILED)
		{
			perror("mmap");
			buffer = NULL;
		}
		else
		{
			*fsize = file_size;
		}
	}

end:
        if(fd) close(fd);
        return buffer;
}

/* Create a temporary magic file to search for a specific string sequence */
char *create_magic_file(char *search_string)
{
	FILE *fp = NULL;
	char *file_path = NULL, *file_contents = NULL;
	int file_contents_size = 0;

	file_contents_size = (strlen(search_string) * 2) + strlen(STRING_ENTRY_FORMAT);

	file_contents = malloc(file_contents_size + 1);
	if(file_contents)
	{
		memset(file_contents, 0, (file_contents_size + 1));
		sprintf(file_contents, STRING_ENTRY_FORMAT, search_string, search_string);

		fp = fopen(TMP_MAGIC_FILE, "wb");
		if(fp)
		{
			fwrite(file_contents, 1, strlen(file_contents), fp);
			fclose(fp);	
			file_path = strdup(TMP_MAGIC_FILE);
		}
		else
		{
			perror(TMP_MAGIC_FILE);
		}

		free(file_contents);
	}
	else
	{
		perror("malloc");
	}

	return file_path;
}

/* Remove any temporary magic files created during execution */
void cleanup_magic_file(void)
{
	unlink(TMP_MAGIC_FILE);
}

/* Print messages to both the log file and stdout, as appropriate */
void print(const char* format, ...)
{
        va_list file_args;
	va_list stdout_args;

        va_start(file_args, format);
	va_start(stdout_args, format);

        if(globals.fsout != NULL)
        {
                vfprintf(globals.fsout, format, file_args);
                fflush(globals.fsout);
        }
        if(globals.quiet == 0)
        {
                vfprintf(stdout, format, stdout_args);
                fflush(stdout);
        }

        va_end(file_args);
	va_end(stdout_args);
        return;
}

/* Returns the current timestamp as a string */
char *timestamp()
{
        time_t t = { 0 };
        struct tm *tmp = NULL;
        char *ts = NULL;

        t = time(NULL);
        tmp = localtime(&t);
        if(!tmp)
        {
                perror("Localtime failure");
                goto end;
        }

        ts = malloc(MAX_TIMESTAMP_SIZE);
        if(!ts)
        {
                perror("Malloc failure");
                goto end;
        }
        memset(ts,0,MAX_TIMESTAMP_SIZE);

        if(strftime(ts,MAX_TIMESTAMP_SIZE-1,TIMESTAMP_FORMAT,tmp) == 0)
        {
                if(ts) free(ts);
                ts = NULL;
        }

end:
        return ts;
}

/* Performs a case-insensitive string search */
int string_contains(char *haystack, char *needle)
{
	char *my_haystack = NULL, *my_needle = NULL;
	int retval = 0;

	/* Duplicate the strings, as we will be converting them to all uppercase */
	my_haystack = strdup(haystack);
	my_needle = strdup(needle);

	if(!my_haystack || !my_needle)
	{
		perror("strdup");
	}
	else
	{
		/* Convert string to all upper case */
		uppercase(my_haystack);
		uppercase(my_needle);

		/* Search for needle in haystack */
		if(strstr(my_haystack, my_needle) != NULL)
		{
			retval = 1;
		}
	}

	if(my_haystack) free(my_haystack);
	if(my_needle) free(my_needle);
	return retval;
}

/* Convert a given string to all upper case */
void uppercase(char *string)
{
	int i = 0;

	for(i=0; i<strlen(string); i++)
	{
		string[i] = toupper(string[i]);
	}

	return;
}

