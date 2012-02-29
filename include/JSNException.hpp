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

#ifndef _JSNException_
#define _JSNException_
#define		SIZE_ERROR_BUFFER	1024

/* Include */
#include <stdio.h>
#include <exception>
#include <cerrno>

/* Using */
using std::exception;

/* Interface + Implementation */
namespace jsnSock
{
    class JSNException : public exception
    {
	private:
	    char *error_buffer;
	public:
	    JSNException( const char *error_msg )
	    {
		error_buffer = new char[SIZE_ERROR_BUFFER];
		snprintf(error_buffer, SIZE_ERROR_BUFFER, "%s : %s [END]", error_msg, strerror(errno));
	    }

	    ~JSNException() noexcept
	    {
		delete [] error_buffer;
	    }

	    const char * what() const noexcept
	    {
		return error_buffer;
	    }
    }; /* JSNException class */
} /* jsnSock namespace */

#endif /* _JSNException_ */
