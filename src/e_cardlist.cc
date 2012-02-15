/*$Id: e_cardlist.cc,v 1.14 2010-09-07 07:46:22 felix Exp $ -*- C++ -*-
 * vim:sw=2:ts=8:et:
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
 * Base class for "cards" in the circuit description file
 * This file contains functions that process a list of cards
 */
//testing=script 2006.07.10
#include "u_time_pair.h"
#include "e_node.h"
#include "u_nodemap.h"
#include "e_model.h"
#include "io_node.h"
/*--------------------------------------------------------------------------*/
#define trace_func_comp() trace0((__func__ + (":" + (**ci).long_label())).c_str())
/*--------------------------------------------------------------------------*/
CARD_LIST::CARD_LIST(const CARD* owner)
  :_parent(NULL),
   _nm(new NODE_MAP),
   _params(NULL),
   _language(NULL),
   _owner(owner),
   _origin(NULL)
{
}
/*--------------------------------------------------------------------------*/
CARD_LIST::CARD_LIST(const CARD* model, CARD* owner,
		     const CARD_LIST* scope, PARAM_LIST* p)
  :_parent(NULL),
   _nm(new NODE_MAP),
   _params(NULL),
   _language(NULL),
   _owner(owner),
   _origin(model->subckt())
{
  assert(model);
  assert(model->subckt());
  assert(owner);
  assert(!p || scope);

  attach_params(p, scope);
  shallow_copy(model->subckt());
  set_owner(owner);
  map_subckt_nodes(model, owner);
  // rewire_nodenames(model->subckt());
  // _origin = scope; // hack?

  trace1("CARD_LIST::CARD_LIST owner "+ owner->short_label(), _nm->how_many() );
  trace1("CARD_LIST::CARD_LIST owner "+ owner->short_label(), model->subckt()->nodes()->how_many() );
}
/*--------------------------------------------------------------------------*/
CARD_LIST::~CARD_LIST()
{
  erase_all();
  delete _nm;
  if (!_parent) {
    delete _params;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* CARD_LIST::params()
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* CARD_LIST::params()const
{
  if (_params) {
    return _params;
  }else{ //BUG//const
    static PARAM_LIST empty_params;
    return &empty_params;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST::iterator CARD_LIST::find_again(const std::string& short_name,
					  CARD_LIST::iterator Begin)
{
  return notstd::find_ptr(Begin, end(), short_name);
}
/*--------------------------------------------------------------------------*/
CARD_LIST::const_iterator CARD_LIST::find_again(const std::string& short_name,
						CARD_LIST::const_iterator Begin)const
{
  return notstd::find_ptr(Begin, end(), short_name);
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::erase(iterator ci)
{
  assert(ci != end());
  delete *ci;
  _cl.erase(ci);
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::erase(CARD* c)
{
  delete c;
  _cl.remove(c);
  return *this;
}
/*--------------------------------------------------------------------------*/
/* erase_all: empty the list, destroy contents
 * Beware: something else may be pointing to them, leaving dangling ptr.
 */
CARD_LIST& CARD_LIST::erase_all()
{
  while (!_cl.empty()) {
    delete _cl.back();
    _cl.pop_back();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::set_owner(CARD* owner)
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).set_owner(owner);
  }
  _owner = owner;
  return *this;
}
/*--------------------------------------------------------------------------*/
/* set_slave: set a whole circuit to "slave" mode.
 * Only useful for subckts.
 */
CARD_LIST& CARD_LIST::set_slave()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).set_slave();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* expand: expand (flatten) a list of components (subckts)
 * Scan component list.  Expand each subckt: create actual elements
 * for flat representation to use for simulation.
 */
CARD_LIST& CARD_LIST::expand()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).precalc_first();
  }
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).expand_first();
  }
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).expand();
  }
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).expand_last();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::precalc_first()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).precalc_first();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
//CARD_LIST& CARD_LIST::tt_next()
//{
//  for (iterator ci=begin(); ci!=end(); ++ci) {
//    trace_func_comp();
//    (**ci).tt_next();
//  }
//  return *this;
//}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::precalc_last()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).precalc_last();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* map_nodes: create mapping between user node names and internal numbers
 */
CARD_LIST& CARD_LIST::map_nodes()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).map_nodes();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
unsigned CARD_LIST::adp_nodes()const{
  unsigned ret = nodes()->how_many_adp();
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    if ((*ci)->is_device()) {
      if ((*ci)->subckt()){
        ret+=(*ci)->subckt()->total_nodes();
      }else{
        untested();
      }
    }
  }
  return ret;
}
/*--------------------------------------------------------------------------*/
// FIXME: store nodecound in static CARDLIST::nodecount
// don't recalculate
unsigned CARD_LIST::total_nodes()const{
  unsigned ret = nodes()->how_many_ckt();
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    if ((*ci)->is_device()) {
      if ((*ci)->subckt()){
        ret+=(*ci)->subckt()->total_nodes();
      }else{
        // untested();
      }
    }
  }
  return ret;
}
/*--------------------------------------------------------------------------*/
//void CARD_LIST::init_node_count( unsigned* user_nodes, unsigned* subckt_nodes,
//    unsigned* _model_nodes, unsigned* _adp_nodes) const{
//
//  unsigned ret = nodes()->how_many_ckt();
//  for (const_iterator ci=begin(); ci!=end(); ++ci) {
//    if ((*ci)->is_device()) {
//      if ((*ci)->subckt()){
//        ret+=(*ci)->subckt()->total_nodes();
//      }else{
//        untested();
//      }
//    }
//  }
//
//}
/*--------------------------------------------------------------------------*/
NODE_BASE* CARD_LIST::node(string s) const{
  const COMPONENT* o = dynamic_cast<const COMPONENT*>(owner());
  const CARD_LIST* scope = _origin;
  trace1("CARD_LIST::node",s);

  if (0 && scope){
    trace1("CARD_LIST::node have an origin scope " + s, scope->nodes()->how_many() );
    trace1("",scope);

    NODE_BASE* n = scope->node(s);
    if(n){
      uint_t nn = 0; // n->user_number();
      trace1("CARD_LIST::node found " + s, nn);
      node_t* N = &(o->n_(nn));
      assert(N);

      assert(N->n_()); // fails...

      return N->n_();
    }else{
      trace0("CARD_LIST::node cannot find " + s);
      return NULL;
    }
  }

  trace0("CARD_LIST::node falling back to nodes " + s);
  NODE_MAP* NM=nodes();
  NODE_BASE* ret = (*NM)[s];
  trace1("CARD_LIST::node ", hp(ret));
  return(ret);
}
/*--------------------------------------------------------------------------*/
/* tr_iwant_matrix: allocate solution matrix
 * also sets some flags for mixed-mode
 */
CARD_LIST& CARD_LIST::tr_iwant_matrix()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_iwant_matrix();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_begin: first pass on a new transient simulation (initial DC)
 */
CARD_LIST& CARD_LIST::tr_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_restore: first pass on restarting a transient simulation
 */
CARD_LIST& CARD_LIST::tr_restore()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_restore();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*
 */
CARD_LIST& CARD_LIST::tt_behaviour_commit()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
//    trace_func_comp();
    (**ci).tt_behaviour_commit();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dc_advance: first pass on a new step in a dc sweep
 */
CARD_LIST& CARD_LIST::dc_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).dc_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::stress_apply(  ) 
{
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    (*ci)->stress_apply();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
const CARD_LIST& CARD_LIST::tr_stress_last(  ) const
{
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    (*ci)->tr_stress_last();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* apply things to all cards
 */
const CARD_LIST& CARD_LIST::do_forall( void (CARD::*thing)( ) const  ) const
{
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    ((*ci)->*thing)( );
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* apply things to all cards
 */
CARD_LIST& CARD_LIST::do_forall( void (CARD::*thing)( )  )
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    ((*ci)->*thing)( );
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::do_forall( void (CARD::*thing)( int ), int i  )
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    ((*ci)->*thing)( i );
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_advance: first pass on a new time step
 */
CARD_LIST& CARD_LIST::tr_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::tt_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tt_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_regress: throw away the last result and try again, first pass on redo
 */
CARD_LIST& CARD_LIST::tr_regress()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_regress();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval: determine if anything needs to be evaluated
 */
bool CARD_LIST::tr_needs_eval()const
{
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    if ((**ci).tr_needs_eval()) {
      return true;
    }else{itested();
    }
  }
  return false;
}
/*--------------------------------------------------------------------------*/
/* tr_queue_eval: build evaluator queue
 */
CARD_LIST& CARD_LIST::tr_queue_eval()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_queue_eval();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_eval: evaluate a list of models
 * evaluates a list (or sublist), checks convergence, etc.
 * does not load the matrix
 * argument is the head of the netlist.
 * recursively called to evaluate subcircuits
 */
bool CARD_LIST::do_tr()
{
  bool isconverged = true;
  if (OPT::bypass) {
    for (iterator ci=begin(); ci!=end(); ++ci) {
      trace_func_comp();
      if ((**ci).tr_needs_eval()) {
	isconverged &= (**ci).do_tr();
      }else{
      }
    }
  }else{
    for (iterator ci=begin(); ci!=end(); ++ci) {
      trace_func_comp();
      isconverged &= (**ci).do_tr();
    }
  }
  return isconverged;
}
/*--------------------------------------------------------------------------*/
/* tr_load: load list of models to the matrix
 * recursively called to load subcircuits
 * Called only when either !OPT::traceload or !SIM::inc_mode
 */
CARD_LIST& CARD_LIST::tr_load()
{
  if (CKT_BASE::_sim->is_inc_mode()) {itested();
    assert(!OPT::traceload);
    for (iterator ci=begin(); ci!=end(); ++ci) {itested();
      trace_func_comp();
      CARD* brh = *ci;
      if (!brh->is_constant()) {itested();
	brh->tr_load();
      }else{itested();
      }
    }
  }else{
    for (iterator ci=begin(); ci!=end(); ++ci) {
      trace_func_comp();
      CARD* brh = *ci;
      brh->tr_load();
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::tt_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tt_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR CARD_LIST::tt_review()
{
  TIME_PAIR time_by;
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    time_by.min((**ci).tt_review());
  }
  return time_by;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR CARD_LIST::tr_review()
{
  TIME_PAIR time_by(NEVER,NEVER);

  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    time_by.min((**ci).tr_review());
  }
  return time_by;
}
/*--------------------------------------------------------------------------*/
/* tr_accept: final acceptance of a time step, before moving on
 */
CARD_LIST& CARD_LIST::tr_accept()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_accept();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_unload: remove a list of models from the matrix
 * recursively called to unload subcircuits
 */
CARD_LIST& CARD_LIST::tr_unload()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).tr_unload();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_iwant_matrix: allocate solution matrix
 */
CARD_LIST& CARD_LIST::ac_iwant_matrix()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).ac_iwant_matrix();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_begin: first pass on a new ac simulation
 */
CARD_LIST& CARD_LIST::ac_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).ac_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::do_ac()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    if (!(**ci).evaluated()) {
      (**ci).do_ac();
    }else{
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_load: load list of models to the matrix
 * recursively called to load subcircuits
 */
CARD_LIST& CARD_LIST::ac_load()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    trace_func_comp();
    (**ci).ac_load();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
void CARD_LIST::attach_params(PARAM_LIST* p, const CARD_LIST* scope)
{
  if (p) {
    assert(scope);
    if (_params) {
      delete _params;
      _params = NULL;
    }else{
    }
    _params = new PARAM_LIST;
    _params->eval_copy(*p, scope);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void CARD_LIST::shallow_copy(const CARD_LIST* p)
{
  assert(p);
  _parent = p;
  for (const_iterator ci = p->begin(); ci != p->end(); ++ci) {
    trace_func_comp();
    if ((**ci).is_device() || dynamic_cast<MODEL_CARD*>(*ci)) {
      trace0("CARD_LIST::shallow_copy cloning " + (*ci)->long_label() );
      CARD* copy = (**ci).clone();
      push_back(copy);
    }else{
      trace0("CARD_LIST::shallow_copy not cloning " + (*ci)->long_label() );
    }
  }
}
/*--------------------------------------------------------------------------*/
// set up the map of external to expanded node numbers
void CARD_LIST::map_subckt_nodes(const CARD* model, const CARD* owner)
{
  trace1("CARD_LIST::map_subckt_nodes", owner->long_label());
  assert(model);
  assert(model->subckt());
  assert(model->subckt()->nodes());
  assert(owner);
  //trace1("model nodenames", *(model->subckt()->nodes()));
  //trace1("... now own", *(owner->scope()->nodes())  );
  //trace2("now own sckt", *nodes(), hp(nodes()));
//  NODE_MAP* nm = owner->scope()->nodes();

//:w
//trace_nodenames(owner->subckt());
  //assert(owner->subckt());
  //assert(owner->subckt() == this);

  trace0("model: "+model->long_label());

  uint_t num_nodes_in_subckt = model->subckt()->nodes()->how_many();
  trace2("CARD_LIST::map_subckt_nodes ", owner->long_label(), num_nodes_in_subckt);
  // uint_t* map = new uint_t[num_nodes_in_subckt+1];
  uint_t map[num_nodes_in_subckt+1];
  {
    map[0] = 0;
    // self test: verify that port node numbering is correct
    for (uint_t port = 0; port < model->net_nodes(); ++port) {
      assert(model->n_(port).e_() <= num_nodes_in_subckt);
      //assert(model->n_(port).e_() == port+1);
      trace3("CARD_LIST::map_subckt_nodes ports", 
          port, model->n_(port).e_(), owner->n_(port).t_());
    }
    {
      // take care of the "port" nodes (external connections)
      // map them to what the calling circuit wants
      uint_t i=0;
      for (i=1; i <= model->net_nodes(); ++i) {
	map[i] = owner->n_(i-1).t_();
        trace2("map", i, map[i]);
	// trace3("ports", i, map[i], owner->n_(i-1).t_());
      }
    
      // get new node numbers, and assign them to the remaining
      for (assert(i==model->net_nodes() + 1); i <= num_nodes_in_subckt; ++i) {
        //untested();
	// for each remaining node in card_list
        // these are the internal nodes.
        string label = (*(model->subckt()->nodes())) [i] ; // HACK!
        assert(this);
        unsigned k = CKT_BASE::_sim->_total_nodes;
        owner->n_(i-1).new_sckt_node( label, this); // this correct??
        assert (k+1 ==  CKT_BASE::_sim->_total_nodes);
	//map[i] = CKT_BASE::_sim->newnode_subckt();
        trace2("int map", i, owner->n_(i-1).t_() );
	map[i] = owner->n_(i-1).t_();

        // irgendwie sowas?
	//map[i] = owner->scope()->new_node()
        //owner->n_(i-1).new_sckt_node( label, subckt());

      }
      //trace1("CARD_LIST::map_subckt_nodes done map", *nodes());

    }
  }

  // "map" now contains a translation list,
  // from subckt local numbers to matrix index numbers

  // Device nodes (type node_t) points to the NODE in the parent.
  // Mapping is done in node_t.

  // scan the list, map the nodes
  for (CARD_LIST::iterator ci = begin(); ci != end(); ++ci) {
    if ((**ci).is_device()) {
      for (uint_t ii = 0;  ii < (**ci).net_nodes();  ++ii) {
	// for each connection node in card
        const CARD* c = *ci;
        trace4("CARD_LIST::map_subckt_nodes subdevice node ", c->long_label(),
            c->n_(ii).short_label(),  c->n_(ii).e_() ,  c->n_(ii).t_()  );


        if ( c->n_(ii).t_() > model->net_nodes() ) {
          trace3("INTERNAL:", c->n_(ii).short_label(), c->n_(ii).t_() , map[c->n_(ii).t_()] );
        } else {
          trace1("EXTERNAL", c->n_(ii).short_label());
        }

        c->n_(ii).map_subckt_node(map, owner);
      }
    }else{
      assert(dynamic_cast<MODEL_CARD*>(*ci));
    }
  }
}
/*--------------------------------------------------------------------------*/
///ADP_NODE* CARD_LIST::new_adp_node{
///  assert(d);
///  assert(d->scope());
///
///  NODE_MAP* Map = nodes();
///  assert(Map);
///  ADP_NODE* a Map->new_adp_node(node_name);
///  trace2("CARDLIST::new_adp_node", node_name, _nnn->user_number());
///  assert(_nnn);
///}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
