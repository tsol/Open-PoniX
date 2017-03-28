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

#include "gtkvtbar.h"
#include <stdio.h>

static void gtk_vtbar_class_init    (GtkVtbarClass   *klass);
static void gtk_vtbar_init          (GtkVtbar        *box);
static void gtk_vtbar_size_request  (GtkWidget      *widget,
				    GtkRequisition *requisition);
static void gtk_vtbar_size_allocate (GtkWidget      *widget,
				    GtkAllocation  *allocation);
#define MAX_CHILD_HEIGHT  40

GtkType
gtk_vtbar_get_type (void)
{
  static GtkType vtbar_type = 0;

  if (!vtbar_type)
    {
      static const GtkTypeInfo vtbar_info =
      {
	"GtkVtbar",
	sizeof (GtkVtbar),
	sizeof (GtkVtbarClass),
	(GtkClassInitFunc) gtk_vtbar_class_init,
	(GtkObjectInitFunc) gtk_vtbar_init,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      vtbar_type = gtk_type_unique (GTK_TYPE_BOX, &vtbar_info);
    }

  return vtbar_type;
}

static void
gtk_vtbar_class_init (GtkVtbarClass *class)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) class;

  widget_class->size_request = gtk_vtbar_size_request;
  widget_class->size_allocate = gtk_vtbar_size_allocate;
}

static void
gtk_vtbar_init (GtkVtbar *vtbar)
{
    vtbar->max_child_height = MAX_CHILD_HEIGHT;
}

GtkWidget*
gtk_vtbar_new (gint max_child_height, gint spacing)
{
  GtkVtbar *vtbar;

  vtbar = gtk_type_new (gtk_vtbar_get_type ());

  GTK_BOX (vtbar)->spacing = spacing;
  GTK_BOX (vtbar)->homogeneous = TRUE;
  GTK_VTBAR (vtbar)->max_child_height = max_child_height;

  return GTK_WIDGET (vtbar);
}


static void
gtk_vtbar_size_request (GtkWidget      *widget,
		       GtkRequisition *requisition)
{
    GtkBox *box;
    GtkBoxChild *child;
    GList *children;
    gint nvis_children;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_VTBAR (widget));
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
            child_requisition.height = MIN(child_requisition.height, MAX_CHILD_HEIGHT);
            requisition->width = MAX (requisition->width, child_requisition.width);
            requisition->height = MAX (requisition->height, child_requisition.height);

            nvis_children += 1;
	}
    }

    if (nvis_children > 0)
    {
        requisition->height *= nvis_children;
        requisition->height += (nvis_children - 1) * box->spacing;
    }

    requisition->height += GTK_CONTAINER (box)->border_width * 2;
    requisition->width += GTK_CONTAINER (box)->border_width * 2;
    /*
    fprintf(stderr, "gtk_vtbar_size_request: req=(%d, %d)\n",
    requisition->width, requisition->height);
    */
}

static void
gtk_vtbar_size_allocate (GtkWidget     *widget,
			GtkAllocation *allocation)
{
    GtkBox *box;
    GtkVtbar *tb;
    GtkBoxChild *child;
    GList *children;
    GtkAllocation child_allocation;
    gint nvis_children;
    gint height;
    gint extra;
    gint y;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_VTBAR (widget));
    g_return_if_fail (allocation != NULL);

    box = GTK_BOX (widget);
    tb  = GTK_VTBAR (widget);
    widget->allocation = *allocation;
    /*
    fprintf(stderr, "gtk_vtbar_size_allocate: alloc=(%d, %d)\n",
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
    height = (allocation->height
          - GTK_CONTAINER (box)->border_width * 2
          - (nvis_children - 1) * box->spacing);
    extra = MIN(height / nvis_children, tb->max_child_height);
    /*
    fprintf(stderr, "gtk_vtbar_size_allocate: task height = %d vis_taskno = %d\n",
          extra, nvis_children);
    */
    y = allocation->y + GTK_CONTAINER (box)->border_width;
    child_allocation.x = allocation->x + GTK_CONTAINER (box)->border_width;
    child_allocation.width = MAX (1,
          (gint) allocation->width - (gint) GTK_CONTAINER (box)->border_width * 2);

    children = box->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        if ((child->pack == GTK_PACK_START) && GTK_WIDGET_VISIBLE (child->widget))
        {
            child_allocation.height = extra;
            child_allocation.y = y;
            height -= extra;
            nvis_children -= 1;
            y += child_allocation.height + box->spacing;
          
            gtk_widget_size_allocate (child->widget, &child_allocation);           
        }
    }    
}
