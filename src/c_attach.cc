/*$Id: c_attach.cc,v 1.2 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et
 * Copyright (C) 2007 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 */
//testing=informal
#include "e_cardlist.h"
#include "c_comand.h"
#include "constant.h"
#include "globals.h"
// #include "u_opt.h"
/*--------------------------------------------------------------------------*/
namespace {

  bool search_file( std::string &name ){
    const char* h ="HOME";
    const char* home= getenv(h);

    if (name[0] == '/') return true; // fixme

    std::string pathlist[4] = { OPT::libpath , "./",  LIBDIR,
      std::string(home) + std::string("/.gnucap/lib/") };

    // FIXME. use libpath 

    for(int i=1; i<4 ; i++) {
      if ( FILE* tmp = fopen( (pathlist[i] + "/" + name).c_str(), "r" ) ) {
        fclose(tmp);
        name = pathlist[i]+"/"+name;
        return true;
      }
      trace0( (" not found " + pathlist[i] + "/" + name).c_str());
    }
    return false;

  }
/*--------------------------------------------------------------------------*/
std::map<const std::string, void*> attach_list;
/*--------------------------------------------------------------------------*/
class CMD_ATTACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    unsigned here = cmd.cursor();
    int dl_scope = RTLD_LOCAL;
    int check = RTLD_NOW;
    // RTLD_NOW means to resolve symbols on loading
    // RTLD_LOCAL means symbols defined in a plugin are local
    do {
      if (cmd.umatch("public ")) {
	dl_scope = RTLD_GLOBAL;
	// RTLD_GLOBAL means symbols defined in a plugin are global
	// Use this when a plugin depends on another.
      }else if (cmd.umatch("lazy ")) {
	check = RTLD_LAZY;
	// RTLD_LAZY means to defer resolving symbols until needed
	// Use when a plugin will not load because of unresolved symbols,
	// but it may work without it.
      }else{
      }
    } while (cmd.more() && !cmd.stuck(&here));

    std::string file_name;
    cmd >> file_name;
    
    void* handle = attach_list[file_name];
    if (handle) {
      if (CARD_LIST::card_list.is_empty()) {
	cmd.warn(bDANGER, here, "\"" + file_name + "\": already loaded, replacing");
	dlclose(handle);
	attach_list[file_name] = NULL;
      }else{untested();
	cmd.reset(here);
	throw Exception_CS("already loaded, cannot replace when there is a circuit", cmd);
      }
    }else{
    }
    
    if ( ! search_file(file_name) ){
      throw Exception_CS("cannot find file", cmd);
    }

    handle = dlopen(file_name.c_str(), check | dl_scope);
    if (handle) {
      attach_list[file_name] = handle;
    }else{itested();
      cmd.reset(here);
      throw Exception_CS(dlerror(), cmd);
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "attach|load", &p1);
/*--------------------------------------------------------------------------*/
class CMD_DETACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    if (CARD_LIST::card_list.is_empty()) {
      unsigned here = cmd.cursor();
      std::string file_name;
      cmd >> file_name;
      
      void* handle = attach_list[file_name];
      if (handle) {
	dlclose(handle);
	attach_list[file_name] = NULL;
      }else{itested();
	cmd.reset(here);
	throw Exception_CS("plugin not attached", cmd);
      }
    }else{itested();
      throw Exception_CS("detach prohibited when there is a circuit", cmd);
    }
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "detach|unload", &p2);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
