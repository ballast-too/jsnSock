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
	$(CPP_DRIVER) $(LINKER_ARGS) -o hello_client hello_client.cpp
	$(CPP_DRIVER) $(LINKER_ARGS) -o hello_server hello_server.cpp

clean:
	rm hello_client
	rm hello_server
