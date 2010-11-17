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
ADP_NODE::ADP_NODE( const COMPONENT* c, const char name_in[] ) : dbg(0)
{
  init();
  _c = c;
  assert(c);
  name = c->short_label() + "." +  std::string(name_in);
}
/*----------------------------------------------------------------------------*/
ADP_NODE::ADP_NODE( const COMPONENT* c, std::string name_in2 ) : dbg(0)
{
  init();
  _c = c;
  assert(c);
  name=c->short_label() + "." + name_in2;
}
/*----------------------------------------------------------------------------*/
ADP_NODE::ADP_NODE( ADP_CARD* ac, const COMPONENT* c, const char name_in[] ) : dbg(0)
{
  init(); _c=c;
  name=c->short_label() + "." + std::string(name_in);
  a=ac; 
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::init(){
  tr_value = (0.);
  tt_value = 0.;
  _delta_expect = 0.;
  _positive = true; //hack
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
void ADP_NODE::tr_add(double x ){
  if ( _c->_sim->_time0 == 0 ) assert (tr_value == 0);
  tr_value += x;
  if(x>0)  _region=2;
  if(x<0)  _region=-2;
  if(x==0) _region=0;
}

/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_set(double x ){
  _val_bef[0] = x;
  tt_value = x;
  assert(_val_bef[0] >=0 || !_positive);
}
/*----------------------------------------------------------------------------*/
hp_float_t ADP_NODE::get_aft_1() const { return _val_bef[1] + _delta[1] ; }
/*----------------------------------------------------------------------------*/
void ADP_NODE::tr_set(double x ){
  std::cout << "* tr_set " << x << "\n";
  tr_value = x;
}
/*----------------------------------------------------------------------------*/
double ADP_NODE::tr_duration()const{ return _c->_sim->_last_time; }
/*----------------------------------------------------------------------------*/
// called right after tr
double ADP_NODE::tt_review( ) {
  double h = tr_duration();
  double delta_model;
  if (_corrector){
    assert(order()>0);
    trace1("ADP_NODE::tt_review: correction ", _delta_expect);
    delta_model = (this->*_corrector)(); // value predicted by model.
    assert( delta_model != HUGE_VAL );
  } else {
    delta_model = _delta_expect;
    assert( delta_model != HUGE_VAL );
  }

  trace1("ADP_NODE::tt_review", _c->_sim->tt_iteration_number());
  assert(delta_model == delta_model);
  hp_float_t myreltol = OPT::adpreltol;
  hp_float_t myabstol = OPT::adpabstol;

  if ( ( tr_value * delta_model ) < 0 ) {
    trace2("ADP_NODE::tt_review: oops, sign has changed", tr_value,delta_model);
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

  // _debug=_rel_tt_err;

  if( myreltol == 0 ) return 0;

  trace2("ADP_NODE::tt_review", myreltol, inf);
  if( _abs_tr_err == 0 ) { _wdT = inf; return inf; }
  if( myreltol == inf && myabstol == inf ) { _wdT = inf; return inf;}
  if( myreltol == 0 && myabstol == 0 ) {_wdT=0; return 0;}

// FIXME: _order.
//
  _wdT = (dT0()-h) * sqrt( myreltol / _rel_tr_err )  + h;
  trace4("ADP_NODE::tt_review", myreltol, inf, h, _rel_tr_err);
  if( _wdT < 0  || _wdT != _wdT) {
    error(bDANGER, " dev: _wdT %s %f %f %f\n", name.c_str(), _wdT, dT0(), _rel_tr_err );
  }

  if( _rel_tr_err != _rel_tr_err ) _wdT = NEVER;
  if( !(_rel_tr_err == _rel_tr_err) ) _wdT = NEVER;

  trace3( ("ADP_NODE::tt_review " + label()).c_str(), _c->_sim->_Time0 , _wdT , myreltol );

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
  trace2( "ADP_NODE::reset()", _c->_sim->_Time0, tr_value );
  std::cout << "rst " << label() << "\n";
  tr_value = 0; // _val_bef[1] = 0; _val_bef[0] =0;
}
void ADP_NODE::reset_tr() 
{ 
  trace1(("ADP_NODE::reset_tr() " + short_label()).c_str(), tr_value );
  // std::cout << "trrst " << label() << " " << _val_bef[1] << "\n";
  tr_value = 0;
}
/*----------------------------------------------------------------------------*/
int ADP_NODE::order() const{
  return( min( (int) _order,(int) _c->_sim->get_tt_order() ) );
} // order used for extrapolation.
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_accept()
{
  // sets t{t,r}_value0 to the accepted values.
  // should compute new wanted_dT.

  //
  //
  trace3(("ADP_NODE::tt_accept() " + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), _c->_sim->get_tt_order());
  assert( _delta[0] == _delta[0] );
  assert( _c->_sim->_last_time >0 );


  int order=_c->_sim->get_tt_order();

  if (order != _order){
    trace2("ADP_NODE::tt_accept order mismatch", order, _order);
    _order=min(_order, order);
  }

  switch( order ){
    case 0:
      // fixme. merge
      return tt_accept_first();
      break;
    case 1:
      tr_dd12 = ( _delta[0] - _delta[1] ) / dT0();
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

  _delta[0] = tr_value; 

  if (_integrator && order){
    trace0("reintegration");
//    tr_value = _delta[0];
    tt_value = (this->*_integrator)(_delta[0]);
  } else {
    untested();
  }

  // std::cout << "acc after int " << label() << tr_value << "\n";



  assert( tt_value == tt_value );

  _val_bef[0] = tt_value;

  tt_value += _delta[0]; // (printed) output value
  //tt_value += 13; // (printed) output value
  if (tt_value <0 && _positive){
    untested();
    tt_value=0;

  }

  trace2(("ADP_NODE::tt_accept" + _c->short_label()).c_str(), tt_value, tr_value);
  trace1(("ADP_NODE::tt_accept done " + _c->short_label()).c_str(), get_tt());
  assert(_val_bef[0] >=0 || !_positive);

  if (( dT0() / tr_duration() ) < 1.000001){
    trace5(("ADP_NODE::tt_accept" + _c->short_label()).c_str(), _val_bef[0] ,
        _val_bef[1], _delta[0], _val_bef[0] - _val_bef[1] - _delta[1], _delta[1] );
    assert(fabs( ( _val_bef[0] - _val_bef[1] - _delta[1]) / (_delta[1] + 1e-10)) < 1e-13);
  }

  // hier schon falsch
 // std::cout << " accept " << label() << " "  << tr_value <<"\n";
  if ( dT0() / tr_duration() < 1.000001 ){
    trace4("ADP_NODE::tt_accept ", _val_bef[0] , _val_bef[1] , _delta[1] ,  _val_bef[0] - _val_bef[1] - _delta[1] );
    assert ( fabs ( ( _val_bef[0] - _val_bef[1] - _delta[1] ) / (_delta[1] + 1e-13) ) < 1e-12 );
  }
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_last( ) {
  _val_bef[0] = _val_bef[0] + _delta[0]; //FIXME. add tt_keep_values.
  if( _val_bef[0] < 0 && _positive ){    //FIXME. add tt_keep_values.
    _val_bef[0] = 0;                       //FIXME. add tt_keep_values.
  }

}
/*----------------------------------------------------------------------------*/
// fixme: merge
void ADP_NODE::tt_accept_first( )
{
  tt_value = _val_bef[0] + tr_value;
  assert(_val_bef[0] >=0 || !_positive);

  assert(order()==0);
  trace3(("ADP_NODE::tt_accept_first" + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), tt_value );
  assert( tr_value == tr_value );

  assert( _c->_sim->get_tt_order() == 0 );

  // assert(0  == _c->_sim->_dT0);
  _val_bef[2] = _val_bef[1];
  _val_bef[1] = _val_bef[0];

  _delta[1] = _delta[0];
  _delta[0] = tr_value;

//  tr_value = 0;

  assert(_val_bef[0] >= -1e-5 );
  assert(_val_bef[0] >= _val_bef[1]);

 // FIXME tt_value??
 
  assert(tt_value == tt_value);
  if (_positive && tt_value < 0 ){
    untested();
    // assert(_val_bef[0] >= 0);
  }
  trace4(("ADP_NODE::tt_accept_first" + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), tt_value, tt_value);

}
/*----------------------------------------------------------------------------*/
ADP_NODE::ADP_NODE( const COMPONENT* c ) : dbg(0)
      { init(); _c = c; name += c->short_label(); }
/*----------------------------------------------------------------------------*/
// accept, print, advance.
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_advance(){
  trace3(("ADP_NODE::tt_advance() " + _c->short_label()).c_str(), get_tt(), _val_bef[0], _val_bef[1]);

  // this is no logner valid, since tr_value has been corrected.
  //  assert(tr_value == _delta[0]);
  _debug = 0;

  _delta[2] = _delta[1];
  _delta[1] = _delta[0];

  _delta[0] = 888; // invalidate

  _val_bef[2] = _val_bef[1];
  _val_bef[1] = _val_bef[0];

  //shift the derivatives.
  _der_aft[2] = _der_aft[1];
  _der_aft[1] = _der_aft[0];

  assert(is_number(_der_aft[2]));
  assert(is_number(_der_aft[1]));

  _val_bef[0] = 12345;
  assert (!_positive || _val_bef[0] >=0 );

  trace2(("ADP_NODE::tt_advance done " + _c->short_label()).c_str(), get_tt(), get_tr());

  if (tr_duration() - dT0() < 1e-10){
    trace4(("ADP_NODE::tt_accept" + _c->short_label()).c_str(), _val_bef[1] ,
        _val_bef[2], _delta[1], _val_bef[1] - _val_bef[2] - _delta[1]);
    assert(fabs(_val_bef[1] - _val_bef[2] - _delta[1]) < 1e-15);
  }

  tr_value = 9988; 
}
/*---------------------------------*/
hp_float_t ADP_NODE::get_total() const{
  double ret= get_tr() + get_tt() ;
  
  if (!(ret>=0 || !_positive)){
    trace3("ADP_NODE::get_total", get_tt(), get_tr(),  _c->_sim->tt_iteration_number() );
    assert(false);
  }

  return( get_tr() + get_tt() );
}
/*---------------------------------*/
void ADP_NODE::tr_stress_last( double val ) {

  tr_value = val - get();
  trace3(("ADP_NODE::tr_stress_last" + label()).c_str(), val, get(), _c->_sim->tt_iteration_number());
  _delta[0] = tr_value; // good idea? no.

  trace4(("ADP_NODE::tr_stress_last(double) " + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), val, get());

 // std::cout << "ADP_NODE::tr_stress_last " << label() << tr_value << "\n";
}
/*---------------------------------*/
hp_float_t ADP_NODE::tr_get( ) const { return ( tr_value );}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_avg(){
  assert(_order==2);

  _delta_expect = (_delta[2]+_delta[1])/2.0;
  tt_expect = tt_integrate_2_linear(_delta_expect);
}
/*---------------------------------*/
double ADP_NODE::tt_integrate_2_linear(double tr_){
  incomplete();
  hp_float_t delta = (_delta[1] + tr_)/2 * ( dT0() / tr_duration() -1 );
  double ret = get_aft_1() + delta; // RET
  assert (ret == ret);
  _debug+=2;

  trace6(( "ADP_NODE::tt_expect2_linear "+_c->short_label()).c_str(), get_tr(),
      get1(), ret, delta, tr_, tr_duration() ); 


  if (ret < 0 && _positive ){
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), Time_delta(), dT1(),
        _c->short_label().c_str(), tt_value, 
        _delta[2], _delta[1], tr_,
        order,
         get1(),
        ret);
//    assert(false);
  }
  _integrator = &ADP_NODE::tt_integrate_2_linear;
  return ret;
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_linear(){
  assert(_order==2);
  trace4(( "ADP_NODE::tt_expect2_linear " + _c->short_label()).c_str(), 
      tr_value, _delta[0], _delta[1], _delta[2] );
  trace3(( "ADP_NODE::tt_expect2_linear " + _c->short_label()).c_str(), Time_delta(), get1(), dT1() );
  // _delta_expect = fabs( (_delta[1]) + ( (_delta[1]) - (_delta[2])) * (hp_float_t) ((Time_delta() )/dT1()));
  //
  // expected tr for time0
  _delta_expect = ( _delta[1] + ( (_delta[1]) - (_delta[2])) * (hp_float_t) ((Time_delta() )/dT1()));
  _debug+=3;

  if(_delta_expect != _delta_expect) assert(false);
  if(_delta[1] != _delta[1]) assert(false);

  tt_expect = tt_integrate_2_linear(_delta_expect);

  if (tt_expect < 0 && _positive ){
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), Time_delta(), dT1(),
        _c->short_label().c_str(), tt_value, 
        _delta[2], _delta[1], _delta_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
  _corrector = NULL; // &ADP_NODE::tr_correct_1_exp; // HACK. ouch. move to ADP_EXP
  _integrator = &ADP_NODE::tt_integrate_2_linear;
}
/*---------------------------------*/
// 3 points denoising expect
void ADP_NODE::tr_expect_3_linear(){
  assert(_order==3);
  trace4(( "ADP_NODE::tt_expect3_linear " + _c->short_label()).c_str(), tr_value, _delta[0], _delta[1], _delta[2] );
  trace3(( "ADP_NODE::tt_expect3_linear " + _c->short_label()).c_str(), Time_delta(), get1(), dT1() );
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
        tt_value, tr_value3, _delta[2], _delta[1], _delta_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
}
/*---------------------------------*/
inline double square(double x){return x*x;}
/*---------------------------------*/

void ADP_NODE::tr_expect_2_square(){
  assert(_order==2);
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

  trace5(( "ADP_NODE::tt_integrate_2_exp "+_c->short_label()).c_str(),
      _delta[2], _delta[1], tr_, tt_expect, delta );

  assert(ret == ret);
  _integrator = &ADP_NODE::tt_integrate_2_linear2;
  return ret;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp(){
  assert(_order==3);
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
double ADP_NODE::tr_correct_3_exp(){
  trace0("ADP_NODE::tr_correct_3_exp");
        return (tr_value+_delta_expect)/2;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tr_correct_generic(){
  // return(18);
  trace0("ADP_NODE::tr_correct_generic");
  return (tr_value+_delta_expect)/2;
}
/*----------------------------------------------------------------------*/
double ADP_NODE::tt_integrate_3_exp(double tr_)
{
  assert(_order==3);
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
    _debug+=1000;

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
  assert(_order==3);
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

  trace6(( "ADP_NODE::tt_expect3_exp() "+_c->short_label()).c_str(),  Time0(), tr_value3, _delta[2], _delta[1], get1(), _delta_expect );
  assert( _delta_expect == _delta_expect );

  hp_float_t E = - c/alpha;
  hp_float_t delta = E * ( - exp( t3*alpha ) + exp(t2*alpha) )/ _c->_sim->_last_time ;
  hp_float_t delta_trapez = (_delta[1] + _delta_expect)/2 * Time_delta() / _c->_sim->_last_time;

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
  assert(_order==3);
  trace3(( "ADP_NODE::tt_expect3_quadratic() "+_c->short_label()).c_str(), tr_value3, _delta[2],  _delta[1] );
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

  hp_float_t diff = (_delta[1] + _delta_expect)/2 * Time_delta() / _c->_sim->_last_time;
  tt_expect = get() + diff;
  assert (tt_expect == tt_expect);
}
/*---------------------------------*/
double ADP_NODE::debug(){
  return _debug;
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_something(){
  assert(_order==2);
  _delta_expect = _delta[1] + (_delta[1] - _delta[2]) * dT0()/dT1();
  tt_expect = get1() + ( Time_delta() ) / _c->_sim->_last_time;
  trace2(( "ADP_NODE::tt_expect2_something "+_c->short_label()).c_str(), get1(), tt_expect );
}
/*---------------------------------*/
void ADP_NODE::tr_expect_1_const(){
  assert(_order==1);
  trace3(( "ADP_NODE::tt_expect_1_const() "+_c->short_label()).c_str(),
      tr_value, _delta[0], _delta[1] );
  trace3(( "ADP_NODE::tt_expect_1_const() "+_c->short_label()).c_str(),
      Time_delta(), _c->_sim->_time0, _c->_sim->_last_time );

  _delta_expect = _delta[1];
  _corrector = &ADP_NODE::tr_correct_1_const;
  tt_expect = tt_integrate_1_const(_delta_expect);


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
  _debug=3;
  double delta = ( _delta[1] + tr_ )/2 *  (dT0() / tr_duration() - 1) ;
//   double ret = _val_bef[1] + delta + tr_; // RET
  
  long double ret = get_aft_1() + delta; // RET

  trace5("ADP_NODE::tt_integrate_1_const", _val_bef[1], delta , tr_, ret, dT0() / tr_duration());

  if (ret < 0 && _positive){
    error(bDANGER, "* ADP_NODE::tt_integrate_const neg error step %i,"
        " Time0=%f (%f), %s, tt_value = %g, ( %g, %g) \n", 
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0, dT0(),
        _c->short_label().c_str(), tt_expect,
        _delta[0], _delta[1]);
//    assert(false);
     ret=0;
  }

  if (_delta[1] < 0 && tr_ < 0 ){
    _sign = -1;
  }

  _integrator = &ADP_NODE::tt_integrate_1_const;
  assert (!_positive || ret >=0);
  return(ret);
}
/*---------------------------------*/
double ADP_NODE::tr_correct_1_const(){
  trace0("ADP_NODE::tr_correct_1_const");
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
//  _val_bef[1] += diff;
void ADP_NODE::tt_commit_first( )
{
  assert (order()==0);
  _integrator = 0;
  _corrector = 0;
  trace2(("ADP_NODE::tt_commit_first() " + _c->short_label()).c_str(), get_tt(), get_tr());


  tt_first_time = _c->_sim->_Time0;
  tt_first_value = _val_bef[0];
  assert(_delta_expect != HUGE_VAL );
}
/*---------------------------------*/
void ADP_NODE::tt_commit( )
{
 //  called before apply...
 //  sets expected values for already guessed dT0

  if (order()==0)
    tt_commit_first();

  trace1(( "ADP_NODE::tt_commit "+_c->short_label()).c_str(), tt_value );
  trace6(("ADP_NODE::tt_commit " + _c->short_label()).c_str(),
      dT0(), dT1(),
      _c->_sim->get_tt_order(),
      _c->_sim->_Time0,
      _val_bef[0], _val_bef[1]
      );

  // sets _delta_expect and tt_expect
  tr_expect_();

  assert(_delta_expect != HUGE_VAL );
  assert(_delta_expect == _delta_expect );

  // this is before sweep....
  assert( tt_expect == tt_expect );
  assert( _c->_sim->_last_time >0 );

//  std::cout << " commit expect " << tr_value << " <= " << _delta_expect <<"\n";

  tr_value = _delta_expect; // FIXME only accd values into _delta[0]
  tt_value = tt_expect; // FIXME

  assert( tt_value == tt_value );
  assert( tt_value >=0 || !_positive );
  // tr_value will be printed as "tr"

  if (tt_expect < -1e-8 ){
    //positive?
    error(bDANGER, "* ADP_NODE::tt_commit neg error step %i, order %i, tt_expect=%e\n", \
        _c->_sim->tt_iteration_number(), _order,tt_expect);
  }
  trace6(("tt_values " + _c->short_label() + " ").c_str(),
      tr_value3, _delta[2], _delta[1], _delta_expect, _c->_sim->tt_iteration_number(), tt_expect - _val_bef[1]);
  trace6(("tt_values " + _c->short_label() + " ").c_str(),
      tt_value3, _val_bef[2], _val_bef[1], tt_expect, _c->_sim->tt_iteration_number(), _c->_sim->get_tt_order() );



}
/*---------------------------------*/
void ADP_NODE::tr_expect_( ){
  _integrator = 0;
  _corrector = 0;
  _order = _c->_sim->get_tt_order();

  trace2("ADP_NODE::tr_expect_", _order, _c->_sim->tt_iteration_number());
  assert(_order <= _c->_sim->tt_iteration_number());


  switch(_order){
    case 0:
      assert(false);
      break;
    case 1:
      tr_expect_1();
      break;
    case 2:
      tr_expect_2();
      if ( _delta[1]<0 && _delta[2]<0 ){
        trace3( "ADP_NODE::tr_expect_ order 2", _delta[2], _delta[1], _delta_expect );
      }

      break;
    case 3:
      // FIXME assert( tr_dd12 == ( _delta[1] - _delta[2] ) / dT1());
      tr_dd12 = ( _delta[1] - _delta[2] ) / dT1();
      tr_expect_3();
      if ( _delta[1]<0 && _delta[2]<0 && tr_value3<0 && _delta_expect>0) {
        error(bDANGER, "* ADP_NODE::tt_expect_ error step %i, Time0=%f, %s, tt_value = %g, ( %g, %g, %g, %g) tte: %g\n", \
            _c->_sim->tt_iteration_number(),
            dT0(),
            _c->short_label().c_str(), tt_value, tr_value, _delta[0], _delta[1], _delta[2], tt_expect);
        assert (_delta_expect <= 0);

      }
      break;
    default:
      assert(false);
  }
  trace5( "ADP_NODE::tr_expect_ done ", _delta[2], _delta[1], _delta_expect, _order, tt_expect);
  assert(_val_bef[0] >=0 || !_positive);
  assert(tt_expect >=0 || !_positive);
  assert(_delta_expect == _delta_expect);

//  return _delta_expect;
 // std::cout << "exp_ " << _delta[1] <<  " " <<  short_label() << " "  <<
 // tt_value  << " " << get1() << " order " << _order << " val1 " <<
 // _val_bef[1] <<
 //  "ttex " << tt_expect <<  "\n";
}
/*-----------------------------------------*/
int ADP_NODE::region() const{
        return _region;
}
/*-----------------------------------------*/
void ADP_NODE::tr_expect_1_exp(){
  // constant
  assert(_order==1);
  _sign=1;
  if(_delta[1] < 0) _sign=-1;
  trace3(( "ADP_NODE::tt_expect_1_exp() "+_c->short_label()).c_str(),
     tr_value, _delta[0], _delta[1] );
  trace3(( "ADP_NODE::tt_expect_1_exp() "+_c->short_label()).c_str(),
      Time_delta(), _c->_sim->_time0, _c->_sim->_last_time );

  _delta_expect = _delta[1];

  _corrector = &ADP_NODE::tr_correct_1_exp;
  tt_expect = tt_integrate_1_const(_delta_expect);

  _integrator = &ADP_NODE::tt_integrate_1_exp;
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_exp(){
  trace2("ADP_NODE::tr_expect_2_exp()", _delta[1], _delta[2]);
  double h = tr_duration();
  _debug+=1000;
  assert( order() == 2 );
  assert( _delta[1] == _delta[1] );
  assert( _delta[2] == _delta[2] );
  _sign = 1;
  if (_delta[1] <= 0 && _delta[2] < 0 ){
    _sign=-1;
  }

  // need total ordering.
  if ( !( ( _delta[2] <= _delta[1] && _delta[1] <= 0) 
       || ( _delta[2] >= _delta[1] && _delta[1] >= 0))){
//    _order--;
    trace2("ADP_NODE::tr_expect_2_exp no total ord linear...",  _delta[2], _delta[1]);
    _debug+=5000;
    tr_expect_2_linear();
    _corrector = &ADP_NODE::tr_correct_1_exp;
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
  trace0("ADP_NODE::tr_correct_1_exp");

  double new_delta; // return value. new value for tr_value.
  double h = tr_duration();

  //assert(_val_bef[1] == _val_bef[1]);

  long double a = _delta[1];
  long double b = tt_expect;
  long double c = tr_value;
  long double d = _val_bef[1];
  long double dT = dT0();

  trace4(("ADP_NODE::tr_correct_1_exp, _der_aft? " + label()).c_str(),
      _delta[1], tt_expect, tr_value, _val_bef[1] );

  //model value as (does it make sense?)
  /// t \mapsto K* ( 1- exp (tau*t) )+ d 
  // long double K = (b-d)/(c/a-1);
  // long double tau = log((c-a)/(b-d) + 1) / h;

  //_delta_expect = K *(- exp(tau* (dT0()+h) )  + exp ((tau*dT0())) );

  // a*(a - b - c + d)^(dT0/h)/(-b + d)^(dT0/h)
  //
  
  long double d_b = d-b;
  if (  fabs ( d_b )  < 1e-20 )  {
    trace6("ADP_NODE::tr_correct_1_exp ", _delta[1], tr_value, _val_bef[1],
        tt_expect, 1e-13, fabs ( d_b / (fabs(d)+fabs(b))));
    _integrator=&ADP_NODE::tt_integrate_1_const;
    return(tr_value + _delta[1])/2;
  }
  if ( ( fabs ( d_b ) / (fabs(d)+fabs(b) ) < 1e-13 ) ) {
    trace6("ADP_NODE::tr_correct_1_exp ", _delta[1], tr_value, _val_bef[1],
        tt_expect, 1e-13, fabs ( d_b / (fabs(d)+fabs(b))));
    _integrator=&ADP_NODE::tt_integrate_1_const;
    return(tr_value + _delta[1])/2;
  }
  long double B = (1 + (a - c)/(d_b));
  long double E = ( dT/h );

  if (B<0){
    B=0;
    untested();
  }
  long double P = pow(B,E);

//  new_delta = (double)( a*pow((a - c + d-b)/(d-b),(dT/h)) );
  new_delta = (double)( a*P  );

  trace6(("ADP_NODE::tr_correct_1_exp, " + label()).c_str(), \
      a, d_b, P, dT0(), B, E );
  trace6(("ADP_NODE::tr_correct_1_exp, " + label()).c_str(), tt_expect,
      _val_bef[1], tt_value, _delta[1], tr_value, new_delta);
  trace2( "ADP_NODE::tr_correct_1_exp " , _c->_sim->tt_iteration_number(), b-a);

  assert(is_number(B));
  assert(is_number(E));
  assert(new_delta == new_delta);
  assert(new_delta * _delta[1] >= 0);

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
  trace4("ADP_NODE::tt_integrate_2_exp", tr_duration(), dT0(), _val_bef[1] +
      _delta[1], _val_bef[1]);
  _debug=1;
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

  trace6(( "ADP_NODE::tt_integrate_1_exp "+_c->short_label()).c_str(),
      _delta[2], _delta[1], tr_, tt_expect, delta, ret );


  assert(ret == ret);

  assert (_sign*tr_ >= 0 );
  _integrator = &ADP_NODE::tt_integrate_1_exp;

  if( _positive && ret < 0){
        ret=0;
  }

  assert(tt_expect >=0 || !_positive);
  assert(ret >=0 || !_positive);

  return double(ret);
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
