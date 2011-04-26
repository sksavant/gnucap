/*$Id: u_parameter.cc,v 1.4 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et
 * Copyright (C) 2005 Albert Davis
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
 * A class for parameterized values
 * Used for .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script,sparse 2006.07.14
#include "l_stlextra.h"
#include "u_parameter.h"
#include "u_lang.h"
#include "e_card.h"
#include "e_model.h"
// #include "d_rcd.h" // hack
/*--------------------------------------------------------------------------*/

//template <>
//MODEL_BUILT_IN_BTI PARAMETER<MODEL_BUILT_IN_BTI>::_NOT_INPUT(){ return MODEL_BUILT_IN_BTI() ;}

//template <>
//MODEL_BUILT_IN_RCD PARAMETER<MODEL_BUILT_IN_RCD>::_NOT_INPUT(){ return MODEL_BUILT_IN_RCD() ;}
//
//template <>
//MODEL_CARD PARAMETER<MODEL_CARD>::_NOT_INPUT(){ return MODEL_CARD() ;}

//template <>
//CARD PARAMETER<CARD>::_NOT_INPUT(){ return CARD() ;}

//template <>
//std::string PARAMETER<std::string>::_NOT_INPUT(){ return "";}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::parse(CS& cmd)
{
  (cmd >> "real |integer "); // ignore type
  unsigned here = cmd.cursor();
  for (;;) {
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) {
      break;
    }else{
    }
    std::string Name;
    PARAMETER<double> Value;
    cmd >> Name >> '=' >> Value;
    if (cmd.stuck(&here)) {untested();
      break;
    }else{
    }
    if (OPT::case_insensitive) {
      notstd::to_lower(&Name);
    }else{
    }
    _pl[Name] = Value;
  }
  cmd.check(bDANGER, "syntax error");
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::print(OMSTREAM& o, LANGUAGE* lang)const
{
  for (const_iterator i = _pl.begin(); i != _pl.end(); ++i) {
    if (i->second.has_hard_value()) {
      print_pair(o, lang, i->first, i->second);
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
bool PARAM_LIST::is_printable(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->second.has_hard_value();
    }else{
    }
  }
  return false;
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::name(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->first;
    }else{
    }
  }
  return "";
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::value(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->second.string();
    }else{
    }
  }
  return "";
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::eval_copy(PARAM_LIST& p, const CARD_LIST* scope)
{
  assert(!_try_again);
  _try_again = p._try_again;

  for (iterator i = p._pl.begin(); i != p._pl.end(); ++i) {
    if (i->second.has_hard_value()) {
      if (_pl[i->first].has_hard_value()) {untested();
	_pl[i->first] = i->second.e_val(_pl[i->first], scope);
      }else{
	_pl[i->first] = i->second.e_val(NOT_INPUT, scope);
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
const PARAMETER<double>& PARAM_LIST::deep_lookup(std::string Name)const
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  PARAMETER<double> & rv = _pl[Name];
  if (rv.has_hard_value()) {
    // found a value, return it
    return rv;
  }else if (_try_again) {
    // didn't find one, look in enclosing scope
    return _try_again->deep_lookup(Name);
  }else{
    // no enclosing scope to look in
    // really didn't find it, give up
    // return garbage value (NOT_INPUT)
    return rv;
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const double value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  _pl[Name] = value;
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const std::string& Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  _pl[Name] = Value;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, PARAMETER<bool>* val)
{
  if (cmd.umatch(key + ' ')) {
    if (cmd.skip1b('=')) {
      cmd >> *val;
    }else{
      *val = true;
    }
    return true;
  }else if (cmd.umatch("no" + key)) {
    *val = false;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, PARAMETER<int>* val)
{
  if (cmd.umatch(key + " {=}")) {
    *val = int(cmd.ctof());
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string PARAMETER<std::string>::e_val_normal(const std::string& def, const CARD_LIST* scope)const
{
  trace0("PARAMETER<std::string>::e_val_normal " + _s + " default: " + def + " val " + _v);
  assert(scope);

  static int recursion=0;
  static const std::string* first_name = NULL;
  if (recursion == 0) {
    first_name = &_s;
  }else{
  }
  assert(first_name);
 
  if (_s == "inf") {
    return my_infty();
  }
  ++recursion;
  if (_s == "") {
    // blank string means to use default value
    _v = def;
    if (recursion > 1) {
      error(bWARNING, "parameter " + *first_name + " has no value\n");
    }else{
    }
  }else if (_s == "{") {
    trace0("lookup");
    // anything else means look up the value
    if (recursion <= OPT::recursion) {
      _v = lookup_solve(def, scope);
      if (_v == "") {untested();itested();
	error(bDANGER, "parameter " + *first_name + " has no value\n");
      }else{
      }
    }else{untested();
      _v = def;
      error(bDANGER, "parameter " + *first_name + " recursion too deep\n");
    }
  }else{
    // start with # means we have a final value
    _v=_s;
  }
  --recursion;
  trace0("PARAMETER<T>::e_val "+ _v);
  return _v;
}

/*--------------------------------------------------------------------------*/
string PARAMETER<string>::my_infty()const{ return "inf"; }
/*--------------------------------------------------------------------------*/
string PARAMETER<string>::value()const {
  trace0(("PARAMETER::std::string " + _s + " -> " + _v ).c_str());
  return to_string(_v);
}
/*--------------------------------------------------------------------------*/
string PARAMETER<string>::string()const {
  trace0(("PARAMETER::std::string " + _s + " -> " + _v ).c_str());
  return to_string(_s);
}
/*--------------------------------------------------------------------------*/
bool PARAMETER<string>::operator==(const PARAMETER& p)const
{
  bool ret= (_v == p._v && _s == p._s );
  trace1("PARAMETER<string>operator== " + _v + " ?= " + p._v 
                             + " and " + _s  + " ?= " + p._s , ret);
  return ret;
}
/*--------------------------------------------------------------------------*/
