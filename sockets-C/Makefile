#
# Internetworking with TCP/IP, Volume III example code Makefile
#
#	David L Stevens, Internetworking Research Group at Purdue
#	Tue Sep 17 19:40:42 EST 1991
#

INCLUDE =

#CLNTS = TCPecho TCPdaytime TCPtecho UDPecho UDPtime UDPtime1
#SERVS = TCPdaytimed TCPechod TCPmechod UDPtimed daytimed UDPtimed1
CLNTS = UDPecho UDPmath TCPfile
SERVS = UDPechod UDPmathd TCPfiled
OTHER = superd TCPmtfiled

DEFS =
CFLAGS = -W -pedantic -ansi -g ${DEFS} ${INCLUDE}

HDR =
CSRC = UDPecho.c UDPmath.c TCPtfile.c TCPfile.c
CXSRC = connectsock.c errexit.c

SSRC = UDPechod.c UDPmathd.c TCPmfiled.c TCPmtfiled.c TCPfiled.c \
 superd.c
SXSRC = passivesock.c sv_funcs.c

CXOBJ = connectsock.o errexit.o
SXOBJ = passivesock.o errexit.o

PROGS = ${CLNTS} ${SERVS} ${OTHER}

all: ${PROGS}

${CLNTS}: ${CXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${CXOBJ}

${SERVS}: ${SXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ} -lm

superd: ${SXOBJ} superd.o sv_funcs.o
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ} sv_funcs.o

TCPmtfiled: TCPmtfiled.o ${SXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ} -lpthread

clients: ${CLNTS}
servers: ${SERVS}

clean: FRC
	rm -f Makefile.bak a.out core errs lint.errs ${PROGS} *.o

depend: ${HDR} ${CSRC} ${SSRC} ${TNSRC} FRC
	maketd -a ${DEFS} ${INCLUDE} ${CSRC} ${SSRC} ${TNSRC}

install: all FRC
	@echo "Your installation instructions here."

lint: ${HDR} ${XSRC} ${CSRC} ${SSRC} FRC
	lint ${DEFS} ${INCLUDE} ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}

print: Makefile ${SRC} FRC
	lpr Makefile ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}

spotless: clean FRC
	rcsclean Makefile ${HDR} ${SRC}

tags: ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}
	ctags ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}

${HDR} ${CSRC} ${CXSRC} ${SSRC} ${SXSRC}:
	co $@

UDPecho: UDPecho.o
UDPmath: UDPmath.o
TCPfile: TCPfile.o
TCPtfile: TCPtfile.o
TCPmfiled: TCPmfiled.o
UDPechod: UDPechod.o
UDPmathd: UDPmathd.o
TCPfiled: TCPfiled.o
TCPmtfiled: TCPmtfiled.o
superd: superd.o

FRC:
	
# DO NOT DELETE THIS LINE - maketd DEPENDS ON IT
