#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <gmp.h>
#include <paillier.h>

void generateKeys(paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey) {
	FILE *fp;

	paillier_keygen(128, &publicKey, &privateKey, &paillier_get_rand_devurandom);

	char *publicKeyInHex = paillier_pubkey_to_hex(publicKey);
	char *privateKeyInHex = paillier_prvkey_to_hex(privateKey);

	fp = fopen("paillierKeys.txt", "w");
	fprintf(fp, "Public Key: %s \nPrivate Key: %s\n", publicKeyInHex, privateKeyInHex);
	fclose(fp);
	
	printf("**Make sure to update UDF with new public key.**\n");
}


int main (int argc, char **argv) {
	paillier_pubkey_t *publicKey = (paillier_pubkey_t *) malloc(sizeof(paillier_pubkey_t));
	paillier_prvkey_t *privateKey = (paillier_prvkey_t *) malloc(sizeof(paillier_prvkey_t));

	printf("Generating...\n");
	generateKeys(publicKey, privateKey);

	return 0;
}
