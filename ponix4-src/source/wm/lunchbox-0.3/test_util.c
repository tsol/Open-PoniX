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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

int main(int argc, char **argv) {
  int *integers = NULL;
  integers = calloc(sizeof(int), 8);
  integers = clean_realloc(integers, sizeof(int) * 8, sizeof(int) * 16);
  for(int i = 0; i < 16; i++) printf("%d\n", integers[i]);
  return 1;
}
