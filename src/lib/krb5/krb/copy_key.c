/*
 * $Source$
 * $Author$
 *
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 * 
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 *
 * krb5_copy_keyblock()
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_copy_key_c[] =
"$Id$";
#endif	/* !lint & !SABER */

#include <krb5/krb5.h>
#include <krb5/ext-proto.h>

/*
 * Copy a keyblock, including alloc'ed storage.
 */
krb5_error_code
krb5_copy_keyblock(from, to)
const krb5_keyblock *from;
krb5_keyblock **to;
{
	krb5_keyblock	*new_key;

	if (!(new_key = (krb5_keyblock *) malloc(sizeof(krb5_keyblock))))
		return ENOMEM;
	*new_key = *from;
	if (!(new_key->contents = (krb5_octet *)malloc(new_key->length))) {
		krb5_xfree(new_key);
		return(ENOMEM);
	}
	memcpy((char *)new_key->contents, (char *)from->contents,
	       new_key->length);
	*to = new_key;
	return 0;
}
