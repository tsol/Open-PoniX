/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include "gtkhtbar.h"
#include <stdio.h>

static void gtk_htbar_class_init    (GtkHtbarClass   *klass);
static void gtk_htbar_init          (GtkHtbar        *box);
static void gtk_htbar_size_request  (GtkWidget      *widget,
				    GtkRequisition *requisition);
static void gtk_htbar_size_allocate (GtkWidget      *widget,
				    GtkAllocation  *allocation);
#define MAX_CHILD_WIDTH  250

GtkType
gtk_htbar_get_type (void)
{
  static GtkType htbar_type = 0;

  if (!htbar_type)
    {
      static const GtkTypeInfo htbar_info =
      {
	"GtkHtbar",
	sizeof (GtkHtbar),
	sizeof (GtkHtbarClass),
	(GtkClassInitFunc) gtk_htbar_class_init,
	(GtkObjectInitFunc) gtk_htbar_init,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      htbar_type = gtk_type_unique (GTK_TYPE_BOX, &htbar_info);
    }

  return htbar_type;
}

static void
gtk_htbar_class_init (GtkHtbarClass *class)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) class;

  widget_class->size_request = gtk_htbar_size_request;
  widget_class->size_allocate = gtk_htbar_size_allocate;
}

static void
gtk_htbar_init (GtkHtbar *htbar)
{
    htbar->max_child_width = MAX_CHILD_WIDTH;
}

GtkWidget*
gtk_htbar_new (gint max_child_width, gint spacing)
{
  GtkHtbar *htbar;

  htbar = gtk_type_new (gtk_htbar_get_type ());

  GTK_BOX (htbar)->spacing = spacing;
  GTK_BOX (htbar)->homogeneous = TRUE;
  GTK_HTBAR (htbar)->max_child_width = max_child_width;

  return GTK_WIDGET (htbar);
}


static void
gtk_htbar_size_request (GtkWidget      *widget,
		       GtkRequisition *requisition)
{
    GtkBox *box;
    GtkBoxChild *child;
    GList *children;
    gint nvis_children;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_HTBAR (widget));
    g_return_if_fail (requisition != NULL);

    box = GTK_BOX (widget);
    requisition->width = 0;
    requisition->height = 0;
    nvis_children = 0;

    children = box->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        if (GTK_WIDGET_VISIBLE (child->widget))
	{
            GtkRequisition child_requisition;

            gtk_widget_size_request (child->widget, &child_requisition);
            child_requisition.width = MIN(child_requisition.width, MAX_CHILD_WIDTH);
            requisition->width = MAX (requisition->width, child_requisition.width);
            requisition->height = MAX (requisition->height, child_requisition.height);

            nvis_children += 1;
	}
    }

    if (nvis_children > 0)
    {
        requisition->width *= nvis_children;
        requisition->width += (nvis_children - 1) * box->spacing;
    }

    requisition->width += GTK_CONTAINER (box)->border_width * 2;
    requisition->height += GTK_CONTAINER (box)->border_width * 2;
    /*
    fprintf(stderr, "gtk_htbar_size_request: req=(%d, %d)\n",
    requisition->width, requisition->height);
    */
}

static void
gtk_htbar_size_allocate (GtkWidget     *widget,
			GtkAllocation *allocation)
{
    GtkBox *box;
    GtkHtbar *tb;
    GtkBoxChild *child;
    GList *children;
    GtkAllocation child_allocation;
    gint nvis_children;
    gint width;
    gint extra;
    gint x;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_HTBAR (widget));
    g_return_if_fail (allocation != NULL);

    box = GTK_BOX (widget);
    tb  = GTK_HTBAR (widget);
    widget->allocation = *allocation;

    /*
    fprintf(stderr, "gtk_htbar_size_allocate: alloc=(%d, %d)\n",
    allocation->width, allocation->height);
    */
    nvis_children = 0;
    children = box->children;

    while (children)
    {
        child = children->data;
        children = children->next;
        if (GTK_WIDGET_VISIBLE (child->widget))
            nvis_children += 1;
    }

    if (!nvis_children)
        return;
    
    //widget->requisition.width;
    //width = MIN(widget->requisition.width, allocation->width);
    width = (allocation->width
          - GTK_CONTAINER (box)->border_width * 2
          - (nvis_children - 1) * box->spacing);
    extra = MIN(width / nvis_children, tb->max_child_width);

    /*
    fprintf(stderr, "gtk_htbar_size_allocate: task width = %d vis_taskno = %d\n",
          extra, nvis_children);
    */
    x = allocation->x + GTK_CONTAINER (box)->border_width;
    child_allocation.y = allocation->y + GTK_CONTAINER (box)->border_width;
    child_allocation.height = MAX (1,
          (gint) allocation->height - (gint) GTK_CONTAINER (box)->border_width * 2);

    children = box->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        if ((child->pack == GTK_PACK_START) && GTK_WIDGET_VISIBLE (child->widget))
        {
            child_allocation.width = extra;
            child_allocation.x = x;
            width -= extra;
            nvis_children -= 1;
            x += child_allocation.width + box->spacing;
          
            gtk_widget_size_allocate (child->widget, &child_allocation);           
        }
    }    
}
