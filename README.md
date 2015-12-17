# CS 174A (F15) - Database Final Project: Cloud Paillier Cryptosystem
#### Grade: Passed all but 2 final tests.
##### Sahil Bissessur (SBissessur) and Vincent Chang (Styxx)

## Server Setup:
1. SSH into AWS instance
2. Edit config file: `$ sudo vim /etc/mysql/my.cnf`
3. Change bind-address to server's private IP
4. Restart MySQL service: `$ sudo service mysql restart`
   * If it's taking too long to restart, make sure you have the correct IP
5. Compile UDF: `$ make udf`
6. Login to MySQL terminal: `$ mysql -u root -p`
7. Initialize database: `mysql> source initialize.sql`
8. Use correct database: `mysql> use project;`
9. Create function: `CREATE AGGREGATE FUNCTION SUM_HE RETURNS STRING SONAME 'sum_he.so'`;
10. Give client access: `GRANT ALL ON project.\* TO root@<client public IP> IDENTIFIED BY 'cs174$'`;



## Client Setup:
1. Run on local machine (or SSH into a separate AWS instance from server).
2. Install GMP and paillier library if not done beforehand.
   * Paillier: acsc.cs.utexas.edu/libpaillier/
   * GMP: https://gmplib.org/
3. Change connection IP to server public IP in `queries.c`.
4. Compile and run client:
   * `make`
   * `./client`


## Running the code
* Client code:
    * `make` to compile
    * `./client` to run client
* UDF:
    * `make udf` to compile and setup UDF code
    * Go through server setup to ensure updated function created and loaded into MySQL database
* Keys:
    * `make keys` to compile `generateKeys.c`
    * `./generateKeys` to generate new Paillier keys

## UDF problems
* If the UDF isn't properly set, all queries with `SUM` will result in problems.
* Make sure to go through proper server setup.
* If you have an error (often #2013, #2006, or something similar:)
    * Log into mysql server on server side: `mysql -u root -p`
    * `mysql> SELECT * FROM mysql.func`
        * You'll see the `SUM_HE` function.
    * `mysql> DELETE FROM project WHERE name='SUM_HE'`
    * Recompile UDF. Redo server setup starting from step 6

## Current known bugs
* `SELECT SUM` on an empty database will result in a bad query.
* `SELECT [int]` will result in a bad query
    * This is due to a hotfix in the else-if conditional on line 172 in `queries.c`. The current code no longer takes into account a query with 2 words without the `SUM` word.

## Files submitted:
* `queries.c`
* `SUM_HE.c`
* `libpaillier-0.8/`
* `Makefile`
* `initialize.sql`
* `paillierKeys.txt`
* `joogKey.pem`
* `README.md` (older version)
* `gccCommands.txt` (integrated into Makefile)
* `generateKeys.c`
