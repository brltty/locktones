CC = cc
CCFLAGS = -O

LD = $(CC)
LDFLAGS =

INSTALL=install
OWNER=root
GROUP=root
BINMODE=0755
TXTMODE=0644

BINDIR=/usr/local/sbin

RM=rm

Executable = locktones
Objects = lktMain.o lktUnix.o lktLinux.o lktGopt.o lktGoptw.o
Libs =

include ../common.mak

install: $(Executable)
	$(INSTALL) -o $(OWNER) -g $(GROUP) -m $(BINMODE) $(Executable) $(BINDIR)/$(Executable)

redhat: install
	$(INSTALL) -o $(OWNER) -g $(GROUP) -m $(BINMODE) sysv/rc.script /etc/rc.d/init.d/$(Executable)
	$(INSTALL) -o $(OWNER) -g $(GROUP) -m $(TXTMODE) sysv/rc.config /etc/sysconfig/$(Executable)
	chkconfig --add $(Executable)
	chkconfig $(Executable) reset

uninstall:
	$(RM) -f $(BINDIR)/$(Executable)

