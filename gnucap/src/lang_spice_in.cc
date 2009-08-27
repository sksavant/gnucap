/*$Id: lang_spice_in.cc,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
 */
//testing=script 2007.07.13
#include "globals.h"
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "lang_spice.h"

// header hack
#include "d_logic.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
static void skip_pre_stuff(CS& cmd)
{
  cmd.skipbl();
  while (cmd.umatch(CKT_PROMPT)) {untested();
    /* skip any number of copies of the prompt */
  }
  cmd.umatch(ANTI_COMMENT); /* skip mark so spice ignores but gnucap reads */
}
/*--------------------------------------------------------------------------*/
/* count_ports: figure out how many ports
 * returns the number of ports
 * side effect:  "CS" is advanced to past the ports, ready for what follows
 */
static int count_ports(CS& cmd, int maxnodes, int minnodes, int leave_tail, int start)
{
  assert(start < maxnodes);
  assert(minnodes <= maxnodes);

  int num_nodes = 0;
  std::vector<unsigned> spots;
  int paren = cmd.skip1b('(');
  int i = start;
  // loop over the tokens to try to guess where the nodes end
  // and other stuff begins
  spots.push_back(cmd.cursor());
  for (;;) {
    ++i;
    //cmd.skiparg();
    std::string node_name;
    cmd >> node_name;
    spots.push_back(cmd.cursor());

    if (paren && cmd.skip1b(')')) {
      num_nodes = i;
      break;
    }else if (cmd.is_end()) {
      // found the end, no '='
      if (i <= minnodes) {
	num_nodes = i;
      }else if (i <= minnodes + leave_tail) {
	num_nodes = minnodes;
      }else if (i <= maxnodes + leave_tail) {
	num_nodes = i - leave_tail;
      }else{
	num_nodes = maxnodes;
      }
      break;
    }else if (cmd.skip1b("({})")) {
      // found '(', it's past the end of nodes
      if (i > maxnodes + leave_tail) {
	num_nodes = maxnodes;
      }else{
	num_nodes = i - leave_tail;
      }
      break;
    }else if (cmd.skip1b('=')) {
      // found '=', it's past the end of nodes
      if (i > maxnodes + leave_tail + 1) {
	num_nodes = maxnodes;
      }else{
	num_nodes = i - leave_tail - 1;
      }
      break;
    }else{
    }
  }
  if (num_nodes < start) {
    cmd.reset(spots.back());
    throw Exception("what's this?");
  }else{
  }

  cmd.reset(spots[static_cast<unsigned>(num_nodes-start)]);

  //cmd.warn(bDANGER, "past-nodes?");
  //BUG// assert fails on current controlled sources with (node node dev) syntax
  // it's ok with (node node) dev syntax or node node dev syntax
  assert(num_nodes <= maxnodes);
  return num_nodes;
}
/*--------------------------------------------------------------------------*/
/* parse_ports: parse circuit connections from input string
 * fills in the rest of the array with 0
 * returns the number of nodes actually read
 * The purpose of this complexity is to handle the variants of Spice formats,
 * which might have keys between nodes,
 * an unknown number of nodes with unknown number of args and no delimeters.
 * Consider:  X1 a b c d e f g h i j k
 * Clearly (?) a,b,c,d,e are nodes, f is a subckt name, the rest are args.
 * But how do you know?
 *
 * Args:
 * maxnodes: the maximum number of port nodes to parse.
 *	Stop here, even if it seems there should be more.
 *
 * minnodes: the minimum number of port nodes to parse.
 *	It is an error if there are fewer than this.
 *
 * leave_tail: The number of arguments that are not nodes,
 *	and don't have equal signs, following.
 *	It is an aid to distinguishing the nodes from things that follow.
 *	minnodes wins over leave_tail, but leave_tail wins over maxnodes.
 *	The above example would need leave_tail=6 to parse correctly.
 *	This one: X1 a b c d e f g=h i=j k
 *	where a,b,c,d,e are nodes, f is a subckt or model identifier
 *	would parse correctly with leave_tail=1.
 *	Usual values for leave_tail are 0 or 1.
 *
 * start: start at this number, used for continuing after HSPICE kluge.
 *	Other than that, the only useful value for start is 0.
 *
 * all_new: All node names must be new (not already defined) and unique.
 *	This is used for the header line in .subckt, which starts clean.
 *	The main benefit is to reject duplicates.
 */
void LANG_SPICE_BASE::parse_ports(CS& cmd, COMPONENT* x, int minnodes,
			     int start, int num_nodes, bool all_new)
{
  assert(x);

  int paren = cmd.skip1b('(');
  int ii = start;
  unsigned here1 = cmd.cursor();
  try{
    for (;;) {
      here1 = cmd.cursor();
      if (paren && cmd.skip1b(')')) {
	--paren;
	break; // done.  have closing paren.
      }else if (ii >= num_nodes) {
	break; // done.  have maxnodes.
      }else if (!cmd.more()) {untested();
	break; // done.  premature end of line.
      }else if (OPT::keys_between_nodes &&
		(cmd.umatch("poly ")
		 || cmd.umatch("pwl ")
		 || cmd.umatch("vccap ")
		 || cmd.umatch("vcg ")
		 || cmd.umatch("vcr "))
		) {
	cmd.reset(here1);
	break; // done, found reserved word between nodes
      }else{
	//----------------------
	unsigned here = cmd.cursor();
	std::string node_name;
	cmd >> node_name;
	if (cmd.stuck(&here)) {itested();
	  // didn't move, probably a terminator.
	  throw Exception("bad node name");
	}else{
	  // legal node name, store it.
	  x->set_port_by_index(ii, node_name);
	}
	//----------------------
	if (!(x->node_is_connected(ii))) {untested();
	  break; // illegal node name, might be proper exit.
	}else{
	  if (all_new) {
	    if (x->node_is_grounded(ii)) {untested();
	      cmd.warn(bDANGER, here1, "node 0 not allowed here");
	    }else{
	    }
	  }else{
	  }
	  ++ii;
	}
      }
    }
  }catch (Exception& e) {itested();
    cmd.warn(bDANGER, here1, e.message());
  }
  if (ii < minnodes) {itested();
    cmd.warn(bDANGER, "need " + to_string(minnodes-ii) +" more nodes");
  }else{
  }
  if (paren != 0) {untested();
    cmd.warn(bWARNING, "need )");
  }else{
  }
  //assert(x->_net_nodes == ii);
  
  // ground unused input nodes
  for (int iii = ii;  iii < minnodes;  ++iii) {itested();
    x->set_port_to_ground(iii);
  }
  //assert(x->_net_nodes >= ii);
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_element_using_obsolete_callback(CS& cmd, COMPONENT* x)
{
  assert(x);
  ELEMENT* xx = dynamic_cast<ELEMENT*>(x);
  assert(xx);

  {
    unsigned here = cmd.cursor();
    int stop_nodes = x->max_nodes() - x->num_current_ports();
    int num_nodes = count_ports(cmd, stop_nodes, 0,  0,   0);
    //				     max	 min tail already_got
    cmd.reset(here);
    parse_ports(cmd, x, 0,  0,		num_nodes, false);
    //			min already_got
  }
  int gotnodes = x->_net_nodes;
  COMMON_COMPONENT* c = NULL;

  if (gotnodes < x->min_nodes()) {
    // HSPICE compatibility kluge.
    // The device type or function type could be stuck between the nodes.
    xx->skip_dev_type(cmd); // (redundant)
    c = EVAL_BM_ACTION_BASE::parse_func_type(cmd);
    {
      unsigned here = cmd.cursor();
      int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), 0, gotnodes);
      cmd.reset(here);
      parse_ports(cmd, x, x->min_nodes(), gotnodes, num_nodes, false);
    }
  }else{
    // Normal mode.  nodes first, then data.
  }

  if (!c) {
    xx->skip_dev_type(cmd); // (redundant)
    c = new EVAL_BM_COND;
  }else{
  }
  assert(c);

  // we have a blank common of the most general type
  // (or HSPICE kluge)
  // let it continue parsing

  unsigned here = cmd.cursor();
  c->parse_common_obsolete_callback(cmd); //BUG//callback
  if (cmd.stuck(&here)) {untested();
    cmd.warn(bDANGER, "needs a value");
  }else{
  }

  // At this point, there is ALWAYS a common "c", which may have more
  // commons attached to it.  Try to reduce its complexity.
  // "c->deflate()" may return "c" or some simplification of "c".
  
  COMMON_COMPONENT* dc = c->deflate();
  
  // dc == deflated_common
  // It might be just "c".
  // It might be something else that is simpler but equivalent.

  // check for a simple value
  EVAL_BM_VALUE* dvc = dynamic_cast<EVAL_BM_VALUE*>(dc);

  // dvc == deflated value common
  // It might be "dc", if "dc" is a value common.
  // It might be nothing.

  if (dvc && !dvc->has_ext_args()) {
    // If it is a simple value, don't use a common.
    // Just store the value directly.
    x->set_value(dvc->value());
    x->_mfactor = dvc->mfactor();
    delete c;
  }else{
    x->attach_common(dc);
    if (dc != c) {
      delete c;
    }else{
    }
  }
  cmd.check(bDANGER, "what's this?");
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_logic_using_obsolete_callback(CS& cmd, COMPONENT* x)
{
  assert(x);
  {
    unsigned here = cmd.cursor();
    int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), x->tail_size(), 0/*start*/);
    cmd.reset(here);
    parse_ports(cmd, x, x->min_nodes(), 0/*start*/, num_nodes, false);
  }
  int incount = x->net_nodes() - x->min_nodes() + 1;
  assert(incount > 0);

  std::string modelname = cmd.ctos(TOKENTERM);

  COMMON_LOGIC* common = 0;
  if      (cmd.umatch("and " )) {untested();common = new LOGIC_AND;}
  else if (cmd.umatch("nand ")) {common = new LOGIC_NAND;}
  else if (cmd.umatch("or "  )) {untested();common = new LOGIC_OR;}
  else if (cmd.umatch("nor " )) {common = new LOGIC_NOR;}
  else if (cmd.umatch("xor " )) {untested();common = new LOGIC_XOR;}
  else if (cmd.umatch("xnor ")) {untested();common = new LOGIC_XNOR;}
  else if (cmd.umatch("inv " )) {common = new LOGIC_INV;}
  else {itested();
    cmd.warn(bWARNING,"need and,nand,or,nor,xor,xnor,inv");
    common=new LOGIC_NONE;
  }
  
  assert(common);
  common->incount = incount;
  common->set_modelname(modelname);
  x->attach_common(common);
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_type(CS& cmd, CARD* x)
{
  assert(x);
  std::string new_type;
  cmd >> new_type;
  x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_args(CS& cmd, CARD* x)
{
  assert(x);
  COMPONENT* xx = dynamic_cast<COMPONENT*>(x);

  if (!x->use_obsolete_callback_parse()) {
    int paren = cmd.skip1b('(');
    unsigned here = cmd.cursor();
    if (xx && cmd.is_float()) {
      std::string value;
      cmd >> value;
      x->set_param_by_name(xx->value_name(), value);
    }else if (cmd.match1("'{")) {
      std::string value;
      cmd >> value; // strips off the quotes
      value = '{' + value + '}'; // put them back
      x->set_param_by_name(xx->value_name(), value);
    }else{
    }
    for (int i=0; ; ++i) {
      if (paren && cmd.skip1b(')')) {
	break;
      }else if (!cmd.more()) {
	break;
      }else{
	std::string Name, value;
	cmd >> Name >> '=';
	if (cmd.match1("'{")) {
	  cmd >> value; // strips off the quotes
	  value = '{' + value + '}'; // put them back
	}else{
	  cmd >> value;
	}
	unsigned there = here;
	if (cmd.stuck(&here)) {untested();
	  break;
	}else{
	  try{
	    x->set_param_by_name(Name, value);
	  }catch (Exception_No_Match&) {itested();
	    cmd.warn(bDANGER, there, "bad parameter, ignored");
	  }
	}
      }
    }
  }else if (MODEL_CARD* pp = dynamic_cast<MODEL_CARD*>(x)) {
    // used only for "table"
    int paren = cmd.skip1b('(');
    bool in_error = false;
    for (;;) {
      unsigned here = cmd.cursor();
      pp->parse_params_obsolete_callback(cmd);  //BUG//callback//
      if (!cmd.more()) {
	break;
      }else if (paren && cmd.skip1b(')')) {untested();
	break;
      }else if (cmd.stuck(&here)) {untested();
	if (in_error) {untested();
	  // so you don't get two errors on name = value.
	  cmd.skiparg();
	  in_error = false;
	}else{untested();
	  cmd.warn(bDANGER, "bad paramerter -- ignored");
	  cmd.skiparg().skip1b("=");
	  in_error = true;
	}
      }else{
	in_error = false;
      }
    }
  }else{untested();
    // using obsolete_callback
  }
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_label(CS& cmd, CARD* x)
{
  assert(x);
  std::string my_name;
  cmd >> my_name;
  x->set_label(my_name);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_SPICE_BASE::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_SPICE_BASE::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset();
  skip_pre_stuff(cmd);

  unsigned here = cmd.cursor();

  std::string s;
  cmd >> s;
  cmd.reset(here);
  if (!command_dispatcher[s]) {
    cmd.skip();
    ++here;
  }else{
  }
  CMD::cmdproc(cmd, scope);

  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
MODEL_CARD* LANG_SPICE_BASE::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset();
  cmd >> ".model ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  parse_args(cmd, x);
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT* LANG_SPICE_BASE::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);

  // header
  cmd.reset();
  (cmd >> ".subckt |.macro ");
  parse_label(cmd, x);
  {
    unsigned here = cmd.cursor();
    int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), 
				0/*no unnamed par*/, 0/*start*/);
    cmd.reset(here);
    parse_ports(cmd, x, x->min_nodes(), 0/*start*/, num_nodes, true/*all new*/);
  }
  x->subckt()->params()->parse(cmd);

  // body
  parse_module_body(cmd, x, x->subckt(), name() + "-subckt>", NO_EXIT_ON_BLANK, ".ends |.eom ");
  return x;
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE_BASE::parse_module_body(CS& cmd, MODEL_SUBCKT* x, CARD_LIST* Scope,
		const std::string& prompt, EOB exit_on_blank, const std::string& exit_key)
{
  try {
    for (;;) {
      cmd.get_line(prompt);
      
      if ((exit_on_blank==EXIT_ON_BLANK && cmd.is_end()) 
	  || cmd.umatch(exit_key)) {
	break;
      }else{
	skip_pre_stuff(cmd);
	new__instance(cmd, x, Scope);
      }
    }
  }catch (Exception_End_Of_Input& e) {
  }
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_SPICE_BASE::parse_instance(CS& cmd, COMPONENT* x)
{
  try {
    assert(x);
    cmd.reset().umatch(ANTI_COMMENT);
    
    // ACS dot type specifier
    if (cmd.skip1b('.')) {
      parse_type(cmd, x);
    }else{
    }
    
    parse_label(cmd, x);
    
    if (x->use_obsolete_callback_parse()) {
      parse_element_using_obsolete_callback(cmd, x);
    }else if (DEV_LOGIC* xx = dynamic_cast<DEV_LOGIC*>(x)) {
      parse_logic_using_obsolete_callback(cmd, xx);
    }else{
      {
	unsigned here = cmd.cursor();
	int num_nodes = count_ports(cmd, x->max_nodes(), x->min_nodes(), x->tail_size(), 0);
	cmd.reset(here);
	parse_ports(cmd, x, x->min_nodes(), 0/*start*/, num_nodes, false);
      }
      if (x->print_type_in_spice()) {
	parse_type(cmd, x);
      }else{
      }
      parse_args(cmd, x);
    }
  }catch (Exception& e) {
    cmd.warn(bDANGER, e.message());
  }
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_SPICE_BASE::find_type_in_string(CS& cmd)
{
  cmd.umatch(ANTI_COMMENT); /* skip mark so spice ignores but gnucap reads */

  unsigned here = cmd.cursor();
  std::string s;
  char id_letter = cmd.peek();
  if (OPT::case_insensitive) {
    id_letter = static_cast<char>(toupper(id_letter));
  }else{
  }
  switch (id_letter) {
  case '\0':untested();
    s = "";
    break;
  case '.':
    cmd >> s;
    cmd.reset(here);
    if (!command_dispatcher[s]) {
      cmd.skip();
      ++here;
      s = s.substr(1);
    }else{
    }
    break;
  case 'G':
    here = cmd.cursor();

    if (cmd.scan("vccap |vcg |vcr |vccs ")) {
      s = cmd.trimmed_last_match();
    }else{
      s = "G";
    }
    break;
  default:
    s = id_letter;
    break;
  }
  cmd.reset(here);
  return s;
}
/*--------------------------------------------------------------------------*/
void LANG_SPICE::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-spice>");
  if (cmd.is_file()) {itested();
    if ((Scope == &CARD_LIST::card_list)
	&& (Scope->is_empty())
	&& (head == "'")) {itested();	//BUG// ugly hack
      head = cmd.fullstring();
      IO::mstdout << head << '\n';
    }else{itested();
    }
  }else{itested();
    new__instance(cmd, NULL, Scope);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
