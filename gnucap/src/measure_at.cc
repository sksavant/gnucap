/*$Id: measure_at.cc,v 26.113 2009/08/12 03:37:19 al Exp $ -*- C++ -*-
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
#include "s__.h"
#include "m_wave.h"
#include "u_function.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class MEASURE : public FUNCTION {
public:
  std::string eval(CS& Cmd, CARD_LIST* Scope)const
  {
    std::string probe_name;
    PARAMETER<double> x;
    bool derivative = false;

    unsigned here = Cmd.cursor();
    Cmd >> probe_name;
    WAVE* w = SIM::find_wave(probe_name);

    if (!w) {
      Cmd.reset(here);
    }else{
    }

    here = Cmd.cursor();
    do {
      ONE_OF
	|| Get(Cmd, "probe",	    &probe_name)
	|| Get(Cmd, "x",	    &x)
	|| Get(Cmd, "at",	    &x)
	|| Get(Cmd, "deriv{ative}", &derivative)
	;
    }while (Cmd.more() && !Cmd.stuck(&here));

    if (!w) {
      w = SIM::find_wave(probe_name);
    }else{
    }
    
    if (w) {
      x.e_val(BIGBIG, Scope);
      return to_string((derivative) ? (w->v_out(x).f1) : (w->v_out(x).f0));
    }else{
      throw Exception_No_Match(probe_name);
    }
  }
} p1;
DISPATCHER<FUNCTION>::INSTALL d1(&measure_dispatcher, "at", &p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
