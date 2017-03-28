/*
 *  g_ccache_name.c
 *
 *  Copyright (c) 2007 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mglueP.h"

OM_uint32 KRB5_CALLCONV
gss_krb5_ccache_name(OM_uint32 *minor_status,
		     const char *name,
		     const char **out_name)
{
    OM_uint32		status;
    gss_mechanism	mech;
    gss_OID_desc	krb5oid = {9, "\052\206\110\206\367\022\001\002\002"};

    GSS_INITIALIZE;

    mech = __gss_get_mechanism (&krb5oid);

    if (mech) {

	if (mech->krb5_ccache_name) {
	    status = mech->krb5_ccache_name(minor_status,
					    name,
					    out_name);
	}
	else {
	    status = GSS_S_BAD_BINDINGS;
	}

	return(status);
    }

    return(GSS_S_NO_CONTEXT);
}
