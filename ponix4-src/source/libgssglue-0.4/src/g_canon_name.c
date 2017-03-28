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
 *  glue routine for gss_canonicalize_name
 */

#include "mglueP.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <errno.h>

OM_uint32 KRB5_CALLCONV
gss_canonicalize_name (OM_uint32 *minor_status,
		       const gss_name_t input_name,
		       const gss_OID mech_type,
		       gss_name_t *canon_name)
{
    OM_uint32		minor, status;
    gss_union_name_t	union_input_name, union_canon_name;
    gss_name_t		mech_name = GSS_C_NO_NAME;
    gss_mechanism	mech;

    GSS_INITIALIZE;

    if (minor_status != NULL)
	*minor_status = 0;

    /* if canon_name is NULL, simply return */
    if (canon_name == NULL)
	return GSS_S_COMPLETE;

    *canon_name = GSS_C_NO_NAME;

    if (input_name == GSS_C_NO_NAME)
	return GSS_S_BAD_NAME;

    union_input_name = (gss_union_name_t) input_name;
    mech = __gss_get_mechanism (mech_type);
    if (mech == NULL || mech->gss_canonicalize_name == NULL)
	return GSS_S_BAD_MECH;

    status = GSS_S_FAILURE;

    /*
     * Create the union name struct that will hold the canonicalized
     * name and the name type.
     */
    union_canon_name = (gss_union_name_t) malloc (sizeof(gss_union_name_desc));
    if (union_canon_name == NULL) {
	*minor_status = ENOMEM;
	goto failure;
    }

    union_canon_name->name_type = GSS_C_NO_OID;
    union_canon_name->external_name.length = 0;
    union_canon_name->external_name.value = 0;

    union_canon_name->mech_type = GSS_C_NO_OID;
    union_canon_name->mech_name = GSS_C_NO_NAME;

    if (union_input_name->mech_type != GSS_C_NO_OID &&
	g_OID_equal(&mech->mech_type, union_input_name->mech_type)) {
	/* Input name was specific to this mech, just canonicalize */
	status = mech->gss_canonicalize_name(minor_status,
					     union_input_name->mech_name,
					     &mech->mech_type,
					     &mech_name);
	if (status != GSS_S_COMPLETE)
	    goto failure;
    } else {
	/*
	 * Input name was generic or specific to another mech; import
	 * then canonicalize.
	 */
	gss_name_t internal_name = GSS_C_NO_NAME;

	if (mech->gss_import_name == NULL) {
	    status = GSS_S_BAD_MECH;
	    goto failure;
	}

	status = mech->gss_import_name(minor_status,
				       &union_input_name->external_name,
				       union_input_name->name_type,
				       &internal_name);
	if (status != GSS_S_COMPLETE)
	    goto failure;

	status = mech->gss_canonicalize_name(minor_status,
					     internal_name,
					     &mech->mech_type,
					     &mech_name);
	if (status != GSS_S_COMPLETE)
	    goto failure;

	__gss_release_internal_name(&minor, &mech->mech_type, &internal_name);
    }

    union_canon_name->mech_name = mech_name;

    status = __gss_copy_namebuf(&union_input_name->external_name,
				&union_canon_name->external_name);
    if (status != GSS_S_COMPLETE)
	goto failure;

    if (union_input_name->name_type != GSS_C_NO_OID) {
	status = generic_gss_copy_oid(minor_status,
				      union_input_name->name_type,
				      &union_canon_name->name_type);
	if (status != GSS_S_COMPLETE) 
	    goto failure;
    }

    status = generic_gss_copy_oid(minor_status,
				  &mech->mech_type,
				  &union_canon_name->mech_type);
    if (status != GSS_S_COMPLETE) 
	goto failure;

    *canon_name = union_canon_name;

    return GSS_S_COMPLETE;

failure:
    gss_release_name (&minor, (gss_name_t *)&union_canon_name);

    return status;
}

