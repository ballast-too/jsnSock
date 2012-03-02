# jsnSock library Makefile
# CLANG_PATH=/usr/local/bin/clang++
#

SRCDIR=src
INCLUDEDIR=include
PREFIX=/usr/local
LIB=jsnsock

ifdef CLANG_PATH # will create a .dylib
	lib_name_long = lib$(LIB).1.dylib
	lib_name_short = lib$(LIB).dylib
    	CPP_DRIVER=$(CLANG_PATH)
    	OPTS=-std=c++0x -stdlib=libc++
	LINKER = ld *.o -dylib -undefined dynamic_lookup -v -o $(lib_name_long)
else
	lib_name_long = lib$(LIB).so.1.0.0
	lib_name_short = lib$(LIB).so.1
	CPP_DRIVER=g++
	OPTS=-Wall -ansi -pedantic -pedantic-errors -fPIC
	LINKER = $(CPP_DRIVER) -shared -Wl,-soname,$(lib_name_short) -o $(lib_name_long) *.o
	ldconfig = ldconfig
endif

all:
	@echo using $(CPP_DRIVER)...
	$(CPP_DRIVER) $(OPTS) -I$(INCLUDEDIR) -g -c $(SRCDIR)/*.cpp

	$(LINKER)

	ar rcs lib$(LIB).a *.o

install:
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/$(INCLUDEDIR)
	install -m 0644 lib$(LIB).a $(PREFIX)/lib
	install -m 0755 $(lib_name_long) $(PREFIX)/lib
	ln -sf $(PREFIX)/lib/$(lib_name_long) $(PREFIX)/lib/$(lib_name_short)
	install -m 0644 $(INCLUDEDIR)/*.hpp $(PREFIX)/$(INCLUDEDIR)
	$(ldconfig)

clean:
	rm *.o
	rm lib$(LIB).a
	rm $(lib_name_long)

uninstall:
	rm $(PREFIX)/lib/$(lib_name_short)
	rm $(PREFIX)/lib/$(lib_name_long)
	rm $(PREFIX)/lib/lib$(LIB).a
	rm $(PREFIX)/$(INCLUDEDIR)/usock.hpp
	rm $(PREFIX)/$(INCLUDEDIR)/usock_exception.hpp
