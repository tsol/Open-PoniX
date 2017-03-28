/* #ident  "@(#)g_dsp_name.c 1.2     96/02/06 SMI" */

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
 *  glue routine for gss_export_name()
 *
 */

#include "mglueP.h"

OM_uint32 KRB5_CALLCONV
gss_export_name (OM_uint32 *minor_status,
	         const gss_name_t input_name,
	         gss_buffer_t exported_name)
{
    gss_union_name_t	union_name;
    gss_name_t		internal_name = GSS_C_NO_NAME;
    gss_mechanism	mech;
    OM_uint32		status;
    OM_uint32		minor;

    GSS_INITIALIZE;

    if (input_name == GSS_C_NO_NAME)
	return GSS_S_BAD_NAME;

    if (minor_status != NULL)
	*minor_status = 0;

    if (exported_name != GSS_C_NO_BUFFER) {
	exported_name->value = NULL;
	exported_name->length = 0;
    }

    union_name = (gss_union_name_t) input_name;

    /*
     * If it's not a mechanism specific name type then we pass
     * it on to the default mechanism (usually Kerberos) to deal
     * with
     */
    if (union_name->mech_type != GSS_C_NO_OID) {
	mech = __gss_get_mechanism(union_name->mech_type);
    } else {
	mech = __gss_get_mechanism(GSS_C_NO_OID);
    }

    if (mech == NULL || mech->gss_export_name == NULL)
	return GSS_S_BAD_MECH;

    if (union_name->mech_type == GSS_C_NO_OID) {
	if (mech->gss_import_name == NULL)
	    return GSS_S_BAD_MECH;

	status = mech->gss_import_name(minor_status,
				       &union_name->external_name,
				       union_name->name_type,
				       &internal_name);
	if (status != GSS_S_COMPLETE)
	    return GSS_S_BAD_NAME;
    }

    status = mech->gss_export_name(minor_status,
				   internal_name ? internal_name :
				   union_name->mech_name,
				   exported_name);

    if (internal_name != GSS_C_NO_NAME) {
	__gss_release_internal_name(&minor, union_name->name_type,
				    &internal_name);
    }

    return status;
}

