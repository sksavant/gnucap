/*$Id: measure_eval.cc,v 1.3 2010-09-17 12:26:00 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et
 * Copyright (C) 2008 Albert Davis
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
#include "u_parameter.h"
//#include "globals.h"
//#include "m_wave.h"
#include "u_function.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class MEASURE : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const
  {
    trace0("eval");
    PARAMETER<double> arg;
    Cmd >> arg;
    arg.e_val(BIGBIG, Scope);
     // std::cout << to_string(double(arg));
    return to_string(double(arg));
  }
} p1;
DISPATCHER<FUNCTION>::INSTALL d1(&measure_dispatcher, "eval", &p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
