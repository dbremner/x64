# SPTRCBOL makefile using tccSE
host?=unix_64
HOST=$(host)

DEBUG:=$(debug)

nasm?=nasm

debug?=0

os?=unix
OS:=$(os)

ws?=64
WS=$(ws)

asm?=as
ASM=$(asm)


TARGET=$(OS)_$(WS)

trc?=0
TRC:=$(trc)
ifneq ($(TRC),0)
TRCOPT:=:trc
TRCDEF:=-Dzz_trace
endif

basebol?=./bin/unix_64
BASEBOL=$(basebol)

cc?=gcc
CC:=$(cc)

ifeq	($(DEBUG),1)
GFLAG=-g
endif

ARCH=-D$(TARGET)  -m$(WS)

CCOPTS:= $(TRCDEF) $(GFLAG) 
LDOPTS:= $(GFLAG)
LMOPT:=-lm

ifeq ($(OS),unix)
ELF:=elf$(WS)
else
EL:F=macho$(WS)
endif


OSINT=./osint

vpath %.c $(OSINT)

# Assembler info -- Intel 64-bit syntax
ifeq	($(DEBUG),0)
ASMOPTS =  -D$(TARGET) $(TRCDEF)
else
ASMOPTS = -g  -D$(TARGET) $(TRCDEF)
endif

OSXOPTS = -f macho64 -Dosx_64 $(TRCDEF)
# tools for processing Minimal source file.

# implicit rule for building objects from C files.
./%.o: %.c
#.c.o:
	$(CC)  $(CCOPTS) -c  -o$@ $(OSINT)/$*.c

unix_64:
	$(CC) -Dunix_64 -m64 $(CCOPTS) -c osint/*.c
	./bin/sbl_unix_64 -u unix_64 lex.sbl
	./bin/sbl_unix_64 -r -u unix_64:$(TRCOPT) -1=sbl.lex -2=sbl.tmp -3=sbl.err -4=sbl.equ asm.sbl
	./bin/sbl_unix_64 -u unix_64 -1=sbl.err -2=err.s err.sbl
#	cat sys.asm err.s sbl.tmp >sbl.s
	cat sys.asm err.s sbl.tmp >sbl.s
	$(ASM) -o sbl.o sbl.s
	$(CC) -lm -Dunix_64 -m64 $(LDOPTS)  *.o -lm  -osbl 

osx_64:
	$(CC) $(CCOPTS) -c osint/*.c
	$(BASEBOL)  -u osx_64 lex.sbl
	$(BASEBOL)  -r -u osx_64:$(TRCOPT) -1=sbl.lex -2=sbl.tmp -3=sbl.err asm.sbl
	$(BASEBOL)  -u osx_64 -1=sbl.err -2=err.s err.sbl
	cat sys.asm err.s sbl.tmp >sbl.s
	$(ASM) -f macho64 -Dosx_64 -o sbl.o sbl.s
	$(CC) -lm -Dosx_64 -m64 $(LDOPTS)  *.o -lm  -osbl 

# link spitbol with dynamic linking
spitbol-dynamic: $(OBJS) $(NOBJS)
	$(CC) $(LDOPTS) $(OBJS) $(NOBJS) $(LMOPT)  -osbl 

# bootbol is for bootstrapping just link with what's at hand
#bootbol: $(OBJS)
# no dependencies so can link for osx bootstrap
bootbol: 
	$(CC) $(LDOPTS)  $(OBJS) $(LMOPT) -obootbol

osx-export: 
	
	cp sbl.s  osx/sbl.s
	$(ASM) -Dosx_64 -f macho64 -o osx/sbl.o osx/sbl.s

osx-import: 
	gcc -arch i386 -c osint/*.c
	cp osx/*.o .
	gcc -arch i386  -o sbl *.o

# install binaries from ./bin as the system spitbol compilers
install:
	sudo cp ./bin/sbl /usr/local/bin
.PHONY:	clean
clean:
	rm -f $(OBJS) *.o s.lex s.tmp err.s sbl.s ./sbl sbl.lex sbl.tmp sbl_unix_64 tb*

z:
	nm -n s.o >s.nm
	sbl map-$(WS).sbl <s.nm >s.dic
	sbl z.sbl <ad >ae

sclean:
# clean up after sanity-check
	make clean
	rm tbol*

test_unix_64:
# Do a sanity test on spitbol to  verify that spitbol is able to compile itself.
# This is done by building the system three times, and comparing the generated assembly (.s)
# filesbl. Normally, all three assembly files wil be equal. However, if a new optimization is
# being introduced, the first two may differ, but the second and third should always agree.
#
	rm -f tbol.*
	echo "start 64-bit sanity test"
	cp	./bin/sbl_unix_64 .
	gcc -Dunix_64 -m64 -c osint/*.c
	./sbl_unix_64 -u unix_64 lex.sbl
	./sbl_unix_64 -r -u unix_64: -1=sbl.lex -2=sbl.tmp -3=sbl.err -4=sbl.equ asm.sbl
	./sbl_unix_64 -u unix_64 -1=sbl.err -2=err.s err.sbl
	cat sys.asm err.s sbl.tmp >sbl.s
	as -Dunix_64 -o sbl.o sbl.s
	gcc -lm -Dunix_64 -m64 $(LDOPTS)  *.o -lm  -osbl_unix_64
	mv sbl.lex tbol.lex.0
	mv sbl.s tbol.s.0
	gcc -Dunix_64 -m64 -c osint/*.c
	./sbl_unix_64 -u unix_64 lex.sbl
	./sbl_unix_64 -r -u unix_64: -1=sbl.lex -2=sbl.tmp -3=sbl.err -4=sbl.equ asm.sbl
	./sbl_unix_64 -u unix_64 -1=sbl.err -2=err.s err.sbl
	cat sys.asm err.s sbl.tmp >sbl.s
	as -Dunix_64 -o sbl.o sbl.s
	gcc -lm -Dunix_64 -m64 $(LDOPTS)  *.o -lm  -osbl_unix_64 
	mv sbl.lex tbol.lex.1
	mv sbl.s tbol.s.1
	gcc -Dunix_64 -m64 -c osint/*.c
	./sbl_unix_64 -u unix_64 lex.sbl
	./sbl_unix_64 -r -u unix_64: -1=sbl.lex -2=sbl.tmp -3=sbl.err -4=sbl.equ asm.sbl
	./sbl_unix_64 -u unix_64 -1=sbl.err -2=err.s err.sbl
	cat sys.asm err.s sbl.tmp >sbl.s
	as -Dunix_64 -o sbl.o sbl.s
	gcc -lm -Dunix_64 -m64 $(LDOPTS)  *.o -lm  -osbl_unix_64
	mv sbl.lex tbol.lex.2
	mv sbl.s tbol.s.2
	echo "comparing generated .s files"
	diff tbol.s.1 tbol.s.2
	echo "end sanity test"
	
