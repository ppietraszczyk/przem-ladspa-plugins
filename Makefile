# Ścieżka do katalogu z podprojektem (gdzie znajduje się drugi Makefile)
SUBDIR := ./przem-low-shelf

# Katalog docelowy instalacji (domyślnie /usr/local/bin, można nadpisać np. make install PREFIX=/opt/app)
PREFIX ?= /usr/lib64/ladspa
BINDIR := /usr/lib64/ladspa

# Nazwa skompilowanego pliku binarnego
TARGET := przem-low-shelf

# Deklaracja celów pozornych (niebędących plikami)
.PHONY: all build install clean

# Domyślny cel
all: build

# Wywołanie drugiego Makefile w celu kompilacji
build:
	$(MAKE) -C $(SUBDIR)

# Instalacja skompilowanego pliku w systemie
install: $(TARGET)
	@echo "Instalowanie $(TARGET) w $(BINDIR)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 $(SUBDIR)/*.so $(DESTDIR)$(BINDIR)

# Przekazanie czyszczenia do podkatalogu
clean:
	$(MAKE) -C $(SUBDIR) clean
