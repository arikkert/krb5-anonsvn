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
 * krb5_encode_kdc_rep() function.
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_encode_kdc_c [] =
"$Id$";
#endif	/* !lint & !SABER */

#include <krb5/krb5.h>
#include <krb5/asn1.h>

#include <krb5/ext-proto.h>

/*
 Takes KDC rep parts in *rep and *encpart, and formats it into *enc_rep,
 using message type type and encryption key client_key and encryption type
 etype.

 The string *enc_rep will be allocated before formatting; the caller should
 free when finished.

 returns system errors

 dec_rep->enc_part.ciphertext is allocated and filled in.
*/
/* due to argument promotion rules, we need to use the DECLARG/OLDDECLARG
   stuff... */
krb5_error_code
krb5_encode_kdc_rep(DECLARG(const krb5_msgtype, type),
		    DECLARG(const register krb5_enc_kdc_rep_part *, encpart),
		    DECLARG(const krb5_keyblock *, client_key),
		    DECLARG(register krb5_kdc_rep *, dec_rep),
		    DECLARG(krb5_data **, enc_rep))
OLDDECLARG(const krb5_msgtype, type)
OLDDECLARG(const register krb5_enc_kdc_rep_part *, encpart)
OLDDECLARG(const krb5_keyblock *, client_key)
OLDDECLARG(register krb5_kdc_rep *, dec_rep)
OLDDECLARG(krb5_data **, enc_rep)
{
    krb5_data *scratch;
    krb5_encrypt_block eblock;
    krb5_error_code retval;

    if (!valid_etype(dec_rep->enc_part.etype))
	return KRB5_PROG_ETYPE_NOSUPP;

    switch (type) {
    case KRB5_AS_REP:
    case KRB5_TGS_REP:
	break;
    default:
	return KRB5_BADMSGTYPE;
    }

    retval = encode_krb5_enc_kdc_rep_part(encpart, &scratch);
    if (retval) {
	return retval;
    }

#define cleanup_scratch() { (void) memset(scratch->data, 0, scratch->length); \
krb5_free_data(scratch); }

    /* put together an eblock for this encryption */

    krb5_use_cstype(&eblock, dec_rep->enc_part.etype);
    dec_rep->enc_part.ciphertext.length =
	krb5_encrypt_size(scratch->length, eblock.crypto_entry);
    /* add padding area, and zero it */
    if (!(scratch->data = realloc(scratch->data,
				  dec_rep->enc_part.ciphertext.length))) {
	/* may destroy scratch->data */
	krb5_xfree(scratch);
	return ENOMEM;
    }
    memset(scratch->data + scratch->length, 0,
	  dec_rep->enc_part.ciphertext.length - scratch->length);
    if (!(dec_rep->enc_part.ciphertext.data =
	  malloc(dec_rep->enc_part.ciphertext.length))) {
	retval = ENOMEM;
	goto clean_scratch;
    }

#define cleanup_encpart() { \
(void) memset(dec_rep->enc_part.ciphertext.data, 0, \
	     dec_rep->enc_part.ciphertext.length); \
free(dec_rep->enc_part.ciphertext.data); \
dec_rep->enc_part.ciphertext.length = 0; \
dec_rep->enc_part.ciphertext.data = 0;}

    retval = krb5_process_key(&eblock, client_key);
    if (retval) {
	goto clean_encpart;
    }

#define cleanup_prockey() {(void) krb5_finish_key(&eblock);}

    retval = krb5_encrypt((krb5_pointer) scratch->data,
			      (krb5_pointer) dec_rep->enc_part.ciphertext.data,
			      scratch->length, &eblock, 0);
    if (retval) {
	goto clean_prockey;
    }

    /* do some cleanup */
    cleanup_scratch();

    retval = krb5_finish_key(&eblock);
    if (retval) {
	cleanup_encpart();
	return retval;
    }

    /* now it's ready to be encoded for the wire! */

    switch (type) {
    case KRB5_AS_REP:
	retval = encode_krb5_as_rep(dec_rep, enc_rep);
	break;
    case KRB5_TGS_REP:
	retval = encode_krb5_tgs_rep(dec_rep, enc_rep);
	break;
    }
    if (retval)
	cleanup_encpart();
    return retval;

 clean_prockey:
    cleanup_prockey();
 clean_encpart:
    cleanup_encpart();
 clean_scratch:
    cleanup_scratch();

    return retval;
}


