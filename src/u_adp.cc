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

/*----------------------------------------------------------------------------*/
void ADP_NODE::init(){
  tr_value=(0.);
  tr_value0=(0.);
  tr_value1=(0.);
  tr_value2=(0.);
  tt_value=0.;
  tt_value0=(0.);
  tt_value1=(0.);
  _positive = true;
  name=std::string("");
  _c=(NULL); 
}
/*----------------------------------------------------------------------------*/
double ADP_NODE::tr_duration()const{ return _c->_sim->_last_time; }
/*----------------------------------------------------------------------------*/
double ADP_NODE::tt_review( ) {
  trace1("ADP_NODE::tt_review", _c->_sim->tt_iteration_number());
  assert(tr_expect == tr_expect);
  assert(tr_expect != HUGE_VAL );
  hp_float_t myreltol = OPT::adpreltol;
  hp_float_t myabstol = OPT::adpabstol;

  // FIXME: implement abs err!
  _rel_tr_err = fabs (tr_value - tr_expect) / max( fabs(tr_value) , fabs(tr_expect));
  _rel_tt_err = fabs (tt_value - tt_expect) / (fabs(tt_value) + fabs(tt_expect));
  _abs_tr_err = fabs (tr_value - tr_expect);

  if( myreltol == 0 ) return 0;

  trace2("ADP_NODE::tt_review", myreltol, inf);
  if( myreltol == inf && myabstol == inf ) return inf;
  if( myreltol == 0 && myabstol == 0 ) return 0;

// baut die zeit nicht ein!
// FIXME: _order.
  double wanted_dT = dT0() * sqrt( myreltol / _rel_tr_err ) ;
  if( wanted_dT < 0  || wanted_dT != wanted_dT) {
    error(bDANGER, "wanted_dT %f\n", wanted_dT);
  }

  if( _rel_tr_err != _rel_tr_err ) wanted_dT = NEVER;
  if( !(_rel_tr_err == _rel_tr_err) ) wanted_dT = NEVER;

  trace3( "ADP_NODE::tt_review ", _c->_sim->_Time0 , wanted_dT , myreltol );

  assert(wanted_dT >= 0 );

  _wdT = wanted_dT; // * ( _order+1) / 3;
  return _wdT;
}
/*----------------------------------------------------------------------------*/
TIME_PAIR ADP_NODE::tt_preview( ) {
  TIME_PAIR _time_pre;
    _time_pre.reset();
    return _time_pre;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::reset(){
  trace1( "ADP_NODE::reset()", _c->_sim->_Time0 );
  tr_value = 0; // tt_value1 = 0; tt_value0 =0;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_accept_first( )
{
  trace2(("ADP_NODE::tt_accept_first" + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number());

  assert( _c->_sim->get_tt_order() == 0 );

  assert(0  == _c->_sim->_dT0);
  tt_value2 = tt_value1;
  tt_value1 = tt_value0;  

  tr_value1 = tr_value0;
  tr_value0 = tr_value;
//  tr_value = 0;

  assert(tt_value0 >= 0 );
  assert(tt_value0 >= tt_value1);

  tr_value0 = tr_value;

 // FIXME tt_value??

  tt_value += tr_value0;
}
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_accept()
{
  //_c->_sim->update_tt_order();

  trace3(("ADP_NODE::tt_accept() " + _c->short_label()).c_str(),
      tr_value,_c->_sim->tt_iteration_number(), _c->_sim->get_tt_order());
  assert(tr_value0 == tr_value0);
  assert(_c->_sim->_last_time >0 );

  int order=_c->_sim->get_tt_order();

  // untested();
  if (order != _order){
    trace2("ADP_NODE::tt_accept", order, _order);
  }

  switch( order ){
    case 0:
      return tt_accept_first();
      // tr_dd12 = ( tr_value0 - tr_value1 ) / dT0();
      break;
    case 1:
      tr_dd12 = ( tr_value0 - tr_value1 ) / dT0();
      tr_value0 = tr_value;

      // if ( OPT::adpcorrect )
      // tr_value0 = (tr_value + tr_expect)/2;
      // FIXME tt_value??

      break;
    case 2:
      tr_dd23 = tr_dd12;
      tr_dd123 = (tr_dd12 - tr_dd23)/( dT1() + dT0() ); 
      tr_value0 = tr_value;

      // only output value.
      break;
    case 3:
      tr_value0 = tr_value;

      // if ( OPT::adpcorrect )
      //  tr_value0 = (tr_value + tr_expect)/2;
      // FIXME tt_value??
      //

      // only output value.
      break;
    default:
      assert(false);
  }

  if (_integration_used){
    trace0("reintegration");
    _debug=1000;
    (this->*_integration_used)(tr_value0);
  }

  tt_value += tr_value0; // (printed) output value

  assert (tr_dd123 == tr_dd123 );
  assert (tr_dd123 != -HUGE_VAL );
  assert (tr_dd123 != HUGE_VAL );

  trace2(("ADP_NODE::tt_accept" + _c->short_label()).c_str(), tt_value, tr_value);


  trace1(("ADP_NODE::tt_accept done " + _c->short_label()).c_str(), get_tt());
}
/*----------------------------------------------------------------------------*/
// accept, print, advance.
/*----------------------------------------------------------------------------*/
void ADP_NODE::tt_advance(){
  trace1(("ADP_NODE::tt_advance() " + _c->short_label()).c_str(), get_tt());

  // this is no logner valid, since tr_value has been corrected.
  //  assert(tr_value==tr_value0);

  tr_value3=tr_value2;
  tr_value2=tr_value1;
  tr_value1=tr_value0;

  tr_value0=0;
  tr_value =0;

  tt_value3 = tt_value2;  
  tt_value2 = tt_value1;  
  tt_value1 = tt_value0;

  tt_value0 = tt_expect;
  tt_value0 = tt_value;
  trace2(("ADP_NODE::tt_advance done " + _c->short_label()).c_str(), get_tt(), get_tr());
}
/*----------------------------------------------------------------------------*/
//  tt_value1 += diff;
void ADP_NODE::tt_commit_first( )
{
  trace2(("ADP_NODE::tt_commit_first() " + _c->short_label()).c_str(), get_tt(), get_tr());

//  eigentlich commit mit order 0 ??
  // assert(_c->_sim->_Time0 == 0 ); cont!
  assert(_c->_sim->get_tt_order() == 0 );

  tt_first_time = _c->_sim->_Time0;
  tt_first_value = tt_value0;
  assert(tr_expect != HUGE_VAL );
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
  trace4(( "ADP_NODE::tt_expect2_linear " + _c->short_label()).c_str(), tr_value, tr_value0, tr_value1, tr_value2 );
  trace3(( "ADP_NODE::tt_expect2_linear " + _c->short_label()).c_str(), Time_delta(), get1(), dT1() );
  // tr_expect = fabs( (tr_value1) + ( (tr_value1) - (tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));
  //
  // expected tr for time0
  tr_expect = ( (tr_value1) + ( (tr_value1) - (tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));

  if(tr_expect != tr_expect) assert(false);
  if(tr_value1 != tr_value1) assert(false);

  hp_float_t delta = (tr_value1 + tr_expect)/2 * Time_delta() / tr_duration();
  tt_expect = get1() + delta;
  assert (tt_expect == tt_expect);

  trace6(( "ADP_NODE::tt_expect2_linear "+_c->short_label()).c_str(), get_tr(), get1(), tt_expect, delta, tr_expect, tr_duration() );


  if (tt_expect < 0 && _positive ){
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect2_linear neg step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g; %g) %i tt: %f, expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), Time_delta(), dT1(),
        _c->short_label().c_str(), tt_value, 
        tr_value2, tr_value1, tr_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
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

  tr_expect = c0 + c1 * t3;

  if(tr_expect != tr_expect) tr_expect=0;
  if(tr_value1 != tr_value1) assert(false);

  hp_float_t diff = (tr_value1 + tr_expect)/2 * Time_delta() / tr_duration();
  tt_expect = get1() + diff;
  assert (tt_expect == tt_expect);

  trace6(( "ADP_NODE::tt_expect3_linear "+_c->short_label()).c_str(), get_tr(), get1(), tt_expect, diff, tr_expect, tr_duration() );


  if (tt_expect < 0 ){
    //positive?
    int order = _c->_sim->get_tt_order();
    error(bDANGER, "* ADP_NODE::tt_expect3_linear neg error step %i, Time0=%f (%f,%f,%f), %s, tt_value = %g, ( %g, %g, %g)-> %g --  %i tt: %f, expecting %f\n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0,   dT0(), dT1(), dT2(),
        _c->short_label().c_str(),
        tt_value, tr_value3, tr_value2, tr_value1, tr_expect,
        order,
         get1(),
        tt_expect);
//    assert(false);
  }
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_square(){
  assert(_order==2);
  tr_expect = square( sqrt(tr_value1) + ( sqrt(tr_value1) - sqrt(tr_value2)) * (hp_float_t) ((Time_delta() )/dT1()));
}
/*---------------------------------*/
void ADP_NODE::tr_expect_2_exp(){
  assert(_order==2);
  double sign=1;
  if (tr_value1 < 0 && tr_value2 < 0 ){
        sign=-1;
  }

  if ( !( (tr_value1 < 0 && tr_value2 <0) 
        || ( tr_value1 > 0 && tr_value2 >0))){
    _order--;
    return tr_expect_2_linear();
  }

  //hp_float_t t2 = tr_duration()/2;
  //hp_float_t t1 = t2+dT1();

  double ln2 = log(sign*tr_value2);
  double ln1 = log(sign*tr_value1);

  double ln0 = ln1 + dT0() * (ln1-ln2)/dT1();

  tr_expect= sign * exp(ln0);

  _sign=sign;
  tt_integrate_2_exp(tr_expect);
  tt_expect=tt_value;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tt_integrate_2_exp(double tr_) {
  hp_float_t h = tr_duration();
  
  double ln1 = log(_sign*tr_value1/h);
  double ln0 = log(_sign*tr_/h);

  //int_{h/2}^{dT0} exp( ln1 + v/dT0 * (ln0 - ln1) ) dv

  double delta= dT0()/(ln0-ln1) * ( _sign*tr_/h - exp( ln1 + (h/dT0()) *(ln0-ln1) ) );

  if (delta != delta || delta==-inf || delta==inf){ 
    delta= (tr_ + tr_value1) * (dT0()-h) / h;
    incomplete(); // call another thiongh.
  }

  assert(delta != -inf);
  assert(delta != inf);

  tt_expect = get1() + tr_value1 + _sign * delta;

  trace5(( "ADP_NODE::tt_expect2_exp "+_c->short_label()).c_str(), tr_value2, tr_value1, tr_expect, tt_expect, delta );
  assert(tt_expect==tt_expect);
  _integration_used=&ADP_NODE::tt_integrate_2_exp;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp(){

  assert(_order==3);
  hp_float_t sign=1;

  if ( ( tr_value2 > 0 && tr_value1 > 0 && tr_value1 < tr_value2 && tr_value3 < tr_value2 )
      || ( tr_value2 < 0 && tr_value1 < 0 && tr_value1 > tr_value2 && tr_value3 > tr_value2 ) ){
    _order--;
    _debug+=8;
    return tr_expect_2_exp();
  }
        
  if (tr_value1 < 0 && tr_value2 < 0 && tr_value3 <0){
        sign=-1;
  } else if ( ! (tr_value1 > 0 && tr_value2 > 0 && tr_value3 >0) ){
     _debug+=2;
     return tr_expect_3_linear();       
  }

  if ( tr_value3 > tr_value2 && tr_value2 > tr_value1 ){
    _debug+=4;
  }


  double ln3= log(sign*tr_value3/tr_duration());
  double ln2= log(sign*tr_value2/tr_duration());
  double ln1= log(sign*tr_value1/tr_duration());

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

  tr_expect = sign*exp(ln0);


  // int_0^x exp( ln1 + t/dT0*(ln0-ln1) ) @ x=dT0
  tr_expect *= tr_duration();
  _sign=sign;
  tt_integrate_2_exp(tr_expect); // 3 doesnt make sense yet?
  tt_expect = tt_value;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tt_integrate_3_exp(double tr_)
{
  assert(_order==3);
  hp_float_t sign=1;
  double h=tr_duration();

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

  //double ln3= log(_sign*tr_value3/tr_duration());
  //double ln2= log(_sign*tr_value2/tr_duration());
  double ln1= log(_sign*tr_value1/tr_duration());




  double ln0 = log(_sign*tr_/tr_duration());

  hp_float_t delta = _sign* (exp( ln1 + (ln0-ln1) ) - (_sign*tr_value1/tr_duration()) )/(ln0-ln1)* dT0();

  if (delta != delta || delta==inf || delta==-inf){

    // fixme: cll anothert integrator.
    hp_float_t delta_trapez = (tr_value1 + tr_)/2 * ( dT0()-h) / tr_duration() ;
    trace2("ADP_NODE::tt_expect3_exp preferring delta_trapez", delta_trapez, delta);
    tt_value = get1() + delta_trapez;

    return;
  }

  tt_value = get1() + delta * _sign;

  _integration_used = &ADP_NODE::tt_integrate_3_exp;
}
/*----------------------------------------------------------------------*/
void ADP_NODE::tr_expect_3_exp_fit(){
  assert(_order==3);
  hp_float_t sign=1;

  if ( ( tr_value2 > 0 && tr_value1 > 0 && tr_value1 < tr_value2 && tr_value3 < tr_value2 )
      || ( tr_value2 < 0 && tr_value1 < 0 && tr_value1 > tr_value2 && tr_value3 > tr_value2 ) ){
    _order--;
    _debug+=8;
    return tr_expect_2_exp();
  }
        
  if (tr_value1 < 0 && tr_value2 < 0 && tr_value3 <0){
        sign=-1;
  } else if ( ! (tr_value1 > 0 && tr_value2 > 0 && tr_value3 >0) ){
     _debug+=2;
     return tr_expect_3_linear();       
  }

  if ( tr_value3 > tr_value2 && tr_value2 > tr_value1 ){
    _debug+=4;
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

  tr_expect = sign*c * exp(alpha*t3);

  trace6(( "ADP_NODE::tt_expect3_exp() "+_c->short_label()).c_str(),  Time0(), tr_value3, tr_value2, tr_value1, get1(), tr_expect );
  assert( tr_expect == tr_expect );

  hp_float_t E = - c/alpha;
  hp_float_t delta = E * ( - exp( t3*alpha ) + exp(t2*alpha) )/ _c->_sim->_last_time ;
  hp_float_t delta_trapez = (tr_value1 + tr_expect)/2 * Time_delta() / _c->_sim->_last_time;

  trace4("ADP_NODE::tt_expect3_exp", delta, delta_trapez, tt_value1, get() );
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
         tr_value3, tr_value2, tr_value1, tr_expect,
        tt_expect
        );
//    assert(false);
  }

  //sanitycheck
  if ( f0<0 && f1<0 && f2 <0)
    assert (tr_expect <= 0);

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


  tr_expect = -(((tr_value2 - tr_value3)/dT2() - (tr_value1 - tr_value2)/dT1())*(dT0() + dT1())/(dT1() + dT2()) -
    (tr_value2 - tr_value3)/dT2())*(dT0() + dT1() + dT2()) + tr_value3;

  hp_float_t diff = (tr_value1 + tr_expect)/2 * Time_delta() / _c->_sim->_last_time;
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
  tr_expect = tr_value1 + (tr_value1 - tr_value2) * dT0()/dT1();
  tt_expect = get1() + ( Time_delta() ) / _c->_sim->_last_time;
  trace2(( "ADP_NODE::tt_expect2_something "+_c->short_label()).c_str(), get1(), tt_expect );
}
/*---------------------------------*/
void ADP_NODE::tr_expect_1_const(){
  assert(_order==1);
  trace3(( "ADP_NODE::tt_expect1_const() "+_c->short_label()).c_str(), tr_value, tr_value0, tr_value1 );
  trace3(( "ADP_NODE::tt_expect1_const() "+_c->short_label()).c_str(), Time_delta(), _c->_sim->_time0, _c->_sim->_last_time );
  tr_expect = tr_value1;
  double    delta =  tr_value1 *  dT0() / tr_duration();
  tt_expect = get1()+delta; 

  if (tt_expect < 0 && _positive){
    error(bDANGER, "* ADP_NODE::tt_expect1_const error step %i, Time0=%f (%f), %s, tt_value = %g, ( %g, %g, %g, %g) \n", \
        _c->_sim->tt_iteration_number(),
        _c->_sim->_Time0, dT0(),
        _c->short_label().c_str(), tt_expect, tr_value, tr_value0, tr_value1, tr_value2);
//    assert(false);
  }
}
/*---------------------------------*/

void ADP_NODE::tt_integrate_( double ){
// take order and tr_value
// integrate things. write to tt_value.

}
/*---------------------------------*/
/*---------------------------------*/
void ADP_NODE::tt_commit( )
{
  trace1(( "ADP_NODE::tt_commit "+_c->short_label()).c_str(), tt_value );

  trace6(("ADP_NODE::tt_commit " + _c->short_label()).c_str(),
      dT0(), dT1(),
      _c->_sim->get_tt_order(),
      _c->_sim->_Time0,
      tt_value0, tt_value1
      );


  assert( tr_expect != HUGE_VAL );
  assert( tr_expect == tr_expect );

  tr_expect_();

  assert(tr_expect != HUGE_VAL );
  assert(tr_expect == tr_expect);

  // this is before sweep....
  assert( tr_expect == tr_expect );
  assert( tt_expect == tt_expect );
  assert( _c->_sim->_last_time >0 );

  tr_value0 = tr_value = tr_expect;
  tt_value0 = tt_value = tt_expect;
  // tr_value will be printed as "tr"

  if (tt_expect < -1e-8 ){
    //positive?
    error(bDANGER, "* ADP_NODE::tt_commit neg error step %i, order %i, tt_expect=%e\n", \
        _c->_sim->tt_iteration_number(), _order,tt_expect);
  }
  trace6(("tt_values " + _c->short_label() + " ").c_str(),
      tr_value3, tr_value2, tr_value1, tr_expect, _c->_sim->tt_iteration_number(), tt_expect - tt_value1);
  trace6(("tt_values " + _c->short_label() + " ").c_str(),
      tt_value3, tt_value2, tt_value1, tt_expect, _c->_sim->tt_iteration_number(), _c->_sim->get_tt_order() );



}
/*---------------------------------*/
void ADP_NODE::tr_expect_( ){
  _debug=0;
  _integration_used=0;
  _order = _c->_sim->get_tt_order();

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
        trace3( "ADP_NODE::tr_expect_ order 2", tr_value2, tr_value1, tr_expect );
      }

      break;
    case 3:
      // FIXME assert( tr_dd12 == ( tr_value1 - tr_value2 ) / dT1());
      tr_dd12 = ( tr_value1 - tr_value2 ) / dT1();
      tr_expect_3();
      if ( tr_value1<0 && tr_value2<0 && tr_value3<0 && tr_expect>0) {
        error(bDANGER, "* ADP_NODE::tt_expect_ error step %i, Time0=%f, %s, tt_value = %g, ( %g, %g, %g, %g) tte: %g\n", \
            _c->_sim->tt_iteration_number(),
            dT0(),
            _c->short_label().c_str(), tt_value, tr_value, tr_value0, tr_value1, tr_value2, tt_expect);
        assert (tr_expect <= 0);

      }
      break;
    default:
      assert(false);
  }
}
