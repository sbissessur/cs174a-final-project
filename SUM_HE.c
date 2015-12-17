/*
// CS 174A - Databases
// Sahil Bissessur and Vincent Chang
// SUM_HE.c - UDF Sum function
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gmp.h>
#include <paillier.h>
#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>


my_bool SUM_HE_init(UDF_INIT *initid, UDF_ARGS *args, char message) {
  paillier_ciphertext_t *cipherText = paillier_create_enc_zero();
  initid->ptr = (char *)cipherText;

  if (args->arg_count != 1) {
    strcpy(message,"SUM_HE() requires one arguments");
    return 1;
  }

  if (args->arg_type[0] != STRING_RESULT) {
    strcpy(message, "SUM_HE() requires a string");
    return 1;
  }

  return 0;

}


void SUM_HE_deinit(UDF_INIT *initid) {
  free((paillier_ciphertext_t *)initid->ptr);
}

void SUM_HE_clear(UDF_INIT *initid, char *is_null, char *error) {
  paillier_ciphertext_t *sum = (paillier_ciphertext_t *)initid->ptr;
  mpz_init_set_ui(sum->c, 1);
}


void SUM_HE_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error) {
  char *publicKeyChar = "8d76d3e4f68d3a941f3c8da16d45096f";
  paillier_pubkey_t *publicKey = paillier_pubkey_from_hex(publicKeyChar);

  paillier_ciphertext_t *sum = (paillier_ciphertext_t *)initid->ptr;

  char *cipherChar = (char *)args->args[0];
  paillier_ciphertext_t *cipherText = (paillier_ciphertext_t *) malloc(sizeof(paillier_ciphertext_t));
  mpz_init_set_str(cipherText->c, cipherChar, 16);

  paillier_mul(publicKey, sum, sum, cipherText);
  paillier_freepubkey(publicKey);
  paillier_freeciphertext(cipherText);
}


char* SUM_HE(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error) {
  paillier_ciphertext_t *sum = (paillier_ciphertext_t *)initid->ptr;
  char *sumChar = mpz_get_str(0, 16, sum->c);

  int sumCharLength = strlen(sumChar);

  *length = sumCharLength;
  result = sumChar;
  return sumChar;
}
