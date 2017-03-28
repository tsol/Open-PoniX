#ifndef __MYLIST
#define __MYLIST

#include <stdio.h>

#define MAX_ID_VALUE 4294967295

class Listable
{
 public:
  Listable() { id=0; };
  unsigned long id;
};

struct TMLItem
{
    TMLItem *prev, *next;
    void *item;
};

class TMLSearch
{
    TMLItem *c, *p;

 public:

    char  more();
    void* next();
    void* current();

    void  init(TMLItem *first);

    TMLItem* curnode();
};

class TML
{
   TMLItem *first, *last;

   unsigned long totalItems;
   unsigned long lastId;   // simple id providing

 public:

    TML();
    ~TML();

	unsigned long	getTotalItems() { return totalItems; };

	void			add(void *);	// add to the list
    unsigned long	reg(void *);	// add and assign an id
    unsigned long	reg(unsigned long, void *);  // try to set forced id

    void		remove(void *);
	void		placeNodeBeforeNode(TMLItem *src_node, TMLItem *dest_node);

    void*		getById(unsigned long i);
    void*		getByNumber(unsigned long i);

	void*		pop();	// pop last added item

    TMLItem*	getlastnode() { return last; };
    TMLItem*	getfirstnode() { return first; };
    TMLItem*	list() { return first; };

};


#endif
