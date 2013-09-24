PROG=	main
SRCS=	main.c plugin.c plugin_net.c plugin_pmc.c
NO_MAN=	yes
LDADD=	-lpmc -lkvm -lm

CFLAGS+= -I${.CURDIR}/../include/ -Wall -Wunused -O -g

.include <bsd.prog.mk>
