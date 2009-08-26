/*$Id: lang_spice_out.cc,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
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
//testing=script 2007.07.13
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "lang_spice.h"
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  o << ".model " << x->short_label() << ' ' << x->dev_type() << " (";
  print_args(o, x);
  o << ")\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  o << ".subckt " <<  x->short_label();
  print_ports(o, x);
  o << '\n';
  
  for (CARD_LIST::const_iterator 
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }
  
  o << ".ends " << x->short_label() << "\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_label(o, x);
  print_ports(o, x);
  print_type(o, x);
  print_args(o, x);
  o << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  if (x->comment()[1] != '+') {
    o << x->comment() << '\n';
  }else{
  }
  // Suppress printing of comment lines starting with "*+".
  // These are generated as a way to display calculated values.
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_command(OMSTREAM& o, const DEV_DOT* x)
{
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_args(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " " + x->param_name(ii) + "=" + x->param_value(ii);
	o << arg;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  if (x->print_type_in_spice()) {
    o << "  " << x->dev_type();
  }else if (fix_case(x->short_label()[0]) != fix_case(x->id_letter())) {itested();
    o << "  " << x->dev_type();
  }else{
    // don't print type
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_args(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << ' ';
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	if ((ii != x->param_count() - 1) || (x->param_name(ii) != x->value_name())) {
	  o << " " << x->param_name(ii) << "=";
	}else{
	}
	o << x->param_value(ii);
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  std::string label = x->short_label();
  o << label;
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::print_ports(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);

  o <<  " ( ";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_value(ii);
    sep = " ";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_value(ii);
    sep = " ";
  }
  o << " )";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
