# Examples Makefile
# CLANG_PATH=/usr/local/bin/clang++
# 
ifdef CLANG_PATH
	CPP_DRIVER=$(CLANG_PATH) -std=c++0x -stdlib=libc++ -v
    	LINKER_ARGS=-Wl,-t,-ljsnsock
else  # assume g++
	CPP_DRIVER=g++
	LINKER_ARGS=-ljsnsock
endif

all:
	$(CPP_DRIVER) $(LINKER_ARGS) -o tcp_client tcp_client.cpp
	$(CPP_DRIVER) $(LINKER_ARGS) -o tcp_server tcp_server.cpp

clean:
	rm tcp_client
	rm tcp_server
