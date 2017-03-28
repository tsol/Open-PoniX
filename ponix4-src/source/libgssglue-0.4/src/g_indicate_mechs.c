/* #ident  "@(#)gss_indicate_mechs.c 1.13     95/08/04 SMI" */

/*
 * Copyright 1996 by Sun Microsystems, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Sun Microsystems not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. Sun Microsystems makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 *  glue routine for gss_indicate_mechs
 */

#include "mglueP.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <errno.h>

extern gss_mechanism *__gss_mechs_array;

static gss_OID_set_desc	supported_mechs_desc;
static gss_OID_set supported_mechs = NULL;

OM_uint32 KRB5_CALLCONV
gss_indicate_mechs (minor_status,
                    mech_set)

OM_uint32 *		minor_status;
gss_OID_set *		mech_set;

{
    int i;
    gss_OID_set tmp_set;
    OM_uint32 lmaj;	/* local major status */

    GSS_INITIALIZE;

    if (minor_status)
	*minor_status = 0;

    if (mech_set == NULL)
	return GSS_S_COMPLETE;
    *mech_set = GSS_C_NO_OID_SET;

    if (__gss_mechs_array == NULL) {
	*mech_set = GSS_C_NO_OID_SET;
	return GSS_S_COMPLETE;
    }

    /*
     * Compute list of supported mechanisms if we haven't already done so
     */
    if (supported_mechs == NULL) {

	supported_mechs = &supported_mechs_desc;
	supported_mechs->count = 0;

	/* Build the mech_set from the OIDs in mechs_array. */

	for (i=0; __gss_mechs_array[i]->mech_type.length != 0; i++)
	    supported_mechs->count++;

	supported_mechs->elements =
	    (void *) malloc(supported_mechs->count *
			    sizeof(gss_OID_desc));
	if (supported_mechs->elements == NULL) {
	    goto list_mem_error;
	}

	for (i=0; i < supported_mechs->count; i++) {
	    supported_mechs->elements[i].length =
		__gss_mechs_array[i]->mech_type.length;
	    supported_mechs->elements[i].elements = (void *)
		malloc(__gss_mechs_array[i]->mech_type.length);
	    if (supported_mechs->elements[i].elements == NULL)
		goto list_mem_error;

	    memcpy(supported_mechs->elements[i].elements,
		   __gss_mechs_array[i]->mech_type.elements,
		   __gss_mechs_array[i]->mech_type.length);
	}
    }

    /*
     * List is initialized, return a copy to the caller
     */

    lmaj = gss_create_empty_oid_set(minor_status, &tmp_set);
    if (lmaj != GSS_S_COMPLETE)
	return lmaj;

    tmp_set->count = supported_mechs->count;
    tmp_set->elements =
    	(void*) malloc(tmp_set->count * sizeof(gss_OID_desc));
    if (tmp_set->elements == NULL)
	goto mem_error;

    for (i = 0; i < tmp_set->count; i++) {
	tmp_set->elements[i].length = supported_mechs->elements[i].length;
	tmp_set->elements[i].elements =
			(void *) malloc(tmp_set->elements[i].length);
	if (tmp_set->elements[i].elements == NULL)
	    goto mem_error;

	memcpy(tmp_set->elements[i].elements,
		    supported_mechs->elements[i].elements,
		    tmp_set->elements[i].length);
    }
    *mech_set = tmp_set;    
    return GSS_S_COMPLETE;

list_mem_error:
    supported_mechs = NULL;
mem_error:
    *minor_status = ENOMEM;
    return GSS_S_FAILURE;
}
