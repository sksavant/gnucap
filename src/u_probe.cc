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
   _brh(brh),
   _lo(0.),
   _hi(0.)
{
  if (_brh) {
    _brh->inc_probes();
    trace1( ( "PROBE::PROBE ++probe: " + _what ).c_str(),  _brh->probes());
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const PROBE& p)
  :_arg(p._arg),
   _what(p._what),
   _brh(p._brh),
   _lo(p._lo),
   _hi(p._hi),
   _next(p._next)
{
  if (_brh) {
    _brh->inc_probes();
    trace1( ( "PROBE::PROBE copy ++probe: " + _what ).c_str(),  _brh->probes());
  }else{
  }
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
    _brh->dec_probes();
    trace1( ( "PROBE::detach --probe: " + label() ).c_str(),  _brh->probes());
  }else{
    untested();
  }
  _what = "";
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
    return _what + '(' + _brh->long_label() + ')';
  }else{
    return _what + "(0)";
  }
}
/*--------------------------------------------------------------------------*/
double PROBE::value(void)const
{
  // trace0(("PROBE::value()"+_what).c_str());

  if (_brh) {
    return _brh->probe_num(_what);
  }else{
    return probe_node();
  }
}
/*--------------------------------------------------------------------------*/
double PROBE::probe_node(void)const
{
  if (Umatch(_what, "iter ")) {
    assert(iPRINTSTEP - sCOUNT == 0);
    assert(iSTEP      - sCOUNT == 1);
    assert(iTOTAL     - sCOUNT == 2);
    assert(iCOUNT     - sCOUNT == 3);
    return CKT_BASE::_sim->_iter[sCOUNT];
  }else if (Umatch(_what, "bdel ") && CKT_BASE::_sim->_mode == s_TTT) {
    return CKT_BASE::tt_behaviour_del;
  }else if (Umatch(_what, "bdel ") && CKT_BASE::_sim->_mode == s_TRAN) {
    return CKT_BASE::tr_behaviour_del;
  }else if (Umatch(_what, "brel ") && CKT_BASE::_sim->_mode == s_TTT) {
    return CKT_BASE::tt_behaviour_rel;
  }else if (Umatch(_what, "brel ") && CKT_BASE::_sim->_mode == s_TRAN) {
    return CKT_BASE::tr_behaviour_rel;
  }else if (Umatch(_what, "rejects ") && CKT_BASE::_sim->_mode == s_TTT) {
    return CKT_BASE::_sim->_tt_rejects;
  }else if (Umatch(_what, "dT ") && CKT_BASE::_sim->_mode == s_TTT) {
    return CKT_BASE::_sim->_dT0;
  }else if (Umatch(_what, "dtt ")) {untested();
    return CKT_BASE::_sim->_dT0;
  }else if (Umatch(_what, "bypass ")) {untested();
    return OPT::bypass + 10*CKT_BASE::_sim->_bypass_ok;
  }else if (Umatch(_what, "control ")) {
    return ::status.control;
  }else if (Umatch(_what, "damp ")) {untested();
    return CKT_BASE::_sim->_damp;
  }else if (Umatch(_what, "gen{erator} ")) {untested();
    return CKT_BASE::_sim->_genout;
  }else if (Umatch(_what, "hidden ")) {untested();
    return ::status.hidden_steps;
  }else if (Umatch(_what, "temp{erature} ")) {untested();
    return CKT_BASE::_sim->_temp_c;
  }else if (Umatch(_what, "gain ")) {
    return CKT_BASE::_sim->_dT0/CKT_BASE::_sim->_dTmin;
  }else if (Umatch(_what, "ttime ")) {untested();
    return CKT_BASE::_sim->_Time0;
  }else if (Umatch(_what, "time ")) {untested();
    return CKT_BASE::_sim->_time0;
  }else{
    return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
void PROBE::push(PROBE* p)
{
  std::cerr << "PROBE::push pushing probe\n";
  PROBE* newp=new PROBE(*p);
  assert(p!=NULL);
  newp->set_next(arg());
  set_arg(newp);
}
/*--------------------------------------------------------------------------*/
MATH_PROBE::MATH_PROBE(const MATH_PROBE& p) : PROBE(p) {  _type = p._type ; }
/*--------------------------------------------------------------------------*/
double MATH_PROBE::value(void)const
{
  untested();
  if( ( _type & MATH_OP_MASK ) == MATH_OP_CONST)
  {
       return NOT_VALID; // not impl.
  }
  PROBE*  token=arg();

//   std::cerr << "PROBE::mathvalue  getting tokenv from token" << token << " \n";
  double ret=token->value();

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
MEAS_PROBE::MEAS_PROBE(const MEAS_PROBE& p) : PROBE(p) {  untested(); }
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
double MEAS_PROBE::value(void)const
{ 
  CS Cmd(CS::_STRING, _cmd);
  std::string function;

  Cmd >> function >> '(';
  FUNCTION* f = measure_dispatcher[function];

  //trace1("MEAS_PROBE::value mode " ,  (double)CKT_BASE::_sim->sim_mode);

  SIM_MODE oldmode=CKT_BASE::_sim->_mode;
  CKT_BASE::_sim->_mode=s_TRAN;
  // argh stupid string
  fun_t value = f->eval(Cmd, _scope);
  CKT_BASE::_sim->_mode=oldmode;


  if (!Cmd.skip1b(')')) {
    Cmd.warn(bWARNING, "MEAS_PROBE::value need )");
  }else{
    // BUG kills n.
    //std::istringstream st(value);
    //double out;
    //st >> out;
    return value;
  }

  return 888;
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
  trace0("MEAS_PROBE::label()");
  return _what;
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
EVAL_PROBE::EVAL_PROBE(const EVAL_PROBE& p) : PROBE(p) {  untested(); }
/*--------------------------------------------------------------------------*/
EVAL_PROBE& EVAL_PROBE::operator=(const EVAL_PROBE& p)
{
  PROBE::operator=(p);
  _cmd = p._cmd;
  return *this;
}
/*--------------------------------------------------------------------------*/
double EVAL_PROBE::value(void)const
{ 
  CS cmd(CS::_STRING, _cmd);
  std::string name;

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
  _scope = scope;
} 
/*--------------------------------------------------------------------------*/
