 /** jsnSock - A High-Level Network (TCP) Sockets Library
 *
 * The files in this directory and elsewhere which refer to this LICENCE
 * file are part of jsnSock -- a network sockets library in c++11 syntax
 *
 * Copyright (C) 2012 Jason Browning, <z.jason.browning@gmail.com>
 *
 * jsnSock is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 3 or (at your option) any later 
 * version.
 *
 * jsnSock is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with jsnSock; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02111-1301 USA.
 */

#ifndef __cplusplus
#error "The JSNSock library is written using C++11"
#endif 

#ifndef _JSNSock_HPP_
#define _JSNSock_HPP_
#define JSN_RECVBUF_SIZE 1024
#define ADDR_STR_LEN 46
#define JSN_CONNECT_MAX 8
/* Includes */
#include "jsnSock_Prefix.hpp"
#include <arpa/inet.h> /* includes <sys/socket.h> and <netinet/in.h> */
#include <string>
#include <signal.h>
#include "JSNException.hpp"

/* Interface Declaration */
namespace jsnSock
{
    class JSNSockBase
    {
	protected:
	    int		sockDesc;
	    uint32_t	domain;		/* uses <sys/socket.h> */
	    uint32_t	type;		/* uses <sys/socket.h> */
	    uint32_t	protocol;	/* uses <netinet/in.h> */

	    double	timeout;	/* applies to each of connect, send, and recv */


	    JSNSockBase() = default;	/* Explicit 'default' causes system to generate	*/
	    				/* a default constructor despite the existence	*/
	    				/* of other constructors.			*/
	    				/* It is 'protected' because it is only for use */
	    				/* by inherited classes that declare their own	*/
	    				/* 'sockDesc'.					*/
	    
	    /* A 2-element array to hold socket address/port information (in a std::pair struct) */
	    std::pair<std::string, uint16_t>	socketInfoArray[2];
	public:
	    /* Explicitly typed 'enum' declarations must	*/
	    /* be explicitly scoped during implementationr.	*/

	    enum target : uint32_t	/* uses <netinet/in.h> */
	    {
		any		= INADDR_ANY,
		broadcast	= INADDR_BROADCAST,
		loopback	= INADDR_LOOPBACK,
		none		= INADDR_NONE
	    }; /* TARGET */

	    enum domain : uint32_t
	    {
		inet		= AF_INET,
		inet6		= AF_INET6
	    };

	    enum type : uint32_t
	    {
		stream		= SOCK_STREAM,
		datagram	= SOCK_DGRAM,
		raw		= SOCK_RAW,
		reliableMsg	= SOCK_RDM,
		sequentialPak	= SOCK_SEQPACKET
	    };

	    enum protocol : uint32_t
	    {
		ip		= IPPROTO_IP,
		tcp		= IPPROTO_TCP,
		udp		= IPPROTO_UDP,
		rawp		= IPPROTO_RAW,
		icmp		= IPPROTO_ICMP
	    };

	    /* Non-Default Constructor 1/1 */
	    JSNSockBase (uint32_t	domain,
		    	 uint32_t	type,
			 uint32_t	protocol = 0, /* zero default tells socket lib to set an appropriate protocol. */
			 double		timeout = 0.0
			);

	    ~JSNSockBase();
	    
	    /** JSNSockBase Function Declarations **/
	    auto close () 						-> void;

	    /* Get/Set Socket Options */
	    auto sockOption(int level, int optname, void *optval, 
		    				socklen_t *optlen)	-> void;
	    auto setSockOption(int level, int optname, void *optval,
		    				socklen_t optlen)	-> void;

	    /* Set the Timeout interval */
	    auto setTimeout(double timeout = 0.0) 			-> void;

	    /* Socket Information Handlers */
	    auto ntoa(in_addr_t addr)					-> std::string;
	    auto socketInfo()						-> void;
	    
	    /* Socket Information Accessor in-line methods */
	    auto remoteAddr()						-> std::string
	    { return socketInfoArray[0].first;}
	    auto remotePort()						-> uint16_t
	    { return socketInfoArray[0].second;}
	    auto localAddr()						-> std::string
	    { return socketInfoArray[0].first;}
	    auto localPort()						-> uint16_t
	    { return socketInfoArray[1].second;}

	    /* Blocking */
	    auto setBlocking(bool on=true)				-> void;
	    auto isBlocking()						-> bool;

	    /* Get Host by Name/Address */
	    auto getHostByName(const std::string &name)			-> std::string;
	    auto getHostByAddr(const std::string &addr)			-> std::string;


    }; /* JSNSockBase class */
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



    /* JSNSockTCP
     * Create a TCP Socket
     */
    class JSNSockTCP : public JSNSockBase
    {
	protected:
	    auto select ()				 		-> void;

	public:
	    JSNSockTCP();
	    
	    JSNSockTCP(int sockDesc, double timeout = 0.0);
	    JSNSockTCP(const std::string &host, uint16_t port, double timeout = 0.0);



	    /** Methods **/
	    auto connect (const std::string &host, uint16_t port) 	-> void;

	    /* Sending Data via TCP */
	    auto send (const std::string &buffer) 			-> void; // text data.
	    auto send (const void *buffer, uint32_t size) 		-> void; // binary data.

	    /* Receiving Data via TCP */
	    auto recv (uint32_t size = JSN_RECVBUF_SIZE) 		-> std::string;
	    auto recv (void *buffer, uint32_t size) 			-> void;
	    auto readline () -> std::string;

	    /* TCP Send Overloaded Operators */
	    template<class T>
		auto operator<< (const T &buffer)			-> void;
	    auto operator<< (const std::string &buffer) 		-> void;

	    /* TCP Receiver Overloaded Operator */
	    auto operator>> (std::string &buffer) 			-> void;
    }; /* JSNSockTCP */
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



    /* JSNSockTCPServer
     * Craft a socket for use as a TCP Server
     */
    class JSNSockTCPServer : public JSNSockTCP
    {
	private:
	    uint32_t		connection_max;
	public:

	    auto bind (
		    	uint16_t port,
			const std::string &address = "",
			uint32_t capacity = JSN_CONNECT_MAX
			)						-> void;
	    auto listen ()						-> void;
	    auto accept ()						-> JSNSockTCP;
	    auto accept ( void (*handler)(JSNSockTCP &socket) )		-> bool;
    };
} /* namespace jsnSock */
#endif
