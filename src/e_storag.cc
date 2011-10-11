/*$Id: e_storag.cc,v 1.6 2010-07-27 07:45:35 felix Exp $ -*- C++ -*-
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
 * Base class for storage elements of a circuit
 */
//testing=obsolete,script 2006.06.14 
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
/* table for selecting local integraton method
 * Determines which one wins in a conflict.
 * "only" wins over non-only.  local (_method_u) wins over opt.
 */
//                     OPT::method    _method_u
METHOD STORAGE::method_select[meNUM_METHODS][meNUM_METHODS] = {
  /*vv OPT vv*/
  //local>>>EULER,EULERONLY,TRAP,TRAPONLY,GEAR2,GEAR2ONLY,TRAPGEAR,TRAPEULER
  /*meUNKNOWN*/
  {mTRAPGEAR,mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meEULER*/
  {mEULER,   mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meEULERONLY*/
  {mEULER,   mEULER,mEULER,mEULER,mTRAP,mEULER,mGEAR,mEULER,   mEULER},
  /*meTRAP*/
  {mTRAP,    mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meTRAPONLY*/
  {mTRAP,    mTRAP, mEULER,mTRAP, mTRAP,mTRAP, mGEAR,mTRAP,    mTRAP},
  /*meGEAR*/
  {mGEAR,    mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meGEAR2ONLY*/
  {mGEAR,    mGEAR, mEULER,mGEAR, mTRAP,mGEAR, mGEAR,mGEAR,    mGEAR},
  /*meTRAPGEAR*/
  {mTRAPGEAR,mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meTRAPEULER*/
  {mTRAPEULER,mEULER,mEULER,mTRAP,mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER}
};
/*--------------------------------------------------------------------------*/
void STORAGE::precalc_last()
{
  ELEMENT::precalc_last();

  set_converged();
  assert(!is_constant()); /* because of integration */
 
  _method_a = method_select[OPT::method][_method_u];
  //assert(_loss0 == 0.);
  //assert(_loss1 == 0.);
  /* m0 and acg are frequency/time dependent and cannot be set here.
   * If this is a coupled inductor, there is a subckt, which is expanded
   * by the mutual pseudo-element.
   * Assigning the values here becomes unnecessary, but harmless.
   */
}
/*--------------------------------------------------------------------------*/
// not yet. only cap.
//void STORAGE::tr_init(double x)
//{
//  untested0("uic");
//  for (int i = 0;  i < OPT::_keep_time_steps;  ++i) {
//    _i[i] = FPOLY1(0., 0., 0.);
//  }
//  _m1 = _m0 = CPOLY1(x, 0., 0.);
//
//  tr_load();
//}
/*--------------------------------------------------------------------------*/
void STORAGE::tr_begin()
{
  ELEMENT::tr_begin();
  _method_a = method_select[OPT::method][_method_u];
  for (int i = 0;  i < OPT::_keep_time_steps;  ++i) {
    _i[i] = FPOLY1(0., 0., 0.);
  }
  _m1 = _m0 = CPOLY1(0., 0., 0.);
}
/*--------------------------------------------------------------------------*/
void STORAGE::tr_restore()
{
  ELEMENT::tr_restore();
  _method_a = method_select[OPT::method][_method_u];
}
/*--------------------------------------------------------------------------*/
void STORAGE::dc_advance()
{
  ELEMENT::dc_advance();

  for (int i = 1;  i < OPT::_keep_time_steps;  ++i) {
    _i[i] = _i[0];
  }
}
/*--------------------------------------------------------------------------*/
void STORAGE::tr_advance()
{
  ELEMENT::tr_advance();
  
  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    _i[i] = _i[i-1];
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval: check to see if this device needs to be evaluated
 * this works, and saves significant time
 * but possible errors.
 * Errors do not seem significant, but I can't tell without more data.
 * To disable:  option nolcbypass
 */
bool STORAGE::tr_needs_eval()const
{
  //assert(!is_q_for_eval());
  return (!OPT::lcbypass
	  || !converged() 
	  || _sim->is_advance_or_first_iteration()
	  || !conchk(_y[0].x, tr_input(), OPT::abstol)
	  || _sim->uic_now());
}
/*--------------------------------------------------------------------------*/
/* differentiate: this is what Spice calls "integrate".
 * returns an approximation of the time derivative of _q,
 * where _q is an array of states .. charge for capacitors, flux for inductors.
 * return value is current for capacitors, volts for inductors.
 *
 * inductor case:
 * q.x=voltage
 * q.f0=q(u_new)
 * q.f1=dq/du
 *
 * returns
 * x  <- i.x=q.x
 * f0 <- dq(u,t)/dt at u_new, t_new (approx.)
 * f1 <- d^2 q(u,t)/dt/dq at u_new, t_new (approx.)
 *
 */
FPOLY1 differentiate(const FPOLY1* q, const FPOLY1* i, double* time, METHOD method)
{
  if (CKT_BASE::_sim->analysis_is_static()) {
    assert(time[0] == 0.);
    return FPOLY1(q[0].x, 0., 0.);
  }else if (CKT_BASE::_sim->analysis_is_restore()) {
    /* leave it alone to restart from a previous solution */
    /* it goes this way to continue a transient analysis */
    assert(time[0] > 0);
    return i[0];
  }else{
    trace1("differentiate " + SIM_MODE_label[CKT_BASE::_sim->_mode],
        CKT_BASE::_sim->_mode);
    assert(CKT_BASE::_sim->analysis_is_tran());
    assert(CKT_BASE::_sim->analysis_is_tran_dynamic());
    if (time[1] == 0) {
      method = mEULER;	// Bogus current in previous step.  Force Euler.
    }else{
    }
    double dt = time[0] - time[1];
    assert(dt > 0.);
    switch (method) {
    case mTRAPGEAR:
      incomplete();
    case mGEAR:
      assert(OPT::_keep_time_steps >= 3);
      return FPOLY1(q[0].x,
		    (3./2.) * (q[0].f0 - q[1].f0) / dt 
		    - (1./2.) * (q[1].f0 - q[2].f0) / (time[1] - time[2]),
		    q[0].f1 * (3./2.) / dt);
    case mTRAPEULER:
      incomplete();
    case mEULER:
      assert(OPT::_keep_time_steps >= 2);
      return FPOLY1(q[0].x,
		    (q[0].f0 - q[1].f0) / dt,
		    q[0].f1 / dt);
    case mTRAP:
      assert(OPT::_keep_time_steps >= 2);
      return FPOLY1(q[0].x,
		    2 * (q[0].f0 - q[1].f0) / dt - i[1].f0,
		    q[0].f1 * 2 / dt);
    }
    unreachable();
    return FPOLY1();
  }
  unreachable();
}
/*--------------------------------------------------------------------------*/
double STORAGE::tr_c_to_g(double c, double g)const
{
  if (_sim->analysis_is_static()) {
    assert(_time[0] == 0.);
    return 0.;
  }else if (_sim->analysis_is_restore()) {itested();
    assert(_time[0] > 0);
    return g;
    // no change, fake
  }else{
    assert(_sim->analysis_is_tran_dynamic());
    METHOD method;
    if (_time[1] == 0) {
      method = mEULER; // Bogus current in previous step.  Force Euler.
    }else{
      method = _method_a;
    }
    g = c / _dt;
    switch (method) {
    case mTRAPGEAR: incomplete();
    case mGEAR:	 g *= 3./2.;	break;
    case mTRAPEULER: incomplete();
    case mEULER: /* g *= 1 */	break;
    case mTRAP:	 g *= 2;	break;
    }
    return g;
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR STORAGE::tr_review()
{
  COMPONENT::tr_review(); // skip ELEMENT
  if (_method_a == mEULER) {
    // Backward Euler, no step control, take it as it comes
  }else{
    double timestep = tr_review_trunc_error(_y);
    _time_by.min_error_estimate(tr_review_check_and_convert(timestep));
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
double STORAGE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "method ")) {untested();
    return static_cast<double>(_method_a);
  }else{
    return ELEMENT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
void STORAGE::set_ic( double x ){
  if(has_common()){
    trace1("STORAGE::set_ic have common "+long_label(), x);
    mutable_common()->set_ic(x);
  } else {
    trace3("STORAGE::set_ic nop", x, _m0, _m1);
  }
} 
/*--------------------------------------------------------------------------*/
//void STORAGE::keep_ic( ){
//
//  trace4("STORAGE::keep_ic",IN1,IN2,_n[IN1].m_(),_n[IN2].m_());
//  double x =  dn_diff(_sim->_vdc[_n[IN1].m_()],_sim->_vdc[_n[IN2].m_()] ); //tr_involts();
//  if(has_common()){
//    trace2("STORAGE::keep_ic", x,long_label());
//    mutable_common()->set_ic(x);
//  } else {
//    trace2("STORAGE::keep_ic b ", x,long_label());
//  *(_value.pointer_hack()) = x;
//  // do_tr();
//  // tr_load();
//  q_eval(); 
//  }
//} 
void STORAGE::keep_ic( ){
  //hack can we use _sim->_v0 instead??
  double x =  dn_diff(_sim->_vdc[_sim->_nm[IN1]],_sim->_vdc[_sim->_nm[IN2]] ); //tr_involts();
  trace3("STORAGE::keep_ic", _sim->_nm[IN1], _sim->_nm[IN2] , x);
  if(has_common()){
    trace1("D_CAP::keep_ic have common " +long_label(),x);
    mutable_common()->set_ic(x);
  } else {
    trace1("D_CAP::keep_ic no common "+ long_label(),x);
    error(bDANGER, "no common for %s\n", long_label().c_str() );
    assert(false);
    // *(_x.pointer_hack()) = x;
  // do_tr(); mutable_common()->set_ic(x);
  // tr_load();
    // q_eval(); 
    // do_tr(); mutable_common()->set_ic(x);
    // tr_load();
    // q_eval(); 
  }
} 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
