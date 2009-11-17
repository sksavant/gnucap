/*$Id: e_card.cc,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
 */
//testing=script 2006.07.12
#include "globals.h"
#include "ap.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 4;
/*--------------------------------------------------------------------------*/
CARD::CARD()
  :CKT_BASE(),
   _evaliter(-100),
   _subckt(0),
   _owner(0),
   _n(0),
   _constant(false),
   _net_nodes(0)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const CARD& p)
  :CKT_BASE(p),
   _evaliter(-100),
   _subckt(0), //BUG// isn't this supposed to copy????
   _owner(0),
   _n(0),
   _constant(p._constant),
   _net_nodes(p._net_nodes)
{
}
/*--------------------------------------------------------------------------*/
const std::string CARD::long_label()const
{
  std::string buffer(short_label());
  for (const CARD* brh = owner(); exists(brh); brh = brh->owner()) {
    //buffer += '.' + brh->short_label();
    buffer = brh->short_label() + '.' + buffer;
  }
  return buffer;
}
/*--------------------------------------------------------------------------*/
/*static*/ double CARD::probe(const CARD *This, const std::string& what)
{
  if (exists(This)) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
/* connects_to: does this part connect to this node?
 * input: a node
 * returns: how many times this part connects to it.
 * does not traverse subcircuits
 */
int CARD::connects_to(const node_t& node)const
{
  untested();
  int count = 0;
  if (is_device()) {
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      untested();
      if (node.n_() == _n[ii].n_()) {
        ++count;
      }else{untested();
      }
    }
  }else{untested();
  }
  return count;
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::scope()
{
  if (owner()) {
    return owner()->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
const CARD_LIST* CARD::scope()const
{
  if (owner()) {
    return owner()->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception if can't find.
 */
CARD* CARD::find_in_my_scope(const std::string& name)
{
  assert(name != "");
  assert(scope());

  CARD_LIST::iterator i = scope()->find_(name);
  if (i == scope()->end()) {
    throw Exception_Cant_Find(long_label(), name,
			      ((owner()) ? owner()->long_label() : "(root)"));
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception if can't find.
 */
const CARD* CARD::find_in_my_scope(const std::string& name)const
{
  assert(name != "");
  assert(scope());

  CARD_LIST::const_iterator i = scope()->find_(name);
  if (i == scope()->end()) {
    throw Exception_Cant_Find(long_label(), name,
			      ((owner()) ? owner()->long_label() : "(root)"));
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_in_parent_scope: find in parent's scope
 * parent is what my scope is a copy of.
 * capable of finding my parent, who should be just like me.
 * If there is no parent (I'm an original), use my scope.
 * throws exception if can't find.
 */
const CARD* CARD::find_in_parent_scope(const std::string& name)const
{
  assert(name != "");
  const CARD_LIST* p_scope = (scope()->parent()) ? scope()->parent() : scope();

  CARD_LIST::const_iterator i = p_scope->find_(name);
  if (i == p_scope->end()) {
    throw Exception_Cant_Find(long_label(), name);
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_looking_out: find in my or enclosing scope
 * capable of finding me, or anything back to root.
 * throws exception if can't find.
 */
const CARD* CARD::find_looking_out(const std::string& name)const
{
  try {
    return find_in_parent_scope(name);
  }catch (Exception_Cant_Find&) {
    if (owner()) {
      return owner()->find_looking_out(name);
    }else if (makes_own_scope()) {
      // probably a subckt or "module"
      CARD_LIST::const_iterator i = CARD_LIST::card_list.find_(name);
      if (i != CARD_LIST::card_list.end()) {untested();
	return *i;
      }else{
	throw;
      }
    }else{
      throw;
    }
  }
}
/*--------------------------------------------------------------------------*/
bool CARD::node_is_grounded(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].is_grounded();
}
/*--------------------------------------------------------------------------*/
bool CARD::node_is_connected(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].is_connected();
}
/*--------------------------------------------------------------------------*/
bool CARD::is_first_expand()const
{
  return !nstat;
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt()
{
  delete _subckt;
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt(const CARD* Model, CARD* Owner,
		      const CARD_LIST* Scope, PARAM_LIST* Params)
{
  delete _subckt;
  _subckt = new CARD_LIST(Model, Owner, Scope, Params);
}
/*--------------------------------------------------------------------------*/
void CARD::renew_subckt(const CARD* Model, CARD* Owner,
		      const CARD_LIST* Scope, PARAM_LIST* Params)
{
  if (is_first_expand()) {
    new_subckt(Model, Owner, Scope, Params);
  }else{untested();
    assert(subckt());
    subckt()->attach_params(Params, scope());
  }
}
/*--------------------------------------------------------------------------*/
node_t& CARD::n_(int i)const
{
  return _n[i];
}
/*--------------------------------------------------------------------------*/
void CARD::set_param_by_name(std::string Name, std::string Value)
{
  //BUG// ugly linear search
  for (int i = param_count() - 1;  i >= 0;  --i) {
    for (int j = 0;  param_name(i,j) != "";  ++j) { // multiple names
      if (Umatch(Name, param_name(i,j) + ' ')) {
	set_param_by_index(i, Value, 0/*offset*/);
	return; //success
      }else{
	//keep looking
      }
    }
  }
  throw Exception_No_Match(Name);
}
/*--------------------------------------------------------------------------*/
/* set_dev_type: Attempt to change the type of an existing device.
 * Usually, it just throws an exception, unless there is no change.
 * Practical use is to override, so you can set things like NPN vs. PNP.
 */
void CARD::set_dev_type(const std::string& New_Type)
{
  if (!Umatch(New_Type, dev_type() + ' ')) {itested();
    //throw Exception_Cant_Set_Type(dev_type(), New_Type);
  }else{
    // it matches -- ok.
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
