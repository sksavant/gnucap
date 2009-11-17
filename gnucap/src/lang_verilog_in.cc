/*$Id: lang_verilog_in.cc,v 26.125 2009/10/15 20:58:21 al Exp $ -*- C++ -*-
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
 */
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "lang_verilog.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void parse_type(CS& cmd, CARD* x)
{
  assert(x);
  std::string new_type;
  cmd >> new_type;
  x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
static void parse_args_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);

  while (cmd >> '.') {
    unsigned here = cmd.cursor();
    std::string name, value;
    try{
      cmd >> name >> '=' >> value >> ';';
      x->set_param_by_name(name, value);
    }catch (Exception_No_Match&) {untested();
      cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
    }
  }
}
/*--------------------------------------------------------------------------*/
static void parse_args_instance(CS& cmd, CARD* x)
{
  assert(x);

  if (cmd >> "#(") {
    if (cmd.match1('.')) {
      // by name
      while (cmd >> '.') {
	unsigned here = cmd.cursor();
	std::string name  = cmd.ctos("(", "", "");
	std::string value = cmd.ctos(",)", "(", ")");
	cmd >> ',';
	try{
	  x->set_param_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
	}
      }
    }else{
      // by order
      int index = 1;
      while (cmd.is_alnum()) {
	unsigned here = cmd.cursor();
	try{
	  std::string value = cmd.ctos(",)", "", "");
	  x->set_param_by_index(x->param_count() - index++, value, 0/*offset*/);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }
    cmd >> ')';
  }else{
    // no args
  }
}
/*--------------------------------------------------------------------------*/
static void parse_label(CS& cmd, CARD* x)
{
  assert(x);
  std::string my_name;
  cmd >> my_name;
  x->set_label(my_name);
}
/*--------------------------------------------------------------------------*/
static void parse_ports(CS& cmd, COMPONENT* x)
{
  assert(x);

  if (cmd >> '(') {
    if (cmd.is_alnum()) {
      // by order
      int index = 0;
      while (cmd.is_alnum()) {
	unsigned here = cmd.cursor();
	try{
	  std::string value;
	  cmd >> value;
	  x->set_port_by_index(index++, value);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }else{
      // by name
      while (cmd >> '.') {
	unsigned here = cmd.cursor();
	try{
	  std::string name, value;
	  cmd >> name >> '(' >> value >> ')' >> ',';
	  x->set_port_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, "mismatch, ignored");
	}
      }
    }
    cmd >> ')';
  }else{untested();
    cmd.warn(bDANGER, "'(' required");
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_VERILOG::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_VERILOG::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset();
  CMD::cmdproc(cmd, scope);
  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
/* "paramset" <my_name> <base_name> ";"
 *    <paramset_item_declaration>*
 *    <paramset_statement>*
 *  "endparamset"
 */
//BUG// no paramset_item_declaration, falls back to spice mode
//BUG// must be on single line

MODEL_CARD* LANG_VERILOG::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset();
  cmd >> "paramset ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  cmd >> ';';
  parse_args_paramset(cmd, x);
  cmd >> "endparamset ";
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
/* "module" <name> "(" <ports> ")" ";"
 *    <declarations>
 *    <netlist>
 * "endmodule"
 */
//BUG// strictly one device per line

MODEL_SUBCKT* LANG_VERILOG::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);

  // header
  cmd.reset();
  (cmd >> "module |macromodule ");
  parse_label(cmd, x);
  parse_ports(cmd, x);
  cmd >> ';';

  // body
  for (;;) {
    cmd.get_line("verilog-module>");

    if (cmd >> "endmodule ") {
      break;
    }else{
      new__instance(cmd, x, x->subckt());
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_VERILOG::parse_instance(CS& cmd, COMPONENT* x)
{
  cmd.reset();
  parse_type(cmd, x);
  parse_args_instance(cmd, x);
  parse_label(cmd, x);
  parse_ports(cmd, x);
  cmd >> ';';
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_VERILOG::find_type_in_string(CS& cmd)
{
  unsigned here = cmd.cursor();
  std::string type;
  if ((cmd >> "//")) {
    assert(here == 0);
    type = "dev_comment";
  }else{
    cmd >> type;
  }
  cmd.reset(here);
  return type;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-verilog>");
  new__instance(cmd, NULL, Scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
