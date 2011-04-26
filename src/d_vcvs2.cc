/*$Id: d_vcvs.cc,v 1.6 2010-04-15 07:42:56 felix Exp $ -*- C++ -*-
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
 * functions for vcvs
 * temporary kluge: it has resistance
 */
//testing=script,complete 2006.07.17
#include "e_homebrew.h"


bool myconchk(double o, double n,
		   double a=OPT::abstol, double r=OPT::reltol)
{
  return (std::abs(n-o) <= (r * std::abs(n) + a));
}
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
  // no longer needed device.
class DEV_VCVS2 : public HOMEBREW {
private:
  explicit DEV_VCVS2(const DEV_VCVS2& p) :HOMEBREW(p) {}
public:
  explicit DEV_VCVS2()		:HOMEBREW() {}
private: // override virtual
  char	   id_letter()const	{return 'A';}
  std::string value_name()const {return "gain";}
  std::string dev_type()const	{return "vcvs2";}
  uint_t	   max_nodes()const	{return 4;}
  uint_t	   min_nodes()const	{return 4;}
  uint_t	   matrix_nodes()const	{return 4;}
  uint_t	   net_nodes()const	{return 4;}
  bool	   use_obsolete_callback_parse()const {untested(); return false;}
  CARD*	   clone()const		{return new DEV_VCVS2(*this);}
  void     precalc_last();
  void	   tr_iwant_matrix();
  void     tr_begin();
  void     tr_regress();
  void     tr_advance();
  bool	   do_tr();
  void	   tr_load();
  void	   tr_unload()		{untested();tr_unload_active();}
  hp_float_t tr_involts()const {return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  hp_float_t tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  hp_float_t tr_amps()const{ std::cout << short_label() ; return 881;}	//HOMEBREW
  bool      has_tr_eval()const {return true;}
  void      tr_eval();
  bool      conv_check() const;

  void	   ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void	   ac_begin();
  void	   do_ac();
  void	   ac_load()		{ac_load_shunt(); ac_load_active();}
  COMPLEX  ac_involts()const	{return _n[IN1].vac() - _n[IN2].vac();}

  std::string port_name(uint_t i)const {
    assert(i !=INVALID_NODE);
    assert(i < 4);
    static std::string names[] = {"p", "n", "ps", "ns"};
    return names[i];
  }
public:
  bool     tr_needs_eval() const {return true;}
 
private:
  virtual void tt_next(); // !const
};
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tr_iwant_matrix() // extended aus ELEMENT
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (uint_t ii = 0;  ii < matrix_nodes();  ++ii) {
    if (_n[ii].m_() != INVALID_NODE ) {
      for (uint_t jj = 0;  jj < ii ;  ++jj) {
	_sim->_aa.iwant(_n[ii].m_(),_n[jj].m_());
	_sim->_lu.iwant(_n[ii].m_(),_n[jj].m_());
      }
    }else{itested();
      // node 1 is grounded or invalid
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tr_advance()
{
  // untested();
  assert(_time[0] < _sim->_time0); // moving forward
  
  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
    _time[i] = _time[i-1];
    _y[i] = _y[i-1];
  }
  _time[0] = _sim->_time0;

  _dt = _time[0] - _time[1];
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tr_load()
{
  // untested();

  tr_load_shunt();

// load_active:
  assert( _m0.c1 == _m0.c1 );
  double d = dampdiff(&_m0.c1, _m1.c1);
  if (d != 0.) {
    _sim->_aa.load_asymmetric(_n[OUT1].m_(), _n[OUT2].m_(),
		       _n[IN1].m_(), _n[IN2].m_(), d);
  }else{
  }
  tr_load_source(); // includes _m1 = _m0

  return; 
  _m0.x  = 0.;
  _m0.c1 = -_loss0 * _y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;

  // tr_load_active(); -----------
  assert( _m0.c1 == _m0.c1 );
  double dd = dampdiff(&_m0.c1, _m1.c1);
  if (d != 0.) {
    _sim->_aa.load_asymmetric(_n[OUT1].m_(), _n[OUT2].m_(),
		       _n[IN1].m_(), _n[IN2].m_(), dd);
  }else{
  }
        
 //  std::cout << _sim->_aa << "\n";

  tr_load_source(); // includes _m1 = _m0
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tr_regress()
{
  // untested();
  assert(_time[0] >= _sim->_time0); // moving backwards
  assert(_time[1] <= _sim->_time0); // but not too far backwards

  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
  }
  _time[0] = _sim->_time0;

  _dt = _time[0] - _time[1];
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tr_eval()
{
  double x = (_y[0].x);
  double f0 = 0.;
  double f1 = 0.;
  double _max=100;
  double _min=-100;

 //       value = uref...
  f0 = value() * (exp( x / value() ) - 1 ) / ( exp(1) -1 ) ; // _c[0] +    x * _c[1] + x * x * _c[2] ;
  f0 = x;
  f1 = 0; // exp(x); //_c[1] + 2 *x * _c[2];
  //f1 = exp(x); //_c[1] + 2 *x * _c[2];
  //
//  f1=1+x;

  // std::cout << "* " << long_label() << " " << f0 << "\n";

  if ( f0 < 0) {
    trace1("falschrum!", f0);
//    f0 = -f0;
    f1 = -f1;
  }
  if (f0 > _max) {
    trace0("zu gross");
    f0 = _max;
    f1 = 0;
  }else if (f0 < _min) {
    trace0("zu klein");
    f0 = _min;
    f1 = 0;
  }
  trace3("DEV_VCVS2 ",x, f0, f1);
  _y[0] = FPOLY1(x, f0, f1);
  //tr_final_adjust(&(d->_y[0]), d->f_is_value());

//void EVAL_BM_ACTION_BASE::tr_final_adjust(FPOLY1* y, bool f_is_value)const
  // *y *= temp_adjust();
  // y->f0 += _ooffset;

  bool bla = conv_check();
  if (!bla) {
    trace1(("DEV_VCVS2::tr_eval not converged " + long_label()).c_str(),_sim->_time0);
  }
  else
  {
//    trace1("DEV_VCVS2::tr_eval convx",_sim->_time0);
  }
  set_converged(bla);
}
/*--------------------------------------------------------------------------*/
bool DEV_VCVS2::conv_check()const
{
//  trace6("DEV_VCVS2::convx_check ", _y1.f1, _y[0].f1 , _y1.f0, _y[0].f0 , _y1.x,  _y[0].x );
//  trace4("DEV_VCVS2::convx_check ",
//      myconchk(_y1.f1, _y[0].f1),
//      myconchk(_y1.f0, _y[0].f0),
//      myconchk(_y1.x,  _y[0].x, OPT::vntol),
//      OPT::vntol );


  return myconchk(_y1.f1, _y[0].f1)
    && myconchk(_y1.f0, _y[0].f0)
    && myconchk(_y1.x,  _y[0].x, OPT::vntol);
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::precalc_last()
{
  // untested();
  HOMEBREW::precalc_last();
  set_constant(!has_tr_eval());
  // untested();
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tr_begin()
{
  // untested();
  std::cerr << "DEV_VCVS2::tr_begin " << value() << " 1\n";

//void HOMEBREW::tr_begin()
  _time[0] = 0.;
  _y[0].x  = 0.;
  _y[0].f0 = LINEAR;
  _y[0].f1 = value();
  _y1 = _y[0];
  for (int i=1; i<OPT::_keep_time_steps; ++i) {
    _time[i] = 0.;
    _y[i] = FPOLY1(0., 0., 0.);
  }
  _dt = NOT_VALID;

  _loss1 = _loss0 = 1./OPT::shortckt;
  _m0.x  = 0.;
  _m0.c1 = -_loss0 * _y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
//  set_converged(true);
}
/*--------------------------------------------------------------------------*/
bool DEV_VCVS2::do_tr()
{
  // taken from d_vcvs. but no !tr_eval case
  _y[0].x = _m0.x = tr_involts_limited();
  tr_eval();
  assert(_y[0].f0 != LINEAR);
  store_values();
  q_load();
  _m0 = CPOLY1(_y[0]);
  _m0 *= -_loss0;

  bool c = converged();

//  if (!c) cout << "???\n";
  return c;
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::tt_next()
{
  // was tut das hier?
  // untested0(("tt_next for " + short_label()).c_str());
  if (_time[0] > _sim->_time0) {itested();
    for (int i=0  ; i<OPT::_keep_time_steps-1; ++i) {itested();
      _time[i] = _time[i+1];
      _y[i] = _y[i+1];
    }
    _time[OPT::_keep_time_steps-1] = 0.;
    _y[OPT::_keep_time_steps-1]    = FPOLY1(0., 0., 0.);
  }else if (_time[0] == _sim->_time0) {
  }else{
    trace0("DEV_VCVS2::tt_next problem");
  }

  //assert(_time[0] == _sim->_time0);
  if (_time[0] != _sim->_time0) {itested();
    trace1("timedelta ", _time[0] - _sim->_time0 );
    trace2( ( "HACK? " + short_label() + ": DEV_VCVS2::tt_next, time mismatch, setting back to 0 " ).c_str(),
        _sim->_time0, _time[0] );
  }else{
  }

  for (int i=OPT::_keep_time_steps-1; i>=0; --i) {
    _time[i]=0.0;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::ac_begin()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  _ev = _y[0].f1;
  _acg = -_loss0 * _ev;
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS2::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = -_loss0 * _ev;
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == hp_float_t(value()));
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_VCVS2 p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "A|vcvs2", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
