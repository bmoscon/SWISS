/*
 * module_manager.hpp
 *
 *
 * Swiss Module Manager
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

#ifndef __MODULE_MANAGER__
#define __MODULE_MANAGER__


#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>
#include <cassert>

#include <dlfcn.h>
#include <dirent.h>

#include "sserver.hpp"


class ModuleManager {

public:
  ModuleManager() {}
  ModuleManager(const char* module_dir)
  {
    loadModules(module_dir);
  }
  
  void loadModules(const char *module_dir)
  {
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir(module_dir)) == NULL) {
      throw "Modules directory does not exist";
    }
    
    while ((ent = readdir(dir)) != NULL) {
      module_st mod;
      char path[PATH_MAX];
      if (ent->d_type == DT_REG) {
	memset(path, 0, PATH_MAX);
	snprintf(path, PATH_MAX, "%s%s", module_dir, ent->d_name);
	
	mod.handle = dlopen(path, RTLD_LAZY);
	if (!mod.handle) {
	  throw "dlopen failed on module";
	}
	
	mod.fps = new module_fps_st;
	
	mod.fps->load = (int (*)())dlsym(mod.handle, "load");
	if (!mod.fps->load) {
	  throw "module did not contain load symbol";
	}
	
	mod.fps->work = (void (*)(void *))dlsym(mod.handle, "work");
	if (!mod.fps->work) {
	  throw "module did not contain work symbol";
	}
	
	mod.fps->unload = (int (*)())dlsym(mod.handle, "unload");
	if (!mod.fps->unload) {
	  throw "module did not contain unload symbol";
	}
	
	module_list_.push_back(mod);
      }
    }
    closedir(dir);
  }
  
  void modLoad()
  {
    for (unsigned int i = 0; i < module_list_.size(); ++i) {
      server_list_.push_back(SServer(4, module_list_[i].fps->load(), module_list_[i].fps->work));
      server_list_[server_list_.size() - 1].start();
    }
  }

  void modUnload()
  {
    for (unsigned int i = 0; i < module_list_.size(); ++i) {
      assert(module_list_[i].fps->unload() == 0);
    }
  }
  
private:
  typedef struct module_fps_st {
    int (*load)(void);
    void (*work)(void *opqaue);
    int (*unload)(void);
  } module_fps_st;

  typedef struct module_st {
    module_fps_st *fps;
    void *handle;
  } module_st;
  
  std::vector<module_st> module_list_;
  std::vector<SServer> server_list_;
};


#endif
