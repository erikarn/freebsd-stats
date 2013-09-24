PROG=	main
SRCS=	main.c plugin.c plugin_net.c pmcstat.c plugin_pmc.c
NO_MAN=	yes
LDADD=	-lpmc -lkvm -lm

CFLAGS+= -I${.CURDIR}/../include/ -Wall -Wunused

.include <bsd.prog.mk>
