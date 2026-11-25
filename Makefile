# Ścieżka do katalogu z podprojektem (gdzie znajduje się drugi Makefile)
SUBDIR1 := ./przem-low-shelf
SUBDIR2 := przem-tone

# Katalog docelowy instalacji (domyślnie /usr/local/bin, można nadpisać np. make install PREFIX=/opt/app)
PREFIX ?= /usr/lib64/ladspa
BINDIR := /usr/lib64/ladspa

# Nazwa skompilowanego pliku binarnego
TARGET1 := przem-low-shelf
TARGET2 := przem-tone

# Deklaracja celów pozornych (niebędących plikami)
.PHONY: all build install clean

# Domyślny cel
all: build

# Wywołanie drugiego Makefile w celu kompilacji
build:
	$(MAKE) -C $(SUBDIR1)
	$(MAKE) -C $(SUBDIR2)

# Instalacja skompilowanego pliku w systemie
install: build
	@echo "Instalowanie $(TARGET1) w $(BINDIR)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 $(SUBDIR1)/*.so $(DESTDIR)$(BINDIR)
	install -m 0755 $(SUBDIR2)/*.so $(DESTDIR)$(BINDIR)

# Przekazanie czyszczenia do podkatalogu
clean:
	$(MAKE) -C $(SUBDIR1) clean
	$(MAKE) -C $(SUBDIR2) clean
