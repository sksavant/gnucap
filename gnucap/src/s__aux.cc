/*$Id: s__aux.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * aux functions associated with the SIM class
 */
//testing=script 2006.07.14
#include "s__.h"
/*--------------------------------------------------------------------------*/
void SIM::set_limit()
{
  for (int ii = 1;  ii <= ::status.total_nodes;  ++ii) {
    CKT_BASE::set_limit(v0[ii]);
  }
}
/*--------------------------------------------------------------------------*/
void SIM::keep_voltages()
{
  if (!freezetime) {
    for (int ii = 1;  ii <= ::status.total_nodes;  ++ii) {
      vdc[ii] = v0[ii];
    }
    last_time = (time0 > 0.) ? time0 : 0.;
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
void SIM::restore_voltages()
{
  for (int ii = 1;  ii <= ::status.total_nodes;  ++ii) {
    vt1[ii] = v0[ii] = vdc[ii];
  }
}
/*--------------------------------------------------------------------------*/
void SIM::zero_voltages()
{
  for (int ii = 1;  ii <= ::status.total_nodes;  ++ii) {
    vt1[ii] = v0[ii] = vdc[ii] = i[ii] = 0.;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
