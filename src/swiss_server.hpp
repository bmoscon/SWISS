/*
 * swiss_server.hpp
 *
 *
 * Swiss Server
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

#ifndef __SWISS_SERVER__
#define __SWISS_SERVER__

#include <cstdio>
#include <cstring>
#include <cassert>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "thread_pool/thread_pool.hpp"
#include "include/module.h"

#define LISTEN_Q_SIZE 1024


class SwissServer {

public:

  SwissServer(unsigned int t, unsigned int port, void (*w)(void *)) : threads_(ThreadPool(t)), 
								      work_fp_(w),
								      run_(true)
  {  
    bzero(&server_addr_, sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr_.sin_port = htons(port);
  }

  ~SwissServer()
  {
    stop();
  }

  void start() 
  { 
    threads_.start();
    threads_.addWork(&mainThread, this);
  }

  void stop()
  {
    run_ = false;
    threads_.stop();
  }

private:

  void handleRequest(void *data)
  {
    threads_.addWork(work_fp_, data);
  }

  static void mainThread(void *server)
  {
    int listen_fd;
    int conn_fd;
    struct sockaddr_in addr = ((SwissServer *)server)->server_addr_;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd);
    
    assert(bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) >= 0);
    
    assert(listen(listen_fd, LISTEN_Q_SIZE) >= 0);
    while (((SwissServer *)server)->run_) {
      swiss_work_st *work_data = new swiss_work_st;
      socklen_t len;
      do {
	len = sizeof(work_data->addr);
	if ((conn_fd = accept(listen_fd, (struct sockaddr *) &(work_data->addr), &len)) < 0) {
	  if ((errno != EPROTO) && (errno != ECONNABORTED)) {
	    assert(false);
	  }
	}
      } while (errno == EPROTO || errno == ECONNABORTED);
      
      work_data->fd = conn_fd;
      ((SwissServer *)server)->handleRequest((void *)work_data);
    }
  }

  ThreadPool threads_;
  struct sockaddr_in server_addr_;
  void (*work_fp_)(void *opaque);
  bool run_;
};


#endif
