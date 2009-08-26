/*$Id: lang_verilog.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
#ifndef LANG_VERILOG_H
#define LANG_VERILOG_H
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class LANG_VERILOG : public LANGUAGE {
  enum MODE {mDEFAULT, mPARAMSET} _mode;
  mutable int arg_count;
  enum {INACTIVE = -1};
public:
  LANG_VERILOG() : arg_count(INACTIVE) {}
  std::string name()const {return "verilog";}
  bool case_insensitive()const {return false;}
  UNITS units()const {return uSI;}
public: // override virtual, used by callback
  std::string arg_front()const {
    switch (_mode) {
    case mPARAMSET: return " .";			    break;
    case mDEFAULT:  return (arg_count++ > 0) ? ", ." : "."; break;
    }
    unreachable();
    return "";
  }
  std::string arg_mid()const {
    switch (_mode) {
    case mPARAMSET: return "="; break;
    case mDEFAULT:  return "("; break;
    }
    unreachable();
    return "";
  }
  std::string arg_back()const {
    switch (_mode) {
    case mPARAMSET: return ";"; break;
    case mDEFAULT:  return ")"; break;
    }
    unreachable();
    return "";
  }

  // lang_verilog_in.cc
public: // override virtual, called by commands
  void		parse_top_item(CS&, CARD_LIST*);
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*);
  DEV_DOT*	parse_command(CS&, DEV_DOT*);
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*);
  MODEL_SUBCKT* parse_module(CS&, MODEL_SUBCKT*);
  COMPONENT*	parse_instance(CS&, COMPONENT*);
  std::string	find_type_in_string(CS&);

  // lang_verilog_out.cc
private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*);
  void print_module(OMSTREAM&, const MODEL_SUBCKT*);
  void print_instance(OMSTREAM&, const COMPONENT*);
  void print_comment(OMSTREAM&, const DEV_COMMENT*);
  void print_command(OMSTREAM& o, const DEV_DOT* c);
private: // local
  void print_args(OMSTREAM&, const MODEL_CARD*);
  void print_args(OMSTREAM&, const COMPONENT*);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
