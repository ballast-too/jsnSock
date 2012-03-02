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

/* Include */
#include "JSNSock.hpp"	/* brings <arpa/in.h> & <signal.h> */
#include <thread>
#include <unistd.h>	/* used for 'close(fd)' method */
#include <iostream>

/* Using */
using namespace jsnSock;

/* Implementation */
auto JSNSockTCPServer::bind(
	uint16_t		port,
	const std::string	&address,
	uint32_t		capacity
	)			-> void
{
    struct sockaddr_in sockAddr;
    connection_max		= capacity;
    sockAddr.sin_family		= domain;
    sockAddr.sin_port		= htons(port);

    if ( (inet_aton(getHostByName(address).c_str(), &sockAddr.sin_addr) == 0) )
	sockAddr.sin_addr.s_addr = any;

    std::cerr << "Binding socket descriptor (" << sockDesc << ") to port (" << port << ")..." << std::endl;
    if ( ::bind(sockDesc, (struct sockaddr*) &sockAddr, sizeof(struct sockaddr)) == -1)
	throw JSNException("JSNSockTCPServer: bind exception.");
}

auto JSNSockTCPServer::listen(
	)			-> void
{
    std::cerr<< "Listening." << std::endl;
    if ( ::listen(sockDesc, connection_max) == -1)
	throw JSNException("JSNSockTCPServer: listen exception.");
}

auto JSNSockTCPServer::accept(
	)			-> JSNSockTCP
{
    int 		peerSockDesc;
    struct sockaddr_in	sAddr;
    socklen_t		size = sizeof(struct sockaddr);

    do
    {
	if ( (peerSockDesc = ::accept(sockDesc, (struct sockaddr *) &sAddr, &size)) == -1)
	    throw JSNException("JSNSockTCPServer: accept exception.");
    } while (peerSockDesc <= 0);

    return JSNSockTCP(peerSockDesc);
}

auto JSNSockTCPServer::accept (
	void 			(*handler)(JSNSockTCP &socket)
	)			-> bool
{
    bool		result = false;
    std::thread		thread;

    auto lambda_accept = [&handler, this](
	    )
    {
	JSNSockTCP spawn_socket = accept();
	handler(spawn_socket);
	spawn_socket.close();
    }; /* lambda_accept */

    std::cerr << "Accepting connections..." << std::endl;
    thread = std::thread(lambda_accept);

    if (thread.joinable())
    {
	thread.join();
	result = true;
    }
    else
	throw JSNException("JSNSockTCPServer : spawn-accept exception.");

    return result;
}
