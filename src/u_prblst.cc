/*$Id: u_prblst.cc,v 1.9 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
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
 * probe list functions
 */
//testing=script,sparse 2006.07.14
#include "e_cardlist.h"
#include "e_node.h"
#include "e_card.h"
#include "u_nodemap.h"
#include "ap.h"
#include "u_prblst.h"
#include "u_probe.h"
/*--------------------------------------------------------------------------*/
void PROBE_LISTS::purge(CKT_BASE* brh)
{
  for (int i = 0;  i < sCOUNT;  ++i) {
    alarm[i].remove_one(brh);
    plot[i] .remove_one(brh);
    print[i].remove_one(brh);
    store[i].remove_one(brh);
  }
}
/*--------------------------------------------------------------------------*/
void PROBELIST::listing(const std::string& label)const
{
  IO::mstdout.form("%-7s", label.c_str());
  for (const_iterator p = begin();  p != end();  ++p) {
    IO::mstdout << ' ' << (*p)->label();
    if ((*p)->range() != 0.) {untested();
      IO::mstdout.setfloatwidth(5) 
	<< '(' << (*p)->lo() << ',' << (*p)->hi() << ')';
    }else{
    }
  }
  IO::mstdout << '\n';
}
/*--------------------------------------------------------------------------*/
PROBELIST::~PROBELIST(){
        clear();
}
/*--------------------------------------------------------------------------*/
void PROBELIST::clear(void)
{
  erase(begin(), end());
}
/*--------------------------------------------------------------------------*/
void PROBELIST::erase( PROBELIST::iterator b, PROBELIST::iterator e )
{
  for (iterator i = b;  i != e;  ++i) {
    trace0("PROBELIST::erase " );
    assert (*i);
    trace0(("PROBELIST::erase " + (*i)->label()).c_str());
    delete(*i);
    //b++;
    trace0("PROBELIST::erase looop ");
  }
  bag.erase(b,e);
}
/*--------------------------------------------------------------------------*/
/* check for match
 * called by STL remove, below
 * both are needed to support different versions of STL
 */
bool operator==(const PROBE* prb, const std::string& par)
{ return *prb == par; }
bool operator!=(const PROBE* prb, const std::string& par)
{ return *prb != par; }
bool operator==(const PROBE* prb, const CKT_BASE& brh)
{ return *prb == brh; }
bool operator!=(const PROBE* prb, const CKT_BASE& brh)
{ return *prb != brh; }

/*--------------------------------------------------------------------------*/
/* remove a complete probe, extract from CS
 * wild card match  ex:  vds(m*)
 */
void PROBELIST::remove_list(CS& cmd)
{ 
  unsigned mark = cmd.cursor();
  std::string parameter(cmd.ctos(TOKENTERM) + '(');
  int paren = cmd.skip1b('(');
  parameter += cmd.ctos(TOKENTERM) + ')';
  paren -= cmd.skip1b(')');
  if (paren != 0) {untested();
    cmd.warn(bWARNING, "need )");
  }else if (parameter.empty()) {untested();
    cmd.warn(bWARNING, "what's this?");
  }else{
  }

  int weg=0;

  iterator x = end();
  for (iterator p = begin();  p != end(); ) {
    assert (*p);
    if ((**p)==parameter){
      // x = remove(begin(), end(), *p);
      weg++;
      // trace1(("PROBELIST::remove_list erasing " + parameter + (*p)->label()).c_str(), i-bag.size() );
      delete(*p);
      p=bag.erase( p);
    } else {
      ++p;}
  }    

  // iterator x = remove(begin(), end(), parameter);
  if (weg!=0) {
    trace0(("PROBELIST::remove_list erased " + parameter ).c_str() );
    // delete *x; ?
   //  erase(x, end());
  }else{itested();
    cmd.warn(bWARNING, mark, "probe isn't set -- can't remove");
  }
  trace1("PROBELIST::remove_list", bag.size());
}
/*--------------------------------------------------------------------------*/
/* check for match
 * called by STL remove, below
 * both are needed to support different versions of stl
 */
/*--------------------------------------------------------------------------*/
/* remove a brh from a PROBELIST
 * removes all probes on brh
 */
void PROBELIST::remove_one(CKT_BASE *brh)
{
  // trace0("PROBELIST::remove_one");
  assert(brh);
  iterator x = remove(begin(), end(), *brh);

  erase( x, end());
}
/*--------------------------------------------------------------------------*/
/* add_list: add a "list" of probes, usually only one
 * This means possibly several probes with a single parameter
 * like "v(r*)" meaning all resistors
 * but not "v(r4) v(r5)" which has two parameters.
 * It also takes care of setting the range for plot or alarm.
 */
PROBE* PROBELIST::add_list(CS& cmd)
{
  CARD_LIST* scope = &CARD_LIST::card_list;
  trace0("PROBELIST::add_list() ");
  PROBE* found_something = NULL;
  int oldcount = size();
  std::string what(cmd.ctos(TOKENTERM));/* parameter */
  trace0( ( "PROBELIST::add_list " + what ).c_str());

  if (what.empty()) {untested();
    cmd.warn(bWARNING, "need a probe");
  }else{
  }


  MATH_OP op;
  int paren;
  if ( (bool) ( op = strtotype(what) ) ) {

        // switch arity...

    trace2( ( "PROBELIST::add_list MATH_OP " + what ).c_str(), cmd.cursor(), (double) op);
    int paren = cmd.skip1b('(');		/* device, node, etc. */

    found_something = add_expr(what,op,&CARD_LIST::card_list, cmd, *this);

    //trace << "pushing back mathop "<< what << " \n";
    bag.push_back( found_something ); //FIXME
    paren -= cmd.skip1b(')');

  } else if( ( paren = cmd.skip1b('(')) ) {
    trace1( ( "PROBELIST::add_list "), paren );
    if (cmd.umatch("nodes ")) {
      // all nodes
      add_all_nodes(what);
    }else if (what == "meas") {
      std::string meas_descr;
      cmd >> meas_descr ; 
     // if (!cmd.skip1b(')')) {
     //   cmd.warn(bWARNING, "add_list: need )");
     // }
//        cmd.warn(bWARNING, "add_list: need )?");

      trace0( ("PROBELIST::add_list measurement: (for tt only)" + meas_descr).c_str());

      MEAS_PROBE* p = new MEAS_PROBE(meas_descr, &CARD_LIST::card_list);
      bag.push_back(p);

    }else if (cmd.umatch("0")) {
      // node 0 means system stuff
      push_new_probe(what, 0);
    }else if (cmd.is_alnum() || cmd.match1("*?")) {
      // branches or named nodes
      unsigned here1 = cmd.cursor();
      trace0("add_list found_something1 adding brancges");
      found_something = add_branches(cmd.ctos(),what,&CARD_LIST::card_list);
      trace0("add_list found_something1 returned from brancges");
      if (!found_something) {
        cmd.warn(bWARNING, here1, "No match (al)");
      }else{
      }
      for (;;) {
        // a list, as in v(r1,r2,r3) or v(1,2,3)
        if (!(cmd.is_alnum() || cmd.match1("*?"))) {
          break;
        }else{
        }
        unsigned here2 = cmd.cursor();
        trace0("add_list found_something2");
        found_something = add_branches(cmd.ctos(),what,&CARD_LIST::card_list);
        if (!found_something) {itested();
          cmd.reset(here2);
          break;
        }else{
        }
      }
    }else{itested();
      cmd.warn(bDANGER, "need device or node");
    }
    paren -= cmd.skip1b(')');
    if (paren != 0) {itested();
      cmd.warn(bWARNING, "need )");
    }else{
    }
  } else {
    trace1( ( "PROBELIST::add_list eval?"), paren );
    EVAL_PROBE* p = new EVAL_PROBE(what,scope);
    bag.push_back(p);
  }

  if (cmd.skip1b('(')) {	/* range for plotting and alarm */
    double lo = cmd.ctof();
    double hi = cmd.ctof();
    for (iterator p = begin() + oldcount;  p != end();  ++p) {
      (*p)->set_limit(lo,hi);
    }    
    if (!cmd.skip1b(')')) {untested();
      cmd.check(bWARNING, "need )");
    }else{
    }
  }else{
  }
  return found_something;
}
/*--------------------------------------------------------------------------*/
void PROBELIST::push_probe( PROBE* p )
{
  bag.push_back(p);
}
/*--------------------------------------------------------------------------*/
void PROBELIST::merge_probe( PROBE* m )
{

   for( iterator p = begin(); p!=end(); p++) {
         if(  (*m) == (**p) ) return;
   }

  bag.push_back(m);
}
/*--------------------------------------------------------------------------*/
PROBE* PROBELIST::push_new_probe(const std::string& param,const CKT_BASE* object)
{
  trace0("PROBELIST::push_new_probe");
  if (param=="V?") {
    cerr << "warning V? not supported" << std::endl;
  }
  PROBE* p = new PROBE(param, object);
  bag.push_back(p);
  return p;
}
/*--------------------------------------------------------------------------*/
void PROBELIST::add_all_nodes(const std::string& what)
{
  untested();
  for (NODE_MAP::const_iterator
       i = CARD_LIST::card_list.nodes()->begin();
       i != CARD_LIST::card_list.nodes()->end();
       ++i) {
    if ((i->first != "0") ) {
      NODE* node = i->second;
      assert (node);
      //      cerr << "Allnodes adding " << what << " i " << i->second << std::endl;
      push_new_probe(what, node);
    }else{
    }
//     if ((i->first != "0") && (i->first.find('.') == std::string::npos)) {
//       NODE* node = i->second;
//       assert (node);
//       push_new_probe(what, node);
//     }else{
//     }
  }
}
/*--------------------------------------------------------------------------*/
MATH_OP strtotype(std::string s)
{
  trace0(( "MATH_OP::strtorype " + s).c_str() );

  if(Umatch(s,"sum ")) 
    return op_sum;
  if(Umatch(s,"exp "))
    return op_exp;
  if(Umatch(s,"diff "))
    return op_diff;
  if(Umatch(s,"neg "))
    return op_neg;
  if(Umatch(s,"quot "))
    return op_quot;
  if(Umatch(s,"abs ")){
    trace0(( "MATH_OP::strtorype abs == " + s).c_str() );
    return op_abs;
  }
  if(Umatch(s,"tan "))
    return op_tan;

  trace0(( "MATH_OP " + s + ": not implemented").c_str());

  return op_null;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* add_expr add things to list
 */
PROBE* PROBELIST::add_expr(const std::string& , 
			     const MATH_OP type,
			     const CARD_LIST* scope,
                             CS& cmd, PROBELIST& )
{
  untested();
  assert(scope);
  trace1( "PROBELIST::add_expr ", cmd.cursor() );
  //trace0(( std::string("PROBELIST::add_expr ") + cmd.fullstring()).c_str() );
  //trace0(( std::string("PROBELIST::add_expr arg0 ") + cmd.cursor()).c_str() );

  PROBE* arg0 = PROBE_LISTS::expr[CKT_BASE::_sim->_mode].add_list(cmd);

  int komma = cmd.skip1b(',');		/* device, node, etc. */
  assert (komma);

  PROBE* arg1 = PROBE_LISTS::expr[CKT_BASE::_sim->_mode].add_list(cmd);

  PROBE* ret = new MATH_PROBE(type);
  ret->push(arg1);
  ret->push(arg0);

  return ret;
}
/*--------------------------------------------------------------------------*/
/* add_branches: add net elements to probe list
 * 	all matching a label with wildcards
 */
PROBE* PROBELIST::add_branches(const std::string&device, 
                               const std::string&param,
                               const CARD_LIST* scope)
{
  trace0( ( "PROBELIST::add_branches " + device + "->" + param + " \n" ).c_str());
  assert(scope);
  PROBE* found_something = NULL;

  std::string::size_type dotplace = device.find_first_of(".");
  if (dotplace != std::string::npos) {
    // has a dot, look deeper
    { // forward (Verilog style)
      std::string dev = device.substr(dotplace+1, std::string::npos);
      std::string container = device.substr(0, dotplace);
      for (CARD_LIST::const_iterator
             i = scope->begin();  i != scope->end();  ++i) {
        CARD* card = *i;
        if (card->is_device()
            && card->subckt()
            && wmatch(card->short_label(), container)) {
          found_something = add_branches(dev, param, card->subckt());
        }else{
        }
      }
    }
    { // reverse (ACS style)
      dotplace = device.find_last_of(".");
      std::string container = device.substr(dotplace+1, std::string::npos);
      std::string dev = device.substr(0, dotplace);
      for (CARD_LIST::const_iterator
             i = scope->begin();  i != scope->end();  ++i) {
        CARD* card = *i;
        //        cerr << " Card Match " << container << " dev "<< dev << " param " << param << std::endl;
        if (card->is_device()
            && card->subckt()
            && wmatch(card->short_label(), container)) {
          found_something = add_branches(dev, param, card->subckt());
        }else{
        }
      }
    }
  }else{
    // no dots, look here
    if (device.find_first_of("*?") != std::string::npos) {
      // there's a wild card.  do linear search for all
      { // nodes
        for (NODE_MAP::const_iterator 
               i = scope->nodes()->begin();
             i != scope->nodes()->end();
             ++i) {
          if (i->first != "0") {
            NODE* node = i->second;
            assert (node);
             
            string paramn(param);
            if (param=="V?") {
              paramn="V";
            }
              
            trace0(( "Node match " +  node->short_label()).c_str() );
            if (wmatch(node->short_label(), device)) {
              found_something = push_new_probe(paramn, node);
            }else{
            }
          }else{
          }
        }
      }
      {// components
        for (CARD_LIST::const_iterator 
               i = scope->begin();  i != scope->end();  ++i) {
          CARD* card = *i;
          if (wmatch(card->short_label(), device)) {
            //            cerr << " Components dev "<< card << " param " << param << std::endl;
            if (param=="V?")
            { 
              for(uint_t ip=1;ip<=card->net_nodes();ip++) {
                char str[10];
                sprintf(str,"V%1d",ip);
                string paramipn(str);
                //                cerr << "               paramn " << paramipn << std::endl;
                found_something =     push_new_probe(paramipn, card);
              }
            }else {
              found_something =     push_new_probe(param, card);
            }
          }else{
          }
        }
      }
    }else{
      // no wild card.  do fast search for one
      { // nodes
        NODE* node = (*scope->nodes())[device];
        if (node) {
          //          cerr << " Node dev "<< node << " param " << param << std::endl;
          found_something =   push_new_probe(param, node);
        }else{
        }
      }
      { //components
        CARD_LIST::const_iterator i = scope->find_(device);
        if (i != scope->end()) {
          //          cerr << " Card List dev "<< *i << " param " << param << std::endl;
          if (param=="V?")
          { 
            for(uint_t ip=1;ip<=(*i)->net_nodes();ip++) {
              char str[10];
              sprintf(str,"V%1d",ip);
              string paramipn(str);
              //                cerr << "               paramn " << paramipn << std::endl;
              found_something =     push_new_probe(paramipn, *i);
            }
          }else {
              found_something =     push_new_probe(param, *i);
          }
          // found_something = 	  push_new_probe(param, *i);
        }else{
        }
      }
    }
  }
  return found_something;
}
/*--------------------------------------------------------------------------*/
ostream& operator<<( ostream& o, const PROBELIST &l)
{
  PROBELIST::const_iterator p;
  o << " -- > ";
  p = l.begin();
  for( p=l.begin(); p!=l.end(); p++) {
    o << (*p)->label();
    if (p+1 != l.end())
      o << ", ";
  }
  return o;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/