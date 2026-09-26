coma: coma.c
	cc coma.c -ledit -o coma

install: coma
	install -m 755 coma /bin/coma
