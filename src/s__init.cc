/* $Id: s__init.cc,v 1.4 2009-12-13 17:55:02 felix Exp $
 * vim:ts=8:sw=2:et
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
 * initialization (allocation, node mapping, etc)
 */
//testing=obsolete
#include "u_status.h"
#include "u_sim_data.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
void SIM::command_base(CS& cmd)
{  reset_timers();
  _sim->reset_iteration_counter(_sim->_mode);
  _sim->reset_iteration_counter(iPRINTSTEP);
  
  _sim->init();
  _sim->alloc_vectors();  
  _sim->_aa.reallocate();
  _sim->_aa.dezero(OPT::gmin);
  _sim->_aa.set_min_pivot(OPT::pivtol);  
  _sim->_lu.reallocate();
  _sim->_lu.dezero(OPT::gmin);
  _sim->_lu.set_min_pivot(OPT::pivtol);
  
  assert(_sim->_nstat);
  try {
    setup(cmd);
    ::status.set_up.stop();
    switch (ENV::run_mode) {
    case rPRE_MAIN:	unreachable();	break;
    case rPIPE:         untested();
    case rBATCH:	itested();
    case rINTERACTIVE:	itested();
    case rSCRIPT:	trace0("SIM::command_base calling sweep");	sweep(); break;
    case rPRESET:	/*nothing*/	break;
    }
   }catch (Exception& e) {untested();
    error(bDANGER, e.message() + '\n');
    _sim->count_iterations(iTOTAL);
    _sim->_lu.unallocate();
    _sim->_aa.unallocate();
  }
  _sim->unalloc_vectors();
  finish();

  ::status.total.stop();
}
/*--------------------------------------------------------------------------*/
SIM::~SIM()
{
  _sim->uninit();
}
/*--------------------------------------------------------------------------*/
void SIM::reset_timers()
{
  ::status.advance.reset();
  ::status.queue.reset();
  ::status.evaluate.reset();
  ::status.load.reset();
  ::status.lud.reset();
  ::status.back.reset();
  ::status.review.reset();
  ::status.accept.reset();
  ::status.output.reset();
  ::status.aux1.reset();
  ::status.aux2.reset();
  ::status.aux3.reset();
  ::status.set_up.reset().start();
  ::status.total.reset().start();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
