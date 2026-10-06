PROG=coma
SRCS=coma.c

U!=uname -s
.if ${U}=="Linux"
LDADD+= -lbsd
.endif
LDADD+= -ledit


afterinstall:
	${INSTALL} -d ${DESTDIR}/usr/share/man/man1
	${INSTALL} -m 644 coma.1 ${DESTDIR}/usr/share/man/man1/coma.1

.include <bsd.prog.mk>
