/*$Id: lang_spice.h,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
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
#ifndef LANG_SPICE_H
#define LANG_SPICE_H
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class LANG_SPICE_BASE : public LANGUAGE {
public:
  enum EOB {NO_EXIT_ON_BLANK, EXIT_ON_BLANK};
public: // override virtual, used by callback
  std::string arg_front()const {return " ";}
  std::string arg_mid()const {return "=";}
  std::string arg_back()const {return "";}

  // lang_spice_in.cc
public: // override virtual, called by commands
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*);
  DEV_DOT*	parse_command(CS&, DEV_DOT*);
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*);
  MODEL_SUBCKT* parse_module(CS&, MODEL_SUBCKT*);
  COMPONENT*	parse_instance(CS&, COMPONENT*);
  std::string	find_type_in_string(CS&);
public: // "local?", called by own commands
  void parse_module_body(CS&, MODEL_SUBCKT*, CARD_LIST*, const std::string&,
			 EOB, const std::string&);
private: // local
  void parse_type(CS&, CARD*);
  void parse_args(CS&, CARD*);
  void parse_label(CS&, CARD*);
  void parse_ports(CS&, COMPONENT*, int minnodes, int start, int num_nodes, bool all_new);
private: // compatibility hacks
  void parse_element_using_obsolete_callback(CS&, COMPONENT*);
  void parse_logic_using_obsolete_callback(CS&, COMPONENT*);

  // lang_spice_out.cc
private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*);
  void print_module(OMSTREAM&, const MODEL_SUBCKT*);
  void print_instance(OMSTREAM&, const COMPONENT*);
  void print_comment(OMSTREAM&, const DEV_COMMENT*);
  void print_command(OMSTREAM&, const DEV_DOT*);
private: // local
  void print_args(OMSTREAM&, const MODEL_CARD*);
  void print_type(OMSTREAM&, const COMPONENT*);
  void print_args(OMSTREAM&, const COMPONENT*);
  void print_label(OMSTREAM&, const COMPONENT*);
  void print_ports(OMSTREAM&, const COMPONENT*);
};
/*--------------------------------------------------------------------------*/
class LANG_SPICE : public LANG_SPICE_BASE {
public:
  std::string name()const {return "spice";}
  bool case_insensitive()const {return true;}
  UNITS units()const {return uSPICE;}
  void parse_top_item(CS&, CARD_LIST*);
};
/*--------------------------------------------------------------------------*/
class LANG_ACS : public LANG_SPICE_BASE {
public:
  std::string name()const {return "acs";}
  bool case_insensitive()const {return true;}
  UNITS units()const {return uSPICE;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
