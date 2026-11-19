
INSTALL_PLUGINS_DIR	=	/usr/local/lib/ladspa/

CC	= gcc
CFLAGS	= -I. -O3 -Wall -fomit-frame-pointer -fstrength-reduce -funroll-loops -ffast-math -c -fPIC -DPIC
LDFLAGS	= -nostartfiles -shared -Wl,-Bsymbolic -lc -lm -lrt
MODULES = $(wildcard *.c)

all: $(MODULES:%.c=%.so)

przem-low-shelf.o: przem-low-shelf.h

%.o: %.c ladspa.h
	$(CC) $(CFLAGS) $< -o $@

%.so: %.o
	$(CC) -o $@ $< $(LDFLAGS)

install: all
	-mkdir -p          $(INSTALL_PLUGINS_DIR)
	cp *.so            $(INSTALL_PLUGINS_DIR)

clean:
	-rm -f *.so *.o *~

.PHONY: all install clean
