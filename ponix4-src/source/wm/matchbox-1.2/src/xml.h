/* 
 *  Matchbox Window Manager - A lightweight window manager not for the
 *                            desktop.
 *
 *  Authored By Matthew Allum <mallum@o-hand.com>
 *
 *  Copyright (c) 2002, 2004 OpenedHand Ltd - http://o-hand.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/wait.h>

#include "config.h"

#ifdef USE_EXPAT
#include <expat.h>
#endif

typedef struct _params {
   char           *key;
   char           *value;
   struct _params *next;
} Params;

typedef struct _nlist {
   struct _xml_node *data;
   struct _nlist    *next;
} Nlist;

typedef struct _xml_node {

   char             *tag;
   Params           *attr;
   char             *cdata;
   struct _xml_node *parent; 
   Nlist            *kids;

} XMLNode;

typedef struct _xmlparser {
   void (* start_element_cb)( struct _xmlparser  *parser,
			      const char *tag, Params *attr);
   void (* end_element_cb)( struct  _xmlparser *parser, const char *tag );
   void (* data_cb)( struct  _xmlparser *parser, char *data );
   XMLNode *root_node;
   XMLNode *_current_node;
} XMLParser;


XMLNode *xml_node_new(const char *name, Params *attr);

#ifdef DEBUG
void xml_dump(XMLNode *node, int depth);
#endif

/* --------------------------------------------------------------- */

Params *xml_params_new(void);

XMLParser *xml_parser_new(void);

XMLNode *xml_parse_data_dom(XMLParser *parser, char *data);

XMLNode *xml_parse_file_dom(XMLParser *parser, char *filename);

void xml_parser_free(XMLParser *parser, XMLNode *root);
