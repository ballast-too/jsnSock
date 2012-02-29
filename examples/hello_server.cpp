/**
 * An elementary TCP server
 *
 * It listens for connections on the given part; whenever a connection is requested
 * from a client, it spawns a new process and processes the connection via a transparent 
 * callback function.
 *
 * (C) 2012 Jason Browning
 */

#include <iostream>
#include <JSNSock.hpp>

using namespace std;
using namespace jsnSock;

void callback_func (JSNSockTCP& s)  {
        s.socketInfo();
	cout << "Connection from " << s.remoteAddr() << endl;
	s << "Hello " + s.remoteAddr() + "\n";
	s.close();
}

int main(int argc, const char *argv[])  {

    if (argc != 2)
    {
	cout << "Usage: " << argv[0] << " port" << endl;
	exit(-1);
    }

    cout << "Args: " << argv[0] << ", " << argv[1] << endl;
    JSNSockTCPServer ss;

    try {
	ss.bind(atoi(argv[1]));
	ss.listen();
    }
    catch (JSNException e) {
	cerr << "Exception Occurred during initialization: " << e.what() << endl;
    }

    while(1)  {
	    try  {
		    bool result = ss.accept(callback_func);
/*		    JSNSockTCP peer = ss.accept();
		    foo(peer);
*/	    }

	    catch (JSNException e)  {
		    cerr << "Exception Occurred while accepting/handling a connection: " << e.what() << endl;
	    }
    }

    ss.close();
}

