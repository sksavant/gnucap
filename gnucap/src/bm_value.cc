/*$Id: bm_value.cc,v 26.110 2009/05/28 15:32:04 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * behavioral modeling simple value
 * used with tc, etc, and conditionals
 */
//testing=script 2005.10.07
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_value (0);
/*--------------------------------------------------------------------------*/
static EVAL_BM_VALUE p1(CC_STATIC);
static DISPATCHER<COMMON_COMPONENT>::INSTALL
d1(&bm_dispatcher, "value", &p1);
/*--------------------------------------------------------------------------*/
EVAL_BM_VALUE::EVAL_BM_VALUE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _value(0)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_VALUE::EVAL_BM_VALUE(const EVAL_BM_VALUE& p)
  :EVAL_BM_ACTION_BASE(p),
   _value(p._value)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_VALUE* p = dynamic_cast<const EVAL_BM_VALUE*>(&x);
  bool rv = p
    && _value == p->_value
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    incomplete();
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << _value;
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::precalc(const CARD_LIST* Scope)
{
  assert(Scope);

  EVAL_BM_ACTION_BASE::precalc(Scope);
  if (modelname() != "") {
    _value = modelname();
  }
  _value.e_val(_default_value, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::parse_numlist(CS& cmd)
{
  unsigned here = cmd.cursor();
  PARAMETER<double> new_value(NOT_VALID);
  cmd >> new_value;
  if (cmd.gotit(here)) {
    _value = new_value;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "=", &_value)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
