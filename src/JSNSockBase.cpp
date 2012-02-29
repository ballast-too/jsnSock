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

/* Includes */
#include "JSNSock.hpp"
#include <fcntl.h>	/* used for the 'fcntl' method and associated constants */
#include <netdb.h>	/* used for the 'hostent' struct in 'getHostBy...' methods */
#include <unistd.h>	/* used for the 'close(2)' method */

/* Using */
using std::string;
using namespace jsnSock;

/* Implementation */
JSNSockBase::JSNSockBase(
	uint32_t	domain,
	uint32_t	type,
	uint32_t	protocol,
	double		timeout
)
{
    /* Generate a new socket descriptor with the given parameters */
    this->domain	= domain;
    this->type		= type;
    this->protocol	= protocol;
    this->timeout	= timeout;

    sockDesc = socket(domain, type, protocol);
    if (sockDesc == -1) // something went wrong, throw an error.
	throw JSNException("Unable to create a base socket descriptor.");
} /* JSNSockBase constructor 1/1 */

JSNSockBase::~JSNSockBase()
{
    close();
}

auto JSNSockBase::close (
	)		-> void
{
    ::close(sockDesc);
}

auto JSNSockBase::sockOption(
	int		level,
	int		optname,
	void		*optval,
	socklen_t	*optlen
	)		-> void
{
    /* The 'getsockopt' method is declared via <sys/socket.h> */
    if (::getsockopt(sockDesc, level, optname, optval, optlen) < 0)
		throw JSNException("Unable to obtain socket options.");
}

auto JSNSockBase::setSockOption(
	int		level,
	int		optname,
	void		*optval,
	socklen_t	optlen
	)		-> void
{
    if (::setsockopt(sockDesc, level, optname, optval, optlen) < 0)
		throw JSNException("Unable to set socket options.");
}

auto JSNSockBase::setTimeout(
	double		timeout
	)		-> void
{
    this->timeout = timeout;
}

auto JSNSockBase::ntoa(
	in_addr_t	addr
	)		-> string
{
    struct in_addr	a;
    a.s_addr		= addr;
    char		*addr_char;

    addr_char = inet_ntoa(a);
    if (!addr_char)
	throw JSNException("Unable to create an ascii representation of the socket's current address (via inet_ntoa).");

    return string(addr_char);
}

auto JSNSockBase::socketInfo(
	)			-> void
{
    std::string		address_str;
    uint16_t		port_num;
    struct sockaddr_in	sockAddr;
    socklen_t		len;
    int8_t		resultCode;

    len = sizeof(struct sockaddr_in);

    /* clear the contents of the sockaddr_in struct */
    bzero((char *) &sockAddr, sizeof(sockAddr));

    if (getpeername(sockDesc, (struct sockaddr *) &sockAddr, &len) == -1)
	throw JSNException("JSNSockBase: socketInfo (peer name) exception.");
    else
    {
	address_str = ntoa(sockAddr.sin_addr.s_addr);
	port_num = htons(sockAddr.sin_port);
	socketInfoArray[0] = std::make_pair(address_str, port_num);
    }

    bzero((char *) &sockAddr, sizeof(sockAddr));

    if (getsockname(sockDesc, (struct sockaddr *) &sockAddr, &len) == -1)
	throw JSNException("JSNSockBase: socketInfo (sock name) exception.");
    else
    {
	address_str = ntoa(sockAddr.sin_addr.s_addr);
	port_num = htons(sockAddr.sin_port);
	socketInfoArray[1] = std::make_pair(address_str, port_num);
    }
}

auto JSNSockBase::isBlocking(
	)		-> bool
{
    int32_t	flags;

    if ((flags = fcntl(sockDesc, F_GETFL, 0)) == -1)
    {
	throw JSNException("Unable to obtain socket blocking flags (via fcntl).");
    }

    return ((flags & O_NONBLOCK) ? true : false);
}

auto JSNSockBase::setBlocking(
	bool	on
	)	-> void
{
    int32_t	flags;

    if ((flags = fcntl(sockDesc, F_GETFL, 0)) == -1)
    {
	throw JSNException("Unable to obtain socket blocking flags (via fcntl).");
    }

    if (on)
    {
	if (fcntl(sockDesc, F_SETFL, flags & ~O_NONBLOCK) == -1) /* clear the non-block flag */
	{
	    throw JSNException("Unable to set socket blocking (via fcntl).");
	}
    }
    else
    {
	if (fcntl(sockDesc, F_SETFL, flags | O_NONBLOCK) == -1)
	{
	    throw JSNException("Unable to set socket to not block (via fcntl).");
	}
    }
}

auto JSNSockBase::getHostByName(
	const string		&name
	)			-> string
{
    const std::unique_ptr<char> response(new char[ADDR_STR_LEN]); /* RAII; anti-exploit measure */
    struct hostent		*host;

    if ( (host = gethostbyname(name.c_str())) )	/* gethostbyname returns a NULL ptr on error */
    {
	snprintf (response.get(), ADDR_STR_LEN, "%u.%u.%u.%u",
		(unsigned char) host->h_addr[0],
		(unsigned char) host->h_addr[1],
		(unsigned char) host->h_addr[2],
		(unsigned char) host->h_addr[3]
		);
    }

    return string(response.get());
}

auto JSNSockBase::getHostByAddr(
	const string	&addr
	)		-> string
{
    struct in_addr	*intAddr;
    struct hostent	*host;

    if ( inet_aton(addr.c_str(), intAddr) != 0 ) /* inet_aton does format sanity check */
    {
	host = gethostbyaddr(&intAddr->s_addr, 4, AF_INET);
    }

    return string(host->h_name); /* may be empty */
}
