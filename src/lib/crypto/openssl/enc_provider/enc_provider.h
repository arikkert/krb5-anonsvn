/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (C) 1998 by the FundsXpress, INC.
 *
 * All rights reserved.
 *
 * Export of this software from the United States of America may require
 * a specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of FundsXpress. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  FundsXpress makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "k5-int.h"

extern const struct krb5_enc_provider krb5int_enc_des;
extern const struct krb5_enc_provider krb5int_enc_des3;
extern const struct krb5_enc_provider krb5int_enc_arcfour;
extern const struct krb5_enc_provider krb5int_enc_aes128;
extern const struct krb5_enc_provider krb5int_enc_aes256;
extern const struct krb5_enc_provider krb5int_enc_aes128_ctr;
extern const struct krb5_enc_provider krb5int_enc_aes256_ctr;
#ifdef CAMELLIA_CCM
extern const struct krb5_enc_provider krb5int_enc_camellia128_ctr;
extern const struct krb5_enc_provider krb5int_enc_camellia256_ctr;
#endif

#include <openssl/aes.h>
#define aes_ctx AES_KEY
#define krb5int_aes_enc_key(key, klen, ctx) AES_set_encrypt_key(key, klen * 8, ctx)
#define aes_enc_blk AES_encrypt

