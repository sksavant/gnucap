/*$Id: lang_spectre_commands.cc,v 26.109 2009/02/02 06:39:10 al Exp $ -*- C++ -*-
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
#include "d_subckt.h"
#include "e_model.h"
#include "globals.h"
#include "lang_spectre.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
LANG_SPECTRE lang_spectre;
DISPATCHER<LANGUAGE>::INSTALL
	d(&language_dispatcher, lang_spectre.name(), &lang_spectre);
/*--------------------------------------------------------------------------*/
class CMD_MODEL : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    // already got "model"
    std::string my_name, base_name;
    cmd >> my_name;
    unsigned here = cmd.cursor();    
    cmd >> base_name;

    //const MODEL_CARD* p = model_dispatcher[base_name];
    const CARD* p = lang_spectre.find_proto(base_name, NULL);
    if (p) {
      MODEL_CARD* new_card = dynamic_cast<MODEL_CARD*>(p->clone());
      if (exists(new_card)) {
	assert(!new_card->owner());
	lang_spectre.parse_paramset(cmd, new_card);
	Scope->push_back(new_card);
      }else{
	cmd.warn(bDANGER, here, "model: base has incorrect type");
      }
    }else{
      cmd.warn(bDANGER, here, "model: no match");
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "model", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SUBCKT : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    MODEL_SUBCKT* new_module = new MODEL_SUBCKT;
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    lang_spectre.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "subckt", &p2);
/*--------------------------------------------------------------------------*/
class CMD_SIMULATOR : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    command("options " + cmd.tail(), Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "simulator", &p3);
/*--------------------------------------------------------------------------*/
class CMD_SPECTRE : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=spectre", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "spectre", &p8);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
