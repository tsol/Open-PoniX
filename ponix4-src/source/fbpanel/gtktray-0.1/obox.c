/* OBox Copyright (C) 2002 Red Hat Inc. based on GtkHBox */
/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include "obox.h"

#include <gtk/gtkhbox.h>
#include <gtk/gtkvbox.h>

static void gtk_obox_class_init    (GtkOBoxClass   *klass);
static void gtk_obox_init          (GtkOBox        *box);
static void gtk_obox_size_request  (GtkWidget      *widget,
				    GtkRequisition *requisition);
static void gtk_obox_size_allocate (GtkWidget      *widget,
				    GtkAllocation  *allocation);


GtkType
gtk_obox_get_type (void)
{
  static GtkType obox_type = 0;

  if (!obox_type)
    {
      static const GtkTypeInfo obox_info =
      {
	"GtkOBox",
	sizeof (GtkOBox),
	sizeof (GtkOBoxClass),
	(GtkClassInitFunc) gtk_obox_class_init,
	(GtkObjectInitFunc) gtk_obox_init,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      obox_type = gtk_type_unique (GTK_TYPE_BOX, &obox_info);
    }

  return obox_type;
}

static void
gtk_obox_class_init (GtkOBoxClass *class)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) class;

  widget_class->size_request = gtk_obox_size_request;
  widget_class->size_allocate = gtk_obox_size_allocate;
}

static void
gtk_obox_init (GtkOBox *obox)
{
  obox->orientation = GTK_ORIENTATION_HORIZONTAL;
}

GtkWidget*
gtk_obox_new (void)
{
  GtkOBox *obox;

  obox = gtk_type_new (gtk_obox_get_type ());

  return GTK_WIDGET (obox);
}

static GtkWidgetClass*
get_class (GtkOBox *obox)     
{
  GtkWidgetClass *klass;

  switch (obox->orientation)
    {
    case GTK_ORIENTATION_HORIZONTAL:
      klass = GTK_WIDGET_CLASS (gtk_type_class (GTK_TYPE_HBOX));
      break;
    case GTK_ORIENTATION_VERTICAL:
      klass = GTK_WIDGET_CLASS (gtk_type_class (GTK_TYPE_VBOX));
      break;
    default:
      g_assert_not_reached ();
      klass = NULL;
      break;
    }

  return klass;
}

static void
gtk_obox_size_request (GtkWidget      *widget,
		       GtkRequisition *requisition)
{
  GtkWidgetClass *klass;
  GtkOBox *obox;

  obox = GTK_OBOX (widget);

  klass = get_class (obox);

  klass->size_request (widget, requisition);
}

static void
gtk_obox_size_allocate (GtkWidget     *widget,
			GtkAllocation *allocation)
{
  GtkWidgetClass *klass;
  GtkOBox *obox;

  obox = GTK_OBOX (widget);

  klass = get_class (obox);

  klass->size_allocate (widget, allocation);
}

void
gtk_obox_set_orientation (GtkOBox       *obox,
                          GtkOrientation orientation)
{
  g_return_if_fail (GTK_IS_OBOX (obox));

  if (obox->orientation == orientation)
    return;
  
  obox->orientation = orientation;

  gtk_widget_queue_resize (GTK_WIDGET (obox));
}
