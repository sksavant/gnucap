/*$Id$ 
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2009 Analog Rails, Inc.
 * Authors: Kevin Cameron, Gennadiy Serdyuk <gserdyuk@analograils.com>
 *
 * This file is a part of plugin "Gnucap-Icarus" to "Gnucap", 
 * the Gnu Circuit Analysis Package
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
 * transient extension - used with extpwl element to integrate Icarus Verilog simulator 
 */
//testing = ??
#include "s_tr.h"
#include "globals.h"
#include "extlib.h"

namespace{

class EXT_TRANSIENT: public TRANSIENT {
public:
    explicit EXT_TRANSIENT(): TRANSIENT(){
      trace0("EXT_TRANSIENT");
    }
    ~EXT_TRANSIENT(){}
public:
  virtual void	first();
  virtual void	accept();
};

void EXT_TRANSIENT::first(){
  TRANSIENT::first();
  trace0("EXT_TRANSIENT::first calling ExtStartSim");
  ExtStartSim("TRAN"); 
}
    
void EXT_TRANSIENT::accept(){
  TRANSIENT::accept();
  trace0("calling ExtContSim");
  ExtContSim("TRAN",_sim->_time0); 
}


static EXT_TRANSIENT p51;
DISPATCHER<CMD>::INSTALL d51(&command_dispatcher, "transientt", &p51);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
