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
#include "e_adp.h"
#include <iostream>
#include <fstream>
#include <string>
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_NODESET : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {

    std::string what(cmd.ctos(TOKENTERM));/* parameter */
        
    unsigned paren = cmd.skip1b('(');
    string name;

    if(what=="V"||what=="v"){
        name=  cmd.ctos();
        trace1("CMD_NODESET::do_it", name);

    }else{
      incomplete();
      assert(false);

    }

    paren -= cmd.skip1b(')');

    if (paren) {itested();
      cmd.warn(bWARNING, "need )");
      return;
    }
    double v;
    cmd >> v;

    CKT_BASE::_sim->init();

    NODE_BASE* n = NODE_BASE::lookup_node(name, Scope);

    NODE* x = dynamic_cast<NODE*>(n);
    ADP_NODE* a = dynamic_cast<ADP_NODE*>(n);

    if(x){
      _sim->_vdc[ x->matrix_number() ] = 0;
    } else if(a){
      incomplete();

    }





  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "nodeset", &p1);
/*--------------------------------------------------------------------------*/
class CMD_DUMP : public CMD {
  void dump(CS&, OMSTREAM out, CARD_LIST*);
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(30);
    out.outset(cmd);
    dump(cmd, out, Scope);
    out.outreset();
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "nodedump", &p2);
/*--------------------------------------------------------------------------*/
void CMD_DUMP::dump(CS&, OMSTREAM out, CARD_LIST*)
{
//  CARD_LIST::card_list.precalc_first();

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

  for ( uint_t i = 1;  CKT_BASE::_sim->_total_nodes + 1 + CKT_BASE::_sim->_adp_nodes > i ; ++i){
    out <<  CKT_BASE::_sim->_vdc[i] << "\n";
  }
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
