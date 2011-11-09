/* adms base 2011 felix salfelder
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
 * Base class for adms models (former ELEMENT & STORAGE)
 */
//testing=script 2006.07.12
#include "m_divdiff.h"
#include "u_xprobe.h"
#include "e_aux.h"
#include "e_adms.h"
/*--------------------------------------------------------------------------*/
ADMS_BASE::ADMS_BASE():
  BASE_SUBCKT(),
   _loaditer(0),
   _m0(),
   _m1(),
   _loss0(0.),
   _loss1(0.),
   _acg(0.),
   _ev(0.),
   _dt(0.),
  	_method_u(meUNKNOWN), _method_a(mTRAPGEAR) 
{
  _n = _nodes;
  assert(_y[0].x == 0. && _y[0].f0 == 0. && _y[0].f1 == 0.);
  assert(_y1 == _y[0]);

  std::fill_n(_time, int(OPT::_keep_time_steps), 0.);
}
/*--------------------------------------------------------------------------*/
ADMS_BASE::ADMS_BASE(const ADMS_BASE& p)
    : BASE_SUBCKT(p),
   _loaditer(0),
   _m0(),
   _m1(),
   _loss0(p._loss0),
   _loss1(p._loss1),
   _acg(0.),
   _ev(0.),
   _dt(0.),
	 _method_u(p._method_u), _method_a(p._method_a)
{
  trace0(long_label().c_str());
  _n = _nodes;
  if (p._n == p._nodes) {
    for (int ii = 0;  ii < NODES_PER_BRANCH;  ++ii) {
      _n[ii] = p._n[ii];
    }
  }else{
    assert(p._nodes);
    // the constructor for a derived class will take care of it
  }
  assert(_y[0].x == 0. && _y[0].f0 == 0. && _y[0].f1 == 0.);
  assert(_y1 == _y[0]);

  notstd::copy_n(p._time, int(OPT::_keep_time_steps), _time);
}
/*--------------------------------------------------------------------------*/
bool ADMS_BASE::skip_dev_type(CS& cmd)
{
  return cmd.umatch(dev_type() + ' ');
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::precalc_last()
{
  BASE_SUBCKT::precalc_last();

  //BUG// This is needed for AC analysis without doing op (or dc or tran ...) first.
  // Something like it should be moved to ac_begin.
  if (_sim->is_first_expand()) {
    _y[0].x  = 0.;
    _y[0].f0 = LINEAR;
    _y[0].f1 = value();
  }else{
  }

  /// from sto
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
void ADMS_BASE::tr_begin()
{
  trace0(("ADMS_BASE::tr_begin for " + short_label()).c_str());
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

  //from st.
  _method_a = method_select[OPT::method][_method_u];
  for (int i = 0;  i < OPT::_keep_time_steps;  ++i) {
    _i[i] = FPOLY1(0., 0., 0.);
  }
  _m1 = _m0 = CPOLY1(0., 0., 0.);
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_restore()
{
  trace0(("ADMS_BASE::tr_restore for " + short_label()).c_str());
  if (_time[0] > _sim->_time0) {itested();
    trace0("shift back");
    for (int i=0  ; i<OPT::_keep_time_steps-1; ++i) {itested();
      _time[i] = _time[i+1];
      _y[i] = _y[i+1];
    }
    _time[OPT::_keep_time_steps-1] = 0.;
    _y[OPT::_keep_time_steps-1]    = FPOLY1(0., 0., 0.);
  }else if (_time[0] == _sim->_time0) {
    trace2( "*no shift ", _time[0] , _sim->_time0 );
  }else{untested();
  }

  //if( _time[0] != _sim->_time0 )
//  assert(_time[0] == _sim->_time0);
  if (_time[0] != _sim->_time0) {itested();
    error(bDANGER, "//BUG// restore time mismatch.  t0=%.12f, s->t=%.12f\n",
	 _time[0], _sim->_time0);
    //BUG// happens when continuing after a ^c,
    // when the last step was not printed
    // _time[0] is the non-printed time.  _sim->_time0 is the printed time.
  }else{
  }

  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
  }

  _method_a = method_select[OPT::method][_method_u];
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::dc_advance() // from elt.
{
  assert(_sim->_time0 == 0.); // DC

  bool ass = true;;

  for (int i=OPT::_keep_time_steps-1; i>=0; --i) {
    trace2(( "ADMS_BASE::dc_advance " + long_label()).c_str(), i, _time[i]);
    ass &= _time[i] == 0.;
  }
  assert(ass);

  _dt = NOT_VALID;
  

  //from storag
  for (int i = 1;  i < OPT::_keep_time_steps;  ++i) {
    _i[i] = _i[0];
  }
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_advance()
{
  trace1("ADMS_BASE::tr_advance ", _sim->_time0 );
  assert(_time[0] < _sim->_time0); // moving forward
  
  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
    _time[i] = _time[i-1];
    _y[i] = _y[i-1];
  }
  _time[0] = _sim->_time0;

  _dt = _time[0] - _time[1];

  //storag
  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    _i[i] = _i[i-1];
  }
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_regress()
{
  assert(_time[0] >= _sim->_time0); // moving backwards
  assert(_time[1] <= _sim->_time0); // but not too far backwards

  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
  }
  _time[0] = _sim->_time0;

  _dt = _time[0] - _time[1];
}
/*--------------------------------------------------------------------------*/
TIME_PAIR ADMS_BASE::tr_review()
{
  BASE_SUBCKT::tr_review();
  if (_method_a == mEULER) {
    // Backward Euler, no step control, take it as it comes
  }else{
    double timestep = tr_review_trunc_error(_y);
    _time_by.min_error_estimate(tr_review_check_and_convert(timestep));
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_iwant_matrix_passive()
{
  assert(matrix_nodes() == 2);
  assert(is_device());
  //assert(!subckt()); ok for subckt to exist for logic
  trace2(long_label().c_str(), _n[OUT1].m_(), _n[OUT2].m_());

  if (_n[OUT1].m_() == INVALID_NODE) {
    std::cerr << "ADMS_BASE::tr_iwant_matrix_passive " << long_label() << "\n";
    exit(4);
  }
  assert(_n[OUT2].m_() != INVALID_NODE);

  _sim->_aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_iwant_matrix_active()
{
  std::cerr << "ADMS_BASE::tr_iwant_matrix_active " << long_label() << "\n";
  assert(matrix_nodes() == 4);
  assert(is_device());
  assert(!subckt());

  assert(_n[OUT1].m_() != INVALID_NODE);
  assert(_n[OUT2].m_() != INVALID_NODE);
  assert(_n[IN1].m_() != INVALID_NODE);
  assert(_n[IN2].m_() != INVALID_NODE);

  //_sim->_aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_aa.iwant(_n[OUT1].m_(),_n[IN1].m_());
  _sim->_aa.iwant(_n[OUT1].m_(),_n[IN2].m_());
  _sim->_aa.iwant(_n[OUT2].m_(),_n[IN1].m_());
  _sim->_aa.iwant(_n[OUT2].m_(),_n[IN2].m_());
  //_sim->_aa.iwant(_n[IN1].m_(),_n[IN2].m_());

  //_sim->_lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[IN1].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[IN2].m_());
  _sim->_lu.iwant(_n[OUT2].m_(),_n[IN1].m_());
  _sim->_lu.iwant(_n[OUT2].m_(),_n[IN2].m_());
  //_sim->_lu.iwant(_n[IN1].m_(),_n[IN2].m_());
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_iwant_matrix_extended()
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (uint_t ii = 0;  ii < matrix_nodes();  ++ii) {
    if (_n[ii].m_()  != INVALID_NODE) {
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
void ADMS_BASE::ac_iwant_matrix_passive()
{
  trace2(long_label().c_str(), _n[OUT1].m_(), _n[OUT2].m_());
  _sim->_acx.iwant(_n[OUT1].m_(),_n[OUT2].m_());
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::ac_iwant_matrix_active()
{
  //_sim->_acx.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_acx.iwant(_n[OUT1].m_(),_n[IN1].m_());
  _sim->_acx.iwant(_n[OUT1].m_(),_n[IN2].m_());
  _sim->_acx.iwant(_n[OUT2].m_(),_n[IN1].m_());
  _sim->_acx.iwant(_n[OUT2].m_(),_n[IN2].m_());
  //_sim->_acx.iwant(_n[IN1].m_(),_n[IN2].m_());
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::ac_iwant_matrix_extended()
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (uint_t ii = 0;  ii < matrix_nodes();  ++ii) {
    if (_n[ii].m_() !=INVALID_NODE) {
      for (uint_t jj = 0;  jj < ii ;  ++jj) {
	_sim->_acx.iwant(_n[ii].m_(),_n[jj].m_());
      }
    }else{itested();
      // node 1 is grounded or invalid
    }
  }
}
/*--------------------------------------------------------------------------*/
hp_float_t ADMS_BASE::tr_amps()const
{
	//return 44;
  trace5("ADMS_BASE::tr_amps " + long_label(), _loss0, tr_outvolts(), _m0.c1, tr_involts(), _m0.c0);
  trace1("ADMS_BASE::tr_amps " + long_label(), _loss0 * tr_outvolts() + _m0.c1 * tr_involts() + _m0.c0 );
  hp_float_t root = fixzero((_loss0 * tr_outvolts() + _m0.c1 * tr_involts() + _m0.c0),
      _m0.c0);

  assert (root==root);
  return (root);
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tr_save_amps(int n){

  trace2( "ADMS_BASE::tr_save_amps ",  n, _sim->_tt_accepted);
  hp_float_t tramps = tr_amps();

  hp_float_t _tr_amps_diff_cur;
  hp_float_t _tr_amps_sum_cur;

  if( _amps_new == 0 ){
    incomplete(); // somebody forgot to initilize
    return;
  }

  if(_amps!=0) {
    trace3( "saving _amps[ ",  n ,  net_nodes(), _amps[n] );
    _tr_amps_sum_cur = fabs( _amps[n] ) + fabs( tramps );
    _tr_amps_diff_cur = fabs( _amps[n] - tramps );
    //		std::cerr << "ADMS_BASE::tr_save_amps: not first. diff= " << _tr_amps_diff_cur << "\n";
  } else {
    trace2( "dummy _amps[ ",  n ,  net_nodes() );
    _tr_amps_diff_cur = 0;
    _tr_amps_sum_cur = 1;
  }

  //	std::cerr << short_label() << ": saving _amps[ " << n << " ]" << _amps << " \n";
  trace2( (short_label() + " have ").c_str(), tramps, n);
  _amps_new[ n ]= tramps;


  trace1("ADMS_BASE::tr_save_amps", _tr_amps_sum_cur);
  tr_behaviour_del = _tr_amps_diff_cur;
  tr_behaviour_rel = _tr_amps_diff_cur / _tr_amps_sum_cur;


  //	tr_behaviour_del = 0;
  //	tr_behaviour_rel = 0;

  BASE_SUBCKT::tr_behaviour();

}
/*--------------------------------------------------------------------------*/
double ADMS_BASE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v{out} ")) {
    return tr_outvolts();
  }else if (Umatch(x, "amps_diff ")) {
    return tr_amps_diff();
  }else if (Umatch(x, "vi{n} ")) {
    return tr_involts();
  }else if (Umatch(x, "i ")) {
    return tr_amps();
  }else if (Umatch(x, "p ")) {
    return tr_amps() * tr_outvolts();
  }else if (Umatch(x, "pd ")) {
    double p = tr_amps() * tr_outvolts();
    return (p > 0.) ? p : 0.;
  }else if (Umatch(x, "ps ")) {
    double p = tr_amps() * tr_outvolts();
    return (p < 0.) ? -p : 0.;
  }else if (Umatch(x, "in{put} ")) {
    return _y[0].x;
  }else if (Umatch(x, "f ")) {
    if (_y[0].f0 == LINEAR) {itested();
      return _y[0].x * _y[0].f1;
    }else{
      return _y[0].f0;
    }
  }else if (Umatch(x, "ev |df ")) {
    return _y[0].f1;
  }else if (Umatch(x, "dx ")) {
    return _y1.x-_y[0].x;
  }else if (Umatch(x, "res ")) {
    return _y1.f0-_y[0].f0;
  }else if (Umatch(x, "conv ")) {
    // assert(converged()==conv_check());
    if (conv_check()) { 
      return 10; } 
    //  fprintf(stderr,"hi\n");
    return 1993;
  }else if (Umatch(x, "nv ")) {
    return value();
  }else if (Umatch(x, "eiv ")) {untested();
    return _m0.x;
  }else if (Umatch(x, "y ")) {
    return _m0.c1;
  }else if (Umatch(x, "is{tamp} ")) {
    return _m0.f0();
  }else if (Umatch(x, "iof{fset} ")) {itested();
    return _m0.c0;
  }else if (Umatch(x, "ip{assive} ")) {itested();
    return _m0.c1 * tr_involts();
  }else if (Umatch(x, "il{oss} ")) {untested();
    return _loss0 * tr_outvolts();
  }else if (Umatch(x, "dt ")) {
    return _dt;
  }else if (Umatch(x, "dtr{equired} ")) {
    return ((_time_by._error_estimate - _time[0]) > 0)
      ? _time_by._error_estimate - _time[0]
      : _time_by._error_estimate - _time[1];
  }else if (Umatch(x, "time ")) {untested();
    return _time[0];
  }else if (Umatch(x, "timeo{ld} ")) {untested();
    return _time[1];
  //}else if (Umatch(x, "didt ")) {untested();
    //double i0  = (_m0.c1  * _m0.x  + _m0.c0);
    //double it1 = (mt1.f1 * mt1.x + mt1.c0);
    //return  (i0 - it1) / (_sim->_time0 - _time1);
  }else if (Umatch(x, "r ")) {
    return (_m0.c1!=0.) ? 1/_m0.c1 : MAXDBL;
  }else if (Umatch(x, "z ")) {
    return port_impedance(_n[OUT1], _n[OUT2], _sim->_lu, (double) (mfactor()*(_m0.c1+_loss0)));
  }else if (Umatch(x, "zraw ")) {
    return port_impedance(_n[OUT1], _n[OUT2], _sim->_lu, 0.);
  }else{
    return BASE_SUBCKT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
COMPLEX ADMS_BASE::ac_amps()const
{
  assert(!is_source());
  return (ac_involts() * _acg + ac_outvolts() * (double)_loss0);
}
/*--------------------------------------------------------------------------*/
XPROBE ADMS_BASE::ac_probe_ext(const std::string& x)const
{
  COMPLEX admittance = (is_source()) ? _loss0 : _acg+ (double)_loss0;

  if (Umatch(x, "v{out} ")) {			/* volts (out) */
    return XPROBE(ac_outvolts());
  }else if (Umatch(x, "vin ")) {		/* volts (in) */
    return XPROBE(ac_involts());
  }else if (Umatch(x, "i ")) {			/* amps */
    return XPROBE(ac_amps());
  }else if (Umatch(x, "p ")) {			/* complex "power" */
    return XPROBE(ac_outvolts() * conj(ac_amps()), mtREAL, 10.);
  }else if (Umatch(x, "nv ")) {untested();	/* nominal value */
    return XPROBE(value());
  }else if (Umatch(x, "ev ")) {			/* effective value */
    return XPROBE((COMPLEX)_ev);
  }else if (Umatch(x, "y ")) {untested();		/* admittance */
    return XPROBE(admittance, mtREAL);
  }else if (Umatch(x, "r ")) {			/* complex "resistance" */
    if (admittance == 0.) {untested();
      return XPROBE(MAXDBL);
    }else{untested();
      return XPROBE(1. / admittance);
    }
  }else if (Umatch(x, "z ")) {			/* port impedance */
    return XPROBE(port_impedance(_n[OUT1], _n[OUT2], _sim->_acx, mfactor()*admittance));
  }else if (Umatch(x, "zraw ")) {		/* port impedance, raw */
    return XPROBE(port_impedance(_n[OUT1], _n[OUT2], _sim->_acx, COMPLEX(0.)));
  }else{ 					/* bad parameter */
    return BASE_SUBCKT::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
double ADMS_BASE::tr_review_trunc_error(const FPOLY1* q)
{
  trace1("ADMS_BASE::tr_review_trunc_error", order());
  int error_deriv = order()+1;
  double timestep;
  trace1("ADMS_BASE::tr_review_trunc_error", error_deriv);
  trace2("ADMS_BASE::tr_review_trunc_error", _time[0], error_deriv);
  if (_time[0] <= 0.) {
    // DC, I know nothing
    timestep = NEVER;
  }else if (_time[error_deriv] <= 0.) {
    // first few steps, I still know nothing
    // repeat whatever step was used the first time
    timestep = _dt;
  }else{
    for (int i=error_deriv; i>0; --i) {
      assert(_time[i] < _time[i-1]); // || _time[i] == 0.);
    }

    double c[OPT::_keep_time_steps];
    for (int i=0; i<OPT::_keep_time_steps; ++i) {
      c[i] = q[i].f0;
    }
    assert(error_deriv < OPT::_keep_time_steps);
    derivatives(c, OPT::_keep_time_steps, _time);
    // now c[i] is i'th derivative
    
    assert(OPT::_keep_time_steps >= 5);
    trace0(("ts" + long_label()).c_str());
    trace5("time", _time[0], _time[1], _time[2], _time[3], _time[4]);
    trace5("charge", q[0].f0, q[1].f0, q[2].f0, q[3].f0, q[4].f0);
    trace5("deriv", c[0], c[1], c[2], c[3], c[4]);
    
    if (c[error_deriv] == 0) {
      timestep = NEVER;
    }else{
      double chargetol = std::max(OPT::chgtol,
				OPT::reltol * std::max((double)std::abs(q[0].f0), (double) std::abs(q[1].f0)));
      double tol = OPT::trtol * chargetol;
      double denom = error_factor() * std::abs(c[error_deriv]);
      assert(tol > 0.);
      assert(denom > 0.);
      switch (error_deriv) { // pow is slow.
      case 1:	timestep = tol / denom;		break;
      case 2:	timestep = sqrt(tol / denom);	break;
      case 3:	timestep = cbrt(tol / denom);	break;
      default:	timestep = pow((tol / denom), 1./(error_deriv)); break;
      }
      trace4("", chargetol, tol, denom, timestep);
    }
  }
  assert(timestep > 0.);
  return timestep;
}
/*--------------------------------------------------------------------------*/
double ADMS_BASE::tr_review_check_and_convert(double timestep)
{
  double time_future;
  if (timestep == NEVER) {
    time_future = NEVER;
  }else{
    if (timestep < _sim->_dtmin) {
      timestep = _sim->_dtmin;
    }else{
    }

    if (timestep < _dt * OPT::trreject) {
      if (_time[order()] == 0) {
	error(bWARNING, "initial step rejected:" + long_label() + '\n');
	error(bWARNING, "new=%g  old=%g  required=%g\n",
	      timestep, _dt, _dt * OPT::trreject);
      }else{
	error(bTRACE, "step rejected:" + long_label() + '\n');
	error(bTRACE, "new=%g  old=%g  required=%g\n",
	      timestep, _dt, _dt * OPT::trreject);
      }
      time_future = _time[1] + timestep;
      trace3("reject", timestep, _dt, time_future);
    }else{
      time_future = _time[0] + timestep;
      trace3("accept", timestep, _dt, time_future);
    }
  }
  assert(time_future > 0.);
  assert(time_future > _time[1]);
  return time_future;
}
/*--------------------------------------------------------------------------*/
void ADMS_BASE::tt_next()
{
  // das tut das hier?
  // untested0(("tt_next for " + short_label()).c_str());
  trace2(("ADMS_BASE::tt_next for " + short_label()).c_str(), _sim->_time0, _sim->_dt0);
  if (_time[0] > _sim->_time0) {itested();
    for (int i=0  ; i<OPT::_keep_time_steps-1; ++i) {itested();
      _time[i] = _time[i+1];
      _y[i] = _y[i+1];
    }
    _time[OPT::_keep_time_steps-1] = 0.;
    _y[OPT::_keep_time_steps-1]    = FPOLY1(0., 0., 0.);
  }else if (_time[0] == _sim->_time0) {

  }else{

  }

  //assert(_time[0] == _sim->_time0);
  if (_time[0] != _sim->_time0) {itested();
    trace1("ADMS_BASE::tt_next timedelta ", _time[0] - _sim->_time0 );
    trace2( ( "HACK? " + short_label() + ": ADMS_BASE::tt_next, time mismatch, setting back to 0 " ).c_str(),
        _sim->_time0, _time[0] );
  }else{
    trace2(("tt_next for " + short_label()).c_str(), _time[0], _sim->_time0);
  }

  for (int i=OPT::_keep_time_steps-1; i>=0; --i) {
        // FIXME: copy all timesteps to 0
    _time[i]=0.0;
        //    assert(_time[i] < _time[i-1] || _time[i] == 0.);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:et
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
#include "e_adms.h"
/*--------------------------------------------------------------------------*/
/* table for selecting local integraton method
 * Determines which one wins in a conflict.
 * "only" wins over non-only.  local (_method_u) wins over opt.
 */
//                     OPT::method    _method_u
METHOD ADMS_BASE::method_select[meNUM_METHODS][meNUM_METHODS] = {
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
void	  ADMS_BASE::tr_queue_eval()	{
	BASE_SUBCKT::tr_queue_eval();

	if(tr_needs_eval()) {
		q_eval();
	}else{
	}
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval: check to see if this device needs to be evaluated
 * this works, and saves significant time
 * but possible errors.
 * Errors do not seem significant, but I can't tell without more data.
 * To disable:  option nolcbypass
 */
bool ADMS_BASE::tr_needs_eval()const
{
  //assert(!is_q_for_eval());
  return (!OPT::lcbypass
	  || !converged() 
	  || _sim->is_advance_or_first_iteration()
	  || !conchk(_y[0].x, tr_input(), OPT::abstol)
	  || _sim->uic_now());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double ADMS_BASE::tr_c_to_g(double c, double g)const
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
/*--------------------------------------------------------------------------*/

