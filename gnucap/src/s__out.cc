/*$Id: s__out.cc,v 26.110 2009/05/28 15:32:04 al Exp $ -*- C++ -*-
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
 * tr,dc analysis output functions (and some ac)
 */
//testing=obsolete,script 2005.09.17
#include "m_wave.h"
#include "u_prblst.h"
#include "declare.h"	/* plottr, plopen */
#include "s__.h"
/*--------------------------------------------------------------------------*/
/* SIM::____list: access probe lists
 */
const PROBELIST& SIM::alarmlist()const
{
  return PROBE_LISTS::alarm[_mode];
}
const PROBELIST& SIM::plotlist()const
{
  return PROBE_LISTS::plot[_mode];
}
const PROBELIST& SIM::printlist()const
{
  return PROBE_LISTS::print[_mode];
}
const PROBELIST& SIM::storelist()const
{
  return PROBE_LISTS::store[_mode];
}

WAVE* SIM::waves = NULL;
/*--------------------------------------------------------------------------*/
/* SIM::out: output the data, "keep" for ac reference
 */
void SIM::outdata(double x)
{
  ::status.output.start();
  plottr(x, plotlist());
  print_results(x);
  alarm();
  store_results(x);
  reset_iteration_counter(iPRINTSTEP);
  ::status.hidden_steps = 0;
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
/* SIM::head: print column headings and draw plot borders
 */
void SIM::head(double start, double stop, const std::string& col1)
{
  if (waves) {
    delete [] waves;
  }else{
  }

  waves = new WAVE [storelist().size()];


  if (!plopen(start, stop, plotlist())) {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);

    _out.form(format, '#', col1.c_str());

    for (PROBELIST::const_iterator
	   p=printlist().begin();  p!=printlist().end();  ++p) {
      _out.form(format, ' ', p->label().c_str());
    }
    _out << '\n';
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::print_results: print the list of results (text form) to _out
 * The argument is the first column (independent variable, aka "x")
 */
void SIM::print_results(double x)
{
  if (!IO::plotout.any()) {
    _out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    assert(x != NOT_VALID);
    _out << x;
    for (PROBELIST::const_iterator
	   p=printlist().begin();  p!=printlist().end();  ++p) {
      _out << p->value();
    }
    _out << '\n';
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::alarm: print a message when a probe is out of range
 */
void SIM::alarm(void)
{
  _out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
  for (PROBELIST::const_iterator
	 p=alarmlist().begin();  p!=alarmlist().end();  ++p) {
    if (!p->in_range()) {
      _out << p->label() << '=' << p->value() << '\n';
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::store: store data in preparation for post processing
 */
void SIM::store_results(double x)
{
  int ii = 0;
  for (PROBELIST::const_iterator
	 p=storelist().begin();  p!=storelist().end();  ++p) {
    waves[ii++].push(x, p->value());
  }
}
/*--------------------------------------------------------------------------*/
WAVE* SIM::find_wave(const std::string& probe_name)
{
  int ii = 0;
  for (PROBELIST::const_iterator
       p  = PROBE_LISTS::store[CKT_BASE::_mode].begin();
       p != PROBE_LISTS::store[CKT_BASE::_mode].end();
       ++p) {
    if (wmatch(p->label(), probe_name)) {
      return &(waves[ii]);
    }else{
    }
    ++ii;
  }
  return NULL;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
