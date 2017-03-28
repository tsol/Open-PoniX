/**************************************************************************
    Lunchbox Window Manager
    Copyright (C) 2008 Alysander Stanley

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
/**
@brief  Same as realloc but initializes all memory to 0.
@param  old_size.  Length in bytes of the original buffer.
@param  new_size.  Length in bytes of the new buffer.
@return void *
**/
void *
clean_realloc(void *ptr, size_t old_size, size_t new_size) {
  char *temp = realloc(ptr, new_size);
  if(temp == ptr) return ptr;
  for(size_t i = old_size; i < new_size; i++) {
    temp[i] = 0;
  }
  return temp;
}

/**
@brief    strnadd concatinates s1 and s2 and writes the result into s0 provided the length of s1 and s2 
          is less that the limit, which is usually defined as the length of s0.  
          
@return   If any of the passed strings are NULL
          s0 is returned unmodified.  If the limit is less than the length of s2, s0 is returned unmodified. 
@note     All strings must be NULL terminated and this function ensures that s0 will always be null terminated 
   
**/
char *
strnadd(char *restrict s0, char *restrict s1, char *restrict s2, size_t limit) {
  size_t length;
  if(!s0  ||  !s1  ||  !s2)  return s0;
  length = strlen(s1) + strlen(s2) + 1;
  if(length > limit) return s0;
  if(s0 != s1) strcpy(s0, s1);
  strcat(s0, s2);
  s0[length-1] = '\0';
  //printf("s0: %s\n", s0);
  return s0;
}

/**
@brief Establish the array index of an item given a pointer to an item inside an array
@retun the calcualted array index.  Should never be negative.
**/
int get_offset_in_array(void *pointer_in_array, void *array, size_t item_size) {
  return ((char *)pointer_in_array - (char *)array)/(item_size);
}
