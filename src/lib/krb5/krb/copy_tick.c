/*
 * $Source$
 * $Author$
 *
 * Copyright 1990 by the Massachusetts Institute of Technology.
 *
 * For copying and distribution information, please see the file
 * <krb5/copyright.h>.
 *
 * krb5_copy_ticket()
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_copy_tick_c[] =
"$Id$";
#endif	/* !lint & !SABER */

#include <krb5/copyright.h>
#include <krb5/krb5.h>
#include <errno.h>

#include <krb5/ext-proto.h>

static krb5_error_code
krb5_copy_enc_tkt_part(partfrom, partto)
const krb5_enc_tkt_part *partfrom;
krb5_enc_tkt_part **partto;
{
    krb5_error_code retval;
    krb5_enc_tkt_part *tempto;
    krb5_data *scratch;

    if (!(tempto = (krb5_enc_tkt_part *)malloc(sizeof(*tempto))))
	return ENOMEM;
    *tempto = *partfrom;
    if (!(tempto->session =
	  (krb5_keyblock *)malloc(sizeof(*tempto->session)))) {
	xfree(tempto);
	return ENOMEM;
    }
    if (retval = krb5_copy_keyblock(partfrom->session,
				    tempto->session)) {
	xfree(tempto->session);
	xfree(tempto);
	return retval;
    }
    if (retval = krb5_copy_principal(partfrom->client, &tempto->client)) {
	krb5_free_keyblock(tempto->session);
	xfree(tempto);
	return retval;
    }
    if (retval = krb5_copy_data(&partfrom->transited, &scratch)) {
	krb5_free_principal(tempto->client);
	krb5_free_keyblock(tempto->session);
	xfree(tempto);
	return retval;
    }
    tempto->transited = *scratch;
    xfree(scratch);
    if (retval = krb5_copy_addresses(partfrom->caddrs, &tempto->caddrs)) {
	xfree(tempto->transited.data);
	krb5_free_principal(tempto->client);
	krb5_free_keyblock(tempto->session);
	xfree(tempto);
	return retval;
    }
    if (partfrom->authorization_data) {
	if (retval = krb5_copy_authdata(partfrom->authorization_data,
					&tempto->authorization_data)) {
	    krb5_free_address(tempto->caddrs);
	    xfree(tempto->transited.data);
	    krb5_free_principal(tempto->client);
	    krb5_free_keyblock(tempto->session);
	    xfree(tempto);
	    return retval;
	}
    }
    *partto = tempto;
    return 0;
}

krb5_error_code
krb5_copy_ticket(from, pto)
const krb5_ticket *from;
krb5_ticket **pto;
{
    krb5_error_code retval;
    krb5_ticket *tempto;
    krb5_data *scratch;

    if (!(tempto = (krb5_ticket *)malloc(sizeof(*tempto))))
	return ENOMEM;
    *tempto = *from;
    if (retval = krb5_copy_principal(from->server, &tempto->server))
	return retval;
    if (retval = krb5_copy_data(&from->enc_part, &scratch)) {
	krb5_free_principal(tempto->server);
	xfree(tempto);
	return retval;
    }
    tempto->enc_part = *scratch;
    xfree(scratch);
    if (retval = krb5_copy_enc_tkt_part(from->enc_part2, &tempto->enc_part2)) {
	xfree(tempto->enc_part.data);
	krb5_free_principal(tempto->server);
	xfree(tempto);
	return retval;
    }	
    *pto = tempto;
    return 0;
}
