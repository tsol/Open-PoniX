#define INTERSECTS_BEFORE(x2, w2, x1, w1)  ((x2 < x1)   &&  (x2 + w2 > x1) && (x2 + w2 <= x1 + w1))
#define INTERSECTS_AFTER(x2, w2, x1, w1)   ((x2 >= x1)  &&  (x2 < x1 + w1) && (x2 + w2 > x1 + w1))
#define INTERSECTS_WITHIN(x2, w2, x1, w1)  ((x2 <= x1)  &&  (x2 + w2 >= x1 + w1))
#define INTERSECTS_OUTSIDE(x2, w2, x1, w1) ((x2 >= x1)  &&  (x2 + w2 <= x1 + w1))
#define IDENTICAL(x1,w1,x2,w2)  ((x1 == x2) && (x1 + w1 == x2 + w2))
#define INTERSECTS(x1, w1, x2, w2) (INTERSECTS_BEFORE(x1, w1, x2, w2) || IDENTICAL(x1, w1, x2, w2) || INTERSECTS_AFTER(x1, w1, x2, w2) || INTERSECTS_WITHIN(x1, w1, x2, w2) || INTERSECTS_OUTSIDE(x1, w1, x2, w2))

/**
@file     space.h
@brief    macros/inline functions for determining whether co-ordinates intersect.  Contains a datastructure for manipulating rectangles.
@author   Alysander Stanley
**/


struct Rectangle {
  int x,y,w,h;
};

struct Rectangle_list {
  unsigned int used, max;
  struct Rectangle *list;  
};

void add_rectangle                             (struct Rectangle_list *list, struct Rectangle new);
void remove_rectangle                          (struct Rectangle_list *list, struct Rectangle old);
struct Rectangle_list largest_available_spaces (struct Rectangle_list *used_spaces, int w, int h);
double calculate_displacement                  (struct Rectangle source, struct Rectangle dest, int *dx, int *dy);
struct Rectangle_list get_free_screen_spaces   (Display *display, Bool only_panels, struct Workspace *frames, struct Themes *themes);
