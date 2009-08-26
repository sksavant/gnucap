/*$Id: lang_spice_commands.cc,v 26.96 2008/10/09 05:36:27 al Exp $ -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
 */
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "globals.h"
#include "lang_spice.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
LANG_SPICE lang_spice;
DISPATCHER<LANGUAGE>::INSTALL
	ds(&language_dispatcher, lang_spice.name(), &lang_spice);
LANG_ACS lang_acs;
DISPATCHER<LANGUAGE>::INSTALL
	da(&language_dispatcher, lang_acs.name(), &lang_acs);
DEV_COMMENT p0;
DISPATCHER<CARD>::INSTALL
	d0(&device_dispatcher, ";|#|*|'|\"|dev_comment", &p0);
/*--------------------------------------------------------------------------*/
class CMD_MODEL : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    // already got "model"
    std::string my_name, base_name;
    cmd >> my_name;
    unsigned here1 = cmd.cursor();    
    cmd >> base_name;
    
    // "level" kluge ....
    // if there is a "level" keyword, with integer argument,
    // tack that onto the given modelname and look for that
    cmd.skip1b('(');
    int level = 0;
    {
      unsigned here = cmd.cursor();
      scan_get(cmd, "level ", &level);
      if (!cmd.stuck(&here)) {
	char buf[20];
	sprintf(buf, "%u", level);
	base_name += buf;
      }else{
      }
    }

    const MODEL_CARD* p = model_dispatcher[base_name];

    if (p) {
      MODEL_CARD* new_card = dynamic_cast<MODEL_CARD*>(p->clone());
      if (exists(new_card)) {
	assert(!new_card->owner());
	lang_spice.parse_paramset(cmd, new_card);
	Scope->push_back(new_card);
      }else{untested();
	cmd.warn(bDANGER, here1, "model: base has incorrect type");
      }
    }else{
      cmd.warn(bDANGER, here1, "model: \"" + base_name + "\" no match");
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, ".model", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SUBCKT : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    MODEL_SUBCKT* new_module = new MODEL_SUBCKT;
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    lang_spice.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, ".subckt|.macro", &p2);
/*--------------------------------------------------------------------------*/
enum Skip_Header {NO_HEADER, SKIP_HEADER};
/*--------------------------------------------------------------------------*/
/* getmerge: actually do the work for "get", "merge", etc.
 */
static void getmerge(CS& cmd, Skip_Header skip_header, CARD_LIST* Scope)
{
  ::status.get.reset().start();
  assert(Scope);

  std::string file_name, section_name;
  cmd >> file_name;
  
  bool  echoon = false;		/* echo on/off flag (echo as read from file) */
  bool  liston = false;		/* list on/off flag (list actual values) */
  bool  quiet = false;		/* don't echo title */
  unsigned here = cmd.cursor();
  do{
    ONE_OF
      || Get(cmd, "echo",  &echoon)
      || Get(cmd, "list",  &liston)
      || Get(cmd, "quiet", &quiet)
      || Get(cmd, "section", &section_name)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  if (cmd.more()) {
    cmd >> section_name;
  }else{
  }
  cmd.check(bWARNING, "need section, echo, list, or quiet");

  CS file(CS::_INC_FILE, file_name);

  if (skip_header) { // get and store the header line
    file.get_line(">>>>");
    head = file.fullstring();

    if (!quiet) {
      IO::mstdout << head << '\n';
    }else{untested();
    }
  }else{
  }
  if (section_name == "") {
    lang_spice.parse_module_body(file, NULL, Scope, ">>>>", lang_spice.NO_EXIT_ON_BLANK, ".end ");
  }else{
    try {
      for (;;) {
	file.get_line("lib " + section_name + '>');
	if (file.umatch(".lib " + section_name + ' ')) {
	  lang_spice.parse_module_body(file, NULL, Scope, section_name,
			lang_spice.NO_EXIT_ON_BLANK, ".endl {" + section_name + "}");
	}else{
	  // skip it
	}
      }
    }catch (Exception_End_Of_Input& e) {
    }
  }
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_lib: lib command
 */
class CMD_LIB : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    unsigned here = cmd.cursor();
    std::string section_name, more_stuff;
    cmd >> section_name >> more_stuff;
    if (more_stuff != "") {
      cmd.reset(here);
      getmerge(cmd, NO_HEADER, Scope);
    }else{
      for (;;) {
	cmd.get_line(section_name + '>');
	if (cmd.umatch(".endl {" + section_name + "}")) {
	  break;
	}else{
	  // skip it
	}
      }
    }
  }
} p33;
DISPATCHER<CMD>::INSTALL d33(&command_dispatcher, ".lib|lib", &p33);
/*--------------------------------------------------------------------------*/
/* cmd_include: include command
 * as get or run, but do not clear first, inherit the run-mode.
 */
class CMD_INCLUDE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    getmerge(cmd, NO_HEADER, Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, ".include", &p3);
/*--------------------------------------------------------------------------*/
/* cmd_merge: merge command
 * as get, but do not clear first
 */
class CMD_MERGE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {untested();
    SET_RUN_MODE xx(rPRESET);
    getmerge(cmd, NO_HEADER, Scope);
  }
} p4;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, ".merge|merge", &p4);
/*--------------------------------------------------------------------------*/
/* cmd_run: "<" and "<<" commands
 * run in batch mode.  Spice format.
 * "<<" clears old circuit first, "<" does not
 * get circuit from file, execute dot cards in sequence
 */
class CMD_RUN : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    while (cmd.match1('<')) {untested();
      command("clear", Scope);
      cmd.skip();
      cmd.skipbl();
    }
    SET_RUN_MODE xx(rSCRIPT);
    getmerge(cmd, SKIP_HEADER, Scope);
  }
} p5;
DISPATCHER<CMD>::INSTALL d5(&command_dispatcher, "<", &p5);
/*--------------------------------------------------------------------------*/
/* cmd_get: get command
 * get circuit from a file, after clearing the old one
 * preset, but do not execute "dot cards"
 */
class CMD_GET : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    SET_RUN_MODE xx(rPRESET);
    command("clear", Scope);
    getmerge(cmd, SKIP_HEADER, Scope);
  }
} p6;
DISPATCHER<CMD>::INSTALL d6(&command_dispatcher, ".get|get", &p6);
/*--------------------------------------------------------------------------*/
/* cmd_build: build command
 * get circuit description direct from keyboard (or stdin if redirected)
 * Command syntax: build <before>
 * Bare command: add to end of list
 * If there is an arg: add before that element
 * null line exits this mode
 * preset, but do not execute "dot cards"
 */
class CMD_BUILD : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    SET_RUN_MODE xx(rPRESET);
    ::status.get.reset().start();
    lang_spice.parse_module_body(cmd, NULL, Scope, ">", lang_spice.EXIT_ON_BLANK, ". ");
    ::status.get.stop();
  }
} p7;
DISPATCHER<CMD>::INSTALL d7(&command_dispatcher, ".build|build", &p7);
/*--------------------------------------------------------------------------*/
class CMD_SPICE : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=spice", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "spice", &p8);
/*--------------------------------------------------------------------------*/
class CMD_ACS : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=acs", Scope);
  }
} p9;
DISPATCHER<CMD>::INSTALL d9(&command_dispatcher, "acs", &p9);
/*--------------------------------------------------------------------------*/
class CMD_ENDC : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    if (OPT::language == &lang_acs) {
      command("options lang=spice", Scope);
    }else{
    }
  }
} p88;
DISPATCHER<CMD>::INSTALL d88(&command_dispatcher, ".endc", &p88);
/*--------------------------------------------------------------------------*/
class CMD_CONTROL : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    if (OPT::language == &lang_spice) {
      command("options lang=acs", Scope);
    }else{
    }
  }
} p99;
DISPATCHER<CMD>::INSTALL d99(&command_dispatcher, ".control", &p99);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
