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

/** Includes **/
#include "JSNSock.hpp"
#include <sys/select.h>
#include <sys/types.h>
#include <sstream>

/** Using **/
using namespace jsnSock;

/** Implementation **/


JSNSockTCP::JSNSockTCP(	/* Default Constructor */
	)
: JSNSockBase(domain::inet, type::stream, protocol::tcp)
{
}


JSNSockTCP::JSNSockTCP(	/* Constructor 1/2 */
	int	sockDesc,
	double	timeout
	)
{
    this->sockDesc	= sockDesc;
    domain		= domain::inet;
    type		= type::stream;
    protocol		= protocol::tcp;
    this->timeout	= timeout;
}

JSNSockTCP::JSNSockTCP(	/* Constructor 2/2 */
	const std::string	&host,
	uint16_t		port,
	double			timeout
	)
: JSNSockBase(domain::inet, type::stream, protocol::tcp, timeout)
{
    connect(host, port);
}

auto JSNSockTCP::select(
	)		-> void
{
	int selectSays;

	do
	{
	    struct timeval tv;
	    fd_set set;

	    tv.tv_sec	= (int)timeout;
	    tv.tv_usec	= (int)( (timeout * 1000) - (double)(tv.tv_sec *1000) ); /* regain precision lost after cast to int */
	    
	    FD_ZERO(&set);
	    FD_SET(sockDesc, &set);

	    if ((selectSays = ::select(sockDesc+1, NULL, &set, NULL, &tv)) == -1)
	    {
		if (errno == EINTR)
		{
		    throw JSNException("JSNSockTCP: connection interupted.");
		}
		else
		{
		    throw JSNException("JSNSockTCP: check timeout sanity and check file descriptors.");
		}
	    }
	    else if (selectSays) /* true if value greater than zero */
	    {
		break;
	    }
	    else
	    {
		throw JSNException("JSNSockTCP: connection timed-out.");
	    }
	}
	while (1);
}

auto JSNSockTCP::connect (
	const std::string	&host,
	uint16_t		port
	)			-> void
{
    struct sockaddr_in	sockAddr;

    sockAddr.sin_family		= domain;
    sockAddr.sin_port		= htons(port);
    sockAddr.sin_addr.s_addr	= inet_addr( getHostByName(host).c_str() );

    if (timeout == 0.0)
    {
	if ( ::connect(sockDesc, (struct sockaddr *) &sockAddr, sizeof(struct sockaddr)) == -1)
	{
	    throw JSNException("JSNSockTCP::connect : connection exception.");
	}
    }
    else
    {
	setBlocking(false);
	if ( ::connect(sockDesc, (struct sockaddr *) &sockAddr, sizeof(struct sockaddr)) == -1)
	{
	    if (errno == EAGAIN)
	    {
		select();
	    }
	    else
	    {
		JSNException("JSNSockTCP: connection exception.");
	    }
	}
	setBlocking(true);
    }
} /* JSNSockTCP::connect */

auto JSNSockTCP::send(
	const std::string	&buffer
	)			-> void
{
    if (timeout == 0.0)
    {
	if ( ::send(sockDesc, buffer.c_str(), buffer.length(), 0) == -1 )
	{
	    throw JSNException("JSNSockTCP: exception during attempt to send data.");
	}
    }
    else
    {
	setBlocking(false);
	if ( ::send(sockDesc, buffer.c_str(), buffer.length(), 0) == -1 )
	{
	    if (errno == EAGAIN)
	    {
		select();
	    }
	    else
	    {
		JSNException("JSNSockTCP: exception during an attempt to send data.");
	    }
	}
	setBlocking(true);
    }
} /* JSNSockTCP::send(const std::string &) */

auto JSNSockTCP::send(
	const void	*buffer,
	uint32_t	size
	)		-> void
{
    if (timeout == 0.0)
    {
	if ( ::send(sockDesc, buffer, size, 0) == -1 )
	{
	    throw JSNException("JSNSockTCP: exception during attempt to send binary data.");
	}
    }
    else
    {
	setBlocking(false);
	if ( ::send(sockDesc, buffer, size, 0) == -1 )
	{
	    if (errno == EAGAIN)
	    {
		select();
	    }
	    else
	    {
		JSNException("JSNSockTCP: exception during an attempt to send binary data.");
	    }
	}
	setBlocking(true);
    }
} /* JSNSockTCP::send(const void *buffer, uint32_t size) */

auto JSNSockTCP::recv(
	uint32_t	size
	)		-> std::string
{
    std::unique_ptr<char>	buffer(new char[size]);

    if (timeout == 0.0)
    {
	if ( ::recv(sockDesc, buffer.get(), size, 0) == -1)
	{
	    throw JSNException("JSNSockTCP: recv (string) exception.");
	}
    }
    else
    {
	setBlocking(false);
	if ( ::recv(sockDesc, buffer.get(), size, 0) == -1)
	{
	    if (errno == EAGAIN)
	    {
		select();
	    }
	    else
	    {
		throw JSNException("JSNSockTCP: recv (string) exception.");
	    }
	}
	setBlocking(true);
    }

    return (buffer.get()); /* may be empty */
} /* JSNSockTCP::recv (uint32_t size) -> std::string */

auto JSNSockTCP::recv(
	void		*buffer,
	uint32_t	size
	)		-> void
{
    int32_t bytesReceived;

    if (timeout == 0.0)
    {
	if ( (bytesReceived = ::recv(sockDesc, buffer, size, 0)) == -1)
	{
	    throw JSNException("JSNSockTCP: recv exception.");
	}
    }
    else
    {
	setBlocking(false);
	if ( (bytesReceived = ::recv(sockDesc, buffer, size, 0)) == -1)
	{
	    if (errno == EAGAIN)
	    {
		select();
	    }
	    else
	    {
		throw JSNException("JSNSockTCP: recv exception.");
	    }
	}
	setBlocking(true);
    }

    if (!bytesReceived)
	buffer = nullptr;
} /* JSNSockTCP::recv (void *buffer, uint32_t size) -> void */

auto JSNSockTCP::readline(
	)		-> std::string
{
    bool caughtEOF	= false;
    bool caughtEOL	= false;
    std::string	line;

    while (!caughtEOL && !caughtEOF)
    {
	char		buffer;
	uint32_t	bytesReceived;

	bytesReceived = ::recv(sockDesc, &buffer, 1, 0);
	if (bytesReceived < 1)
	    caughtEOF = true;
	else if (buffer == '\n')
	    caughtEOL = true;
	else if (buffer != '\r')
	    line += buffer;
    }

    if (line.empty())
	(caughtEOF) ? line = "" : line = "\r";

    return line;
} /* JSNSockTCP::readline() -> std::string */

template<class T>
auto JSNSockTCP::operator<< (
	const T 	&buffer
	)		-> void
{
    std::stringstream sstream(std::stringstream::in | std::stringstream::out);
    sstream << buffer;
    send (sstream.str());
}

auto JSNSockTCP::operator<< (
	const std::string	&buffer
	)			-> void
{
    send (buffer);
}

auto JSNSockTCP::operator>> (
	std::string		&buffer
	)			-> void
{
    buffer = recv();
}
