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

#include "e_adp.h"
#include <gsl/gsl_fit.h>
#define DEBUG_ADP

/*
''=working; accessed by get.
0=current time step.
1=old time step. accepted value only
2=history
*/

/*----------------------------------------------------------------------------*/
uint_t ADP_NODE::order() const{
  return( min( (int) _order,(int) CKT_BASE::_sim->get_tt_order() ) );
} // order used for extrapolation.
/*----------------------------------------------------------------------------*/
//ADP_NODE::ADP_NODE( const ADP_NODE& p ) :
//  // NODE(p),
//  _number(p._number)
//{
//  unreachable();
//  std::cout << "copy?? (should not happen)\n";
//  assert(false);
//  tr_lo = inf;
//  tr_hi = -inf;
//}
/*----------------------------------------------------------------------------*/
ADP_NODE::ADP_NODE( const std::string n, const COMPONENT* c):
  NODE_BASE(),
  _number(0),
  dbg(0),
  tr_noise(0)
{
  trace2("ADP_NODE::ADP_NODE ", n, c->long_label());
  set_label( c->long_label() + "." + n );
  tr_value = (0.);
  tr_noise = 0;
  dbg=0;

  tt_value = 0.;
  _delta_expect = 0.;
  _positive = true; //hack
  //_debug = 0;
  _order = 0;
  _wdT = 0;
  _rel_tr_err = 0;
  _rel_tt_err = 0;
  _abs_tr_err = 0;
  _abs_tt_err = 0;
  dbg++;

  _number=_sim->newnode_adp();
  ADP_NODE_LIST::adp_node_list.push_back( this );

  trace1(("ADP_NODE::init " + long_label()).c_str(), _number );

  // tt_value3=NAN; //FIXME
  tt_expect=NAN; //FIXME
  tr_value3=NAN; //FIXME

  // _n[n_ic].new_model_node("." + long_label() + ".ic", this);
  assert(c);
  tr_lo = inf;
  tr_hi = -inf;
}
/*----------------------------------------------------------------------------*/
//ADP_NODE::ADP_NODE( const COMPONENT* c, const std::string n ) :
//  NODE(),
//  _number(0),
//  dbg(0),
//  tr_noise(0)
//{
//  assert(false);
//  trace0("ADP_NODE::ADP_NODE ");
//  init(c,n);
//  assert(c);
//  tr_lo = inf;
//  tr_hi = -inf;
//}
/*----------------------------------------------------------------------------*/
// ADP_NODE_UDC::ADP_NODE_UDC( const COMPONENT* c ) : ADP_NODE(c, "udc") { }
/*----------------------------------------------------------------------------*/
// ADP_NODE_RCD::ADP_NODE_RCD( const COMPONENT* c ) : ADP_NODE(c, "rcd"), udc(.2) {}
/*----------------------------------------------------------------------------*/
// obsolete
void ADP_NODE::init(const COMPONENT* c, const std::string name_in){
  set_label( c->long_label() + "." +  name_in );
  tr_value = (0.);
  tr_noise = 0;
  dbg=0;

  tt_value = 0.;
  _delta_expect = 0.;
  _positive = true; //hack
  //_debug = 0;
  _order = 0;
  _wdT = 0;
  _rel_tr_err = 0;
  _rel_tt_err = 0;
  _abs_tr_err = 0;
  _abs_tt_err = 0;
  dbg++;

  _number=_sim->newnode_adp();
  ADP_NODE_LIST::adp_node_list.push_back( this );

  trace1(("ADP_NODE::init " + long_label()).c_str(), _number );

  // tt_value3=NAN; //FIXME
  tt_expect=NAN; //FIXME
  tr_value3=NAN; //FIXME

  // _n[n_ic].new_model_node("." + long_label() + ".ic", this);
}
/*----------------------------------------------------------------------------*/
ADP_NODE::~ADP_NODE(){
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::set_tr( hp_float_t x ){
  if(x>tr())  _region= 2;
  if(x<tr())  _region=-2;
  if(x==tr()) _region= 0;
  tr() = x;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::set_tt( hp_float_t x ){
  assert(is_number(x));
  assert(x >=0 || !_positive);
  tt() = x;
}
/*----------------------------------------------------------------------------*/
hp_float_t ADP_NODE::get_aft_1() const { return tt1() + _delta[1] ; }
/*----------------------------------------------------------------------------*/
double ADP_NODE::tr_duration()const{ return CKT_BASE::_sim->_last_time; }
/*----------------------------------------------------------------------------*/
// called right after tr
TIME_PAIR ADP_NODE_RCD::tt_review( ) {
  double h = tr_duration();
  double delta_model;
  if (_corrector){
    assert(order()>0);
    trace1(("ADP_NODE::tt_review: correction " + label()).c_str(), _delta_expect);
    delta_model = (this->*_corrector)(); // value predicted by model.
  } else {
    delta_model = _delta_expect;
    trace1(("ADP_NODE::tt_review: no corrector " + label()).c_str(), delta_model );
  }
  assert( is_number (delta_model ) );

  trace1("ADP_NODE::tt_review", CKT_BASE::_sim->tt_iteration_number());
  assert(delta_model == delta_model);
  hp_float_t myreltol = OPT::adpreltol;
  hp_float_t myabstol = OPT::adpabstol;

  if ( ( tr_value * delta_model ) < 0 ) {
    trace2(("ADP_NODE::tt_review: oops, sign has changed "+ label()).c_str(), tr_value,delta_model);
  }
  // FIXME: implement abs err!
  _abs_tr_err = fabs (tr_value - delta_model);

  if (_abs_tr_err < 1e-12 ) { 
    _rel_tr_err = 0;
    _abs_tr_err = 0;
  } else {
    _rel_tr_err = _abs_tr_err / max( fabs(tr_value) , fabs(delta_model));
  }
  _rel_tt_err = fabs (tt_value - tt_expect) / (fabs(tt_value) + fabs(tt_expect));

  tr_value = delta_model;

  trace3(("ADP_NODE::tt_review" + label() + "got tol").c_str(), myreltol, _abs_tr_err, _rel_tr_err);
  if( _abs_tr_err == 0 ) {
    trace1(("ADP_NODE::tt_review" + label() + "noabs").c_str(), TIME_PAIR(0, NEVER)._event );
    _wdT = inf; return TIME_PAIR(0, NEVER);
  }
  if( myreltol == inf && myabstol == inf ) { _wdT = inf; return TIME_PAIR();}
  if( myreltol == 0 && myabstol == 0 ) {_wdT=0; return TIME_PAIR(0,0);}
  if( myreltol == 0 ) {_wdT=0; return TIME_PAIR(0,0); }

// FIXME: _order.
//
//???
  _wdT = (dT0()-h) * sqrt( myreltol / _rel_tr_err )  + h;
  _wdT =  sqrt( 1+  myreltol / _rel_tr_err )  + h;

  if (_rel_tr_err >= 1 ){
    error( bDANGER, "stepdebug %i dev: %s reltr %f>1 model %E, tr_ %E fill %E\n",
        CKT_BASE::_sim->tt_iteration_number(),
        label().c_str(),   _rel_tr_err, delta_model, tr_value, tt_value );
  }
  if (_wdT < 1.1*h){
    error( bDANGER, "stepdebug  dev: _wdT %s wdT: %g %g %f rel_tr %f abserr: %g, corr %i ord %i\n",
        label().c_str(), _wdT, dT0(), myreltol,  _rel_tr_err, _abs_tr_err, (bool)_corrector, order() );
  }

  trace4("ADP_NODE::tt_review adaptive _wdT", myreltol, inf, h, _rel_tr_err);
  if( _wdT < 0  || _wdT != _wdT) {
    error(bDANGER, " dev: _wdT %s %f %f %f\n", label().c_str(), _wdT, dT0(), _rel_tr_err );
  }

  if( _rel_tr_err != _rel_tr_err ) _wdT = NEVER;
  if( !(_rel_tr_err == _rel_tr_err) ) _wdT = NEVER;

  trace3( ("ADP_NODE::tt_review " + label()).c_str(), CKT_BASE::_sim->_Time0 , _wdT , myreltol );

  assert(_wdT >= 0 );

  assert(tr_value == tr_value);
  assert(_delta_expect == _delta_expect);
  tt_value += tr_value; // only printed if OPT::printrejected

  return( TIME_PAIR(0, _wdT));
}
/*----------------------------------------------------------------------------*/
TIME_PAIR ADP_NODE::tt_preview( ) {
  TIME_PAIR _time_pre;
    _time_pre.reset();
    return _time_pre;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tr_add( double x ) const { 
  assert(is_number(tr())); 
  tr() += x;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::reset() {
  trace2( "ADP_NODE::reset()", CKT_BASE::_sim->_Time0, tr_value );
  std::cout << "rst " << label() << "\n";
  assert(false); //BUG
  tr_value = 0; // tt1() = 0; tt() =0;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_accept()
{

  return;
  // sets t{t,r}_value0 to the accepted values.
  // should compute new wanted_dT.
  
  trace4(("ADP_NODE::tt_accept() " + long_label()).c_str(),
      tr_value, tt_value, CKT_BASE::_sim->tt_iteration_number(), CKT_BASE::_sim->get_tt_order());
  assert( is_number(tr_value) );
  assert( CKT_BASE::_sim->_last_time >0 );

  uint_t order=CKT_BASE::_sim->get_tt_order();

  if (order != _order){
    set_order (min(_order, order) );
  }

  switch( order ){
    case 0:
      // fixme. merge
      return tt_accept_first();
      break;
    case 1:
      tr_dd12 = ( tr() - tr1() ) / dT0();
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


  if (_integrator && order){
    trace0("ADP_NODE::tt_accept reintegration");
//    tr_value = _delta[0];
    assert(tr()<10);
//    tt_value = (this->*_integrator)(_delta[0]);
    assert( !_positive || tt_value > E_min );

    if( tt_value > 1 ){ 
      error(bDANGER, "ADP_NODE::tt_accept overshoot? %f\n", label().c_str(), tt_value );
      tt_value=1;
    }
  } else {
    untested();
  }

  assert(tt_value <=1 );

  // std::cout << "acc after int " << label() << tr_value << "\n";

  assert( tt_value == tt_value );

  tt() = tt_value;

  //tt_value += 13; // (printed) output value
  if (tt_value <0 && _positive){
    untested();
    tt_value=0;
//    _delta[0] -= tt();
  }

  trace2(("ADP_NODE::tt_accept" + short_label()).c_str(), tt_value, tr_value);
  trace1(("ADP_NODE::tt_accept done " + short_label()).c_str(), get_tt());
  assert(tt() >=0 || !_positive);

  if (( dT0() / tr_duration() ) < 1.000000001){
    trace1(("ADP_NODE::tt_accept" + short_label()).c_str(), dT0() / tr_duration() );
    trace6(("ADP_NODE::tt_accept" + short_label()).c_str(), tt() ,
        tt1(), tr(), tt() - tt1() - tr1(), tr1(),
         ( tt() - tt1() - tr1()) / (tr1() + 1e-10)  );
  }

}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_last( ) {
  unreachable();
 // tt() = tt() + tr(); //FIXME. add tt_keep_values.
 // if( tt() < 0 && _positive ){    //FIXME. add tt_keep_values.
 //   tr()+=tt();
 //   tt() = 0;                       //FIXME. add tt_keep_values.
 // }

}
/*----------------------------------------------------------------------------*/
// fixme: merge
void ADP_NODE::tt_accept_first( )
{

  return;
  tt_value = tt() + tr_value;
  assert(tt() >=0 || !_positive);
  assert (tt()<=1);

  trace3(("ADP_NODE::tt_accept_first" + short_label()).c_str(),
      tr_value,CKT_BASE::_sim->tt_iteration_number(), tt_value );
  assert( tr_value == tr_value );
  assert(order()==0);
  assert( CKT_BASE::_sim->get_tt_order() == 0 );
  assert( CKT_BASE::_sim->tt_iteration_number() == 0 );


  // assert(0  == CKT_BASE::_sim->_dT0);

//  tr_value = 0;


 // FIXME tt_value??
 
  assert(tt_value == tt_value);
  if (_positive && tt_value < 0 ){
    untested();
    // assert(tt() >= 0);
  }
  trace4(("ADP_NODE::tt_accept_first" + short_label()).c_str(),
      tr_value,CKT_BASE::_sim->tt_iteration_number(), tt_value, tt_value);

}
/*----------------------------------------------------------------------------*/
// accept, print, advance.
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_advance(){
  unreachable();

//  trace4(("ADP_NODE::tt_advance() " + short_label()).c_str(), get_tt(), _tt[0], _tt[1], tt_iteration_number());
//
//  // this is no logner valid, since tr_value has been corrected.
//  //  assert(tr_value == _delta[0]);
//  // _debug = 0;
//  tr_value = NAN;  //invalidate
//
//
//  assert(is_number(_tt[2])|| CKT_BASE::_sim->tt_iteration_number() < 2);
//  assert(is_number(_tt[1])|| CKT_BASE::_sim->tt_iteration_number() < 1);
//
//  trace3(("ADP_NODE::tt_advance done " + short_label()).c_str(), get_tt(1), get_tr(1),_tt[0]);
//  assert (( !_positive || _tt[1] >=0 ) || CKT_BASE::_sim->tt_iteration_number() < 1 );
}
/*---------------------------------*/
hp_float_t ADP_NODE::get_total() const{
  return( NAN );
}
void ADP_NODE::tr_stress_last( ) { unreachable(); }
/*---------------------------------

void ADP_NODE::tr_stress_last( double val ) {

  tr_value = val - get();
  trace3(("ADP_NODE::tr_stress_last" + label()).c_str(), val, get(), CKT_BASE::_sim->tt_iteration_number());
  _delta[0] = tr_value; // good idea? no.

  trace4(("ADP_NODE::tr_stress_last(double) " + short_label()).c_str(),
      tr_value,CKT_BASE::_sim->tt_iteration_number(), val, get());

 // std::cout << "ADP_NODE::tr_stress_last " << label() << tr_value << "\n";
 //
 //
}
---------------------------------*/
/*---------------------------------*/
void ADP_NODE::tr_expect_2_avg(){
  assert(order()==2);

  _delta_expect = (_delta[2]+_delta[1])/2.0;
  tt_expect = tt_integrate_2_linear(_delta_expect);
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_2_linear(double tr_){
  incomplete(); // regression?
  hp_float_t delta = (_delta[1] + tr_)/2 * ( dT0() / tr_duration() -1 );
  double ret = get_aft_1() + delta; // RET
  assert (ret == ret);
  // _debug+=2;

  trace6(( "ADP_NODE::tt_integrate_2_linear "+short_label()).c_str(), get_tr(),
      get1(), ret, delta, tr_, tr_duration() ); 


  if (ret < -1e-10 && _positive ){
    int order = CKT_BASE::_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_integrate_2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        CKT_BASE::_sim->tt_iteration_number(),
        CKT_BASE::_sim->_Time0,   dT0(), Time_delta(), dT1(),
        short_label().c_str(), tt_value, 
        _delta[2], _delta[1], tr_,
        order,
         get1(),
        ret);
//    assert(false);
  }
  if (ret < 0 && _positive ) ret =0;
  _integrator = &ADP_NODE::tt_integrate_2_linear;
  return ret;
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_linear(){
  assert(order()==2);
  trace4(( "ADP_NODE::tt_expect_2_linear " + short_label()).c_str(), 
      tr_value, tr(), tr1(), tr2() );
  trace3(( "ADP_NODE::tt_expect_2_linear " + short_label()).c_str(), Time_delta(), get1(), dT1() );
  // _delta_expect = fabs( (_delta[1]) + ( (_delta[1]) - (_delta[2])) * (hp_float_t) ((Time_delta() )/dT1()));
  //
  // expected tr for time0
  _delta_expect = ( tr1() + ( (tr1()) - (tr2())) * (hp_float_t) ((Time_delta() )/dT1()));
  // _debug+=3;

  if(_delta_expect != _delta_expect) assert(false);
  if(tr1() != tr1()) assert(false);

//  tt_expect = tt_integrate_2_linear(_delta_expect);

  if (tt_expect < -1e-10 && _positive ){
    int order = CKT_BASE::_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect_2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        CKT_BASE::_sim->tt_iteration_number(),
        CKT_BASE::_sim->_Time0,   dT0(), Time_delta(), dT1(),
        short_label().c_str(), tt_value, 
        tr2(), tr1(), _delta_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
  if (tt_expect < 0 && _positive ) tt_expect =0;

  _corrector = NULL; // &ADP_NODE::tr_correct_1_exp; // HACK. ouch. move to ADP_EXP

  tr()=_delta_expect;
//  _integrator = &ADP_NODE::tt_integrate_2_linear;
}
/*---------------------------------*/
// 3 points denoising expect
void ADP_NODE::tr_expect_3_linear(){
  assert(order()==3);
  trace4(( "ADP_NODE::tt_expect3_linear " + short_label()).c_str(), tr_value, _delta[0], _delta[1], _delta[2] );
  trace3(( "ADP_NODE::tt_expect3_linear " + short_label()).c_str(), Time_delta(), get1(), dT1() );
  // tr_expect = fabs( (_delta[1]) + ( (_delta[1]) - (_delta[2])) * (hp_float_t) ((Time_delta() )/dT1()));
  //

  hp_float_t t1 = dT2();
  hp_float_t t2 = dT1()+dT2();
  hp_float_t t3 = dT0()+dT1()+dT2();

  // expected tr for time0

  const double x[3]={0, t1, t2};
  const double y[3]={tr_value3, _delta[2], _delta[1]};

  double c0, c1, cov00, cov01, cov11, sumsq;
  gsl_fit_linear( x, 1, y, 1, 3, &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

  _delta_expect = c0 + c1 * t3;

  if(_delta_expect != _delta_expect) _delta_expect=0;
  if(_delta[1] != _delta[1]) assert(false);

  hp_float_t diff = (_delta[1] + _delta_expect)/2 * Time_delta() / tr_duration();
  tt_expect = get1() + diff;
  assert (tt_expect == tt_expect);

  trace6(( "ADP_NODE::tt_expect3_linear "+short_label()).c_str(), get_tr(),
      get1(), tt_expect, diff, _delta_expect, tr_duration() );

  if (tt_expect < 0 ){
    //positive?
    int order = CKT_BASE::_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect3_linear neg error step %i, Time0=%f  \
       * (%f,%f,%f), %s, tt_value = %g, ( %g, %g, %g)-> %g --  %i tt: %f, \
        expecting %f\n", \
        CKT_BASE::_sim->tt_iteration_number(),
        CKT_BASE::_sim->_Time0,   dT0(), dT1(), dT2(),
        short_label().c_str(),
        tt_value, tr_value3, _delta[2], _delta[1], _delta_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
}
/*---------------------------------*/
/*---------------------------------*/

void ADP_NODE::tr_expect_2_square(){
  assert(order()==2);
  _delta_expect = square( sqrt(_delta[1]) + ( sqrt(_delta[1]) - sqrt(_delta[2])) * (hp_float_t) ((Time_delta() )/dT1()));
}
/*---------------------------------*/
double foo(double x){
  // FIxme: horner.
  if(fabs(x)<1e-4)
    return (1-x/2 + 11/24*x*x -7/16*x*x*x )*exp(1);
  return pow((1+x),(1/x));

}
/*----------------------------------------------------------------------*/
double my_log(double x){
  if(x>0) return log(x);
  return -log(x);
}
/*---*/
double ADP_NODE::tt_integrate_2_linear2(double tr_) {
  trace2("ADP_NODE::tt_integrate_2_exp", _delta[1], tr_);
  trace2("ADP_NODE::tt_integrate_2_exp", _delta[1],  _sign);
  hp_float_t h = tr_duration();
  
  double del0 = tr_;
  double del1 = _delta[1];

  double x = (del0 - del1) / 2;
  double delta = (_delta[1]  + tr_ )/2 * dT0()/h   + x ;

  if (delta != delta || delta==-inf || delta==inf){ 
    delta= (tr_ + _delta[1])/2 * (dT0()-h) / h ;
    incomplete(); // call another thing.
  }

  assert(delta != -inf);
  assert(delta != inf);

  double ret = get_aft_1() + _sign * delta; // RET

  trace5(( "ADP_NODE::tt_integrate_2_exp "+short_label()).c_str(),
      _delta[2], _delta[1], tr_, tt_expect, delta );

  assert(ret == ret);
  _integrator = &ADP_NODE::tt_integrate_2_linear2;
  return ret;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp(){
  assert(order()==3);
  _sign=1;

  if ( ( _delta[2] > 0 && _delta[1] > 0 && _delta[1] < _delta[2] && tr_value3 < _delta[2] )
      || ( _delta[2] < 0 && _delta[1] < 0 && _delta[1] > _delta[2] && tr_value3 > _delta[2] ) ){
    _order--;
    return tr_expect_2_exp();
  }
        
  if (_delta[1] < 0 && _delta[2] < 0 && tr_value3 <0){
        _sign=-1;
  } else if ( ! (_delta[1] > 0 && _delta[2] > 0 && tr_value3 >0) ){
     return tr_expect_3_linear();       
  }

  if ( tr_value3 > _delta[2] && _delta[2] > _delta[1] ){
  }


  double ln3= log(_sign * tr_value3/tr_duration());
  double ln2= log(_sign * _delta[2]/tr_duration());
  double ln1= log(_sign * _delta[1]/tr_duration());

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
double ADP_NODE::tr_correct_3_exp()
{
  trace0(("ADP_NODE::tr_correct_3_exp correction " + label()).c_str());
  return (tr_value+_delta_expect)/2;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tr_correct_generic()
{
  // return(18);
  trace0(("ADP_NODE::tr_correct_generic correction " + label()).c_str());
  return (tr_value+_delta_expect)/2;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tt_integrate_3_exp(double tr_)
{
  assert(order()==3);
  hp_float_t sign=1;
  double h=tr_duration();

  if ( ( _delta[2] > 0 && _delta[1] > 0 && _delta[1] < _delta[2] && tr_value3 < _delta[2] )
      || ( _delta[2] < 0 && _delta[1] < 0 && _delta[1] > _delta[2] && tr_value3 > _delta[2] ) ){
    _order--;
    assert(false);
    //return tr_expect_2_exp();
  }
        
  if (_delta[1] < 0 && _delta[2] < 0 && tr_value3 <0){
        sign=-1;
  } else if ( ! (_delta[1] > 0 && _delta[2] > 0 && tr_value3 >0) ){
//    fixme.
    assert(false);
//     return tr_expect_3_linear();       
  }

  if ( tr_value3 > _delta[2] && _delta[2] > _delta[1] ){
  }

  double ln1= log(_sign*_delta[1]/tr_duration());
  double ln0 = log(_sign*tr_/tr_duration());

  hp_float_t delta = _sign* (exp( ln1 + (ln0-ln1) ) - (_sign*_delta[1]/tr_duration()) )/(ln0-ln1)* dT0();

  if (delta != delta || delta==inf || delta==-inf){

    // fixme: cll anothert integrator.
    hp_float_t delta_trapez = (_delta[1] + tr_)/2 * ( dT0()-h) / tr_duration() ;
    trace2("ADP_NODE::tt_expect3_exp fallback to delta_trapez", delta_trapez, delta);
    // _debug+=1000;

    _integrator = &ADP_NODE::tt_integrate_1_const;
    return( get1() + delta_trapez);
  }

  tt_value = get1() + delta * _sign;
  assert(tt_value == tt_value);
  assert(tt_value >=0 || !_positive );

  _integrator = &ADP_NODE::tt_integrate_3_exp;
  return tt_value;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp_fit(){
  assert(order()==3);
  hp_float_t sign=1;

  if ( ( _delta[2] > 0 && _delta[1] > 0 && _delta[1] < _delta[2] && tr_value3 < _delta[2] )
      || ( _delta[2] < 0 && _delta[1] < 0 && _delta[1] > _delta[2] && tr_value3 > _delta[2] ) ){
    _order--;
    return tr_expect_2_exp();
  }
        
  if (_delta[1] < 0 && _delta[2] < 0 && tr_value3 <0){
        sign=-1;
  } else if ( ! (_delta[1] > 0 && _delta[2] > 0 && tr_value3 >0) ){
     return tr_expect_3_linear();       
  }

  if ( tr_value3 > _delta[2] && _delta[2] > _delta[1] ){
  }

  hp_float_t f0 = sign*tr_value3;
  hp_float_t f1 = sign*_delta[2];
  hp_float_t f2 = sign*_delta[1];

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

  trace6(( "ADP_NODE::tt_expect3_exp() "+short_label()).c_str(),  Time0(), tr_value3, _delta[2], _delta[1], get1(), _delta_expect );
  assert( _delta_expect == _delta_expect );

  hp_float_t E = - c/alpha;
  hp_float_t delta = E * ( - exp( t3*alpha ) + exp(t2*alpha) )/ CKT_BASE::_sim->_last_time ;
  hp_float_t delta_trapez = (_delta[1] + _delta_expect)/2 * Time_delta() / CKT_BASE::_sim->_last_time;

  trace4("ADP_NODE::tt_expect3_exp", delta, delta_trapez, tt1(), get() );
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
    error(bDANGER, "* ADP_NODE::tt_expect3_exp neg error step %i, Time0=%f, %s,"
        " tt_value = %g, ( %g, %g, %g; %g) tte: %g \n", \
        CKT_BASE::_sim->tt_iteration_number(),
         dT0(), short_label().c_str(), tt_value,
         tr_value3, _delta[2], _delta[1], _delta_expect,
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
  unreachable();
  assert(order()==3);
  trace3(( "ADP_NODE::tr_expect_3_quadratic() "+short_label()).c_str(), tr_value3, _delta[2],  _delta[1] );
  // uses 1 2 3 to set expect.
  assert( tr_dd12 == ( _delta[1] - _delta[2] ) / dT1());
  hp_float_t sum = dT0() + dT1() + dT2();
  hp_float_t tr_expect_kaputt;

//        doch nicht kaputt.
  tr_expect_kaputt = ( \
      sum * ( _delta[1]  + tr_dd12 * dT0() ) \
      - dT0() * (( - dT1()* tr_dd12 + _delta[1] ) + (dT0()+dT1()) * ( tr_dd12 + tr_dd123* ( -dT2()-dT1() )  ) ) \
      ) / (dT2()+dT1());


  _delta_expect = -(((_delta[2] - tr_value3)/dT2() - (_delta[1] - _delta[2])/dT1())*(dT0() + dT1())/(dT1() + dT2()) -
    (_delta[2] - tr_value3)/dT2())*(dT0() + dT1() + dT2()) + tr_value3;

  hp_float_t diff = (_delta[1] + _delta_expect)/2 * Time_delta() / CKT_BASE::_sim->_last_time;
  tt_expect = get() + diff;
  assert (tt_expect == tt_expect);
}
/*---------------------------------*/
double ADP_NODE::debug(){
  return _debug;
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_something(){
  assert(order()==2);
  _delta_expect = _delta[1] + (_delta[1] - _delta[2]) * dT0()/dT1();
  tt_expect = get1() + ( Time_delta() ) / CKT_BASE::_sim->_last_time;
  trace2(( "ADP_NODE::tt_expect2_something "+short_label()).c_str(), get1(), tt_expect );
}
/*---------------------------------*/
void ADP_NODE::tr_expect_1_const(){
  assert(order()==1);
  trace2(( "ADP_NODE::tt_expect_1_const() "+short_label()).c_str(),
       tr(), tr1() );
  trace3(( "ADP_NODE::tt_expect_1_const() "+short_label()).c_str(),
      Time_delta(), CKT_BASE::_sim->_time0, CKT_BASE::_sim->_last_time );

  tr() = tr1();
//  _corrector = &ADP_NODE::tr_correct_1_const;
  // tr() = tt_integrate_1_const(_delta_expect);


  assert (!_positive || tt_expect >=0);
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_1_exp( double tr_ ){
  // FIXME implement quadratic model
  return( tt_integrate_1_const(tr_) );
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_1_const( double tr_){
  _sign = 1;
  // _debug=3;
  double delta = ( tr1() + tr_ )/2 *  (dT0() / tr_duration() - 1) ;
//   double ret = tt1() + delta + tr_; // RET
  
  long double ret = get_aft_1() + delta; // RET

  trace5(("ADP_NODE::tt_integrate_1_const" + label()).c_str(), tt1(),
      delta , tr_, ret, dT0() / tr_duration());
  trace1(("ADP_NODE::tt_integrate_1_const" + label()).c_str(),tr_noise);
  // trace1("ADP_NODE::tt_integrate_1_const", tt());

  if (ret < 0 && _positive){
    error(bDANGER, "* ADP_NODE::tt_integrate_1_const neg error step %i,"
        " Time0=%f (%f), %s, tt_value = %g, ( %g, %g) \n", 
        CKT_BASE::_sim->tt_iteration_number(),
        CKT_BASE::_sim->_Time0, dT0(),
        short_label().c_str(), tt_expect,
        _delta[0], _delta[1]);
//    assert(false);
     ret=0;
  }

  if (_delta[1] < 0 && tr_ < 0 ){
    _sign = -1;
  }

  _integrator = &ADP_NODE::tt_integrate_1_const;
  assert (!_positive || ret >=0);
  return( (double) ret);
}
/*---------------------------------*/
double ADP_NODE::tr_correct_1_const(){
  trace0(("ADP_NODE::tr_correct_1_const correction " + label()).c_str());
  return  (_delta_expect + tr_value)/2;
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_( double ){
// take order and tr_value
// integrate things. write to tt_value.
  return 0;
}
/*---------------------------------*/
/*----------------------------------------------------------------------------*/
//  tt1() += diff;
void ADP_NODE::tt_commit_first( )
{
  trace2(("ADP_NODE::tt_commit_first() " + short_label()).c_str(), tt(), tr());
  assert (order()==0);
  _integrator = 0;
  _corrector = 0;


  tt_first_time = CKT_BASE::_sim->_Time0;
  assert(_delta_expect != HUGE_VAL );
}
/*---------------------------------*/
void ADP_NODE::tt_commit( )
{
 //  called before CARD_LIST apply...
 //  extrapolates tr values for guessed dT0
 //
 //   FIXME: cleanup.

  if (order()==0)
    tt_commit_first();

  trace3(( "ADP_NODE::tt_commit "+short_label()).c_str(), tt_value, tr(),
     CKT_BASE::_sim->tt_iteration_number() );
  trace6(("ADP_NODE::tt_commit " + short_label()).c_str(),
      dT0(), dT1(),
      CKT_BASE::_sim->get_tt_order(),
      CKT_BASE::_sim->_Time0,
      tt(), tt1()
      );

  // sets _delta_expect and tt_expect
  if(!is_number( tr1()) && CKT_BASE::_sim->tt_iteration_number()>=2 )
  {
    error(bDANGER,"ADP_NODE::tt_commit history broken %s, %i, step %i\n", long_label().c_str(), m_(), _sim->tt_iteration_number());
    assert(false);
  }
  tr_expect_();


  // this is before sweep....
  assert( CKT_BASE::_sim->_last_time >0 );

  assert( tt_value == tt_value );
  assert( tt_value >=0 || !_positive );
  // tr_value will be printed as "tr"

  if (tt_expect < -1e-8 ){
    //positive?
    error(bDANGER, "* ADP_NODE::tt_commit neg error step %i, order %i, tt_expect=%e\n", \
        CKT_BASE::_sim->tt_iteration_number(), _order,tt_expect);
  }
}
/*---------------------------------*/
// tr relative to Time1
hp_float_t ADP_NODE::tr_rel( double dT ) const{
  long double now_rel = dT;

  switch(order()){
    case 0: // transient sim
      assert( _sim->analysis_is_tran() );
      return tr();
    case 1:
      return tr1();
    case 2:
      trace1("ADP_NODE::tr 2", now_rel/ dT1());
      return tr1() + ( tr1() - tr2()) * double ((now_rel )/(long double) dT1());
    case 3:
      return  -(((tr2() - tr3())/dT2() - (tr1() - tr2())/dT1())*(hp_float_t(now_rel) + dT1())/(dT1() + dT2()) -
        (tr2() - tr3())/dT2())*(hp_float_t(now_rel) + dT1() + dT2()) + tr3();
    default:
      assert(false);
  }
}
/*---------------------------------*/
// order:
// 0 during 1st timeframe
// 1 during 2nd timeframe.

hp_float_t ADP_NODE::tr( double time ) const{
  long double Time1 = Time0() - dT0();
  long double now_rel = time - Time1; // dT0, oder?

  switch(order()){
    case 0: // transient sim
      assert( _sim->analysis_is_tran() );
      return tr();
    case 1:
      return tr1();
    case 2:
      trace1("ADP_NODE::tr 2", now_rel/ dT1());
      return tr1() + ( tr1() - tr2()) * hp_float_t((now_rel )/(long double) dT1());
    case 3:
      return  -(((tr2() - tr3())/dT2() - (tr1() - tr2())/dT1())*(hp_float_t(now_rel) + dT1())/(dT1() + dT2()) -
        (tr2() - tr3())/dT2())*hp_float_t((now_rel) + dT1() + dT2()) + tr3();
    default:
      assert(false);
  }
}
/*---------------------------------*/
void ADP_NODE::tr_expect_( ){
  _integrator = 0;
  _corrector = 0;
  _order = min(_order+1,CKT_BASE::_sim->get_tt_order());

  trace2("ADP_NODE::tr_expect_", _order, CKT_BASE::_sim->tt_iteration_number());
  trace4("ADP_NODE::tr_expect_ ", _sim->_Time0, tr(_sim->_Time0), tr1(), tr2() );
  assert(_order <= CKT_BASE::_sim->tt_iteration_number());
  assert ( isnan(tr()) || order()==0 );

  switch(_order){
    case 0:
      trace0("ADP_NODE::tr_expect_, not doing anything");
      return;
    case 2:
      trace3(("ADP_NODE::tr_expect_ extradebug" + long_label()).c_str(), tr2(), tr1(), tr(Time0()) );
      if ( fabs(tr2()-tr_rel(-dT1())) > 1e-6   && !(is_almost(tr2(), tr(Time2())))){
        error( bDANGER, "ADP_NODE::tr_expect_ tt_iteration_number %i\n", tt_iteration_number());
        error( bDANGER, "ADP_NODE::tr_expect_ mismatch, T0: %.20E, T1 %.20E, Time2 %.20E\n", Time0(), Time1(), Time2());
        error( bDANGER, "ADP_NODE::tr_expect_ mismatch dT1() %E dT0() %E\n", dT1(), dT0() );
        error( bDANGER, "ADP_NODE::tr_expect_ mismatch, tr1:   %E, tr2: %E d %E  \n", tr1(), tr2(), tr1()-tr2() );
        error( bDANGER, "ADP_NODE::tr_expect_ mismatch, tr2():tr(Time2()=%E))= %E : %E\n", Time2(), tr2(), tr(Time2()) );
        error( bDANGER, "ADP_NODE::tr_expect_ delta, tr2()-tr(Time2()))= %E\n", tr2()- tr(Time2()) );
        error( bDANGER, "ADP_NODE::tr_expect_ mismatch in %s\n", long_label().c_str() );

        throw(Exception(" mismatch " + long_label()));
        assert( false);
      }
    case 1:
      if (! is_almost(tr1(), tr(Time1()))) {
        error( bDANGER, "ADP_NODE::tr_sanity check failed %1.19g %1.19g \n",tr1(),tr(Time1()));
        untested();
      }
      // assert(is_almost(tr1(), tr(Time1())));
      //tr()=tr(_sim->_Time0);
      tr()=tr_rel(dT0());

      break;
    case 3:
      // FIXME assert( tr_dd12 == ( _delta[1] - _delta[2] ) / dT1());
      tr_dd12 = ( tr1() - tr2() ) / dT1();
      tr()=tr(_sim->_Time0);

      break;
    default:
      assert(false);
  }
  trace6( "ADP_NODE::tr_expect_ done ", tr1(), _delta_expect, _order, 
      tt(), tt1(), _number);
  assert(tt1() >=0 || !_positive);

  // assert(is_number(_tt[0]));

//  return _delta_expect;
 // tt_value  << " " << get1() << " order " << _order << " val1 " <<
 // tt1() <<
 //  "ttex " << tt_expect <<  "\n";
}
/*-----------------------------------------*/
int ADP_NODE::region() const{
        return _region;
}
/*-----------------------------------------*/
void ADP_NODE::tr_expect_1_exp(){
  // constant
  assert(order()==1);
  _sign=1;
  if(_delta[1] < 0) _sign=-1;
  trace3(( "ADP_NODE::tt_expect_1_exp() "+short_label()).c_str(),
     tr_value, _delta[0], _delta[1] );
  trace3(( "ADP_NODE::tt_expect_1_exp() "+short_label()).c_str(),
      Time_delta(), CKT_BASE::_sim->_time0, CKT_BASE::_sim->_last_time );

  _delta_expect = _delta[1];
  assert (_delta_expect < 0.1);

  _corrector = &ADP_NODE::tr_correct_1_exp;
  tt_expect = tt_integrate_1_const(_delta_expect);

  _integrator = &ADP_NODE::tt_integrate_1_exp;
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_exp(){
  trace2("ADP_NODE::tr_expect_2_exp()", _delta[1], _delta[2]);
  double h = tr_duration();
  // _debug+=1000;
  assert( order() == 2 );
  assert( is_number(_delta[1]) );
  assert( is_number(_delta[2]) );
  _sign = 1;
  if (_delta[1] <= 0 && _delta[2] < 0 ){
    _sign=-1;
  }

  // need total ordering.
  if ( !( ( _delta[2] <= _delta[1] && _delta[1] <= 0) 
       || ( _delta[2] >= _delta[1] && _delta[1] >= 0))){
//    _order--;
    trace2(("ADP_NODE::tr_expect_2_exp no total ord linear... " +
          short_label()).c_str(),  _delta[2], _delta[1]);
    incomplete();
//    _debug+=5000;
//    tr_expect_2_linear();
    _order--;
    tr_expect_1_exp();
    _corrector = &ADP_NODE::tr_correct_1_exp;
    return;
  }
  if ( fabs(_delta[1] - _delta[2]) < tr_noise ){
    trace3(("ADP_NODE::tr_expect_2_exp just noise " +
          short_label()).c_str(),  _delta[2], _delta[1], tr_noise);
    incomplete();
    _order--;
    tr_expect_1_exp();
  //  _corrector = &ADP_NODE::tr_correct_1_exp;
    return;
  }
  //hp_float_t t2 = tr_duration()/2;
  //hp_float_t t1 = t2+dT1();
  //dashier geht nicht. kleine schritte zu schlecht.
  double d1 = _delta[1];
  double d2 = _delta[2];

  d1/=h;
  d2/=h;
  double alpha = - log ( d2/d1 ) / dT1();

  //d1=_der_aft[1]; // dashier muesste man noch ausrechnen.
  //
  //
  // expect using deltas as derivatives.
  d1=_delta[1] / h;
  d2=_delta[2] / h;
  //  d2=_der_aft[2];

  alpha = - log ( d2/d1 ) / dT1();
  double q =  ( d1/d2 ) ;

  if ( _sign*d1 <= _sign*d2 && q != q ) q=1;

 //expect using deltas.
  d1=_delta[1] / h;
  d2=_delta[2] / h;

 // _delta_expect = pow(d1,((dT0() + dT1())/dT1())) / pow(d2,(dT0()/dT1()))*h;

  _delta_expect = pow(q, (dT0()/dT1()))*d1*h;

  trace6("ADP_NODE::tr_expect_2_exp", _delta_expect - _delta[1], _der_aft[2], _der_aft[1],
      _delta[1], _delta[2], dT0());
  trace6("ADP_NODE::tr_expect_2_exp", _delta_expect, _der_aft[2], _der_aft[1],
      _delta[1] - _delta[2], _delta_expect, _sign);
  //trace6("ADP_NODE::tr_expect_2_exp", _delta[1], _delta[2], _delta_expect, p, r, q);

  assert(_delta_expect == _delta_expect);
  assert(_delta_expect < 1e6);
  _corrector = &ADP_NODE::tr_correct_1_exp;
  assert (_sign*_delta[1] >= 0 );

  assert( 1e-15 >= _sign*(_delta_expect - _delta[1]) );

  tt_expect = tt_integrate_2_exp(_delta_expect);
}
/*---------------------------------*/
double ADP_NODE::tr_correct_1_exp(){

  assert(order()!=0);
  trace0(("ADP_NODE::tr_correct_1_exp correction " + label()).c_str());

  double new_delta; // return value. new value for tr_value.
  double h = tr_duration();

  //assert(tt1() == tt1());

  long double a = _delta[1];
  //long double c = tr_value;
  long double b = tt_expect;
  long double d = tt1();
  long double dT = dT0();

  trace4(("ADP_NODE::tr_correct_1_exp, _der_aft? " + label()).c_str(),
      _delta[1], tt_expect, tr_value, tt1() );

  //model value as (does it make sense?)
  /// t \mapsto K* ( 1- exp (tau*t) )+ d 
  // long double K = (b-d)/(c/a-1);
  // long double tau = log((c-a)/(b-d) + 1) / h;

  //_delta_expect = K *(- exp(tau* (dT0()+h) )  + exp ((tau*dT0())) );

  // a*(a - b - c + d)^(dT0/h)/(-b + d)^(dT0/h)
  //
  
  long double d_b = d-b;
  long double a_c = _delta[1] - tr_value;

  
  if (  fabs ( a_c ) < tr_noise )  {
    trace2("ADP_NODE::tr_correct_1_exp ", a_c, tr_noise );
    _integrator=&ADP_NODE::tt_integrate_1_const;
    return(tr_value + _delta[1])/2;
  }

  if (  fabs ( d_b )  < 1e-20 )  {
    trace6("ADP_NODE::tr_correct_1_exp ", _delta[1], tr_value, tt1(),
        tt_expect, 1e-13, fabs ( d_b / (fabs(d)+fabs(b))));
    _integrator=&ADP_NODE::tt_integrate_1_const;
    return(tr_value + _delta[1])/2;
  }
  if ( ( fabs ( d_b ) / (fabs(d)+fabs(b) ) < 1e-13 ) ) {
    trace6("ADP_NODE::tr_correct_1_exp ", _delta[1], tr_value, tt1(),
        tt_expect, 1e-13, fabs ( d_b / (fabs(d)+fabs(b))));
    _integrator=&ADP_NODE::tt_integrate_1_const;
    return(tr_value + _delta[1])/2;
  }
  long double B = (1 + (a_c)/(d_b));
  long double gain = ( dT/h );
  assert(is_number(gain));

  if (B<0){
    B=0;
    untested();
  }
  long double P = pow(B,gain);

  if ( !is_number((double)P) ) {
    trace0("ADP_NODE::tr_correct_1_exp P is not number. probably just noise");
    return(tr_value + _delta[1])/2;
  }

//  new_delta = (double)( a*pow((a - c + d-b)/(d-b),(dT/h)) );
  new_delta = (double)( a*P  );

  trace6(("ADP_NODE::tr_correct_1_exp, " + label()).c_str(), \
      a, tr_value, P, dT0(), B, gain );
  trace6(("ADP_NODE::tr_correct_1_exp, " + label()).c_str(), tt_expect,
      tt1(), tt_value, _delta[1], tr_value, new_delta);
  trace4( "ADP_NODE::tr_correct_1_exp " , CKT_BASE::_sim->tt_iteration_number(), d_b, a_c, 
      CKT_BASE::_sim->_dT0/CKT_BASE::_sim->_dTmin );

  assert(is_number(B));
  assert(is_number(P));
  assert(is_number(new_delta));
  assert(new_delta * _delta[1] >= 0);
  assert(new_delta<1);

  if( (_delta[1] >= new_delta && new_delta >= 0)
      ||  (_delta[1] <= new_delta && new_delta <= 0)){
  } else {
    _integrator=&ADP_NODE::tt_integrate_1_const;
  }

  return new_delta;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tt_integrate_2_exp(double tr_) {
  trace4("ADP_NODE::tt_integrate_2_exp", _delta[1], tr_ , _sign, tr_value);
  trace4("ADP_NODE::tt_integrate_2_exp", tr_duration(), dT0(), tt1() +
      _delta[1], tt1());
  hp_float_t h = tr_duration();

  assert (_sign * tr_       >= 0 );
  assert (_sign * _delta[1] >= 0 );
  trace4( "ADP_NODE::tt_integrate_2_exp ", _delta[1],  tr_, _delta[1]- tr_, _sign);

  if (_sign*_delta[1] < _sign*tr_) {
    untested();
  }
  
  long double ln1 = log(_sign*_delta[1]/h);
  long double ln0 = log(_sign*tr_/h);

  //int_{h}^{dT0} exp( ln1 + v/dT0 * (ln0 - ln1) ) dv
  long double delta = dT0()/(ln0-ln1) * ( _sign*tr_/h - exp( ln1 + (h/dT0())
        *(ln0-ln1) ) ) * _sign;


  //int_{h/2}^{dT0-h/2} exp( ln1 + v/dT0 * (ln0 - ln1) ) dv
  delta = dT0()/(ln0-ln1) * ( exp( ln1 + (1-h/2/dT0()) * (ln0-ln1) ) 
                            - exp( ln1 + (  h/2/dT0()) * (ln0-ln1) ) ) * _sign;

  if (delta != delta || delta==-inf || delta==inf){ 
    return ( tt_integrate_1_const( tr_) );
    trace0( "FIXME" ); // call another thiongh.
  }

  assert(delta != -inf);
  assert(delta != inf);

  long double ret = get1() + _delta[1] + delta; // RET

  if ( h == dT0() && fabs(delta)> 1e-14 ){
    trace2("ADP_NODE::tt_integrate_1_exp 0", _delta[1], delta);
    assert(false); // throw something?
  }

  trace6(( "ADP_NODE::tt_integrate_1_exp "+short_label()).c_str(),
      _delta[2], _delta[1], tr_, tt_expect, delta, ret );

  assert(ret == ret);

  assert (_sign*tr_ >= 0 );
  _integrator = &ADP_NODE::tt_integrate_1_exp;

  if( _positive && ret < 0){
        ret=0;
  }

  assert(tt_expect >=0 || !_positive);
  assert(ret >=0 || !_positive);
  assert(ret<2); // RCD?

  return double(ret);
}
/*----------------------------------------------------------------------*/
// ADP_CARD::ADP_CARD() {unreachable();}
/*----------------------------------------------------------------------*/
ADP_CARD::ADP_CARD(const ADP_CARD& p) : COMPONENT(p) {unreachable();} 
/*----------------------------------------------------------------------*/

TIME_PAIR ADP_NODE_UDC::tt_review( ) {
  return ADP_NODE::tt_review();
}
/*----------------------------------------------------------------------*/
TIME_PAIR ADP_NODE::tt_review( ) {
  hp_float_t myreltol = OPT::adpreltol;
  hp_float_t myabstol = OPT::adpabstol;
  double h = tr_duration();
  double delta_model;
  //fixme: where corrector?
  //if (_corrector){
  //  assert(order()>0);
  //  trace1(("ADP_NODE::tt_review: correction " + label()).c_str(), _delta_expect);
  //  delta_model = (this->*_corrector)(); // value predicted by model.
  //} else {
  delta_model = 0;
  //  trace1(("ADP_NODE::tt_review: no corrector " + label()).c_str(), delta_model );
  //}

  trace3("ADP_NODE::tt_review", CKT_BASE::_sim->tt_iteration_number(), myabstol, myreltol);
  assert(delta_model == delta_model);
  if( myreltol == 0 ) {_wdT=0; return TIME_PAIR(0,0); }
  if( myabstol == 0 ) {_wdT=0; return TIME_PAIR(0,0); }

  if ( ( tr_value * delta_model ) < 0 ) {
    trace2(("ADP_NODE::tt_review: oops, sign has changed "+ label()).c_str(), tr_value,delta_model);
  }
  // FIXME: implement abs err!
  double tr_high = tr_value + tr_noise;
  double tr_low = tr_value - tr_noise;

  if ( delta_model > tr_high ) {
    _abs_tr_err = tr_high - delta_model;
  }  else if (delta_model < tr_low ){
    _abs_tr_err = delta_model - tr_low;
  }else {
    _abs_tr_err = fabs(delta_model - tr_value);
  }

  trace2("ADP_NODE::tt_review", long_label(), tr_noise);

  if (tr_noise<0 || !is_number(tr_noise)){
    error(bDANGER, "ADP_NODE::tt_review noise error: %s\n", long_label().c_str());
    assert (tr_noise >=0);
  }

  _abs_tr_err = fabs (tr_value - delta_model);
  _abs_tr_err = max ( fabs (tr_value - delta_model) - tr_noise ,0.0 );
//  _abs_tr_err = fabs (tr_value - delta_model) / tr_noise ;

  if (_abs_tr_err < 1e-40 ) { 
    _rel_tr_err = 0;
    _abs_tr_err = 0;
  } else {
    _rel_tr_err = _abs_tr_err; //  / max( fabs(tr_value) , fabs(delta_model));

    // use noise for predictor only?
    // _rel_tr_err = fabs( tr_value-delta_model )/ fabs(tr_noise ) ;
  }
  _rel_tt_err = fabs (tt_value - tt_expect) / (fabs(tt_value) + fabs(tt_expect));

  tr_value = delta_model;

  trace3(("ADP_NODE::tt_review" + label() + "got tol").c_str(), myreltol, _abs_tr_err, _rel_tr_err);
  if( _abs_tr_err == 0 ) {
    trace1(("ADP_NODE::tt_review" + label() + "noabs").c_str(), TIME_PAIR(0, NEVER)._event );
    _wdT = inf; return TIME_PAIR(0, NEVER);
  }
  if( myreltol == inf && myabstol == inf ) { _wdT = inf; return TIME_PAIR();}
  if( myreltol == 0 && myabstol == 0 ) {_wdT=0; return TIME_PAIR(0,0);}

// FIXME: _order.
// 
  _wdT = dT0() * sqrt( myreltol / _rel_tr_err )  + h;
  _wdT = dT0() * log( 1+ myreltol / _rel_tr_err )  + h;


  if (_rel_tr_err >= 1 ){
    error( bDANGER, "stepdebug %i dev: %s reltr %f>1 model %E, tr_ %E fill %E\n",
        CKT_BASE::_sim->tt_iteration_number(),
        label().c_str(),   _rel_tr_err, delta_model, tr_value, tt_value );
  }
  if (_wdT < 1.1*h){
    error( bDANGER, "stepdebug  dev: _wdT %s wdT: %g %g %f rel_tr %f abserr: %g, corr %i ord %i\n",
        label().c_str(), _wdT, dT0(), myreltol,  _rel_tr_err, _abs_tr_err, (bool)_corrector, order() );
  }

  trace4("ADP_NODE::tt_review adaptive _wdT", myreltol, inf, h, _rel_tr_err);
  if( _wdT < 0  || _wdT != _wdT) {
    error(bDANGER, " dev: _wdT %s %f %f %f\n", label().c_str(), _wdT, dT0(), _rel_tr_err );
  }

  if( _rel_tr_err != _rel_tr_err ) _wdT = NEVER;
  if( !(_rel_tr_err == _rel_tr_err) ) _wdT = NEVER;

  trace3( ("ADP_NODE::tt_review " + label()).c_str(), CKT_BASE::_sim->_Time0 , _wdT , myreltol );

  assert(_wdT >= 0 );

  assert(tr_value == tr_value);
  assert(_delta_expect == _delta_expect);
  tt_value += tr_value; // only printed if OPT::printrejected

  return( TIME_PAIR(0, _wdT));
}

void ADP_NODE_RCD::tr_expect_1(){ return tr_expect_1_exp();}
void ADP_NODE_RCD::tr_expect_2(){ return tr_expect_2_exp();}
void ADP_NODE_RCD::tr_expect_3(){ return tr_expect_3_exp();}

void ADP_NODE_UDC::tr_expect_1(){ return tr_expect_1_const();}
void ADP_NODE_UDC::tr_expect_2(){ return tr_expect_2_linear();}
void ADP_NODE_UDC::tr_expect_3(){ return tr_expect_3_quadratic();}
