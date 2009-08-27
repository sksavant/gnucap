/*$Id: lang_spectre_in.cc,v 26.117 2009/08/19 15:28:06 al Exp $ -*- C++ -*-
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
#include "lang_spectre.h"
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
static void parse_args(CS& cmd, CARD* x)
{
  assert(x);
  
  unsigned here = 0;
  while (cmd.more() && !cmd.stuck(&here)) {
    try{
      std::string name, value;
      cmd >> name >> '=' >> value;
      //std::cout << name << '=' << value << ";\n";
      x->set_param_by_name(name, value);
    }catch (Exception_No_Match&) {untested();
      cmd.warn(bDANGER, here, "bad parameter, ignored");
    }
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
    cmd >> ')';
  }else{
    unsigned here = cmd.cursor();
    OPT::language->find_type_in_string(cmd);
    unsigned stop = cmd.cursor();
    cmd.reset(here);

    int index = 0;
    while (cmd.cursor() < stop) {
      here = cmd.cursor();
      try{
	std::string value;
	cmd >> value;
	x->set_port_by_index(index++, value);
      }catch (Exception_Too_Many& e) {untested();
	cmd.warn(bDANGER, here, e.message());
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_SPECTRE::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_SPECTRE::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset().skipbl();
  if ((cmd >> "model |simulator |parameters |subckt ")) {
    cmd.reset();
    CMD::cmdproc(cmd, scope);
  }else{
    std::string label;
    cmd >> label;
    
    if (label != "-") {
      unsigned here = cmd.cursor();
      std::string command;
      cmd >> command;
      cmd.reset(here);
      std::string file_name = label + '.' + command;
      std::string s = cmd.tail() + " > " + file_name;
      CS augmented_cmd(CS::_STRING, s);
      CMD::cmdproc(augmented_cmd, scope);
    }else{
      CMD::cmdproc(cmd, scope);
    }
  }
  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
MODEL_CARD* LANG_SPECTRE::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset().skipbl();
  cmd >> "model ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  parse_args(cmd, x);
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT* LANG_SPECTRE::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);

  // header
  cmd.reset().skipbl();
  cmd >> "subckt ";
  parse_label(cmd, x);
  parse_ports(cmd, x);

  // body
  for (;;) {
    cmd.get_line("spectre-subckt>");

    if (cmd >> "ends ") {
      break;
    }else{
      new__instance(cmd, x, x->subckt());
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_SPECTRE::parse_instance(CS& cmd, COMPONENT* x)
{
  cmd.reset();
  parse_label(cmd, x);
  parse_ports(cmd, x);
  parse_type(cmd, x);
  parse_args(cmd, x);
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_SPECTRE::find_type_in_string(CS& cmd)
{
  // known to be not always correct

  cmd.reset().skipbl();
  unsigned here = 0;
  std::string type;
  if ((cmd >> "*|//")) {itested();
    assert(here == 0);
    type = "dev_comment";
  }else if ((cmd >> "model |simulator |parameters |subckt ")) {
    // type is first, it's a control statement
    type = cmd.trimmed_last_match();
  }else if (cmd.reset().skiparg().match1("(") && cmd.scan(")")) {
    // node list surrounded by parens
    // type follows
    here = cmd.cursor();
    cmd.reset(here);
    cmd >> type;
  }else if (cmd.reset().scan("=")) {itested();
    // back up two, by starting over
    cmd.reset().skiparg();
    unsigned here1 = cmd.cursor();
    cmd.skiparg();
    unsigned here2 = cmd.cursor();
    cmd.skiparg();
    unsigned here3 = cmd.cursor();
    while (here2 != here3 && !cmd.match1('=')) {
      cmd.skiparg();
      here1 = here2;
      here2 = here3;
      here3 = cmd.cursor();
    }
    here = here1;
    cmd.reset(here);
    cmd >> type;
  }else{
    // type is second
    cmd.reset().skiparg();
    here = cmd.cursor();
    cmd.reset(here);
    cmd >> type;
  }
  cmd.reset(here);
  return type;
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-spectre>");
  new__instance(cmd, NULL, Scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
