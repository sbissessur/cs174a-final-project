/*
// CS 174A
// Sahil Bissessur and Vincent Chang
// queries.c - SQL queries and command line interface
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <gmp.h>
#include <paillier.h>
#include <mysql/my_global.h>
#include <mysql/mysql.h>


/* SQL FUNCTIONS */
char *readQuery(char *query, paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey);
void executeQuery(char *query, paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey);
void executeAVGQuery(char *query, paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey);
MYSQL *connection;

/* PAILLIER FUNCTIONS */
char *encrypt(paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey, char *salary);			//Done
char *decrypt(paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey, char *encryptedVal);			//Done

/* MISC FUNCTIONS */
char *replace_str(char *str, char *orig, char *rep);

char *readQuery (char *inputQuery, paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey) {
        bool SUM = false;
	bool WHERE = false;
	bool GROUPBY = false;
        bool HAVING = false;
	bool AVG = false;
	bool ALL = false;	// Flags for query
	
	int queryLength = strlen(inputQuery);				// Get the length of the query
	inputQuery[queryLength - 1] = '\0';				// Insert null character at end of query
	
	char *query = (char *)malloc(512 * sizeof(char));		// Allocate query space
	char *token;
	int tokenLength;
	char *tokenArray[32];
	int wordInInputQuery = 1;
	int startAt, endAt;
	int WHEREIndex, GROUPBYIndex, HAVINGIndex;

	if (strcmp(inputQuery, "") == 0) {
		fprintf(stderr, "%s\n", "No query given.");
		strcpy(query, "BadQuery");
	}
	else {
			printf("readQuery - Tokenizing...\n");
		token = strtok(inputQuery, " ");
			printf("readQuery - Token: %s\n", token);
		tokenLength = strlen(token);
			printf("readQuery - Allocating memory..\n");
		tokenArray[wordInInputQuery] = (char *) malloc(tokenLength * sizeof(char));
			printf("readQuery - Memory allocated.\n");
		strcpy(tokenArray[wordInInputQuery], token);
			printf("readQuery - copy successful.\n");

		// If INSERT is the first word
			printf("readQuery - Checking INSERT...\n");
			printf("readQuery - tokenArray[wordInInputQuery]: %s\n", tokenArray[wordInInputQuery]);

		if ((strcmp(tokenArray[wordInInputQuery], "insert") == 0) || (strcmp(tokenArray[wordInInputQuery], "INSERT") == 0)) {
			// Tokenize rest of input
				printf("readQuery/INSERT - Tokenizing...\n");
			//token = strtok(inputQuery, " ");	
			token = strtok(NULL, " ");
				printf("readQuery/INSERT - Token: %s\n", token);
			while (token != NULL) {
				tokenLength = strlen(token);
					printf("readQuery/INSERT/while - Allocating memory...\n");
				tokenArray[wordInInputQuery] = (char *)malloc(tokenLength * sizeof(char));
					printf("readQuery/INSERT/while - Memory allocated. \n");
				strcpy(tokenArray[wordInInputQuery], token);
					printf("readQuery/INSERT/while - Copy successful. \n");
					printf("readQuery/INSERT/while - tokenArray[wordInInputQuery]: %s\n", tokenArray[wordInInputQuery]);

				wordInInputQuery++;
					printf("readQuery/INSERT/while - wordInInputQuery: %d\n", wordInInputQuery);
				token = strtok(NULL, " ");
			}
			printf("readQuery/INSERT - Tokenized.\n");	
			// Check for correct INSERT query
			if ((wordInInputQuery) != 4) {
				strcpy(query, "Bad query");
			}
			else {
					printf("readQuery/INSERT - Building Query...\n");
				char *encryptedSalary = encrypt(publicKey, privateKey, tokenArray[3]);
				strcpy(query, "INSERT ");
				strcat(query, "INTO Employees VALUES(");
				strcat(query, tokenArray[1]);
				strcat(query, ", ");
				strcat(query, tokenArray[2]);
				strcat(query, ", '");
				strcat(query, encryptedSalary);
				strcat(query, "');");
					//printf("Query is: %s\n", query);
			}	
		}
		// If SELECT is the first word
		else if ((strcmp(tokenArray[wordInInputQuery], "select") == 0) || (strcmp(tokenArray[wordInInputQuery], "SELECT") == 0)) {
			// Tokenize rest of input
				printf("readQuery/SELECT - Tokenizing...\n");
			token = strtok(NULL, " ");
                        	printf("readQuery/SELECT - Token: %s\n", token);
			while (token != NULL) {
				//token = strtok(NULL, " ");
				//	printf("readQuery/SELECT (in while) - Token: %s\n", token);
				
				if (token != NULL) {
					tokenLength = strlen(token);
	                                tokenArray[wordInInputQuery] = (char *)malloc(tokenLength * sizeof(char));
        	                        strcpy(tokenArray[wordInInputQuery], token);
				
				if ((strcmp(tokenArray[wordInInputQuery], "sum") == 0) || (strcmp(tokenArray[wordInInputQuery], "SUM") == 0)) {
						printf("readQuery/SELECT - SUM found\n");
					SUM = true;
				}
				else if ((strcmp(tokenArray[wordInInputQuery], "where") == 0) || (strcmp(tokenArray[wordInInputQuery], "WHERE") == 0)) {
						printf("readQuery/SELECT - WHERE found\n");
					WHERE = true;
					WHEREIndex = wordInInputQuery;
				}
				else if ((strcmp(tokenArray[wordInInputQuery], "group") == 0) || (strcmp(tokenArray[wordInInputQuery], "GROUP") == 0)) {
						printf("readQuery/SELECT - GROUP BY found\n");
					GROUPBY = true; 
					GROUPBYIndex = wordInInputQuery;
				}
				else if ((strcmp(tokenArray[wordInInputQuery], "having") == 0) || (strcmp(tokenArray[wordInInputQuery], "HAVING") == 0)) {
						printf("readQuery/SELECT - HAVING found\n");
					HAVING = true;
					HAVINGIndex = wordInInputQuery;
				}
				else if ((strcmp(tokenArray[wordInInputQuery], "avg") == 0) || (strcmp(tokenArray[wordInInputQuery], "AVG") == 0)) {
						printf("readQuery/SELECT - AVG found\n");
					AVG = true;
				}
				else if (strcmp(tokenArray[wordInInputQuery], "*") == 0) { 
				//else if (tokenArray[wordInInputQuery] == "*") {
						printf("readQuery/SELECT - ALL found\n");
					ALL = true;
				}
				else { printf("readQuery/SELECT - No flags found\n"); /* None of the above */ }

                                wordInInputQuery++;
					printf("readQuery/SELECT - wordInInputQuery: %d\n", wordInInputQuery);
				token = strtok(NULL, " ");
					printf("readQuery/SELECT (in while) - Token: %s\n", token);
                        	}
			}

			strcpy(query, "SELECT ");

			// Input: SELECT SUM GROUP BY
			if (SUM && GROUPBY) {
					printf("readQuery/SELECT - Input: SELECT SUM GROUP BY\n");
				strcat(query, "age, ");					// Output: SELECT age
			}
			// Input: SELECT *
			else if (ALL) {
					printf("readQuery/SELECT - Input: SELECT *\n");
				strcat(query, "id, age, salary FROM Employees");	// Output: SELECT id, age, salary FROM Employees
			}
			// Input: SELECT emp_id
			else if ((wordInInputQuery == 1) && (!ALL) && (!SUM)) {
					printf("readQuery/SELECT - Input: SELECT emp_id\n");
				strcat(query, "id, age, salary FROM Employees WHERE id = "); // Output: SELECT id, age, salary FROM Employees WHERE ID = emp_id
				strcat(query, tokenArray[1]);
			}

			// Input: SELECT SUM [WHERE] [GROUP BY]
			if (SUM) {
					printf("readQuery/SELECT - Input: SELECT SUM [WHERE] [GROUP BY]\n");
				strcat(query, "SUM_HE(salary) FROM Employees");		// Output: ... SUM_HE(salary) FROM Employees
			}
			// Input:  SELECT AVG [WHERE] [GROUP BY]
			else if (AVG) {
					printf("readQuery/SELECT - Input: SELECT AVG [WHERE] [GROUP BY]\n");
				strcat(query, "AVG(salary) FROM Employees");		// Output ... AVG(salary) FROM Employees
			}

			// Input: ... WHERE id <|=|>|<=|>= num;
			if (WHERE) {
					printf("readQuery/SELECT - Input: ... WHERE id [op] num\n");
				startAt = WHEREIndex + 1;

				// Input: ... WHERE id [op] num GROUP BY ...	
				if (GROUPBY) { 
						printf("readQuery/SELECT - Input: ... WHERE id [op] nom\n");
					endAt = GROUPBYIndex; }			// Output from WHERE to GROUP BY
				else { endAt = wordInInputQuery; }			// Output rest of query

				// Output
				strcat(query, " WHERE");
				int i;
				for (i = startAt; i < endAt; i++) {
					strcat(query, " ");
					strcat(query, tokenArray[i]);
				}
			}

			// Input: ... GROUP BY [something];
			if (GROUPBY) {
					printf("readQuery/SELECT - Input: ... GROUP BY [something]\n");
				startAt = GROUPBYIndex + 2;

				// Input: ... GROUP BY [something] HAVING ...
				if (HAVING) { endAt = HAVINGIndex; }			// Output from GROUP BY to SOMETHING
				else { endAt = wordInInputQuery; }			// Output rest of query

				// Output
				strcat(query, " GROUP BY");
				int i;
				for (i = startAt; i < endAt; i++) {			
					strcat(query, " ");
					strcat(query, tokenArray[i]);
				}
			}
	
			// Input: ... HAVING ...;
			if (HAVING) {
					printf("readQuery/SELECT - Input: ... HAVING ...\n");
				startAt = HAVINGIndex + 1;
				endAt = wordInInputQuery;				// Output rest of query

				// Output
				strcat(query, " HAVING");
				int i;
				for (i = startAt; i < endAt; i++) {
					strcat(query, " ");
					strcat(query, tokenArray[i]);
				}
			}
		}		
		else { strcpy(query, "BadQuery"); /* No INSERT or SELECT */ }
	}
	return query;
}

// http://www.linuxquestions.org/questions/programming-9/replace-a-substring-with-another-string-in-c-170076/
// Example: replace_str("Hello, world!", "world", "Miami");
char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}


char *encrypt(paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey, char *salary) {
	unsigned long int sal;
	char* encryptedSalary;
	char* endptr;

	sal = strtoul(salary, &endptr, 10);
	paillier_plaintext_t *plaintext = paillier_plaintext_from_ui(sal);
	paillier_ciphertext_t *ciphertext = paillier_enc(ciphertext, publicKey, plaintext, paillier_get_rand_devurandom);
	encryptedSalary = mpz_get_str(0, 16, ciphertext->c);

	return encryptedSalary;
}

char *decrypt(paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey, char *encryptedVal) {
		//printf("decrypt - Check 1\n");
	paillier_ciphertext_t *cipherText = (paillier_ciphertext_t *)malloc(sizeof(paillier_ciphertext_t));
		//printf("decrypt - Check 2\n");
	mpz_init_set_str(cipherText->c, encryptedVal, 16);
		//printf("decrypt - Check 3\n");
	paillier_plaintext_t *plainText = paillier_dec(NULL, publicKey, privateKey, cipherText);
		//printf("decrypt - Check 4\n");
	unsigned long int plainTextFromUI = mpz_get_ui(plainText->m);

		//printf("decrypt - Check 5\n");
	// Gets length of an integer (including negative numbers) http://stackoverflow.com/questions/4143000/find-the-string-length-of-an-int
	int integerLength = (plainTextFromUI == 0 ? 1 : ((int)(log10)(fabs(plainTextFromUI))+1 + (plainTextFromUI < 0 ? 1 : 0)));
	
		//printf("decrypt - Check 6\n");
	char *ret = (char *)malloc(integerLength * sizeof(char));
		//printf("decrypt - Check 7\n");
	sprintf(ret, "%lu", plainTextFromUI);

	return ret;

}

void executeAVGQuery(char *query, paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey) {
	MYSQL_RES *sumResult, *countResult;

	// Obtain encrypted SUM
	char *sumQuery = replace_str(query, "AVG", "SUM_HE");
	if (!mysql_query(connection, sumQuery)) {
		sumResult = mysql_store_result(connection);

		if (sumResult == NULL) {
			if (strncmp(sumQuery, "I", 1) != 0) {
				error(false);
				return;
			}
		}
	}
	else { 
		fprintf(stderr, "ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
	}

	
	// Obtain row count
	char *countQuery = replace_str(sumQuery, "SUM_HE(salary)", "COUNT(*)");
	if (!mysql_query(connection, countQuery)) {
		countResult = mysql_store_result(connection);

		if (countResult == NULL) {
			if (strncmp(countQuery, "I", 1) != 0) {
				error(false);
				return;
			}
		}
	}
	else { 
		fprintf(stderr, "eAVG - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
	}

	// If we have valid results returned to us
	if ((sumResult != NULL) && (countResult != NULL)) {
		int i;
		int numberOfSumFields = mysql_num_fields(sumResult);
		int numberOfCountFields = mysql_num_fields(countResult);
		int countRows = 0;
		MYSQL_ROW sumRow, countRow;
		MYSQL_FIELD *sumFields;

		if (numberOfSumFields == numberOfCountFields) {
			for (i = 0; i < numberOfSumFields; i++) {
				printf("+----------");
			}
			printf("+\n");

			sumFields = mysql_fetch_fields(sumResult);

			for (i = 0; i < numberOfSumFields; i++) {
				printf("+----------");
			}
			printf("+\n");

			
			while ((sumRow = mysql_fetch_row(sumResult)) && (countRow = mysql_fetch_row(countResult))) {
				countRows++;

				for (i = 0; i < numberOfSumFields; i++) {
					printf("|");

					if (i == numberOfSumFields - 1) {
						double sum = atof(decrypt(publicKey, privateKey, sumRow[i]));
						int count = atof(countRow[i]);
						double average = sum / count;
						printf("%10.2f|", average);
					}
					else {
						printf("%10s", sumRow[i]);
					}
				}
				
				printf("\n");
			}

			for (i = 0; i < numberOfSumFields; i++) {
				printf("+----------");
			}
			printf("+\n");

			printf("%d row(s) in set.\n\n", countRows);

			mysql_free_result(sumResult);
			mysql_free_result(countResult);
		}

	}

}


void executeQuery(char* query, paillier_pubkey_t *publicKey, paillier_prvkey_t *privateKey) {
	// Check if query is invalid
	if (strcmp(query, "BadQuery") == 0) {
		printf("ERROR: Query is invalid.\n");
	}
	// Check if query contains an AVG (case f)
	else if ((strncmp(query, "SELECT AVG", 10) == 0) || (strncmp(query, "SELECT age, AVG",15) == 0)) {
			printf("executeQuery - Going to executeAVG\n");
		executeAVGQuery(query, publicKey, privateKey);
	}
	// Execute query for all other queries
	else {
			printf("executeQuery - Fetching query...\n");
		
		// Fetch query and output data
		if (!mysql_query(connection, query)) {
			MYSQL_RES *result = mysql_store_result(connection);
			
				printf("executeQuery - Query fetched.\n");
			if (result == NULL) {
				if (strncmp(query, "I", 1) != 0) {
					fprintf(stderr, "eQ - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
				}
			}
			// If you have a result
			else {
				int i;
				int numberOfFields = mysql_num_fields(result);
				int countRows = 0;
				MYSQL_ROW row;
				MYSQL_FIELD *fields;
					
					//printf("executeQuery - Formatting 1\n");	
				// Formatting
				for (i = 0; i < numberOfFields; i++) {
					printf("+---------");
				}
				printf("+\n");


					//printf("executeQuery - Formatting 2\n");
				// Outputting fields
				fields = mysql_fetch_fields(result);
				for (i = 0; i < numberOfFields; i++) {
					if (fields[i].name == "SUM_HE(salary)") {
						printf("|%10s", "sum");
					}
					else {
						printf("|%10s", fields[i].name);
					}
				}
				printf("|\n");
				

					//printf("executeQuery - Formatting 3\n");
				// Formatting
				for (i = 0; i < numberOfFields; i++) {
					printf("+----------");
				}
				printf("+\n");
				

					//printf("executeQuery - Formatting 4\n");
				// Outputting values
				while (row = mysql_fetch_row(result)) {
					countRows++;
					
						//printf("executeQuery/OV - Entering for loop...\n");
					for (i = 0; i < numberOfFields; i++) {
							//printf("executeQuery/OV/for - i: %d\n", i);
						printf("|");
	
						if (i == numberOfFields - 1) {
								//printf("executeQuery/OV/for - Decrypting value...\n");
							char *decryptedValue = decrypt(publicKey, privateKey, row[i]);
								//printf("executeQuery/OV/for - Value decrypted\n");
							printf("%10s|", decryptedValue);
						}
						else {
							printf("%10s", row[i]);
						}		
					}
					printf("\n");
				}

					//printf("executeQuery - Formatting 5\n");
				
				// Formatting
				for (i = 0; i < numberOfFields; i++) {
					printf("+---------");
				}
				printf("+\n");
				
				// Output number of rows
				printf("%d rows in set.\n\n", countRows);
				mysql_free_result(result);
			}
		}
		else { 
			fprintf(stderr, "eQ2 - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
		}
	}
}




int main (int argc, char **argv) {

	/* ESTABLISH MYSQL CONNECTION AND SHOW DATABASES (init) */	

	MYSQL_RES *res;
	MYSQL_ROW row;	

	connection = mysql_init(NULL);	

	if (mysql_real_connect(connection, "54.193.35.94", "root", "cs174$", "project", 0, NULL, 0) == NULL) {
		fprintf(stderr, "m0 - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
		mysql_close(connection);
		exit(1);	
	}
	else { printf("Connection succeeded.\n"); }

	if (connection == NULL) {
		fprintf(stderr, "m1 - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
		mysql_close(connection);
		exit(1);
	}

	if (mysql_query(connection, "use project")) {
		fprintf(stderr, "m2 - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
		mysql_close(connection);
		exit(1);
	}
	if (mysql_query(connection, "show tables")) {
		fprintf(stderr, "m3 - ERROR #%d - %s\n", mysql_errno(connection), mysql_error(connection));
		mysql_close(connection);
		exit(1);
	}

	res = mysql_use_result(connection);

	printf("Tables in database:\n");
	while ((row = mysql_fetch_row(res))) {
		printf("%s \n", row[0]);
	}

	mysql_free_result(res);

	/* PAILLIER STUFF */

	char *publicHex = "8d76d3e4f68d3a941f3c8da16d45096f";
	char *privateHex = "46bb69f27b469d494d836c782a77a658";

	paillier_pubkey_t *publicKey = paillier_pubkey_from_hex(publicHex);
	paillier_prvkey_t *privateKey = paillier_prvkey_from_hex(privateHex, publicKey);

	int cont = 1;

	do {
		char command[256];
		char *query;

		printf("Enter command: \n");
		fgets(command, 256, stdin);

		printf("Got command.\n");

		if (strncmp(command, "quit", 4) == 0) {
			cont = 0;
			printf("Quitting...\n");
		}
		else {
			printf("Reading command.\n");
			query = readQuery(command, publicKey, privateKey);
			printf("%s\n", query);
			executeQuery(query, publicKey, privateKey);
		}

	} while (cont);


	/* CLOSE CONNECTION */
	mysql_close(connection);
	return 0;
}

