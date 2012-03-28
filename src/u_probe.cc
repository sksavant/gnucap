/*$Id: u_probe.cc,v 1.11 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
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
 * general probe object
 */
//testing=script 2009.06.21
#include "u_sim_data.h"
#include "u_status.h"
#include "e_base.h"
#include "u_probe.h"
#include "ap.h"
#include "u_function.h"
#include "globals.h"
#include "mode.h"

/*--------------------------------------------------------------------------*/
PROBE::PROBE(const std::string& what,const CKT_BASE *brh)
  :_what(what),
   _override_label(""),
   _brh(brh),
   _lo(0.),
   _hi(0.),
   _next(0)
{
  if (_brh) {
    _brh->inc_probes();
    trace3(  "PROBE::PROBE copy ++probe: 1 " , _what ,  _brh->probes(), label() );
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool PROBE::operator==(const CKT_BASE& brh)const
{
  trace3("PROBE::operator==(const CKT_BASE& brh)",object()->long_label(), brh.long_label(), (object() == &brh) );
  return (object() == &brh); 
}
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const PROBE& p)
  : _what(p._what),
    _override_label(p._override_label),
    _brh(p._brh),
    _lo(p._lo),
    _hi(p._hi),
    _next(p._next),
    _arg(p._arg)
{
  if (_brh) {
    _brh->inc_probes();
    trace3(  "PROBE::PROBE copy ++probe: 2 " , _what ,  _brh->probes(), label() );
  }else{
  }
}
/*--------------------------------------------------------------------------*/
PROBE::~PROBE(){
  trace2("PROBE::deleting probe",label(),object()->long_label());
  detach();
}
/*--------------------------------------------------------------------------*/
/* operator=  ...  assignment
 * copy a probe
 */
PROBE& PROBE::operator=(const PROBE& p)
{
  trace0("PROBE::operator=()");
  detach();
  _what = p._what;
  _override_label= p._override_label;
  _brh  = p._brh;
  _lo   = p._lo;
  _hi   = p._hi;
  if (_brh) {
    _brh->inc_probes();
    trace1( ("++probe (=): " + _what +
          " " + _brh->long_label()  ).c_str() , _brh->probes());
  }else{
    untested();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* "detach" a probe from a device
 * which means ...  1. tell the device that the probe has been removed
 *		    2. blank out the probe, so it doesn't reference anything
 * does not remove the probe from the list
 */
void PROBE::detach()
{
  if (_brh) {
    trace1( ( "PROBE::detach --probe: " + label() ).c_str(),  _brh->probes());
    _brh->dec_probes();
  }else{
    // could be measurement or probe(0) or something.
    //untested();
  }
  _what = "";
  _override_label = "";
  _brh = 0;
}
/*--------------------------------------------------------------------------*/
/* label: returns a string corresponding to a possible probe point
 * (suitable for printing)
 * It has nothing to do with whether it was selected or not
 */
const std::string PROBE::label(void)const
{
  if (_brh) {

// this is probably a bug
//  label() is used to identify probe (garbage collection)
//    if ( _override_label.compare("")!=0 ) {
//      return _override_label;
//    } else {
      return _what + '(' + _brh->long_label() + ')';
//    }
  }else{
    return _what + "(0)";
  }
}
/*--------------------------------------------------------------------------*/
// to have string probes, one would need a formatted value...
// string PROBE::format(string); or something.
double PROBE::value(void)const
{
  trace0(("PROBE::value()"+_what).c_str());

  if (_brh) {
    return _brh->probe_num(_what);
  }else{
    return probe_node();
  }
}
/*--------------------------------------------------------------------------*/
double PROBE::probe_node(void)const
{
  SIM_DATA* _sim=CKT_BASE::_sim;
  if (Umatch(_what, "iter ")) {
    assert(iPRINTSTEP - sCOUNT == 0);
    assert(iSTEP      - sCOUNT == 1);
    assert(iTOTAL     - sCOUNT == 2);
    assert(iCOUNT     - sCOUNT == 3);
    return _sim->_iter[sCOUNT];
  }else if (Umatch(_what, "bdel ") && _sim->_mode == s_TTT) {
    return CKT_BASE::tt_behaviour_del;
  }else if (Umatch(_what, "bdel ") && _sim->_mode == s_TRAN) {
    return CKT_BASE::tr_behaviour_del;
  }else if (Umatch(_what, "brel ") && _sim->_mode == s_TTT) {
    return CKT_BASE::tt_behaviour_rel;
  }else if (Umatch(_what, "brel ") && _sim->_mode == s_TRAN) {
    return CKT_BASE::tr_behaviour_rel;
  }else if (Umatch(_what, "rejects ") && _sim->_mode == s_TTT) {
    return _sim->_tt_rejects;
  }else if (Umatch(_what, "dT ") && _sim->_mode == s_TTT) {
    return _sim->_dT0;
  }else if (Umatch(_what, "ttorder ")) {untested();
    return _sim->get_tt_order();
  }else if (Umatch(_what, "dtt ")) {untested();
    return _sim->_dT0;
  }else if (Umatch(_what, "bypass ")) {untested();
    return OPT::bypass + 10*_sim->_bypass_ok;
  }else if (Umatch(_what, "control ")) {
    return ::status.control;
  }else if (Umatch(_what, "gmin ")) {
    return OPT::gmin;
  }else if (Umatch(_what, "damp ")) {untested();
    return _sim->_damp;
  }else if (Umatch(_what, "gen{erator} ")) {untested();
    return _sim->_genout;
  }else if (Umatch(_what, "hidden ")) {untested();
    return ::status.hidden_steps;
  }else if (Umatch(_what, "temp{erature} ")) {untested();
    return _sim->_temp_c;
  }else if (Umatch(_what, "gain ")) {
    return _sim->_dT0/_sim->_dTmin;
  }else if (Umatch(_what, "ltt ")) {
    return _sim->_last_Time;
  }else if (Umatch(_what, "ttime|tt0 ")) {
    return _sim->_Time0;
  }else if (Umatch(_what, "dtt0 ")) {
    return _sim->_dT0;
  }else if (Umatch(_what, "tt1 ")) {
    return _sim->_Time0 - _sim->_dT0;
  }else if (Umatch(_what, "time ")) {
    return _sim->_time0;
  }else if (Umatch(_what, "event ")) {
    return ((_sim->_eq.empty())? -1.: _sim->_eq.top());
  }else if (Umatch(_what, "limiting ")) {
    return _sim->_limiting;
  }else{
    return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MATH_PROBE::~MATH_PROBE(){
  trace1("MATH_PROBE::~MATH_PROBE", label());

  PROBE*  token=arg();

  while( token){
    PROBE* nt=token->next();
    trace1("MATH_PROBE::~MATH_PROBE deleting", token->label());

    // delete token;

    token=nt;
  }

}
/*--------------------------------------------------------------------------*/
void MATH_PROBE::push(PROBE* p)
{
  trace1("probe::push", p->label());
//  PROBE* newp = p->clone();
  //
  PROBE* newp = p;
  assert(p!=NULL);
  newp->set_next(arg());
  set_arg(newp);
}
//void PROBE::push(PROBE* p)
//{
//  assert(false);
//  trace1("probe::push", p->label());
//  PROBE* newp = new PROBE(*p);
//  assert(p!=NULL);
//  newp->set_next(arg());
//  unreachable();
//  //set_arg(newp);
//}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MATH_PROBE::MATH_PROBE(const MATH_PROBE& p) : PROBE(p) {  _type = p._type ; _next=p._next; }
/*--------------------------------------------------------------------------*/
const string typelabel(const unsigned a){
  switch(a){
    case 64:
      return "d";
    case 65:
      return "q";

    default:
      return ".";

  }

}
/*--------------------------------------------------------------------------*/
const string  MATH_PROBE::label()const{
  if (is_unary(_type)){
    char x = typetochar(_type);

    return string(&x);
  }

  stringstream s;
  char x = typetochar(_type);

  s <<  "{";

    PROBE*  token=arg();
     
      trace2("MATH_PROBE::label first arg", x, token->label());
      s<<  token->label();

      while( (token=token->next())){
        trace0("MATH_PROBE::label another");
        trace1("MATH_PROBE::label another", token->label());
        s << x <<   token->label();
      }
  s<< "}";
    trace1("MATH_PROBE::label done", s.str());
  return s.str();
}
/*--------------------------------------------------------------------------*/
char typetochar(MATH_OP _type ){
  switch(_type){
    case op_quot:
      return '/';
    case op_diff:
      return '-';
    case op_prod:
      return '*';
    case op_sum:
      return '+';
    case op_exp:
      return 'e';
    case op_abs:
      return '|';
    default:
      return '?';
  }

}
/*--------------------------------------------------------------------------*/
void MATH_PROBE::set_arg( PROBE* p){
  trace1("MATH_PROBE::set_arg()", p->label());
  _arg=p;
}
/*--------------------------------------------------------------------------*/
double MATH_PROBE::value(void)const
{
  trace0(("MATH_PROBE::value()"+_what).c_str());
  if( ( _type & MATH_OP_MASK ) == MATH_OP_CONST)
  {
       return NOT_VALID; // not impl.
  }
  PROBE* token = arg();

//   std::cerr << "PROBE::mathvalue  getting tokenv from token" << token << " \n";
  double ret=token->value();

  //// waah.  C style
  switch(_type){
    case op_quot:
      return( token->next()->value()/ret );
    case op_diff:
      return( token->next()->value()-ret );
    case op_prod:
      while( (token=token->next()))
        ret *= token->value();
      return ret; 
    case op_sum:
      while( (token=token->next()))
        ret += token->value();
       return ret; 
    case op_exp:
      return( exp(token->value()) );
    case op_abs:
      return( fabs(token->value()) );

    default:
      std::cerr << "cannot probe, " << _type << "unknown\n"; 
      return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MEAS_PROBE::MEAS_PROBE(const MEAS_PROBE& p) : 
  PROBE(p),
  f(f),
  w(w)
{  untested(); }
/*--------------------------------------------------------------------------*/
MEAS_PROBE::MEAS_PROBE(const std::string& cmd)
{ 
  trace0("MEAS_PROBE::MEAS_PROBE()");
  _what = "probe"; 
  _cmd = cmd;
} 
/*--------------------------------------------------------------------------*/
MEAS_PROBE::MEAS_PROBE(const std::string& cmd, const CARD_LIST* scope )
{ 
  trace0("MEAS_PROBE::MEAS_PROBE()");
  _what = "probe"; 
  _cmd = cmd;
  _scope = scope;
} 
/*--------------------------------------------------------------------------*/
void MEAS_PROBE::expand(){
  trace0(("MEAS_PROBE::value()"+_what).c_str());
  CS* Cmd = new CS(CS::_STRING, _cmd);
  std::string function;

  *Cmd >> function >> '(';
  f = dynamic_cast<WAVE_FUNCTION*>(( measure_dispatcher[function] )->clone() );

  if(!f) { 
    untested(); 
  } else {
    f->expand( *Cmd, _scope );
    probe_name=f->probe_name;
    // _what=probe_name;
    trace0("MEAS_PROBE::expand " + probe_name);
  }

  if (!Cmd->skip1b(')')) {
    Cmd->warn(bWARNING, "MEAS_PROBE::value need )");
    f=0;
  }

  delete Cmd;
}
/*--------------------------------------------------------------------------*/
double MEAS_PROBE::value(void)const
{ 
  if (!f){
    untested();
    return 0;
  }
  trace1("MEAS_PROBE::value() ", hp(w));

  SIM_MODE oldmode=CKT_BASE::_sim->_mode;
  CKT_BASE::_sim->_mode = s_TRAN;
  fun_t value = f->wave_eval();
  CKT_BASE::_sim->_mode = oldmode;

  return value;
}
/*--------------------------------------------------------------------------*/
MEAS_PROBE& MEAS_PROBE::operator=(const MEAS_PROBE& p)
{
  trace0("MEAS_PROBE::operator=");
  PROBE::operator=(p);
  _cmd = p._cmd;
  return *this;
}
/*--------------------------------------------------------------------------*/
const std::string MEAS_PROBE::label(void)const
{
  trace1("MEAS_PROBE::label()" + probe_name, hp(f));
  // FIXME
  return "meas("+probe_name+")";
}
/*--------------------------------------------------------------------------*/
void MEAS_PROBE::precalc_last()
{
  trace0("MEAS_PROBE::precalc_last() " + probe_name);

  if(f){
    WAVE* w = CKT_BASE::find_wave(probe_name);
    if(w) {
      f->set_wave( w );
    }else{
    trace0("MEAS_PROBE::precalc_last() no wave...");
    }

  } else {
    untested();
  }

}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MATH_PROBE& MATH_PROBE::operator=(const MATH_PROBE& p)
{
  trace0("MATH_PROBE::operator=");
  PROBE::operator=(p);
  _type = p._type;
  return *this;
}
/*--------------------------------------------------------------------------*/
EVAL_PROBE::EVAL_PROBE(const EVAL_PROBE& p) :
  PROBE(p),
  _cmd(p._cmd),
  _scope(p._scope)
{  untested(); }
/*--------------------------------------------------------------------------*/
EVAL_PROBE& EVAL_PROBE::operator=(const EVAL_PROBE& p)
{
  PROBE::operator=(p);
  assert(_scope);
  _cmd = p._cmd;
  return *this;
}
/*--------------------------------------------------------------------------*/
double EVAL_PROBE::value(void)const
{ 
  assert(_scope);
  trace0(("EVAL_PROBE::value()"+_what).c_str());

  CS cmd(CS::_STRING, _cmd);

  Expression e(cmd);
  cmd.check(bDANGER, "syntax error");
  Expression r(e, _scope);

  return r.eval();
}
/*--------------------------------------------------------------------------*/
EVAL_PROBE::EVAL_PROBE(const std::string& cmd, const CARD_LIST* scope)
{ 
  _what = cmd; 
  _cmd = cmd;
  assert(scope);
  _scope = scope;
} 
/*--------------------------------------------------------------------------*/
