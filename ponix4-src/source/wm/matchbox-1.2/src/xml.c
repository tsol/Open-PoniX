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

/*
 *  xml.c provides a very simple DOM for an xml file.  
 *  It can use expat or slightly limited ( no cdata, utf8 ) 
 *  internal parser.
 *
 *  This isn't used by a standalone matchbox. 
 */

#define _GNU_SOURCE

#include "xml.h"

#ifdef USE_EXPAT
static void 
node_start_cb(void *data, const char *tag, const char **expat_attr);
#else
static void node_start_cb( XMLParser *parser, const char *tag, Params *attr);
#endif

#ifdef USE_EXPAT
static void node_end_cb(void *data, const char *tag);
#else
static void node_end_cb( XMLParser *parser, const char *tag );

static char *parse(XMLParser *parser, char *doc);

static void node_cdata_cb( XMLParser *parser, char *cdata);

#endif

static char* load_file(const char* filename);

static void _params_free(Params *params);

static Params *_params_clone(Params *params);

static void _xml_parser_free(XMLNode *node);

static Nlist *list_add_node(XMLNode *dest, XMLNode *new);

#ifndef DEBUG
static jmp_buf Jbuf;
#endif

Params*
attr_to_params(const char **attr)
{
  Params *p = NULL, *q = NULL;
  if (attr != NULL && attr[0] != NULL)
    {
      int i = 0;
      p = q = xml_params_new();
      while(attr[i] && attr[i+1])
	{
	  q->key = strdup(attr[i]);
	  q->value = strdup(attr[i+1]);
	  if (attr[i+2])
	    {
	      q->next = xml_params_new();
	      q = q->next;
	    }
	  i += 2;
	}
      return p;
    } else {
      return NULL;
    }
}
     
Nlist *list_add_node(XMLNode *dest, XMLNode *new)
{
   Nlist *tmp;

   if (dest->kids == NULL)
     {
       dest->kids = (Nlist *)malloc(sizeof(Nlist));
       tmp = dest->kids;
     } 
   else 
     {
       for(tmp = dest->kids; tmp->next != NULL; tmp = tmp->next);
       tmp->next = (Nlist *)malloc(sizeof(Nlist));
       tmp = tmp->next;
     }

   tmp->next = NULL;
   tmp->data = new;
   return tmp;
}

XMLNode *xml_node_new(const char *name, Params *attr)
{
   XMLNode *n = (XMLNode *)malloc(sizeof(XMLNode));
   memset(n, 0, sizeof(XMLNode));

   n->tag    = strdup(name);
   n->attr   = _params_clone(attr);

   return n;
}

#ifdef USE_EXPAT
static void 
node_start_cb(void *data, const char *tag, const char **expat_attr)
{
  XMLParser *parser = (XMLParser *)data;
  Params *attr = attr_to_params(expat_attr);

#else
static void 
node_start_cb( XMLParser *parser, const char *tag, Params *attr)
{
#endif
  if (parser->root_node == NULL)
    {
      parser->root_node = xml_node_new(tag, attr);
      parser->_current_node = parser->root_node;
    } 
  else 
    {
      Nlist *tmp;
      tmp = list_add_node(parser->_current_node, xml_node_new(tag, attr));
      tmp->data->parent = parser->_current_node;
      parser->_current_node = tmp->data; 
    }

  _params_free(attr); /* xml_node_new rather confusingly make a copy */
}

#ifdef USE_EXPAT
static void 
node_end_cb(void *data, const char *tag)
{
  XMLParser *parser = (XMLParser *)data;
#else
static void node_end_cb( XMLParser *parser, const char *tag )
{
#endif
   parser->_current_node = parser->_current_node->parent; 
}


#ifndef USE_EXPAT
static void node_cdata_cb( XMLParser *parser, char *cdata)
{
   if (cdata == NULL) return;
   if (parser->_current_node->cdata == NULL)
     parser->_current_node->cdata = strdup(cdata);
   else 
     {
       parser->_current_node->cdata =
	 (char *)realloc(parser->_current_node->cdata,
			 sizeof(char)*(strlen(parser->_current_node->cdata))
			 + sizeof(char)*(strlen(cdata)+1));
       strcat(parser->_current_node->cdata, cdata);
     }
}
#endif

#ifdef DEBUG
void xml_dump(XMLNode *node, int depth)
{
   Nlist *tmp;
   Params *p;
   char crap[] = "-------";
   char more_crap[] = "       ";
   printf("+%s %s\n", &crap[5-depth], node->tag);
   
   for(p = node->attr; p != NULL; p = p->next)
   {
      printf(" %s %s = %s\n", &more_crap[5-depth], p->key, p->value);
   }

   if (node->cdata)
      printf(" %s%s\n", &more_crap[5-depth], node->cdata);
   
   for(tmp = node->kids; tmp != NULL; tmp = tmp->next)
   {
      xml_dump(tmp->data, depth+2);
   }
}
#endif

static Params * 
_params_clone(Params *params)
{
  Params *params_clone = NULL, *params_cur = NULL;;

  if (params == NULL) return NULL;

  params_cur = params_clone = xml_params_new();

  while(params != NULL)
     {
       if (params->key)   params_cur->key   = strdup(params->key);
       if (params->value) params_cur->value = strdup(params->value);
       if (params->next)  
	 {
	   params_cur->next = xml_params_new();
	   params_cur = params_cur->next;
	 }
       params = params->next;
     }

  return params_clone;
}

static void 
_params_free(Params *params)
{
  Params *params_cpy = NULL;
  while(params != NULL)
    {
      params_cpy = params;

      params = params_cpy->next;

      if (params_cpy->key) free(params_cpy->key);
      if (params_cpy->value) free(params_cpy->value);

      free (params_cpy);
    }
}

static void 
_xml_parser_free(XMLNode *node)
{
  Nlist *tmp, *old_tmp = NULL;

  _params_free(node->attr);
  
  if (node->cdata) free(node->cdata);
  if (node->tag)  free(node->tag);
  
  tmp = node->kids;
  
  while (tmp != NULL)
    {
      old_tmp = tmp->next;
      _xml_parser_free(tmp->data);
      
      free(tmp);
      tmp = old_tmp;
    }

  free(node);
}

Params *xml_params_new(void)
{
   Params *p = (Params *)malloc(sizeof(Params));
   p->next = NULL;
   return p;
}

#ifndef USE_EXPAT

#define CHECK_EOF(p) \
            if ((*p) == '\0') { printf("end of file?\n"); return NULL; }

#define SHD(c) ((*c) != '\0')


static char *parse(XMLParser *parser, char *doc)
{
   char *p = doc;
   
   /* Temporary data */
   char *oe_data = NULL;
   char *e_data = NULL;
   char *ce_data = NULL;
   char *key_data = NULL;
   char *val_data = NULL;
   Params *attr_params = NULL, *start_params = NULL;
   int skip_spaces = 0, in_value = 0;
   int in_comment = 0, done = 0;

   do {
     if (*p == '<') 
       {
	 if (*(p+1) == '!')
	   in_comment++;
	 else if (*(p+1) == '?')
	   while(SHD(p) && *p != '>')
	     p++;
	 else if (!in_comment)
	   done = 1;
       }
	 
     if (in_comment && *p == '-' 
	 && SHD(p+1) && *(p+1) == '-'
	 && SHD(p+2) && *(p+2) == '>' )
       {
	 p += 3;
	 in_comment--;
       }
     if (!done) p++;
   } while( SHD(p) && !done );

   CHECK_EOF(p);

   *p = '\0';

   oe_data = ++p;  /* Pointer to open tag name  */
   

   while (SHD(p) && *p != '>') /* advance to end of first tag  */
   {
      if (isspace(*p) && !skip_spaces)  /* possible attribute key */
      {
	 *p = '\0'; 
	 if (SHD(p+1) && !isspace(*(p+1))
	     && *(p+1) != '>' && *(p+1) != '/')
	 {
	    if (attr_params == NULL)
	    {
	       attr_params = xml_params_new();
	       start_params = attr_params;
	    } else {
	       attr_params->next = xml_params_new();
	       attr_params = attr_params->next;
	    }
	    key_data = p+1;
	    //attr_params->key = p+1;
	 }
	 p++;
      }
      else if (*p == '=' && !in_value)   /* beginning of attribute value  */
      {
	 *p = '\0'; 
	 attr_params->key = strdup(key_data);

	 p += 2; /* skipp first " */
	 val_data = p;
	 //attr_params->value = p;
	 skip_spaces = 1;      /* ignore spaces in value */
	 in_value = 1;
	 key_data = NULL;
      }
      else if (*p == '"')      /* end of attribute value */
      {
	 *p = '\0'; 
	 attr_params->value = strdup(val_data);
	 val_data = NULL;
	 skip_spaces = 0;
	 in_value=0;
	 p++;
      } else {
	 p++;
      }
   }

   *p = '\0';    /* end possible dangling value pointer */

   /* fire all callback */
   parser->start_element_cb(parser, oe_data, start_params);
   
   if (*(p-1) == '/') {    /* catch <tag/>  */
      *(p-1) = '\0';
      parser->data_cb(parser, NULL);
      parser->end_element_cb(parser, oe_data);
      return p +1;          /* TODO: catch possible segfualt */
   }

   do {
      if (*p != '<') p++;  /* TODO: fixme ... maybe  */

      e_data = p;    	   /* grab cdata  */
      while (SHD(p) && *p != '<') p++;

      *p = '\0';
      parser->data_cb(parser, e_data); /* ? */

      if (*(p+1) == '!') 
	{  
	 *p = '\0'; p++;
	 while (!(SHD(p) && *p == '-' 
		  && SHD(p+1) && *(p+1) == '-'
		  && SHD(p+2) && *(p+2) == '>'))
	   p++;
      } else {
      
	//if (!*p) return NULL;  /* catch possible segfualt  */
	 
	 if (*(p+1) == '/')   /* close tag  */
	 {
	   //*p = '\0';
	   // parser->data_cb(parser, e_data); /* cdata callback  */
	    p+=2;                 /* skip /> */
	    ce_data = p;
	    while (SHD(p) && *p != '>') p++; 
	    *p = '\0';
	    parser->end_element_cb(parser, ce_data);
	    return p+1;          /* TODO catch possible segfualt  */
	 } else {
	   //*p = '\0';
	   // parser->data_cb(parser, e_data);
	    *p = '<';
	    p = parse(parser, p);  /* recurse into inner tags */
	 }
      }

   } while (SHD(p));

   return p;
}

#endif

static char* 
load_file(const char* filename) 
{
  struct stat st;
  FILE*       fp;
  char*       str;
  int         len;

  if (stat(filename, &st)) return NULL;

  if (!(fp = fopen(filename, "rb"))) return NULL;

  str = (char *)malloc(sizeof(char)*(st.st_size + 1));
  len = fread(str, 1, st.st_size, fp);
  if (len >= 0) str[len] = '\0';

  fclose(fp);

  return str;
}

#ifndef DEBUG
static void 
catch_sigsegv(int sig)
{
   signal(SIGSEGV, SIG_DFL);
   longjmp(Jbuf, 1);
}
#endif

XMLParser 
*xml_parser_new(void)
{
   XMLParser *parser;
   parser = (XMLParser *)malloc(sizeof(XMLParser));
   parser->start_element_cb = NULL;
   parser->end_element_cb   = NULL;
   parser->data_cb          = NULL;
   parser->root_node        = NULL;
   parser->_current_node    = NULL;

   return parser;
}

void 
xml_parser_free(XMLParser *parser, XMLNode *root)
{
   if (root) _xml_parser_free(root);
   free(parser);
}

XMLNode*
xml_parse_data_dom(XMLParser *parser, char *data)
{
#ifdef USE_EXPAT

  XML_Parser p = XML_ParserCreate(NULL);

  if (! p) {
    fprintf(stderr, "Matchbox: Couldn't allocate memory for XML parser\n");
    exit(-1);
  }

  XML_SetElementHandler(p, node_start_cb, node_end_cb);
  /* XML_SetCharacterDataHandler(p, chars); */

  XML_SetUserData(p, (void *)parser);

  if (! XML_Parse(p, data, strlen(data), 1)) {
    fprintf(stderr, "Matchbox: XML Parse error at line %d:\n%s\n",
	    XML_GetCurrentLineNumber(p),
	    XML_ErrorString(XML_GetErrorCode(p)));
    return NULL;
  }
  
  return parser->root_node;

#else
  int success = 1;
  parser->start_element_cb = &node_start_cb;
  parser->end_element_cb   = &node_end_cb;
  parser->data_cb          = &node_cdata_cb;
   
#ifndef DEBUG
  signal(SIGSEGV, catch_sigsegv);
  if (setjmp(Jbuf)) success = 0;
#endif

   if (parse(parser, data) == NULL) 
     success = 0;

#ifndef DEBUG
   signal(SIGSEGV, NULL);
#endif   

   if (success) return parser->root_node;
   return NULL;
#endif
}

XMLNode*
xml_parse_file_dom(XMLParser *parser, char *filename)
{
  XMLNode *root;
  char *data = load_file(filename);
  if (data != NULL)
    {
      if ((root = xml_parse_data_dom(parser, data)) != NULL)
	{
	  free(data); 
	  return root;
	}
    }
   return NULL;
}

