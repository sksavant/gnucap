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
#include "d_subckt.h"
#include "e_subckt.h"
#include "u_nodemap.h"
#include "io_matrix.h"
#include "u_sim_data.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
void volts_load( fstream *in, CARD_LIST* )
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
      inss = (char) in->get();
      trace1((" garbage ->" + inss + "<- ").c_str(), (int)inss[0]);
    }
  }
}
/*--------------------------------------------------------------------------*/
void volts_save(CS&, OMSTREAM _out, CARD_LIST*)
{
  CARD_LIST::card_list.precalc_first();

  //_out.setfloatwidth(7);
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
  
  _out <<  CKT_BASE::_sim->_last_Time << "\n";

  for ( uint_t i = 1;  CKT_BASE::_sim->_total_nodes + 1 + CKT_BASE::_sim->_adp_nodes > i ; ++i){
    _out <<  CKT_BASE::_sim->_vdc[i] << "\n";
  }
  const NODE_MAP * nm = CARD_LIST::card_list.nodes();
  for (NODE_MAP::const_iterator i = nm->begin(); i != nm->end(); ++i) {
    if (i->first != "0") {
      _out << i->second->long_label() << " vector position " << 
        ", m_ " << i->second->m_() << " , matrix " << i->second->matrix_number() 
          << ", use number " << i->second->user_number() << 
         " x-Entry " <<  CKT_BASE::_sim->_vdc[i->second->matrix_number()] <<"\n";
    }else{
      _out << "Zero Node  "  << "\n";
    }
  }

  _out << CKT_BASE::_sim->_aa << "\n";
  _out << CKT_BASE::_sim->_lu << "\n";
  // cout << CKT_BASE::_sim->_acx << "\n";

  _out << "----------------------------------------- \n";
  //  out << CKT_BASE::_sim->_acx;

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
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "vloaddeb", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SAVE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(3);
    out.outset(cmd);
    volts_save(cmd, out, Scope);
    out.outreset();
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "vdeb", &p2);
/*--------------------------------------------------------------------------*/
class CMD_AA : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* )
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(3);
    out.outset(cmd);

    // volts_save(cmd, out, Scope);
    //
    out << CKT_BASE::_sim->_aa << "\n";
    out.outreset();
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "aa", &p3);
/*--------------------------------------------------------------------------*/
class CMD_LU : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* )
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(3);
    out.outset(cmd);

    // volts_save(cmd, out, Scope);
    //
    out << CKT_BASE::_sim->_lu << "\n";
    out.outreset();
  }
} p4;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "lu", &p4);
/*--------------------------------------------------------------------------*/
class CMD_MSTAT: public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* )
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(30);
    out.outset(cmd);

    CKT_BASE::_sim->init();
    for(unsigned i=0; i < _sim->_total_nodes; ++i){
      for(unsigned j=0; j < _sim->_total_nodes; ++j){
        assert(_sim->_aa.n(i,j)==_sim->_lu.n(i,j));
        out << ((_sim->_aa.n(i,j))?"*":"0");
      }
      out << "\n";
    }

    out.outreset();
  }
} p5k;
DISPATCHER<CMD>::INSTALL d5k(&command_dispatcher, "mstat", &p5k);
/*--------------------------------------------------------------------------*/
class CMD_ACX: public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* )
  {
    OMSTREAM out = IO::mstdout;
    out.setfloatwidth(30);
    out.outset(cmd);

    out <<  "this doesnt make sense. run .ac with \"dm\" flag instead\n";
    out.outreset();
  }
} p5;
DISPATCHER<CMD>::INSTALL d5(&command_dispatcher, "acx", &p5);
/*--------------------------------------------------------------------------*/
/*----*/
class CMD_NL : public CMD {
public:
  void print(OMSTREAM, const CARD_LIST*);
  void do_it(CS& cmd, CARD_LIST* )
  {
    OMSTREAM _out = IO::mstdout;
    _out.setfloatwidth(3);
    _out.outset(cmd);

    _out << "name   ...\n";

    CKT_BASE::_sim->init();

    _out << "have " << _sim->_total_nodes 
      << " nodes. model: " << _sim->_model_nodes 
      << " sckt: " << _sim->_subckt_nodes
      << "\n";
    for(unsigned k=0; k<= _sim->_total_nodes; ++k)
      // _out << CKT_BASE::_sim->_vdc[k]  << ",";
      _out << k << " " << _sim->_nm[k] <<"\n";
    _out << "\n";
    print(_out, &CARD_LIST::card_list);
    _out.outreset();
  }
} p6;
DISPATCHER<CMD>::INSTALL d6(&command_dispatcher, "nodelist|listnodes", &p6);
/*--------------------------------------------------------------------------*/
void CMD_NL::print( OMSTREAM _out, const CARD_LIST* scope){

  const NODE_MAP * nm = scope->nodes();
  for (NODE_MAP::const_iterator i = nm->begin(); i != nm->end(); ++i) {
    if (i->first != "0") {
      stringstream s;
      _out << "|";
      s << setw(8) << i->second->long_label();
      _out << s.str();
      
      _out << " , matrix_number " 
        << i->second->matrix_number() << " (" <<  i->second->m_() << 
        "), user_number " << i->second->user_number() << " nm[t] " <<
        " vdc " <<  CKT_BASE::_sim->_vdc[i->second->matrix_number()] <<"\n";
    }else{
      // _out << "Zero Node  "  << "\n";
    }
  }

  for (CARD_LIST::const_iterator i = scope->begin(); i != scope->end(); ++i) {
    const MODEL_SUBCKT* m = dynamic_cast<const MODEL_SUBCKT*>(*i);
    const COMPONENT* s = dynamic_cast<const COMPONENT*>(*i);
    //const CARD* s=*i;
    if (!m && s)
    if (s->subckt()) {
      _out << "-" << s->long_label() <<"\n";
      print(_out,s->subckt());
    }
  }

}

/*--------------------------------------------------------------------------*/
class CMD_I : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* )
  {
    OMSTREAM _out = IO::mstdout;
    _out.setfloatwidth(3);
    _out.outset(cmd);
    _out << "listing i\n";
    if(_sim->_i){

    for ( uint_t i = 1;  CKT_BASE::_sim->_total_nodes + 1 + CKT_BASE::_sim->_adp_nodes > i ; ++i){
      _out << i;
      _out <<  CKT_BASE::_sim->_i[i] << "\n";
    }
    }else{
      _out<< " there are no currents\n";
    }
    _out.outreset();
  }
} p7;
DISPATCHER<CMD>::INSTALL d7(&command_dispatcher, "ii", &p7);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
