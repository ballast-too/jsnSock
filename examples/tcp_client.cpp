/**
 * This is an elementary TCP client
 * It connects to a server on 'localhost' at the given port and reads a string from the socket
 * (C) 2012 Jason Browning
 */

#include <iostream>
#include <JSNSock.hpp>

using namespace std;
using namespace jsnSock;

int main(int argc, char *argv[])  {

    	if (argc != 2)
	{
	    cout << "Usage: " << argv[0] << " port" << endl;
	    exit(-1);
	}

	try
	{
	    JSNSockTCP s("localhost",atoi(argv[1]));
	    string line = s.readline();
	    cout << "The server sent: " << line << endl;
	    s.close();
	}
	catch (JSNException e)
	{
	    cerr << argv[0] << ": Exception occurred : " << e.what() << endl;
	}
}

