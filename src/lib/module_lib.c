/*
 * module_lib.c
 *
 *
 * Swiss Loadable Module Library
 *
 *
 * Copyright (C) 2013  Bryant Moscon - bmoscon@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution, and in the same 
 *    place and form as other copyright, license and disclaimer information.
 *
 * 3. The end-user documentation included with the redistribution, if any, must 
 *    include the following acknowledgment: "This product includes software 
 *    developed by Bryant Moscon (http://www.bryantmoscon.org/)", in the same 
 *    place and form as other third-party acknowledgments. Alternately, this 
 *    acknowledgment may appear in the software itself, in the same form and 
 *    location as other such third-party acknowledgments.
 *
 * 4. Except as contained in this notice, the name of the author, Bryant Moscon,
 *    shall not be used in advertising or otherwise to promote the sale, use or 
 *    other dealings in this Software without prior written authorization from 
 *    the author.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */


#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>


#include "module_lib.h"



int swiss_recv(int fd, uint8_t *buffer, const size_t len, const int flags)
{
  int32_t  read_bytes;
  
  if ((!buffer) || (!len) || (fd == -1)) {
    // todo: log error
    return (-1);
  }
  
  do {
    if ((read_bytes = recv(fd, buffer, len, flags)) < 0) {
      if (errno != EINTR) {
	// todo: log error
	return (-1);
      }
    }
  } while (errno == EINTR);

  return (read_bytes);
}


int swiss_recvfrom(int fd, uint8_t* buffer, const size_t len, const int flags, 
		   struct sockaddr *addr, socklen_t *addrlen)
{
  int32_t  read_bytes;
  
  if ((!buffer) || (!len) || (fd == -1)) {
    // todo: log error
    return (-1);
  }
  
  do {
    if ((read_bytes = recvfrom(fd, buffer, len, flags, addr, addrlen)) < 0) {
      if (errno != EINTR) {
	// todo: log error
	return (-1);
      }
    }
  } while (errno == EINTR);
  
  return (read_bytes);
}


int swiss_read(int fd, uint8_t *buffer, const size_t len)
{
  int32_t  read_bytes;
  
  if ((!buffer) || (!len) || (fd == -1)) {
    // todo: log error
    return (-1);
  }
  
  do {
    if ((read_bytes = read(fd, buffer, len)) < 0) {
      if (errno != EINTR) {
	// todo: log error
	return (-1);
      }
    }
  } while (errno == EINTR);
  
  return (read_bytes);
}


int swiss_send(int fd, const uint8_t *buffer, const size_t len, const int flags)
{
  uint32_t remaining_bytes = len;
  int32_t  write_bytes;
  
  if ((!buffer) || (!len) || (fd == -1)) {
    // todo: log error
    return (-1);
  }
  
  while (remaining_bytes > 0) {
    if ((write_bytes = send(fd, buffer, remaining_bytes, flags)) <= 0) {
      if ((errno == EINTR) && (write_bytes < 0)) {
	write_bytes = 0;
      } else {
	// todo: log error
	return (-1);
      }
    }
    
    remaining_bytes -= write_bytes;
    buffer += write_bytes;
  }
  
  return (len - remaining_bytes);
}


int swiss_sendto(int fd, const uint8_t *buffer, const size_t len, const int flags, 
		 const struct sockaddr *addr, const socklen_t addrlen)
{
  uint32_t remaining_bytes = len;
  int32_t  write_bytes;
  
  if ((!buffer) || (!len) || (fd == -1)) {
    // todo: log error
    return (-1);
  }
  
  while (remaining_bytes > 0) {
    if ((write_bytes = sendto(fd, buffer, remaining_bytes, flags, addr, addrlen)) <= 0) {
      if ((errno == EINTR) && (write_bytes < 0)) {
	write_bytes = 0;
      } else {
	// todo: log error
	return (-1);
      }
    }
    
    remaining_bytes -= write_bytes;
    buffer += write_bytes;
  }
  
  return (len - remaining_bytes);
}


int swiss_write(int fd, const uint8_t *buffer, const size_t len)
{
  uint32_t remaining_bytes = len;
  int32_t  write_bytes;
  
  if ((!buffer) || (!len) || (fd == -1)) {
    // todo: log error
    return (-1);
  }
  
  while (remaining_bytes > 0) {
    if ((write_bytes = write(fd, buffer, remaining_bytes)) <= 0) {
      if ((errno == EINTR) && (write_bytes < 0)) {
	write_bytes = 0;
      } else {
	// todo: log error
	return (-1);
      }
    }
    
    remaining_bytes -= write_bytes;
    buffer += write_bytes;
  }
  
  return (len - remaining_bytes);
}


void swiss_close(int *fd)
{
  if (fd) {
    close(*fd);
    *fd = -1;
  }
}
