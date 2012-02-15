/*$Id: u_nodemap.cc,v 1.1 2009-10-23 12:01:45 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2002 Albert Davis
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
 * node name to number mapping -- for named nodes
 */
//testing=script,complete 2006.07.14
#include "e_node.h"
#include "e_adp.h"
#include "u_nodemap.h"
/*--------------------------------------------------------------------------*/
NODE ground_node("0",0);
/*--------------------------------------------------------------------------*/
NODE_MAP::NODE_MAP()
  : _node_map(), ckt(0), adp(0)
{
  _node_map["0"] = &ground_node;
}
/*--------------------------------------------------------------------------*/
/* copy constructor: deep copy
 * The std::map copy constructor does a shallow copy,
 * then replace second with a deep copy.
 */
NODE_MAP::NODE_MAP(const NODE_MAP& p)
  : _node_map(p._node_map)
{
  unreachable();
  for (iterator i = _node_map.begin(); i != _node_map.end(); ++i) {
    untested();
    CKT_NODE* s=dynamic_cast<CKT_NODE*>(i->second);

    if (i->first != "0" && s) {
      untested();
      assert(i->second);
      i->second = (NODE_BASE*) new CKT_NODE(s);
    }else{
      untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
NODE_MAP::~NODE_MAP()
{
  trace0("NODE_MAP::~NODE_MAP");
  for (iterator i = _node_map.begin(); i != _node_map.end(); ++i) {
    if (i->first != "0") {
      assert(i->second);
      delete i->second;
    }
  }  
}
/*--------------------------------------------------------------------------*/
//#ifdef DO_TRACE FIXME!
// slow/stupid for debugging only!
string NODE_MAP::operator[](unsigned x)const {
  USE(x);

  for (NODE_MAP::const_iterator ni = _node_map.begin(); ni != _node_map.end(); ++ni) {
    NODE_BASE* n = (*ni).second;
    USE(n);
    string label = (*ni).first;

    CKT_NODE* c = dynamic_cast<CKT_NODE*>(n);
    if(c && c->user_number() == x){ return label; };
  }
  assert(false);
  return("erorr");

}
//#endif
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * returns NULL pointer if no match
 */
NODE_BASE* NODE_MAP::operator[](std::string s)
{
  const_iterator i = _node_map.find(s);
  if (i != _node_map.end()) {
    return i->second;
  }else if (OPT::case_insensitive) {
    notstd::to_lower(&s);
    i = _node_map.find(s);
  }else{
    return NULL;
  }
  return (i != _node_map.end()) ? i->second : NULL;
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * creates a new one if it isn't already there.
 */
CKT_NODE* NODE_MAP::new_node(std::string s_in, const CARD_LIST* p)
{  

  std::string::size_type dotplace = s_in.find_last_of(".");
  string s = s_in;
  if(dotplace != std::string::npos){
    untested();
    trace1("NODE_MAP::new_node name too long (BUG).", s_in);
    string s = s_in.substr(dotplace+1, std::string::npos);
  }

  trace1("NODE_MAP::new_node " ,s);
  if (OPT::case_insensitive) {
    notstd::to_lower(&s);
  }else{
  }
  NODE_BASE* node = _node_map[s];
  CKT_NODE* cnode=dynamic_cast<CKT_NODE*>(node);

  if(node && !cnode){
    incomplete(); // type/name collision
    assert(false);
  }

  // increments how_many() when lookup fails (new s)  
  if (!node) {
    node = new CKT_NODE(s, how_many(), p);
    //trace2("NODE_MAP::new_node", s, node->user_number());
    //                 ^^^^ is really the map number of the new node
    ckt++;
    _node_map[s] = dynamic_cast<NODE_BASE*>( node);
  }
  assert(node);
  return dynamic_cast<CKT_NODE*>(node); 
}
/*--------------------------------------------------------------------------*/
ADP_NODE* NODE_MAP::new_adp_node(std::string s, const COMPONENT* p)
{  
  trace1("NODE_MAP::new_node ", s);
  if (OPT::case_insensitive) {
    notstd::to_lower(&s);
  }else{
  }
  NODE_BASE* node = _node_map[s];
  ADP_NODE* anode = dynamic_cast<ADP_NODE*>(node);

  // increments how_many() when lookup fails (new s)  
  if (!node) {
    anode = new ADP_NODE(s, p);
    node = prechecked_cast<NODE_BASE*>(anode);
    assert(node);
    trace2("NODE_MAP::new_adp_node", anode->long_label(),
                     anode->m_());
    //                ^^^^ is really the map number of the new node
    adp++;
    _node_map[s] = node;
  } else {
    trace1("NODE_MAP::new_adp_node already there.", anode->long_label());
  }
  assert(_node_map[s]);
  return anode;
}
/*--------------------------------------------------------------------------*/
