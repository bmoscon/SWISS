/*
 * example.cc
 *
 *
 * Example Swiss Loadable Module
 *
 *
 * Copyright (C) 2012-2013  Bryant Moscon - bmoscon@gmail.com
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

#include <iostream>
#include <cstring>
#include <stdint.h>

#include "include/module.h"
#include "lib/module_lib.h"

extern "C" int load()
{
  // return the port we are interested in
  return (8080);
}

extern "C" void work(void *data)
{
  int flags;
  int read;
  swiss_work_st *work;
  char read_buffer[10240];
  char buffer[] = "HTTP/1.1 200 OK\nContent-length: 40\nContent-Type: text/html\n\n<html><body><H1>Hello</H1></body></html>";
  
  if (!data) {
    return;
  }
  
  work = (swiss_work_st *)data;
  
  std::cout << "got FD " << work->fd << std::endl;
  std::cout << "Read: \n";

  read = swiss_read(work->fd, (uint8_t *)read_buffer, 10240);
  
  std::cout << read_buffer << std::endl;
  std::cout.flush();
  
  swiss_write(work->fd, (const uint8_t *)buffer, strlen(buffer));
  
  swiss_close(&work->fd);
  
  delete work;
}

extern "C" int unload()
{
  // nothing to do here
  return (0);
}
