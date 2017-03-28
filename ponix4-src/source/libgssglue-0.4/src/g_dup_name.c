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
 *
 * created andros 2.24.01 from g_compare_name.c
 */

/*
 *  glue routine for gss_duplicate_name
 *
 */

#include "mglueP.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <errno.h>

OM_uint32 KRB5_CALLCONV
gss_duplicate_name (minor_status,
		    in_name,
		    exp_name)
OM_uint32 *		minor_status;
const gss_name_t		in_name;
gss_name_t		*exp_name;
{
    OM_uint32		tmp,major_status =  GSS_S_COMPLETE;
    gss_union_name_t	union_in_name, union_exp_name;
    gss_mechanism	mech;

    GSS_INITIALIZE;

    /* if exp_name is NULL, simply return */
    if (exp_name == NULL)
	return (GSS_S_COMPLETE);

    *exp_name = NULL;

    if (in_name == 0)
	return (GSS_S_BAD_NAME);

    union_in_name = (gss_union_name_t) in_name;

    /*
     * Create the union name struct that will hold the exported
     * name and the name type.
     */

    union_exp_name = (gss_union_name_t) malloc (sizeof(gss_union_name_desc));
    if (!union_exp_name) {
	*minor_status = ENOMEM;
	goto allocation_failure;
    }
    union_exp_name->external_name.length = 0;
    union_exp_name->external_name.value = NULL;
    union_exp_name->gss_mech = union_in_name->gss_mech;
    union_exp_name->mech_type = GSS_C_NO_OID;
    if (union_in_name->mech_type != GSS_C_NO_OID &&
    	(generic_gss_copy_oid(&tmp, union_in_name->mech_type,
    			&union_exp_name->mech_type) != GSS_S_COMPLETE)) {
	*minor_status = ENOMEM;
	goto allocation_failure;
    }
    union_exp_name->mech_name = NULL;
    union_exp_name->name_type = GSS_C_NO_OID;
    if (union_in_name->name_type != GSS_C_NO_OID &&
	(generic_gss_copy_oid(&tmp, union_in_name->name_type,
    			&union_exp_name->name_type) != GSS_S_COMPLETE)) {
	*minor_status = ENOMEM;
	goto allocation_failure;
    }
    union_exp_name->external_name.length = union_in_name->external_name.length;
    /*
     * we malloc length+1 to stick a NULL on the end, just in case
     * Note that this NULL is not included in ->length for a reason!
     */

    union_exp_name->external_name.value =
		(void  *) malloc(union_in_name->external_name.length);
    if (!union_exp_name->external_name.value) {
	*minor_status = ENOMEM;
	goto allocation_failure;
    }
    memcpy(union_exp_name->external_name.value,
	union_in_name->external_name.value,
	union_exp_name->external_name.length);

    /*
     * Mechanism specific name
     */

    if (union_in_name->mech_type != GSS_C_NO_OID) {
	mech = __gss_get_mechanism (union_in_name->mech_type);
	if (!mech)
	    return (GSS_S_BAD_MECH);
	if (!mech->gss_duplicate_name)
	    return (GSS_S_BAD_BINDINGS);

	major_status = mech->gss_duplicate_name(minor_status,
	    union_in_name->mech_name, &union_exp_name->mech_name);
	if (major_status != GSS_S_COMPLETE)
	    return (major_status);
    }
    *exp_name = union_exp_name;
    return (major_status);

allocation_failure:
    if (union_exp_name) {
	if (union_exp_name->external_name.value)
	    free(union_exp_name->external_name.value);
	if (union_exp_name->name_type)
	    generic_gss_release_oid(&tmp, &union_exp_name->name_type);
	if (union_exp_name->mech_name)
	    __gss_release_internal_name(minor_status, union_exp_name->mech_type,
		&union_exp_name->mech_name);
	if (union_exp_name->mech_type)
	    generic_gss_release_oid(&tmp, &union_exp_name->mech_type);
	    free(union_exp_name);
    }
return (major_status);

}

