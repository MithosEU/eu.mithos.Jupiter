CC = gcc
PKGFLAGS = $(shell pkg-config --libs --cflags gtk4 wayland-client gtk4-wayland)
CFLAGS = -g -Wall -Wextra -rdynamic
SOURCES = $(wildcard src/*.c)
DATA = data
PREFIX = /usr/local

all:
	mkdir -p build/share/jupiter/icons/hicolor/scalable/actions
	mkdir -p build/bin/
	
	gtk4-builder-tool validate $(DATA)/*.ui
	
	$(CC) $(SOURCES) $(CFLAGS) $(PKGFLAGS) -o build/bin/eu.kosy.Jupiter
	
	cp $(DATA)/*.ui build/share/jupiter/
	cp $(DATA)/icons/*.svg build/share/jupiter/icons/hicolor/scalable/actions
	cp $(DATA)/twoModernExtra.css build/share/jupiter/twoModernExtra.css

run:
	build/bin/eu.kosy.Jupiter
	@echo ""

clean:
	rm -rf build
