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
#include "u_nodemap.h"
#include "e_adp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
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
    cmd.skip1b('=');
    double v;
    cmd >> v;

    CKT_BASE::_sim->init();

    NODE_BASE* n = NODE_BASE::lookup_node(name, Scope);

    NODE* x = dynamic_cast<NODE*>(n);
    ADP_NODE* a = dynamic_cast<ADP_NODE*>(n);

    if(x){
      trace3(" setting node",  x->matrix_number(),  _sim->_vdc[ x->matrix_number()], v);
      _sim->_vdc[ x->matrix_number() ] = v;
    } else if(a){
      incomplete();
    } else{
      cmd.warn(bWARNING, " no node "+ name);
    }


  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "nodeset", &p1);
/*--------------------------------------------------------------------------*/
class CMD_DUMP : public CMD {
  void dump(CS&, OMSTREAM out, CARD_LIST*);
  void printv( OMSTREAM _out, const CARD_LIST* scope);
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    bool _adp = false;
    bool _v = false;

    OMSTREAM _out = IO::mstdout;
    _out.setfloatwidth(30);

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
    unsigned here = cmd.cursor();
    do{
      ONE_OF
        || Get(cmd, "adp",	   &_adp)
        || Get(cmd, "voltages",	   &_v)
        || _out.outset(cmd)
        ;
    }while (cmd.more() && !cmd.stuck(&here));

    if (!_adp && !_v) _v=1;

//    if spice
    string dot=".";


    if(_adp){
      if (! CKT_BASE::_sim->_nstat ) return;
      trace2( "save",  CKT_BASE::_sim->_total_nodes , CKT_BASE::_sim->_adp_nodes );

      _out <<  CKT_BASE::_sim->_last_Time << "\n";

      for(ADP_NODE_LIST::const_iterator ii = ADP_NODE_LIST::adp_node_list.begin( );
            ii != ADP_NODE_LIST::adp_node_list.end(); ++ii ) {
        _out << dot << "nodeset a(" <<  (*ii)->long_label() << ")=" << _sim->_vdc[ (*ii)->m_() + _sim->_total_nodes +1 ]  <<"\n";
      }


    }

    if (_v){
      _out << "*node voltages\n";
      printv(_out, Scope);
    }

    _out.outreset();
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "nodedump", &p2);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
void CMD_DUMP::printv( OMSTREAM _out, const CARD_LIST* scope){

  const NODE_MAP * nm = scope->nodes();
  for (NODE_MAP::const_iterator i = nm->begin(); i != nm->end(); ++i) {
    if (i->first != "0") {
      stringstream s;
      _out << ".nodeset v(" << i->second->long_label() << ")="
       << setw(8) <<  CKT_BASE::_sim->_vdc[i->second->matrix_number()] <<"\n";
    }else{
      // _out << "Zero Node  "  << "\n";
    }
  }

  for (CARD_LIST::const_iterator i = scope->begin(); i != scope->end(); ++i) {
    const COMPONENT* s = dynamic_cast<const COMPONENT*>(*i);
    if ((*i)->is_device())
    if (s->subckt()) {
      _out << "-" << s->long_label() <<"\n";
      printv(_out,s->subckt());
    }
  }

}
/*--------------------------------------------------------------------------*/
