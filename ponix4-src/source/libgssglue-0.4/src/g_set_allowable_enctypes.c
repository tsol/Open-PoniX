/* #ident  "@(#)gss_set_allowable_enctype.c 1.9     95/08/02 SMI" */

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
 *  glue routine for gss_set_allowable_enctypes
 */

#include "mglueP.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>

OM_uint32 KRB5_CALLCONV
gss_set_allowable_enctypes(minor_status,
			   cred_handle,
			   mech_type,
			   num_ktypes,
			   ktypes)

OM_uint32 *		minor_status;
gss_cred_id_t 		cred_handle;
gss_OID			mech_type;
OM_uint32 		num_ktypes;
void *			ktypes;

{
    gss_union_cred_t	union_cred;
    gss_mechanism	mech;
    gss_cred_id_t	mech_cred;

    GSS_INITIALIZE;

    if (cred_handle == GSS_C_NO_CREDENTIAL)
	return (GSS_S_NO_CRED);

    if ((mech = __gss_get_mechanism(mech_type)) == NULL)
	return (GSS_S_BAD_MECH);

    if (!mech->gss_set_allowable_enctypes)
	return (GSS_S_FAILURE);

    /* get the mechanism-specific cred handle */

    union_cred = (gss_union_cred_t) cred_handle;
    mech_cred = __gss_get_mechanism_cred(union_cred, mech_type);

    if (mech_cred == GSS_C_NO_CREDENTIAL)
	return (GSS_S_NO_CRED);

    /* Call the mechanism-specific routine */
    return (mech->gss_set_allowable_enctypes(minor_status,
					     mech_cred, num_ktypes, ktypes));
}

