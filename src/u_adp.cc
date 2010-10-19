/* vim:ts=8:sw=2:et:
 * Copyright (C) 2010 Felix Salfelder
 * Author: felix@salfelder.org
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

#include "u_adp.h"
#include <gsl/gsl_fit.h>
#define DEBUG_ADP

/*
''=working; accessed by get.
0=current time step.
1=old time step. accepted value only
2=history
*/

bool is_number(double x){
  return (( x != inf ) && (x != -inf ) && (x == x)) ;
}

/*----------------------------------------------------------------------------*/
void ADP_NODE::init(){
  tr_value = (0.);
  tr_value0 = (0.);
  tr_value1 = (0.);
  tr_value2 = (0.);
  tt_value = 0.;
  _delta_expect = 0.;
  _positive = true;
  _debug = 0;
  _order = 0;
  _wdT = 0;
  _rel_tr_err = 0;
  _rel_tt_err = 0;
  _abs_tr_err = 0;
  _abs_tt_err = 0;
  name = std::string(""); // inherit label?
  dbg++;
  _val_bef = new double[3];
  _val_aft = new double[3]; // necessary?? should be _delta + _val_bef
  _der_aft = new double[3];
  _delta   = new double[3];
  for (int i = 0; i<2; i++){ // fixme?
    _val_bef[i] = 0;
    _val_aft[i] = 0;
    _der_aft[i] = 0;
    _delta[i] = 0;
  }
}
/*----------------------------------------------------------------------------*/
ADP_NODE::~ADP_NODE(){
  delete [] _delta;
  delete [] _der_aft;
  delete [] _val_bef;
  delete [] _val_aft;
}
/*----------------------------------------------------------------------------*/
ADP_NODE::ADP_NODE( const ADP_NODE& p ):
  _val_bef(0),
  _val_aft(0),
  _der_aft(0),
  _delta(0),
  name(p.name)
{
  std::cout << "copy?? (should not happen)\n";
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
double ADP_NODE::tr_duration()const{ return _c->_sim->_last_time; }
/*----------------------------------------------------------------------------*/
// called right after tr
double ADP_NODE::tt_review( ) {
  // call corrector...
  double delta_model;
  if (_corrector){
    trace0("correction");
    delta_model = (this->*_corrector)(); // value predicted by model.
  } else {
        delta_model = _delta_expect;
  }

  trace1("ADP_NODE::tt_review", _c->_sim->tt_iteration_number());
  assert(delta_model == delta_model);
  assert(delta_model != HUGE_VAL );
  hp_float_t myreltol = OPT::adpreltol;
  hp_float_t myabstol = OPT::adpabstol;

  // FIXME: implement abs err!
  _abs_tr_err = fabs (tr_value - delta_model);

  if (_abs_tr_err < 1e-12 ) { 
    _rel_tr_err = 0;
  } else {
    _rel_tr_err = fabs (tr_value - delta_model) / max( fabs(tr_value) , fabs(delta_model));
  }
  _rel_tt_err = fabs (tt_value - tt_expect) / (fabs(tt_value) + fabs(tt_expect));

  tr_value = delta_model;

  // _debug=_rel_tt_err;

  if( myreltol == 0 ) return 0;

  trace2("ADP_NODE::tt_review", myreltol, inf);
  if( _abs_tr_err == 0 ) { _wdT = inf; return inf; }
  if( myreltol == inf && myabstol == inf ) { _wdT = inf; return inf;}
  if( myreltol == 0 && myabstol == 0 ) {_wdT=0; return 0;}

  double h = tr_duration();
// FIXME: _order.
//
  _wdT = (dT0()-h) * sqrt( myreltol / _rel_tr_err )  + h;
  if( _wdT < 0  || _wdT != _wdT) {
    error(bDANGER, "_wdT %s %f %f %f\n", name.c_str(), _wdT, dT0(), _rel_tr_err );
  }

  if( _rel_tr_err != _rel_tr_err ) _wdT = NEVER;
  if( !(_rel_tr_err == _rel_tr_err) ) _wdT = NEVER;

  trace3( "ADP_NODE::tt_review ", _c->_sim->_Time0 , _wdT , myreltol );

  assert(_wdT >= 0 );

  assert(tr_value == tr_value);
  assert(_delta_expect == _delta_expect);
  tt_value += tr_value; // only printed if OPT::printrejected

  return _wdT;
}
/*----------------------------------------------------------------------------*/
TIME_PAIR ADP_NODE::tt_preview( ) {
  TIME_PAIR _time_pre;
    _time_pre.reset();
    return _time_pre;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::reset() {
  trace1( "ADP_NODE::reset()", _c->_sim->_Time0 );
  tr_value = 0; // _val_bef[1] = 0; _val_bef[0] =0;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_accept()
{
  // sets t{t,r}_value0 to the accepted values.
  // should compute new wanted_dT.

  // _c->_sim->update_tt_order(); ?
  //
  trace3(("ADP_NODE::tt_accept() " + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), _c->_sim->get_tt_order());
  assert(tr_value0 == tr_value0);
  assert(_c->_sim->_last_time >0 );

  int order=_c->_sim->get_tt_order();

  if (order != _order){
    trace2("ADP_NODE::tt_accept", order, _order);
  }

  switch( order ){
    case 0:
      // fixme. merge
      return tt_accept_first();
      break;
    case 1:
      tr_dd12 = ( tr_value0 - tr_value1 ) / dT0();
      break;
    case 2:
      tr_dd23 = tr_dd12;
      tr_dd123 = (tr_dd12 - tr_dd23)/( dT1() + dT0() ); 
      break;
    case 3:
      break;
    default:
      assert(false);
  }

  // DONT call corrector.

  tr_value0 = tr_value; // obsolete
  _delta[0] = tr_value; 

  if (_integrator){ // sets tt_value.
    trace0("reintegration");
    tr_value=tr_value0;
    tt_value = (this->*_integrator)(tr_value0);
  }

  assert(tt_value == tt_value);

  _val_bef[0] = tt_value; // obsolete
  _val_bef[0] = tt_value;

  tt_value += _delta[0]; // (printed) output value

  trace2(("ADP_NODE::tt_accept" + _c->short_label()).c_str(), tt_value, tr_value);
  trace1(("ADP_NODE::tt_accept done " + _c->short_label()).c_str(), get_tt());
}
/*----------------------------------------------------------------------------*/
// fixme: merge
void ADP_NODE::tt_accept_first( )
{
  trace2(("ADP_NODE::tt_accept_first" + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number());
  assert( tr_value == tr_value );

  assert( _c->_sim->get_tt_order() == 0 );

  assert(0  == _c->_sim->_dT0);
  _val_bef[2] = _val_bef[1];
  _val_bef[1] = _val_bef[0];


  tr_value1 = tr_value0; // obsolete
  _delta[1] = _delta[0];

  tr_value0 = tr_value; // obsolete.
  _delta[0] = tr_value;

//  tr_value = 0;

  assert(_val_bef[0] >= 0 );
  assert(_val_bef[0] >= _val_bef[1]);

 // FIXME tt_value??
  tt_value += _delta[0];
  assert(tt_value == tt_value);
}
/*----------------------------------------------------------------------------*/
// accept, print, advance.
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_advance(){
  trace1(("ADP_NODE::tt_advance() " + _c->short_label()).c_str(), get_tt());

  // this is no logner valid, since tr_value has been corrected.
  //  assert(tr_value==tr_value0);

  tr_value3=tr_value2; //obsolete
  tr_value2=tr_value1; //obsolete
  tr_value1=tr_value0; //obsolete
  _delta[2] = _delta[1];
  _delta[1] = _delta[0];

  _delta[0] = 888; // invalidate
  tr_value = 999; 

  _val_bef[1] = _val_bef[0]; //obsolete
  _val_bef[2] = _val_bef[1];
  _val_bef[1] = _val_bef[0];

  //shift the derivatives.
  _der_aft[2] = _der_aft[1];
  _der_aft[1] = _der_aft[0];

  assert(is_number(_der_aft[2]));
  assert(is_number(_der_aft[1]));

  _val_bef[0] = tt_value;

  trace2(("ADP_NODE::tt_advance done " + _c->short_label()).c_str(), get_tt(), get_tr());
  tr_value0=_delta[0]; // obsolete
  _val_bef[0] = tt_value; // obsolete
}
/*----------------------------------------------------------------------------*/
//  _val_bef[1] += diff;
void ADP_NODE::tt_commit_first( )
{
  trace2(("ADP_NODE::tt_commit_first() " + _c->short_label()).c_str(), get_tt(), get_tr());

//  eigentlich commit mit order 0 ??
  // assert(_c->_sim->_Time0 == 0 ); cont!
  assert(_c->_sim->get_tt_order() == 0 );

  tt_first_time = _c->_sim->_Time0;
  tt_first_value = _val_bef[0];
  assert(_delta_expect != HUGE_VAL );
}
/*---------------------------------*/
inline double square(double x){return x*x;}
/*---------------------------------*/
void ADP_NODE::tr_stress_last( double val ) {
  trace1("ADP_NODE::tr_stress_last", _c->_sim->tt_iteration_number());
//  happens after tr sweep.

  tr_value = val - get();
  tr_value0 = tr_value; // good idea? no.

  trace4(("ADP_NODE::tr_stress_last(double) " + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), val, get());
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_linear(){
  assert(_order==2);
  trace4(( "ADP_NODE::tt_expect2_linear " + _c->short_label()).c_str(), 
      tr_value, tr_value0, tr_value1, tr_value2 );
  trace3(( "ADP_NODE::tt_expect2_linear " + _c->short_label()).c_str(), Time_delta(), get1(), dT1() );
  // _delta_expect = fabs( (tr_value1) + ( (tr_value1) - (tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));
  //
  // expected tr for time0
  _delta_expect = ( (tr_value1) + ( (tr_value1) - (tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));

  if(_delta_expect != _delta_expect) assert(false);
  if(tr_value1 != tr_value1) assert(false);

  /// integrate

  hp_float_t delta = (tr_value1 + _delta_expect)/2 * Time_delta() / tr_duration();
  tt_expect = get1() + delta;
  assert (tt_expect == tt_expect);

  trace6(( "ADP_NODE::tt_expect2_linear "+_c->short_label()).c_str(), get_tr(),
      get1(), tt_expect, delta, _delta_expect, tr_duration() ); 

  if (tt_expect < 0 && _positive ){
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), Time_delta(), dT1(),
        _c->short_label().c_str(), tt_value, 
        tr_value2, tr_value1, _delta_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
  _integrator=&ADP_NODE::tt_integrate_2_linear;
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_2_linear(double x){
  incomplete();
  hp_float_t delta = (tr_value1 + x)/2 * Time_delta() / tr_duration();
  tt_expect = get1() + delta;
  assert (tt_expect == tt_expect);

  trace6(( "ADP_NODE::tt_expect2_linear "+_c->short_label()).c_str(), get_tr(),
      get1(), tt_expect, delta, x, tr_duration() ); 

  if (tt_expect < 0 && _positive ){
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), Time_delta(), dT1(),
        _c->short_label().c_str(), tt_value, 
        tr_value2, tr_value1, x,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
  _integrator = &ADP_NODE::tt_integrate_2_linear;
  return tt_expect;
}
/*---------------------------------*/
// 3 points denoising expect
void ADP_NODE::tr_expect_3_linear(){
  assert(_order==3);
  trace4(( "ADP_NODE::tt_expect3_linear " + _c->short_label()).c_str(), tr_value, tr_value0, tr_value1, tr_value2 );
  trace3(( "ADP_NODE::tt_expect3_linear " + _c->short_label()).c_str(), Time_delta(), get1(), dT1() );
  // tr_expect = fabs( (tr_value1) + ( (tr_value1) - (tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));
  //

  hp_float_t t1 = dT2();
  hp_float_t t2 = dT1()+dT2();
  hp_float_t t3 = dT0()+dT1()+dT2();

  // expected tr for time0

  const double x[3]={0, t1, t2};
  const double y[3]={tr_value3, tr_value2, tr_value1};

  double c0, c1, cov00, cov01, cov11, sumsq;
  gsl_fit_linear( x, 1, y, 1, 3, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

  _delta_expect = c0 + c1 * t3;

  if(_delta_expect != _delta_expect) _delta_expect=0;
  if(tr_value1 != tr_value1) assert(false);

  hp_float_t diff = (tr_value1 + _delta_expect)/2 * Time_delta() / tr_duration();
  tt_expect = get1() + diff;
  assert (tt_expect == tt_expect);

  trace6(( "ADP_NODE::tt_expect3_linear "+_c->short_label()).c_str(), get_tr(),
      get1(), tt_expect, diff, _delta_expect, tr_duration() );


  if (tt_expect < 0 ){
    //positive?
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect3_linear neg error step %i, Time0=%f  \
       * (%f,%f,%f), %s, tt_value = %g, ( %g, %g, %g)-> %g --  %i tt: %f, \
        expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), dT1(), dT2(),
        _c->short_label().c_str(),
        tt_value, tr_value3, tr_value2, tr_value1, _delta_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_square(){
  assert(_order==2);
  _delta_expect = square( sqrt(tr_value1) + ( sqrt(tr_value1) - sqrt(tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));

}
/*---------------------------------*/
double foo(double x){
  // FIxme: horner.
  if(fabs(x)<1e-4)
    return (1-x/2 + 11/24*x*x -7/16*x*x*x )*exp(1);
  return pow((1+x),(1/x));

}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_exp(){
  trace0("ADP_NODE::tr_expect_2_exp()");
  double h = tr_duration();
  assert(_order==2);
  assert(tr_value1 = _delta[1]);
  assert(tr_value2 = _delta[2]);
  _sign=1;
  if (tr_value1 < 0 && tr_value2 < 0 ){
    _sign=-1;
  }

  if ( !( (tr_value1 < 0 && tr_value2 < 0) 
        || ( tr_value1 > 0 && tr_value2 >0))){
    _order--;
    trace0("linear...");
    return tr_expect_2_linear();
  }

  //hp_float_t t2 = tr_duration()/2;
  //hp_float_t t1 = t2+dT1();
  //dashier geht nicht. kleine schritte zu schlecht.
  double d1=_delta[1];
  double d2=_delta[2];

  d1/=h;
  d2/=h;
  double alpha = - log ( d2/d1 ) / dT1();
  double _delta_expect_approx = d1 * exp (alpha*dT0()) * h;

  //d1=_der_aft[1]; // dashier muesste man noch ausrechnen.
  //
  //
  // expect using deltas as derivatives.
  d1=_delta[1] / h;
  d2=_delta[2] / h;
//  d2=_der_aft[2];

  alpha = - log ( d2/d1 ) / dT1();
  double q =  ( d2/d1 ) ;
  double p = pow(q,-h/dT1());
  double r = 1-p;
  // derv(x) = d1 * exp (alpha*x);  ##
  // _delta_expect = int_{dT0-h}^dT0 derv ##
  //               = d1 / alpha * (exp(alpha*dT0()) - exp(alpha*(dT0()-h))); ##
  //               = d1 / (beta /dT1()) * (pow(q,-dT0()/dT1)) - pow(q,-(dT0()-h)/dT1)); 
  //               = d1 / beta * dT1()  * (pow(q,-dT0()/dT1)) * ( 1  - pow(q,(-h)/dT1)));
  //               = d1 *dT1()           pow(q,-dT0()/dT1)* ( 1  - pow(q,(-h)))/beta;
  //               = d1 *dT1()           pow(q,-dT0()/dT1)*
  //                     ( 1 - pow(q,-h/dT1)))/ log( q )
  //               = d1 *dT1()           pow(q,-dT0()/dT1)*
  //                ( 1 - pow(q,-h/dt1)))/ log( pow(q,-h/dT1 ()) ) * ( h/dT1)) 
  //               = d1 *dT1()*  pow(q,-dT0()/dT1)*
  //                1/ log( (r+1)^(1/r) ) * ( h/dT1)) 
  _delta_expect_approx  = d1 * pow(q,-dT0()/dT1()) / log( foo(r) ) * ( h) ;

 //
 //expect using deltas.
  d1=_delta[1] / h;
  d2=_delta[2] / h;
  _delta_expect = pow(d1,((dT0() + dT1())/dT1())) / pow(d2,(dT0()/dT1()))*h;

  //std::cout << "***" <<   _delta_expect << " vs " << _delta_expect_approx << "\n";

  // _der_aft_exp = _sign * exp(ln0);
  trace6("ADP_NODE::tr_expect_2_exp", \
      _delta_expect, _der_aft[2], _der_aft[1],
      d1,d2, dT0());
  trace5("ADP_NODE::tr_expect_2_exp", _delta[1]/h, _delta[2]/h, q, p, r);

  assert(_delta_expect == _delta_expect);
  assert(_delta_expect < 1e6);

  _corrector = &ADP_NODE::tr_correct_generic;
  tt_expect = tt_integrate_2_exp(_delta_expect);
}
/*----------------------------------------------------------------------
double tt_integrate_2_exp_alt(double tr_) {
  trace2("ADP_NODE::tt_integrate_2_exp", tr_value1, tr_);
  trace2("ADP_NODE::tt_integrate_2_exp", tr_value1,  _sign);
  assert(tr_value1 == _delta[1]);
  hp_float_t h = tr_duration();
  
  double ln1 = log(_sign*tr_value1/h);
  double ln0 = log(_sign*tr_/h);

  //int_{h/2}^{dT0} exp( ln1 + v/dT0 * (ln0 - ln1) ) dv

  double delta= dT0()/(ln0-ln1) * ( _sign*tr_/h - exp( ln1 + (h/dT0()) *(ln0-ln1) ) );

  if (delta != delta || delta==-inf || delta==inf){ 
    delta= (tr_ + tr_value1)/2 * (dT0()-h) / h ;
    incomplete(); // call another thiongh.
  }

  assert(delta != -inf);
  assert(delta != inf);

  double ret = get1() + tr_value1 + _sign * delta;

  trace5(( "ADP_NODE::tt_integrate_2_exp "+_c->short_label()).c_str(),
      tr_value2, tr_value1, tr_, tt_expect, delta );

  assert(ret == ret);
  _integrator = &ADP_NODE::tt_integrate_2_exp;
  return ret;
}
----------------------------------------------------------------------*/
double my_log(double x){
  if(x>0) return log(x);
  return -log(x);
}
/*---*/
double ADP_NODE::tt_integrate_2_exp(double tr_) {
  trace2("ADP_NODE::tt_integrate_2_exp", tr_value1, tr_);
  trace2("ADP_NODE::tt_integrate_2_exp", tr_value1,  _sign);
  assert(tr_value1 == _delta[1]);
  hp_float_t h = tr_duration();
  
  double del1 = _delta[1];
  double del2 = _delta[2];
  double dTo = dT0();
  double dTi = dT1();
  double delta = (pow(del1,(dTo/dTi))*pow(del2,((dTi + h)/dTi))
                 - pow(del1,((dTi + h)/dTi)) * pow(del2,(dTo/dTi))) *
        dTi/
        (((log(del1) - log(del2)) * 
        pow(del1,(h/dTi)) -
        (log(del1) - log(del2))*
        pow(del2,(h/dTi)))*
        pow(del2,(dTo/dTi)));

  if (delta != delta || delta==-inf || delta==inf){ 
    delta= (tr_ + tr_value1)/2 * (dT0()-h) / h ;
    incomplete(); // call another thing.
  }

  assert(delta != -inf);
  assert(delta != inf);

  double ret = get1() + tr_value1 + _sign * delta;

  trace5(( "ADP_NODE::tt_integrate_2_exp "+_c->short_label()).c_str(),
      tr_value2, tr_value1, tr_, tt_expect, delta );

  assert(ret == ret);
  _integrator = &ADP_NODE::tt_integrate_2_exp;
  return ret;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp(){
  assert(_order==3);
  _sign=1;

  if ( ( tr_value2 > 0 && tr_value1 > 0 && tr_value1 < tr_value2 && tr_value3 < tr_value2 )
      || ( tr_value2 < 0 && tr_value1 < 0 && tr_value1 > tr_value2 && tr_value3 > tr_value2 ) ){
    _order--;
    return tr_expect_2_exp();
  }
        
  if (tr_value1 < 0 && tr_value2 < 0 && tr_value3 <0){
        _sign=-1;
  } else if ( ! (tr_value1 > 0 && tr_value2 > 0 && tr_value3 >0) ){
     return tr_expect_3_linear();       
  }

  if ( tr_value3 > tr_value2 && tr_value2 > tr_value1 ){
  }


  double ln3= log(_sign * tr_value3/tr_duration());
  double ln2= log(_sign * tr_value2/tr_duration());
  double ln1= log(_sign * tr_value1/tr_duration());

  hp_float_t tdh=tr_duration()/2;

  hp_float_t t3 = tdh;
  hp_float_t t2 = t3+dT2();
  hp_float_t t1 = t2+dT1();

  hp_float_t x =  dT2()+dT1()+dT0()+tdh;


 //   exponent= ax^2 + bx + c
 //
  // neville @ x.
  double ln0 = ln3;
  ln0 += (ln2-ln3) * ( x - t3) / (t2-t3);
  ln0 += (ln1 - ln3 - ( ln2 - ln3 ) * (t1-t3)/ (t2-t3) ) * (x-t3) * ( x-t2) / (t1-t3) / (t1-t3);


  // int_0^x exp( ln1 + t/dT0*(ln0-ln1) ) @ x=dT0
  _delta_expect =  _sign*exp(ln0) * tr_duration();
  _corrector = &ADP_NODE::tr_correct_3_exp;
  tt_expect = tt_integrate_2_exp(_delta_expect); // 3 doesnt make sense yet?
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tr_correct_3_exp(){
        return (tr_value+_delta_expect)/2;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tr_correct_generic(){
  // return(18);
  return (tr_value+_delta_expect)/2;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tt_integrate_3_exp(double tr_)
{
  assert(_order==3);
  hp_float_t sign=1;
  double h=tr_duration();

  if ( ( tr_value2 > 0 && tr_value1 > 0 && tr_value1 < tr_value2 && tr_value3 < tr_value2 )
      || ( tr_value2 < 0 && tr_value1 < 0 && tr_value1 > tr_value2 && tr_value3 > tr_value2 ) ){
    _order--;
    assert(false);
    //return tr_expect_2_exp();
  }
        
  if (tr_value1 < 0 && tr_value2 < 0 && tr_value3 <0){
        sign=-1;
  } else if ( ! (tr_value1 > 0 && tr_value2 > 0 && tr_value3 >0) ){
//    fixme.
    assert(false);
//     return tr_expect_3_linear();       
  }

  if ( tr_value3 > tr_value2 && tr_value2 > tr_value1 ){
  }

  double ln1= log(_sign*tr_value1/tr_duration());
  double ln0 = log(_sign*tr_/tr_duration());

  hp_float_t delta = _sign* (exp( ln1 + (ln0-ln1) ) - (_sign*tr_value1/tr_duration()) )/(ln0-ln1)* dT0();

  if (delta != delta || delta==inf || delta==-inf){

    // fixme: cll anothert integrator.
    hp_float_t delta_trapez = (tr_value1 + tr_)/2 * ( dT0()-h) / tr_duration() ;
    trace2("ADP_NODE::tt_expect3_exp preferring delta_trapez", delta_trapez, delta);

    _integrator = &ADP_NODE::tt_integrate_1_const;
    return( get1() + delta_trapez);
  }

  tt_value = get1() + delta * _sign;
  assert(tt_value == tt_value);

  _integrator = &ADP_NODE::tt_integrate_3_exp;
  return tt_value;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp_fit(){
  assert(_order==3);
  hp_float_t sign=1;

  if ( ( tr_value2 > 0 && tr_value1 > 0 && tr_value1 < tr_value2 && tr_value3 < tr_value2 )
      || ( tr_value2 < 0 && tr_value1 < 0 && tr_value1 > tr_value2 && tr_value3 > tr_value2 ) ){
    _order--;
    return tr_expect_2_exp();
  }
        
  if (tr_value1 < 0 && tr_value2 < 0 && tr_value3 <0){
        sign=-1;
  } else if ( ! (tr_value1 > 0 && tr_value2 > 0 && tr_value3 >0) ){
     return tr_expect_3_linear();       
  }

  if ( tr_value3 > tr_value2 && tr_value2 > tr_value1 ){
  }

  hp_float_t f0 = sign*tr_value3;
  hp_float_t f1 = sign*tr_value2;
  hp_float_t f2 = sign*tr_value1;

  hp_float_t t1 = dT2();
  hp_float_t t2 = dT1()+dT2();
  hp_float_t t3 = dT0()+dT1()+dT2();

  hp_float_t a = ( (log(f0) + log(f1) + log(f2)) * ( t1*t1 + t2*t2 ) - (t1 + t2) * ( t1*log(f1) + t2*log(f2) ) ) /
                         (3 * ( t1*t1 + t2*t2 ) - (t1+t2)*(t1+t2) );
  hp_float_t alpha = ( 3*( t1*log(f1) + t2*log(f2)) - (t1 + t2) * ( log(f0)+log(f1) +log(f2) ) ) /
                         (3 * ( t1*t1 + t2*t2 ) - (t1+t2)*(t1+t2) );

//  hack...
  if (alpha != alpha) {
    _order--;
    return tr_expect_2_linear();
  }

  hp_float_t c=exp(a);

  _delta_expect = _delta_expect = sign*c * exp(alpha*t3);

  trace6(( "ADP_NODE::tt_expect3_exp() "+_c->short_label()).c_str(),  Time0(), tr_value3, tr_value2, tr_value1, get1(), _delta_expect );
  assert( _delta_expect == _delta_expect );

  hp_float_t E = - c/alpha;
  hp_float_t delta = E * ( - exp( t3*alpha ) + exp(t2*alpha) )/ _c->_sim->_last_time ;
  hp_float_t delta_trapez = (tr_value1 + _delta_expect)/2 * Time_delta() / _c->_sim->_last_time;

  trace4("ADP_NODE::tt_expect3_exp", delta, delta_trapez, _val_bef[1], get() );
  delta_trapez = delta_trapez; // unused_variable...
  tt_expect = get1() +  sign * delta;

  if ( tt_expect != tt_expect){
     std::cerr << "tt_expect is broken:\n" 
        << "c=" << c << "\n" 
        << "E=" << E << "\n" 
        << "alpha=" << alpha<< "\n" 
        << "( 0.0, " <<                 f0 <<" )\n"
        << "( "      << t1  << ", " <<  f1 <<" )\n"
        << "( "      << t2  << ", " <<  f2 <<" )\n"
        << "( "      << t3  << ", ?    )\n";
     _order--;
     return tr_expect_2_linear();
  }

  if (tt_expect < -1e-8 && _positive){
    error(bDANGER, "* ADP_NODE::tt_expect3_exp neg error step %i, Time0=%f, %s, tt_value = %g, ( %g, %g, %g; %g) tte: %g \n", \
        _c->_sim->tt_iteration_number(),
         dT0(), _c->short_label().c_str(), tt_value,
         tr_value3, tr_value2, tr_value1, _delta_expect,
        tt_expect
        );
//    assert(false);
  }

  //sanitycheck
  if ( f0<0 && f1<0 && f2 <0)
    assert (_delta_expect <= 0);

}
/*---------------------------------*/
void ADP_NODE::tr_expect_3_quadratic(){
  assert(_order==3);
  trace3(( "ADP_NODE::tt_expect3_quadratic() "+_c->short_label()).c_str(), tr_value3, tr_value2,  tr_value1 );
  // uses 1 2 3 to set expect.
  assert( tr_dd12 == ( tr_value1 - tr_value2 ) / dT1());
  hp_float_t sum = dT0() + dT1() + dT2();
  hp_float_t tr_expect_kaputt;

//        doch nicht kaputt.
  tr_expect_kaputt = ( \
      sum * ( tr_value1  + tr_dd12 * dT0() ) \
      - dT0() * (( - dT1()* tr_dd12 + tr_value1 ) + (dT0()+dT1()) * ( tr_dd12 + tr_dd123* ( -dT2()-dT1() )  ) ) \
      ) / (dT2()+dT1());


  _delta_expect = -(((tr_value2 - tr_value3)/dT2() - (tr_value1 - tr_value2)/dT1())*(dT0() + dT1())/(dT1() + dT2()) -
    (tr_value2 - tr_value3)/dT2())*(dT0() + dT1() + dT2()) + tr_value3;

  hp_float_t diff = (tr_value1 + _delta_expect)/2 * Time_delta() / _c->_sim->_last_time;
  tt_expect = get() + diff;
  assert (tt_expect == tt_expect);
}
/*---------------------------------*/
#ifdef DO_TRACE
double ADP_NODE::debug(){
  return _debug;
}
#endif
/*---------------------------------*/
void ADP_NODE::tr_expect_2_something(){
  assert(_order==2);
  _delta_expect = tr_value1 + (tr_value1 - tr_value2) * dT0()/dT1();
  tt_expect = get1() + ( Time_delta() ) / _c->_sim->_last_time;
  trace2(( "ADP_NODE::tt_expect2_something "+_c->short_label()).c_str(), get1(), tt_expect );
}
/*---------------------------------*/
void ADP_NODE::tr_expect_1_const(){
  assert(_order==1);
  trace3(( "ADP_NODE::tt_expect1_const() "+_c->short_label()).c_str(), tr_value, tr_value0, tr_value1 );
  trace3(( "ADP_NODE::tt_expect1_const() "+_c->short_label()).c_str(), Time_delta(), _c->_sim->_time0, _c->_sim->_last_time );

  _delta_expect = tr_value1;

  _corrector = &ADP_NODE::tr_correct_1_exp;
  tt_expect = tt_integrate_1_const(_delta_expect);
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_1_const( double a){
  _sign=1;
  double    delta = (tr_value1+a )/2 *  dT0() / tr_duration();
  double ret = get1()+delta; 

  if (ret < 0 && _positive){
    error(bDANGER, "* ADP_NODE::tt_expect1_const error step %i, Time0=%f (%f), %s, tt_value = %g, ( %g, %g, %g, %g) \n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0, dT0(),
        _c->short_label().c_str(), tt_expect, tr_value, tr_value0, tr_value1, tr_value2);
//    assert(false);
  }

  if (tr_value1 < 0 && a < 0 ){
    _sign=-1;
  }


  _integrator = &ADP_NODE::tt_integrate_2_exp;
  return(ret);
}
/*---------------------------------*/
double ADP_NODE::tr_correct_1_const(){
  _debug=555;
  return  (_delta_expect + tr_value)/2;
}
/*---------------------------------*/
double ADP_NODE::tr_correct_1_exp(){

  double new_delta; // return value. new value for tr_value.
  double h = tr_duration();

  //assert(_val_bef[1] == _val_bef[1]);
  assert(tr_value1 == _delta[1]);

  double a = tr_value1;
  double b = tt_expect;
  double c = tr_value;
  double d = _val_bef[1];
  double dT = dT0();

  trace4(("ADP_NODE::tr_correct_1_exp, _der_aft? " + label()).c_str(),
      tr_value1, tt_expect, tr_value, _val_bef[1] );

  //model value as
  /// t \mapsto K* (exp (tau*t) -1 + d )

//  long double K = a*(b-d)/(c-a);
  long double K = (b-d)/(c/a-1);
  long double tau = log((c-a)/(b-d) + 1) / h;

  //_delta_expect = K *( exp(tau* (dT0()+h) )  - exp ((tau*dT0())) );

  new_delta = a*pow((a - b - c + d)/(-b + d),(dT/h));

  /*
  // _der_aft[0] = tau * K *( exp(tau* (dT+h) )) ;
  _der_aft[0] = (b - d)*pow((a - b - c + d),((dT + h)/h))*log(-(a - c)/(b - d) + 1)/((c/a -
        1)*h*pow((-b + d),((dT + h)/h)));
  //FIXME.
  if (_der_aft[0] != _der_aft[0]) {
    trace1("ADP_NODE::tr_correct_1_exp error", _der_aft[0]);
    _der_aft[0] = new_delta /h;
  }
  */



  // _der_aft[1] = tau * K *( exp(tau* (h) )) ;
  _der_aft[1] =
   -((a*a - a*b - a*c + a*d)*log(-b + d) - (a*a - a*b - a*c + a*d)*log(a -
         b - c + d))/((a - c)*h);


  if (_der_aft[1] != _der_aft[1]) {
    trace5("ADP_NODE::tr_correct_1_exp error", _der_aft[1], a-c, -b+d, a - b - c + d, (a*a - a*b - a*c + a*d) );
    _der_aft[1] = _delta[1]/h;
  }

  // _der_aft[1] = tau * K *( exp(tau* (h) )) ;
  //

  trace5(("ADP_NODE::tr_correct_1_exp, _der_aft? " + label()).c_str(),
     K, tau, _delta_expect,_der_aft[1], _der_aft[0]  );
  assert( is_number(_der_aft[0]));
  assert( is_number(_der_aft[1]));

  trace5(("ADP_NODE::tr_correct_1_exp, " + label()).c_str(), \
     _delta_expect,  _der_aft[1], _der_aft[0], _delta[1], _delta[0]);

  return new_delta;
}
/*---------------------------------*/

double ADP_NODE::tt_integrate_( double ){
// take order and tr_value
// integrate things. write to tt_value.
  return 0;
}
/*---------------------------------*/
/*---------------------------------*/
void ADP_NODE::tt_commit( )
{
 //  called before apply...
 //  sets expected values for already guessed dT0


  trace1(( "ADP_NODE::tt_commit "+_c->short_label()).c_str(), tt_value );
  trace6(("ADP_NODE::tt_commit " + _c->short_label()).c_str(),
      dT0(), dT1(),
      _c->_sim->get_tt_order(),
      _c->_sim->_Time0,
      _val_bef[0], _val_bef[1]
      );

  tr_expect_();
  assert(_delta_expect != HUGE_VAL );
  assert(_delta_expect == _delta_expect );

  // this is before sweep....
  assert( tt_expect == tt_expect );
  assert( _c->_sim->_last_time >0 );

  tr_value = _delta_expect; // FIXME only accd values into tr_value0
  tt_value = tt_expect; // FIXME
  assert( tt_value == tt_value );
  // tr_value will be printed as "tr"

  if (tt_expect < -1e-8 ){
    //positive?
    error(bDANGER, "* ADP_NODE::tt_commit neg error step %i, order %i, tt_expect=%e\n", \
        _c->_sim->tt_iteration_number(), _order,tt_expect);
  }
  trace6(("tt_values " + _c->short_label() + " ").c_str(),
      tr_value3, tr_value2, tr_value1, _delta_expect, _c->_sim->tt_iteration_number(), tt_expect - _val_bef[1]);
  trace6(("tt_values " + _c->short_label() + " ").c_str(),
      tt_value3, _val_bef[2], _val_bef[1], tt_expect, _c->_sim->tt_iteration_number(), _c->_sim->get_tt_order() );



}
/*---------------------------------*/
void ADP_NODE::tr_expect_( ){
  _integrator = 0;
  _corrector = 0;
  _order = _c->_sim->get_tt_order();

  if(tr_value0 != _delta[0]) _delta[0] = tr_value0 ;

  switch(_order){
    case 0:
      assert(false);
      break;
    case 1:
      tr_expect_1();
      break;
    case 2:
      tr_expect_2();
      if ( tr_value1<0 && tr_value2<0 ){
        trace3( "ADP_NODE::tr_expect_ order 2", tr_value2, tr_value1, _delta_expect );
      }

      break;
    case 3:
      // FIXME assert( tr_dd12 == ( tr_value1 - tr_value2 ) / dT1());
      tr_dd12 = ( tr_value1 - tr_value2 ) / dT1();
      tr_expect_3();
      if ( tr_value1<0 && tr_value2<0 && tr_value3<0 && _delta_expect>0) {
        error(bDANGER, "* ADP_NODE::tt_expect_ error step %i, Time0=%f, %s, tt_value = %g, ( %g, %g, %g, %g) tte: %g\n", \
            _c->_sim->tt_iteration_number(),
            dT0(),
            _c->short_label().c_str(), tt_value, tr_value, tr_value0, tr_value1, tr_value2, tt_expect);
        assert (_delta_expect <= 0);

      }
      break;
    default:
      assert(false);
  }
  assert(_delta_expect == _delta_expect);
}

