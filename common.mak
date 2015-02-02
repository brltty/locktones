VPATH = ..
Makefiles = Makefile ../common.mak

.c.o:
	$(CC) $(CCFLAGS) -o $@ -c $<

all: $(Executable)

$(Executable): $(Objects) $(Makefiles)
	$(LD) $(LDFLAGS) -o $@ $(Objects) $(Libs)

lktMain.o: lktMain.c lktMain.h $(Makefiles)

lktGopt.o: lktGopt.c lktGopt.h lktMain.h $(Makefiles)

lktGoptc.o: lktGoptc.c lktGopt.h lktMain.h $(Makefiles)

lktGoptw.o: lktGoptw.c lktGopt.h lktMain.h $(Makefiles)

lktUnix.o: lktUnix.c lktUnix.h lktMain.h $(Makefiles)

lktLinux.o: lktLinux.c lktUnix.h lktMain.h $(Makefiles)

lktDjgpp.o: lktDjgpp.c lktDjgpp.h lktMain.h $(Makefiles)

clean:
	rm -f $(Executable) $(Objects)

