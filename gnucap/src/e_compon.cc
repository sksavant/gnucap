/*$Id: e_compon.cc,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
//testing=obsolete
#include "u_lang.h"
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(const COMMON_COMPONENT& p)
  :_tnom_c(p._tnom_c),
   _dtemp(p._dtemp),
   _temp_c(p._temp_c),
   _mfactor(p._mfactor),
   _modelname(p._modelname),
   _model(p._model),
   _attach_count(0)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::COMMON_COMPONENT(int c)
  :_tnom_c(NOT_INPUT),
   _dtemp(0),
   _temp_c(NOT_INPUT),
   _mfactor(1),
   _modelname(),
   _model(0),
   _attach_count(c)
{
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT::~COMMON_COMPONENT()
{
  trace1("common,destruct", _attach_count);
  assert(_attach_count == 0 || _attach_count == CC_STATIC);
}
/*--------------------------------------------------------------------------*/
const MODEL_CARD* COMPONENT::find_model(const std::string& modelname)const
{
  if (modelname == "") {
    throw Exception(long_label() + ": missing args -- need model name");
    return NULL;
  }else{
    const CARD* c = NULL;
    {
      int bin_count = 0;
      for (const CARD* Scope = this; Scope && !c; Scope = Scope->owner()) {
	c = Scope->find_in_parent_scope(modelname, bDEBUG);
	if (!c) {
	  // didn't find plain model.  try binned models
	  bin_count = 0;
	  for (;;) {
	    std::string extended_name =
	      modelname + '.' + to_string(++bin_count);
	    c = Scope->find_in_parent_scope(extended_name, bDEBUG);
	    if (!c) {
	      // didn't find a binned model
	      break;
	    }
	    const MODEL_CARD* m = dynamic_cast<const MODEL_CARD*>(c);
	    if (m && m->is_valid(this)) {
	      //matching name and correct bin
	      break;
	    }
	  }
	}
      }
      if (!c) {
	if (bin_count <= 1) {
	  throw Exception_Cant_Find(long_label(), modelname);
	}else{
	  throw Exception(long_label() + ": no bins match: " + modelname);
	}
	return NULL;
      }
    }
    // found something, what is it?
    const MODEL_CARD* model = dynamic_cast<const MODEL_CARD*>(c);
    if (!model) {itested();
      throw Exception_Type_Mismatch(long_label(), modelname, ".model");
    }else if (!model->is_valid(this)) {itested();
      error(bWARNING, long_label() + ", " + modelname
	   + "\nmodel and device parameters are incompatible, using anyway\n");
    }else{
    }
    return model;
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::attach_model(const COMPONENT* d)const
{
  assert(d);
  _model = d->find_model(modelname());
  assert(_model);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse_modelname(CS& cmd)
{
  set_modelname(cmd.ctos(TOKENTERM));
}
/*--------------------------------------------------------------------------*/
// called only by COMMON_COMPONENT::parse_obsolete
bool COMMON_COMPONENT::parse_param_list(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  do{
    parse_params_obsolete_callback(cmd); //BUG//callback
  }while (cmd.more() && !cmd.stuck(&here));
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::parse_common_obsolete_callback(CS& cmd) //used
{
  if (cmd.skip1b('(')) {
    // start with a paren
    unsigned start = cmd.cursor();
    parse_param_list(cmd);
    if (cmd.gotit(start)) {		// ( params ( ....
      // named args before num list
      if (cmd.skip1b('(')) {		// ( params ( list ) params )
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
      }else{				// ( params list params )
	parse_numlist(cmd);		//BUG//
      }
      parse_param_list(cmd);
      if (!cmd.skip1b(')')) {untested();
	cmd.warn(bWARNING, "need )");
      }
    }else{
      // no named args before num list
      // but there's a paren
      // not sure whether it belongs to all args or to num list
      if (cmd.skip1b('(')) {		// ( ( list ) params )
	// two parens
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
	parse_param_list(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
      }else{				// ( list ...
	// only one paren
	parse_numlist(cmd);
	if (cmd.skip1b(')')) {		// ( list ) params
	  // assume it belongs to num list
	  // and named params follow
	  parse_param_list(cmd);
	}else{				// ( list params )
	  // assume it belongs to all args
	  parse_param_list(cmd);
	  if (!cmd.skip1b(')')) {
	    cmd.warn(bWARNING, "need )");
	  }
	}
      }
    }
  }else{
    // does not start with a paren
    unsigned start = cmd.cursor();
    parse_param_list(cmd);
    if (cmd.gotit(start)) {
      if (cmd.skip1b('(')) {		// params ( list ) params
	parse_numlist(cmd);
	if (!cmd.skip1b(')')) {untested();
	  cmd.warn(bWARNING, "need )");
	}
      }else if (!(cmd.is_alpha())) {	// params list params
	parse_numlist(cmd);
      }else{				// params   (only)
      }
    }else{				// list params
      assert(!(cmd.skip1b('(')));
      parse_numlist(cmd);
    }
    parse_param_list(cmd);
    if (cmd.skip1b(')')) {
      cmd.warn(bWARNING, start, "need (");
    }
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  print_pair(o, lang, "tnom", _tnom_c,  _tnom_c.has_hard_value());
  print_pair(o, lang, "dtemp",_dtemp,   _dtemp.has_hard_value());
  print_pair(o, lang, "temp", _temp_c,  _temp_c.has_hard_value());
  print_pair(o, lang, "m",    _mfactor, _mfactor.has_hard_value());
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::set_param_by_index(int i, std::string& value, int offset)
{
  switch (i) {
  case 0:  _tnom_c = value; break;
  case 1:  _dtemp = value; break;
  case 2:  _temp_c = value; break;
  case 3:  _mfactor = value; break;
  default: throw Exception_Too_Many(i, 3, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::param_is_printable(int i)const
{
  switch (i) {
  case 0:  return _tnom_c.has_hard_value();
  case 1:  return _dtemp.has_hard_value();
  case 2:  return _temp_c.has_hard_value();
  case 3:  return _mfactor.has_hard_value();
  default: return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_name(int i)const
{
  switch (i) {
  case 0:  return "tnom";
  case 1:  return "dtemp";
  case 2:  return "temp";
  case 3:  return "m";
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_name(int i, int j)const
{
  return (j==0) ? param_name(i) : "";
}
/*--------------------------------------------------------------------------*/
std::string COMMON_COMPONENT::param_value(int i)const
{
  switch (i) {
  case 0:  return _tnom_c.string();
  case 1:  return _dtemp.string();
  case 2:  return _temp_c.string();
  case 3:  return _mfactor.string();
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::precalc(const CARD_LIST* Scope)
{
  if (_model) {
    MODEL_CARD* m = const_cast<MODEL_CARD*>(_model);
    assert(m);
    m->precalc();
  }else{
  }
  assert(Scope);
  _tnom_c.e_val(OPT::tnom_c, Scope);
  _dtemp.e_val(0., Scope);
  _temp_c.e_val(SIM::temp_c + _dtemp, Scope);
  _mfactor.e_val(1, Scope);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::tr_eval(ELEMENT*x)const
{untested();
  assert(_model);
  _model->tr_eval(x);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::ac_eval(ELEMENT*x)const
{untested();
  assert(_model);
  _model->ac_eval(x);
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::operator==(const COMMON_COMPONENT& x)const
{
  return (_modelname == x._modelname
	  && _model == x._model
	  && _tnom_c == x._tnom_c
	  && _dtemp == x._dtemp
	  && _temp_c == x._temp_c
	  && _mfactor == x._mfactor);
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
  if (has_parse_params_obsolete_callback()) {
    std::string args(Name + "=" + Value);
    CS cmd(CS::_STRING, args); //obsolete_callback
    bool ok = parse_params_obsolete_callback(cmd); //BUG//callback
    if (!ok) {untested();
      throw Exception_No_Match(Name);
    }else{itested();
    }
  }else{
    //BUG// ugly linear search
    for (int i = param_count() - 1;  i >= 0;  --i) {
      for (int j = 0;  param_name(i,j) != "";  ++j) {
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
}
/*--------------------------------------------------------------------------*/
//BUG// This is a kluge for the spice_wrapper, to disable virtual functions.
// It is called during expansion only.

void COMMON_COMPONENT::Set_param_by_name(std::string Name, std::string Value)
{
  assert(!has_parse_params_obsolete_callback());

    //BUG// ugly linear search
    for (int i = COMMON_COMPONENT::param_count() - 1;  i >= 0;  --i) {
      for (int j = 0;  COMMON_COMPONENT::param_name(i,j) != "";  ++j) {
	if (Umatch(Name, COMMON_COMPONENT::param_name(i,j) + ' ')) {
	  COMMON_COMPONENT::set_param_by_index(i, Value, 0/*offset*/);
	  return; //success
	}else{
	  //keep looking
	}
      }
    }
    throw Exception_No_Match(Name);

}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::parse_numlist(CS&)
{
  return false;
}
/*--------------------------------------------------------------------------*/
bool COMMON_COMPONENT::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "tnom",   &_tnom_c)
    || Get(cmd, "dtemp",  &_dtemp)
    || Get(cmd, "temp",   &_temp_c)
    || Get(cmd, "m",	  &_mfactor)
    || Get(cmd, "mfactor",&_mfactor)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT()
  :CARD(),
   _common(0),
   _value(0),
   _mfactor(1),
   _converged(false),
   _q_for_eval(-1),
   _time_by()
{
  SIM::uninit();
}
/*--------------------------------------------------------------------------*/
COMPONENT::COMPONENT(const COMPONENT& p)
  :CARD(p),
   _common(0),
   _value(p._value),
   _mfactor(p._mfactor),
   _converged(p._converged),
   _q_for_eval(-1),
   _time_by(p._time_by)
{
  SIM::uninit();
  attach_common(p._common);
  assert(_common == p._common);
}
/*--------------------------------------------------------------------------*/
COMPONENT::~COMPONENT()
{
  detach_common();
  SIM::uninit();
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_by_name(std::string& int_name, std::string& ext_name)
{itested();
  for (int i=0; i<max_nodes(); ++i) {itested();
    if (int_name == port_name(i)) {itested();
      set_port_by_index(i, ext_name);
      return;
    }
    itested();
  }
  untested();
  throw Exception_No_Match(int_name);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_by_index(int num, std::string& ext_name)
{
  if (num <= max_nodes()) {
    _n[num].new_node(ext_name, this);
    if (num+1 > _net_nodes) {
      // make the list bigger
      _net_nodes = num+1;
    }else{
      // it's already big enough, probably assigning out of order
    }
  }else{untested();
    throw Exception_Too_Many(num, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_port_to_ground(int num)
{itested();
  if (num <= max_nodes()) {itested();
    _n[num].set_to_ground(this);
    if (num+1 > _net_nodes) {
      _net_nodes = num+1;
    }else{
    }
  }else{untested();
    throw Exception_Too_Many(num, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_dev_type(const std::string& new_type)
{
  if (common()) {
    if (new_type != dev_type()) {
      COMMON_COMPONENT* c = common()->clone();
      assert(c);
      c->set_modelname(new_type);
      attach_common(c);
    }else{
    }
  }else{
    CARD::set_dev_type(new_type);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::print_args_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  assert(has_common());
  common()->print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::deflate_common()
{
  unreachable();
  if (has_common()) {
    COMMON_COMPONENT* deflated_common = mutable_common()->deflate();
    if (deflated_common != common()) {untested();
      attach_common(deflated_common);
    }
  }else{
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::expand()
{
  CARD::expand();
  if (has_common()) {
    COMMON_COMPONENT* new_common = common()->clone();
    new_common->expand(this);
    COMMON_COMPONENT* deflated_common = new_common->deflate();
    if (deflated_common != common()) {
      attach_common(deflated_common);
    }else{untested();
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::precalc()
{
  CARD::precalc();
  if (has_common()) {
    try {
      mutable_common()->precalc(scope());
    }catch (Exception_Precalc& e) {
      error(bWARNING, long_label() + ": " + e.message());
    }
    _mfactor = common()->mfactor();
  }else{
  }
  
  _mfactor.e_val(1, scope());
  _value.e_val(0.,scope());
}
/*--------------------------------------------------------------------------*/
void COMPONENT::map_nodes()
{
  assert(is_device());
  assert(0 <= min_nodes());
  //assert(min_nodes() <= net_nodes());
  assert(net_nodes() <= max_nodes());
  //assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0; ii < ext_nodes()+int_nodes(); ++ii) {
    _n[ii].map();
  }

  if (subckt()) {
    subckt()->map_nodes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::tr_iwant_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt()) {
      subckt()->tr_iwant_matrix();
    }else{untested();
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::ac_iwant_matrix()
{
  if (is_device()) {
    assert(matrix_nodes() == 0);
    if (subckt()) {
      subckt()->ac_iwant_matrix();
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void COMPONENT::set_parameters(const std::string& Label, CARD *Owner,
			       COMMON_COMPONENT *Common, double Value,
			       int , double [],
			       int node_count, const node_t Nodes[])
{
  set_label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);

  assert(node_count <= net_nodes());
  notstd::copy_n(Nodes, node_count, _n);
}
/*--------------------------------------------------------------------------*/
/* set_slave: force evaluation whenever the owner is evaluated.
 */
void COMPONENT::set_slave()
{
  mark_always_q_for_eval();
  if (subckt()) {
    subckt()->set_slave();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_value(double v, COMMON_COMPONENT* c)
{
  if (c != _common) {
    detach_common();
    attach_common(c);
  }else{
  }
  set_value(v);
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
  if (has_common()) {
    COMMON_COMPONENT* c = common()->clone();
    assert(c);
    c->set_param_by_name(Name, Value);
    attach_common(c);
  }else{
    CARD::set_param_by_name(Name, Value);
  }
}
/*--------------------------------------------------------------------------*/
void COMPONENT::set_param_by_index(int i, std::string& Value, int offset)
{
  if (has_common()) {
    COMMON_COMPONENT* c = common()->clone();
    assert(c);
    c->set_param_by_index(i, Value, offset);
    attach_common(c);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0: _value = Value; break;
    case 1: _mfactor = Value; break;
    default: CARD::set_param_by_index(i, Value, offset);
    }
  }
}
/*--------------------------------------------------------------------------*/
bool COMPONENT::param_is_printable(int i)const
{
  if (has_common()) {
    return common()->param_is_printable(i);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0:  return value().has_hard_value();
    case 1:  return _mfactor.has_hard_value();
    default: return CARD::param_is_printable(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_name(int i)const
{
  if (has_common()) {
    return common()->param_name(i);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0:  return value_name();
    case 1:  return "m";
    default: return CARD::param_name(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_name(int i, int j)const
{
  if (has_common()) {untested();
    return common()->param_name(i,j);
  }else{
    if (j == 0) {
      return param_name(i);
    }else if (i >= CARD::param_count()) {
      return "";
    }else{
      return CARD::param_name(i,j);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string COMPONENT::param_value(int i)const
{
  if (has_common()) {
    return common()->param_value(i);
  }else{
    switch (COMPONENT::param_count() - 1 - i) {
    case 0:  return value().string();
    case 1:  return _mfactor.string();
    default: return CARD::param_value(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
const std::string COMPONENT::port_value(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].short_label();
}
/*--------------------------------------------------------------------------*/
const std::string COMPONENT::current_port_value(int)const 
{
  unreachable();
  static std::string s;
  return s;
}
/*--------------------------------------------------------------------------*/
double COMPONENT::tr_probe_num(const std::string& x)const
{
  CS cmd(CS::_STRING, x);
  if (cmd.umatch("v")) {
    int nn = cmd.ctoi();
    return (nn > 0 && nn <= net_nodes()) ? _n[nn-1].v0() : NOT_VALID;
  }else if (Umatch(x, "error{time} |next{time} ")) {
    return (_time_by._error_estimate < BIGBIG) ? _time_by._error_estimate : 0;
  }else if (Umatch(x, "timef{uture} ")) {
    return (_time_by._error_estimate < _time_by._event) 
      ? _time_by._error_estimate
      : _time_by._event;
  }else if (Umatch(x, "event{time} ")) {
    return (_time_by._event < BIGBIG) ? _time_by._event : 0;
  }else{
    return CARD::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::attach_common(COMMON_COMPONENT*c, COMMON_COMPONENT**to)
{
  assert(to);
  if (c == *to) {
    // The new and old are the same object.  Do nothing.
  }else if (!c) {untested();
    // There is no new common.  probably a simple element
    detach_common(to);
  }else if (!*to) {
    // No old one, but have a new one.
    ++(c->_attach_count);
    trace1("++1", c->_attach_count);
    *to = c;
  }else if (*c != **to) {
    // They are different, usually by edit.
    detach_common(to);
    ++(c->_attach_count);
    trace1("++2", c->_attach_count);
    *to = c;
  }else if (c->_attach_count == 0) {
    // The new and old are identical.
    // Use the old one.
    // The new one is not used anywhere, so throw it away.
    trace1("delete", c->_attach_count);    
    delete c;
  }else{untested();
    // The new and old are identical.
    // Use the old one.
    // The new one is also used somewhere else, so keep it.
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_COMPONENT::detach_common(COMMON_COMPONENT** from)
{
  assert(from);
  if (*from) {
    assert((**from)._attach_count > 0);
    --((**from)._attach_count);
    trace1("--", (**from)._attach_count);
    if ((**from)._attach_count == 0) {
      trace1("delete", (**from)._attach_count);
      delete *from;
    }else{
      trace1("nodelete", (**from)._attach_count);
    }
    *from = NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
