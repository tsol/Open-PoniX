
#include "mylist.h"
#include "logit.h"

void* TMLSearch :: current()
{
    if (p==NULL) return NULL; else return p->item;
};

TMLItem* TMLSearch :: curnode()
{
    return p;
};

void* TMLSearch :: next()
{
    p=c;
    if (c==NULL) return NULL;
    c = c->next;
    return p->item;
}

char TMLSearch :: more()
{
    if (c==NULL) return 0; else return 1;
}

void TMLSearch :: init(TMLItem *first)
{
    c = first;
    p = NULL;
}


// TML

void* TML :: getById(unsigned long i)
{
    TMLSearch b; b.init(first);

    Listable *l;

    while (b.more())
          {
            l = (Listable*)b.next();
            if (l->id==i) { return (void*)l; };
          }

    return NULL;
};

void* TML :: getByNumber(unsigned long i)
{
    TMLSearch b; b.init(first);

    unsigned long j=0;
	void *p;

    while (b.more())
	{
		p = b.next();
		if (j==i)
			return p;
		j++;
	}

    return NULL;
};


void TML :: add(void *i)
{
    if (i==NULL)
       {
            LOG("! TML/add: NULL object");
            return;
       }

    if (first == NULL)
       {
            first = new TMLItem();
            first->prev = NULL;
            first->item = i;
            first->next = NULL;
            last = first;
            totalItems++;
            return;
       }

   totalItems++;

   TMLItem *was_last = last;
   last = new TMLItem();
   was_last->next = last;

   last->prev = was_last;
   last->next = NULL;
   last->item = i;

}
void TML :: placeNodeBeforeNode(TMLItem *src_node, TMLItem *dest_node)
{

	if (src_node==NULL || dest_node==NULL)
	{
            LOG("! TML/placeNodeBeforeNode: NULL nodes");
            return;	
	}

	if (src_node==dest_node)
		return;

	if (src_node==dest_node->prev)
		return;

// remove src_node from chain
	TMLItem *before = src_node->prev;
	TMLItem *after = src_node->next;

	if (before!=NULL)
		before->next = after;
	else
		first=after; // src_node was first element...

	if (after!=NULL)
		after->prev = before;
	else
		last=before; // it was last element..

// insert
	before = dest_node->prev;

	src_node->prev = before;
	src_node->next = dest_node;

	if (before!=NULL)
		before->next=src_node;
	else
		first=src_node; // now it became first

	dest_node->prev = src_node;

}


void TML :: remove(void *i)
{
    if ((first == NULL)||(i == NULL)) return;

    TMLSearch *s = new TMLSearch();
    TMLItem *tmp, *cur;

    s->init(first);

    while (s->more())
          if (s->next()==i)
             {
                    cur = s->curnode();

                    tmp = cur->prev; if (tmp!=NULL) tmp->next = cur->next;
                    if (last == cur) last = tmp;

                    tmp = cur->next; if (tmp!=NULL) tmp->prev = cur->prev;
                    if (first == cur) first = tmp;

                    delete cur;
                    delete s;

                    totalItems--;
                    return;
             }

    LOG("! TML/remove: not on the list");

}

void* TML :: pop()
{
	if (last==NULL)
		return NULL;

	void *p = last->item;

	totalItems--;

	if (last->prev!=NULL)
		last->prev->next=NULL;

	delete last;

	return p;
}

unsigned long TML :: reg(void *i)
{
    add(i);
    lastId++;

    if (lastId==MAX_ID_VALUE)
       {
            LOG("! TML/reg: oops :)");
            return 0;
       }

    Listable *l = (Listable*)last->item;

    if (l->id!=0)
       {
            LOG("! TML/reg: unListable class or allready registered");
            return 0;
       }

    l->id = lastId;

    return lastId;
};

unsigned long TML :: reg(unsigned long nid, void *i)
{
    add(i);

    Listable *l = (Listable*)last->item;

	if (l->id!=0)
	{
		LOG("! TML/reg(id): unListable class or allready registered");
		return 0;
	}

	l->id = nid;

    if (lastId<=l->id) lastId=l->id+1;
    return lastId;
};

TML :: TML()
{
    totalItems = 0;
	lastId = 0;
    first = NULL;
    last = NULL;
}

TML :: ~TML()
{
        TMLSearch s;
        s.init(first);
        while (s.more()) remove(s.next());
}
