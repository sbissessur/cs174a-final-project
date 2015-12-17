all:
	gcc queries.c -I/usr/include/mysql -lmysqlclient -lgmp -lpaillier -o client -lm

udf:
	gcc -fPIC -I /usr/include/mysql -shared -o sum_he.so SUM_HE.c libpaillier-0.8/paillier.c -lgmp
	sudo cp sum_he.so /usr/lib/mysql/plugin
	rm sum_he.so

keys:
	gcc generateKeys.c -lpaillier -o generateKeys -lgmp
