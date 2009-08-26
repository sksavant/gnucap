/*$Id: c_status.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
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
 * Displays the status of the system.  Makes all the calculations associated
 * with allocating memory but does not actually allocate it, unless necessary
 * to make the rest of the calculations.
 *
 *   If "allocate" is changed, this must also be changed.
 */
//testing=script 2006.07.17
#include "s_tr.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_STATUS : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    IO::mstdout << "Gnucap   System status\n";
    
    if (!cmd.umatch("n{otime} ")) {
      ::status.compute_overhead();
      IO::mstdout
	<< "command ------ last -- total\n"
	<< ::status.get
	<< ::status.op
	<< ::status.dc
	<< ::status.tran
	<< ::status.four
	<< ::status.ac
	<< "function ----- last -- total\n"
	<< ::status.set_up
	<< ::status.order
	<< "function ----- last -- total\n"
	<< ::status.advance
	<< ::status.queue
	<< ::status.evaluate
	<< ::status.load
	<< ::status.lud
	<< ::status.back
	<< ::status.review
	<< ::status.accept
	<< ::status.output
	<< ::status.overhead;
      if (OPT::showall) {itested();
	IO::mstdout 
	  << ::status.aux1
	  << ::status.aux2
	  << ::status.aux3;
      }
      IO::mstdout << ::status.total;
    }
    
    IO::mstdout
      << "iterations: op=" << ::status.iter[s_OP]
      << ", dc=" << ::status.iter[s_DC]
      << ", tran=" << ::status.iter[s_TRAN]
      << ", fourier=" << ::status.iter[s_FOURIER]
      << ", total=" << ::status.iter[iTOTAL]
      << "\n";
    IO::mstdout
      << "transient timesteps: accepted=" << TRANSIENT::steps_accepted()
      << ", rejected=" << TRANSIENT::steps_rejected()
      << ", total=" << TRANSIENT::steps_total()
      << "\n";  
    IO::mstdout
      << "nodes: user=" << ::status.user_nodes
      << ", subckt=" << ::status.subckt_nodes
      << ", model=" << ::status.model_nodes
      << ", total=" << ::status.total_nodes
      << "\n";  
    IO::mstdout.form("dctran density=%.1f%%, ac density=%.1f%%\n",
		 aa.density()*100., acx.density()*100.);
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "status", &p);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
