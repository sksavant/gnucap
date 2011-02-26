/*$Id: c_list.cc,v 1.3 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
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
 * list and save commands.
 * save is list with direction to file
 */
//testing=script 2006.07.17
#include "e_cardlist.h"
#include "u_lang.h"
#include "c_comand.h"
#include "globals.h"
#include "e_node.h"
#include <iostream>
#include <fstream>
#include <string>
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
void volts_load( fstream *in, CARD_LIST* scope)
{
  // fixme. -nan
  std::string inss;
  double ind;
  uint_t i=1;
  double Last;
  *in >> Last;
  trace1("volts_load", Last);
  assert(CKT_BASE::_sim->_vdc);
  CKT_BASE::_sim->_last_Time = Last;
  while ( ! (in->eof() ) ){
    assert( i< CKT_BASE::_sim->_total_nodes +2 + CKT_BASE::_sim->_adp_nodes );
    *in >> skipws;

    if( '\n' ==  in->peek()
       ||' ' ==  in->peek() ){
      in->get();
    } else if( 'n' ==  in->peek() ){
      *in >> inss >> skipws;
      if(inss=="nan"){
        trace1(" putting nan ", i);
        CKT_BASE::_sim->_vdc[i] = NAN;
        i++;
      }
    } else if(*in >> ind){
      *in >> skipws;
      trace2(" putting ", i, ind);
      CKT_BASE::_sim->_vdc[i] = ind;
      ++i;
    } else {
      //*in >> skipws >> inss;
      inss = in->get();
      trace1((" garbage ->" + inss + "<- ").c_str(), (int)inss[0]);
    }
  }
}
/*--------------------------------------------------------------------------*/
void volts_save(CS& cmd, OMSTREAM out, CARD_LIST* scope)
{
  CARD_LIST::card_list.precalc_first();

  //out.setfloatwidth(7);
  switch (ENV::run_mode) {
  case rPRE_MAIN:
    unreachable();
    return;
  case rPRESET:
    /* do nothing */
    return;
  case rBATCH:		itested();
  case rINTERACTIVE:	itested();
  case rSCRIPT:
    /* keep going */
    break;
  }

  if (!OPT::language) {
    throw Exception("no language");
  }else{
  }

  if (! CKT_BASE::_sim->_nstat ) return;
  trace2( "save",  CKT_BASE::_sim->_total_nodes , CKT_BASE::_sim->_adp_nodes );
  
  out <<  CKT_BASE::_sim->_last_Time << "\n";

  for ( int i = 1;  CKT_BASE::_sim->_total_nodes + 1 + CKT_BASE::_sim->_adp_nodes > i ; ++i){
    out <<  CKT_BASE::_sim->_vdc[i] << "\n";
  }
}
/*--------------------------------------------------------------------------*/
class CMD_LIST : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    std::string filename;
    cmd >> filename;
    fstream inFile(filename.c_str());
    if (inFile.fail()) {
      cerr << "Unable to open " << filename << " for reading." << endl;
      return;
    }
    volts_load(&inFile, Scope);
    inFile.close();
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "vload", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SAVE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(30);
    out.outset(cmd);
    volts_save(cmd, out, Scope);
    out.outreset();
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "vdump|vsave", &p2);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
