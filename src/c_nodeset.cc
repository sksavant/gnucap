/*$Id: c_list.cc,v 1.3 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 *
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
			NODE_BASE* n =0;

			CKT_BASE::_sim->init();

			if(Umatch("v ", what) || Umatch("a ", what) ){
				trace0("CMD_NODESET::do_it v|a");
				name = cmd.ctos();

				paren -= cmd.skip1b(')');

				if (paren) {itested();
					cmd.warn(bWARNING, "need )");
					return;
				}
				n = NODE_BASE::lookup_node(name, Scope);

				if (!n){
					cmd.warn(bWARNING, "no such node: " + name + "\n");
					return; 
				}
				trace1("CMD_NODESET::do_it have node", n->long_label());
			}

			if(what=="V"||what=="v"){
				trace0("CMD_NODESET::do_it v");

				cmd.skip1b('=');
				double v;
				cmd >> v;

				CKT_BASE::_sim->init();

				CKT_NODE* x = dynamic_cast<CKT_NODE*>(n);

				if(x){
					trace3(" setting node",  x->matrix_number(),  _sim->_vdc[ x->matrix_number()], v);
					_sim->_vdc[ x->matrix_number() ] = v;
				} else{
					cmd.warn(bWARNING, " no node "+ name);
				}

			}else if(Umatch("a",what)){
				trace0("CMD_NODESET::do_it a");
				assert(n);

				trace2("CMD_NODESET::do_it 1",what, n->long_label());
				ADP_NODE* a = dynamic_cast<ADP_NODE*>(n);
				trace1("CMD_NODESET::do_it 2",what);
				if (!a){
					cmd.warn(bWARNING, "not an adp node:  " + name + "\n");
					return; 
				}

				cmd.skip1b('=');
				double v;
				cmd >> v;

				//          trace3(" setting node",  x->matrix_number(),  _sim->_vdc[ x->matrix_number()], v);
///				a->reset();
				a->set_tt(v);

			}else{
				incomplete();
				assert(false);

			}


		}
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "nodeset", &p1);
/*--------------------------------------------------------------------------*/
enum NODETYPE {
	nCKT = 1,
	nADP = 2
};

class CMD_DUMP : public CMD {
	void dump(CS&, OMSTREAM out, CARD_LIST*);
	void printv( OMSTREAM _out, const CARD_LIST* scope, unsigned what=nCKT);
	public:
	void do_it(CS& cmd, CARD_LIST* Scope)
	{
		bool _a = false;
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
				|| Get(cmd, "adp",	   &_a)
				|| Get(cmd, "volt{ages}",	   &_v)
				|| _out.outset(cmd)
				;
		}while (cmd.more() && !cmd.stuck(&here));
		trace2("CMD_NODESET::do_it", _a, _v);

		if (!_a && !_v) _v=1;

		//    if spice
		string dot=".";


		if(_a && 0){
			if (! CKT_BASE::_sim->_nstat ) return;
			trace2( "save",  CKT_BASE::_sim->_total_nodes , CKT_BASE::_sim->_adp_nodes );

			//      _out <<  CKT_BASE::_sim->_last_Time << "\n";
			_out <<  "\n";


			// OBSOLETE
			for(ADP_NODE_LIST::const_iterator ii = ADP_NODE_LIST::adp_node_list.begin( );
					ii != ADP_NODE_LIST::adp_node_list.end(); ++ii ) {
				_out << "* " << (*ii)->short_label()<<"\n";
				_out << dot << "nodeset a(" <<  (*ii)->long_label() << ")=" << _sim->_vdc[ (*ii)->m_() + _sim->_total_nodes +1 ]  <<"\n";
			}


		}

		if (_v||_a){
			_out << "*node voltages"<< _v+2*_a<< "\n";
			printv(_out, Scope, _v+2*_a);
		}

		_out.outreset();
	}
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "nodedump|dumpnodes", &p2);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
void CMD_DUMP::printv( OMSTREAM _out, const CARD_LIST* scope, unsigned what){

	const NODE_MAP * nm = scope->nodes();
	for (NODE_MAP::const_iterator i = nm->begin(); i != nm->end(); ++i) {
		if (i->first != "0") {
			stringstream s;
			string prefix="";
			
			if(scope->owner()){
			_out << "* " << scope->owner()->long_label() << " * " << i->second->short_label()<<" " << what<< "\n";
			prefix=         scope->owner()->long_label() + ".";
			}

			CKT_NODE* N=dynamic_cast<CKT_NODE*>(i->second);
			ADP_NODE* A=dynamic_cast<ADP_NODE*>(i->second);


			if(N && (what & nCKT) ){
				_out << ".nodeset  v(" << i->second->long_label() << ")=";
				_out  << setw(8) <<  CKT_BASE::_sim->_vdc[N->matrix_number()];
			}


			if(A && (what & nADP) ){
				trace3("CMD_NODESET::do_it FIXME", prefix, i->second->short_label(), i->second->long_label());
				_out << ".nodeset  a(" << prefix << i->second->short_label() << ")=";
				_out  << setw(8) <<  CKT_BASE::_sim->_vdc[A->m_()+_sim->_total_nodes +1];
			}
			_out <<"\n";

		}else{
			// _out << "Zero Node  "  << "\n";
		}
	}

	for (CARD_LIST::const_iterator i = scope->begin(); i != scope->end(); ++i) {
		const COMPONENT* s = dynamic_cast<const COMPONENT*>(*i);
		if ((*i)->is_device())
			if (s->subckt()) {
				_out << "* -" << s->long_label() <<"\n";
				printv(_out,s->subckt(), what);
			}
	}

}
/*--------------------------------------------------------------------------*/
