/*
 * $Source$
 * $Author$
 *
 * Copyright 1990 by the Massachusetts Institute of Technology.
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
 * This is the get_entry routine for the file based keytab implementation.
 * It opens the keytab file, and either retrieves the entry or returns
 * an error.
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_krb5_ktfile_get_entry_c[] =
"$Id$";
#endif	/* !lint & !SABER */

#include <krb5/krb5.h>
#include <krb5/ext-proto.h>

#include "ktfile.h"

krb5_error_code
krb5_ktfile_get_entry(DECLARG(krb5_keytab, id),
		      DECLARG(krb5_principal, principal),
		      DECLARG(krb5_kvno, kvno),
		      DECLARG(krb5_keytab_entry *, entry))
OLDDECLARG(krb5_keytab, id)
OLDDECLARG(krb5_principal, principal)
OLDDECLARG(krb5_kvno, kvno)
OLDDECLARG(krb5_keytab_entry *, entry)
{
    krb5_keytab_entry *cur_entry;
    krb5_error_code kerror = 0;

    /* Open the keyfile for reading */
    if (kerror = krb5_ktfileint_openr(id))
	return(kerror);
    
    /* 
     * For efficiency and simplicity, we'll use a while true that 
     * is exited with a break statement.
     */
    while (TRUE) {
	cur_entry = 0;
	if (kerror = krb5_ktfileint_read_entry(id, &cur_entry))
	    break;

	if (((kvno == IGNORE_VNO) || (kvno == cur_entry->vno)) &&
	    krb5_principal_compare(principal, cur_entry->principal)) {
	    /* found a match */
	    break;
	}
	krb5_kt_free_entry(cur_entry);
    }
    if (kerror && kerror != KRB5_KT_END) {
	(void) krb5_ktfileint_close(id);
	return kerror;
    }
    if (!(kerror = krb5_ktfileint_close(id))) {
	if (cur_entry) {
	    *entry = *cur_entry;
	    krb5_xfree(cur_entry);
	} else
	    kerror = KRB5_KT_NOTFOUND;
    } else
	krb5_kt_free_entry(cur_entry);
    return kerror;
}
