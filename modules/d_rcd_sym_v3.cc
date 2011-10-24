/* vim:ts=8:sw=2:et:
 *
 * (c)2010 felix salfelder
 * nonGPL ?
 */


// die paperversion

#include "e_aux.h"
#include "e_storag.h"
// #include "d_mos_base.h"

#include "globals.h"
#include "e_elemnt.h"
#include "e_adp.h"
#include "d_rcd_sym_v3.h"

/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_stress_apply( COMPONENT*  ) const
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V3::tr_stress()
{
  unreachable(); // obsolet....

  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);
  assert(c->sdp());

  assert(false);

}
///*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_RCD_DISPATCHER
{ 
  static DEV_BUILT_IN_RCD_SYM_V3 p4d;
  static MODEL_BUILT_IN_RCD_SYM_V3 p4(&p4d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d3(&model_dispatcher, "rcdsym_v3|rcdsym", &p4);
}
///*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_expand(  COMPONENT* c) const
{
  DEV_BUILT_IN_RCD_SYM_V3* d = dynamic_cast<DEV_BUILT_IN_RCD_SYM_V3*>(c);
  assert(d);
  // d->expand();
  
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD_SYM_V3::dev_type()const
{
  return "rcd";
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V3::MODEL_BUILT_IN_RCD_SYM_V3(const BASE_SUBCKT* p) 
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  // uref=1; 
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V3::MODEL_BUILT_IN_RCD_SYM_V3(const MODEL_BUILT_IN_RCD_SYM_V3& p)  
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V3::expand()
{
  assert(_n);
  assert(common());
  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);
  assert(c->sdp());
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(c->sdp());
  assert(s);

  trace0("DEV_BUILT_IN_RCD_SYM_V3::expand()");

  if (_sim->is_first_expand()) {
    precalc_first();
    precalc_last();
    if (!(_n[n_ic].n_())) {
      if (false) {
        _n[n_ic] = _n[n_b];
      }else{
        _n[n_ic].new_model_node("." + long_label() + ".ic", this);
      }
    }else{
      if (false) {
        assert(_n[n_ic] == _n[n_b]);
      }else{
        //_n[n_ic].new_model_node("ic." + long_label(), this);
      }
    }
  }
}
/*--------------------------------------------------------------------------*/

// _SYM ??
double MODEL_BUILT_IN_RCD_SYM_V3::__tau(double uin, const COMMON_COMPONENT* cc)const
{
  double tau_e = __Re( uin, cc);
  double tau_c = __Rc( uin, cc);

  //   te tc/ (tc+te) 

  if (tau_c < tau_e){
    return tau_c  / ( tau_c/tau_e +1 );
  } else{
    return tau_e  / ( tau_e/tau_c +1 );
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress( const COMPONENT* brh) const
{
  const MODEL_BUILT_IN_RCD_SYM_V3* m = this;
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(c->common());

  double fill = c->_Ccgfill->get_total();
  double uin = c->involts();

  unreachable();
  trace2("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress ", fill, uin );
  trace2("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress ", c->involts(), iteration_number() );
  assert (fill>=E_min);
  if (fill >= 1.0 ){
    error(bDANGER, " RCD_V3 %s fill %E too big\n", brh->long_label().c_str(), fill );
    fill = 1;
  }

  if( m->positive ) {
    if ( c->_Ccgfill->get_total() < 0 ){
      trace1(("DEV_BUILT_IN_RCD_SYM_V3::tr_stress fill is negative: " +
            short_label()).c_str() ,  c->_Ccgfill->get_total() );
    }

    // FIXME: eine ebene hoeher ?
    if (  c->involts() < -2e-1 ){
      error( bDANGER, "DEV_BUILT_IN_RCD_SYM_V3::tr_stress input %s is negative: ",
          short_label().c_str(), c->involts() );
      assert (false );
    }
  }
  assert(!m->use_net());

  //double  fill = _n[n_ic].v0();
  assert (fill==fill);
  assert (uin==uin);

  double h = _sim->_dt0;

  double tau_e_here = m->__Re( uin, cc);
  double tau_c_here = m->__Rc( uin, cc);

  double uend = tau_c_here / ( tau_e_here + tau_c_here );
  double tau = __tau(uin, cc);

  double newfill;
  switch(_sim->_stepno){ incomplete();
    case 0:
    case 1:
    default:
      newfill = (fill - uend) * exp(-h/tau) + uend;
  }
  if( newfill <= 0 ){
    untested();
    newfill = 0;
  }

  trace4("MODEL_BUILT_IN_RCD_SYM_V3::tr_stress ", tau_c_here, tau_e_here, _sim->_Time0, _sim->_time0 );
  trace6("MODEL_BUILT_IN_RCD_SYM_V3::tr_stress ", fill, h, tau, newfill, uin, uend );

  c->_Ccgfill->tr_add(newfill-fill); // ERROR?
  assert(newfill-fill < 1 );

  trace4("DEV_BUILT_IN_RCD_SYM_V3::tr_stress ", fill, h, tau, (newfill-fill)/h );

  if(newfill > 1.000001){
    error(bDANGER, ("* RCD_V3 %f too big\n" + long_label() ).c_str() , newfill );
    newfill=1;
  }
  assert(newfill==newfill);
  assert(h > 0);
  assert(is_number(tau));
  assert(is_number(uend));
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last( long double E, ADP_NODE* _c,
    COMPONENT* dd ) const
{
  // ADP_NODE_UDC* udc= dynamic_cast<ADP_NODE_UDC*>(a);
  //const DEV_BUILT_IN_RCD* d = static_cast<const DEV_BUILT_IN_RCD*>(dd);
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(dd->common());
  const MODEL_BUILT_IN_RCD* m =   static_cast<const MODEL_BUILT_IN_RCD*>(this);
  assert(m);
  assert(is_number(_c->tt()));
  double E_old = _c->tt();


  if (_c->tr()==-inf) _c->tr() = ( _c->tr_lo+_c->tr_hi)/2.0;
  if (!is_number(_c->tr())) _c->tr() = (_c->tr_lo+_c->tr_hi)/2.0;

  long double uin_eff=_c->tr(); // 0 == current estimate

  trace3(("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last " +
        _c->label()).c_str(), E_old, tt_iteration_number(), uin_eff);

  assert(E_old<E_max);
  assert(E<=E_max);

  if (E>1) {
    trace0("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last aligned E");
    untested();
    E=1;
  }
  if (uin_eff < U_min && positive){
    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last %s tr() %LE"
       " bad\n", _c->long_label().c_str(), uin_eff );
    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last lo %E hi %E"
       " bad\n", _c->tr_lo , _c->tr_hi );
    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last history tr %E tr1 %E tr2 %E\n",
        _c->tr(), _c->tr1(), _c->tr2());
    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last time %E %E\n",_sim->_time0, _sim->_Time0);
    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last trstep %i ttstep "
       "%i\n",_sim->iteration_number(), _sim->tt_iteration_number());
    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last order %i\n", _c->order());
  }
  if (uin_eff < 0.0 && positive){
    //assert(false);// doesnt make sense to go on
    _c->set_order(0);
    uin_eff=max( (uin_eff + _c->tr1())/2 , 0.0L);
  }

  long double E_high; 
  long double E_low; 
  long double uin_high = max ( uin_eff + OPT::abstol, uin_eff * (1+OPT::reltol) );
  long double uin_low  = min ( uin_eff - OPT::abstol, uin_eff * (1-OPT::reltol) );
  if( positive ) uin_low = max(uin_low,0.0L);
  assert( !positive || uin_high >= 0 );
  assert (uin_high>=uin_low);

  E_high = cc->__step( uin_high, E_old, CKT_BASE::_sim->_last_time );
  E_low  = cc->__step( uin_low,  E_old, CKT_BASE::_sim->_last_time ); 

  assert (E_low>=0);

  if(double(E_high - E_low) < -1e-20 ){
    error(bDANGER,"MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last uin_high=%LE uin_low=%LE deltaE= %LE; %LE > %LE, diff %LE\n",
        uin_high, uin_low, E_high - E_low, E_high, E_low, E_high-E_low );
    untested();
//        throw(Exception("something wrong with order"));
  }

  bool linear_inversion=false;

  if ( E_low <= E && E <= E_high ) {
    // invert linearly
    uin_eff =uin_low + (uin_high - uin_low) * ( E -E_low)/ (E_high-E_low);
    trace5("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last linv", 1-E_low, 1-E, E_high-E_low, uin_low, uin_high );
    if (! (is_number(uin_eff)) ){
      uin_eff = (uin_high+uin_low)/2; // stupid fallback
      untested2( " nonumber ", uin_high-uin_low, uin_eff );
    }

    if ( (uin_eff <= uin_high) && (uin_low <= uin_eff ) ){
      linear_inversion=true;
    } else {
      assert(false);
    }
  }

  if (!linear_inversion){
    try{
      uin_eff = cc->__uin_iter(uin_eff, E_old, (double)E, _c->tr_lo, _c->tr_hi, dd );
    } catch (Exception &e) {
      error(bDANGER, "Exception in %s\n", long_label().c_str());
      throw(e);
    }
    trace3("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last iteration -> ", E_old,
        E, uin_eff);
  }

  // sanitycheck (monotonicity)
  long double E_test=cc->__step( uin_eff, E_old, CKT_BASE::_sim->_last_time );
  double trvorher= _c->tr();
  if(positive) trvorher=max(0.0,trvorher);
  long double E_vorher=cc->__step( trvorher , E_old, CKT_BASE::_sim->_last_time );
  if (fabs(E_test - E) < fabs(E_vorher-E)){
    trace6("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last new uin better ",
        uin_eff, _c->tr(), E_test - E, E_vorher-E,1-E , linear_inversion);
    trace5("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last                ",
        uin_eff, _c->tr(), E_test - E, E_vorher-E,1-E );
  }else{
    trace3("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last new uin not better ",
        uin_eff, _c->tr(), linear_inversion);
    if ( (E_test - E) * (E_vorher-E) < 0 ){
      uin_eff = (_c->tr() + uin_eff)/2.0;
      untested();
    }
    _c->set_order(0);
  }

  if ( linear_inversion && ((E_low > E_test) || ( E_test > E_high) )){
    trace3("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last monotonicity check ",
        uin_eff-uin_low, uin_high-uin_eff, uin_eff );
    assert( uin_eff<=uin_high && uin_low<=uin_eff);
    trace5("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last ", 1-E, 1-E_test,
        E_test-E_low, E_high-E_test, linear_inversion );
    assert( E_test <= E_high && E_low <= E_test); 
  }

  assert(uin_eff < U_max || positive);
  if (uin_eff <= 0.0 && positive){
//    error(bDANGER, "MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last %s tr() %LE bad, l%i\n",
//        _c->long_label().c_str(), uin_eff, linear_inversion );
    trace3("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last history ",_c->tr(),
        _c->tr1(), _c->tr2());

    _c->set_order(0);
    uin_eff=0;

  }

  if (uin_eff == inf){
    E_low=E;
    E_high=E;
    assert(false);
  }

  uin_high = max ( uin_eff + OPT::abstol, uin_eff * (1 + OPT::reltol) );
  uin_low  = min ( uin_eff - OPT::abstol, uin_eff * (1-OPT::reltol) );
  trace3(("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last E" + _c->label()).c_str(),
      uin_low, uin_eff, uin_high);
  if (m->positive)
    uin_low = max(0.0L, uin_low);

  E_high = cc->__step( uin_high, E_old, CKT_BASE::_sim->_last_time  );
  E_low  = cc->__step( uin_low,  E_old, CKT_BASE::_sim->_last_time  ); 
  trace6(("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last E" + _c->label()).c_str(),
      uin_eff,  E-E_low, E_high - E_low, E, 1-E, E_high-E );
  trace3(("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last E" + _c->label()).c_str(), 
      E-E_low , E_high-E, linear_inversion  );

  if( ( E_old < E_high ) && ( E_low <= E_old ))
    _c->set_order(0);

  if ((double)E_high-(double)E_low < -1e-20){
    error( bDANGER, "COMMON_BUILT_IN_RCD:: sanitycheck ( %LE < %LE )", E_high, E_low);
    untested();
  }

  if (linear_inversion)  _c->set_order(0);
  assert(uin_eff >= 0.0 || !positive );
  //assert ( uin_eff == _c->get_tr());
  //
  //
  //
  assert(is_number(E_high-E_low));
  assert(E_low <= E || double(E)==1.0 || double(E_high)==1.0 || !linear_inversion);

  if(E > E_high && E!=1){
    untested1("COMMON_BUILT_IN_RCD::", linear_inversion);
    error( bDANGER, "COMMON_BUILT_IN_RCD:: sanitycheck ( %LE =E >  E_high=%LE ) del %LE\n", E_high, E, E_high-E);
  }

  _c->set_tr_noise ((double)E_high-(double)E_low);
  _c->set_tr((double)uin_eff);
  
  if ( CKT_BASE::_sim->tt_iteration_number()>1 ){
    if((_c->tr()-_c->tr1())*(_c->tr1()-_c->tr2())<=0 ){
      _c->set_order(0);
    }
  }

  trace1("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress_last done", _c->get_tr_noise());

#ifdef DNFALSE
  //old sanitycheck
  long double u_oben  = max ( uin + OPT::abstol, uin * (1+OPT::reltol) );
  long double u_unten = min ( uin - OPT::abstol, uin * (1-OPT::reltol) );
  if(m->positive) u_unten=max(u_unten,0.0L);
  long double E_hier  = cc->__step( uin , E_old, BASE_SUBCKT::_sim->_last_time );
  long double E_oben  = cc->__step( u_oben , E_old, BASE_SUBCKT::_sim->_last_time );
  long double E_unten = cc->__step( u_unten, E_old, BASE_SUBCKT::_sim->_last_time );

  trace3("COMMON_BUILT_IN_RCD:: sanitycheck", uin, u_oben ,u_unten );
  trace6("COMMON_BUILT_IN_RCD:: sanitycheck", E,E_hier, E - 1, E_oben - E,E- E_unten, E_oben );
  trace4("COMMON_BUILT_IN_RCD:: sanitycheck", E,E_hier, E_oben,E_unten );

  if (E_oben < E_hier){
    error( bDANGER, "COMMON_BUILT_IN_RCD: sanitycheck ( %LE < %LE )", E_oben, E_hier);
    assert((double) E_oben <= (double) E_hier);
  }
  assert(E_unten <= E_hier);

  assert((double)E_oben >= (double)E_unten);

  assert(double(E_oben) >= E);
  // assert(E_unten <= E);

#endif


}


/*--------------------------------------------------------------------------*/
// precalc doesnt know device!!
//void MODEL_BUILT_IN_RCD_SYM_V3::do_tt_prepare( COMPONENT* brh) const{
//  const DEV_BUILT_IN_RCD* c = prechecked_cast<const DEV_BUILT_IN_RCD*>(brh);
//  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(c->common());
//
//  trace1( ( "MODEL_BUILT_IN_RCD_SYM_V3::do_tt_prepare" + brh->short_label()).c_str(),  -cc->_wcorr );
//}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::__Re(double uin, const COMMON_COMPONENT* c) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Re0/uin;
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::__Rc(double uin, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Rc0 * exp( cc->_Rc1 * uin );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::__Ge(double uin, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return uin/cc->_Re0; 
}
/*--------------------------------------------------------------------------*/
long double MODEL_BUILT_IN_RCD_SYM_V3::__E(double uin, long double cur, const COMMON_COMPONENT* c ) const 
{
  assert(false);
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  
  long double Rc0=cc->_Rc0;
  long double Re0=cc->_Re0;
  long double Rc1=cc->_Rc1;
  long double t=_sim->_last_time;

  return ( -(1.0/(1.0 + exp(-Rc1*uin) * Re0/uin/Rc0) - cur) 
       * exp( -(Rc0*uin*exp(Rc1*uin)/Re0 + 1)*t*exp(-Rc1*uin)/Rc0 )
       + 1.0/(1.0 + Re0/uin/Rc0/exp(Rc1*uin)));

// sage output:
  return ( -(Rc0*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin)
        - cur)*exp(-(Rc0*uin*exp(Rc1*uin)/Re0 + 1)*t*exp(-Rc1*uin)/Rc0) +
      Rc0*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin));

}
/*--------------------------------------------------------------------------*/
// limit
double MODEL_BUILT_IN_RCD_SYM_V3::__E(double uin, const COMMON_COMPONENT* c ) const 
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  double tau_e_here = __Re( uin, cc);
  double tau_c_here = __Rc( uin, cc);
  return( tau_c_here / ( tau_e_here + tau_c_here ));
}
/*--------------------------------------------------------------------------*/
//long double MODEL_BUILT_IN_RCD_SYM_V3::__Edu(double uin, long double cur, const COMMON_COMPONENT* ccmp)const{
 // const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(ccmp);
 // assert(cc);
 // return cc->__Edu(uin,cur); 

//}
/*--------------------------------------------------------------------------*/
long double MODEL_BUILT_IN_RCD_SYM_V3::__Edu(long double uin, long double cur, const COMMON_COMPONENT* cc ) const
{
  const COMMON_BUILT_IN_RCD* c = dynamic_cast<const COMMON_BUILT_IN_RCD*>(cc) ;
  long double Rc0=c->_Rc0;
  long double Re0=c->_Re0;
  long double Rc1=c->_Rc1;
  long double t=_sim->_last_time;

//return ( -((cur - 1)*Rc0*Rc0*Rc0*t*uin*uin*exp(3*Rc1*uin) -
//          Rc1*Re0*Re0*Re0*cur*t - (Rc0*Rc0*Rc1*Re0*Re0*uin + Rc0*Rc0*Re0*Re0)*exp(2*Rc1*uin +
//            t*exp(-Rc1*uin)/Rc0 + t*uin/Re0) - ((2*cur - 1)*Rc0*Rc1*Re0*Re0*t*uin -
//            Rc0*Re0*Re0*cur*t)*exp(Rc1*uin) - ((cur - 1)*Rc0*Rc0*Rc1*Re0*t*uin*uin -
//              Rc0*Rc0*Re0*Re0 - ((2*cur - 1)*Rc0*Rc0*Re0*t +
//                Rc0*Rc0*Rc1*Re0*Re0)*uin)*exp(2*Rc1*uin))*exp(-t*exp(-Rc1*uin)/Rc0 -
//              t*uin/Re0)/(Rc0*Rc0*Rc0*Re0*uin*uin*exp(3*Rc1*uin) +
//                2*Rc0*Rc0*Re0*Re0*uin*exp(2*Rc1*uin) + Rc0*Re0*Re0*Re0*exp(Rc1*uin)));

  long double ret =  -((cur - 1.0L)*Rc0*Rc0*Rc0*t*uin*uin*expl(3.0L*Rc1*uin) -
  Rc1*Re0*Re0*Re0*cur*t - (Rc0*Rc0*Rc1*Re0*Re0*uin + Rc0*Rc0*Re0*Re0)*expl(2.0L*Rc1*uin +
  t*expl(-Rc1*uin)/Rc0 + t*uin/Re0) - ((2.0L*cur - 1.0L)*Rc0*Rc1*Re0*Re0*t*uin -
  Rc0*Re0*Re0*cur*t)*expl(Rc1*uin) - ((cur - 1.0L)*Rc0*Rc0*Rc1*Re0*t*uin*uin -
  Rc0*Rc0*Re0*Re0 - ((2.0L*cur - 1.0L)*Rc0*Rc0*Re0*t +
  Rc0*Rc0*Rc1*Re0*Re0)*uin)*expl(2.0L*Rc1*uin))*expl(-t*expl(-Rc1*uin)/Rc0 -
  t*uin/Re0)/(Rc0*Rc0*Rc0*Re0*uin*uin*expl(3.0L*Rc1*uin) +
  2.0L*Rc0*Rc0*Re0*Re0*uin*expl(2.0L*Rc1*uin) + Rc0*Re0*Re0*Re0*expl(Rc1*uin));
  if (!(ret>=0)) {
    fprintf(stderr,"__Edu: ret is not positive %Lg ",ret);
  } 
  assert(ret>=0);
  return(ret);

  ret= (- (Rc0*expl(Rc1*uin)/(Rc0*expl(Rc1*uin) + Re0/uin) - cur)
          *((Rc0*uin*expl(Rc1*uin)/Re0 + 1.0L)*Rc1*t*expl(-Rc1*uin)/Rc0 -
      (Rc0*Rc1*uin*expl(Rc1*uin)/Re0 +
       Rc0*expl(Rc1*uin)/Re0)*t*expl(-Rc1*uin)/Rc0)
          *expl(-(Rc0*uin*expl(Rc1*uin)/Re0 +
       1.0L)*t*expl(-Rc1*uin)/Rc0) +  Rc0*Rc1*expl(Rc1*uin)/(Rc0*expl(Rc1*uin) + Re0/uin)
                               - (Rc0*Rc1*expl(Rc1*uin)/(Rc0*expl(Rc1*uin) + Re0/uin) -
      (Rc0*Rc1*expl(Rc1*uin) - Re0/uin/uin)*Rc0*expl(Rc1*uin)/square(Rc0*expl(Rc1*uin) +
        Re0/uin))*expl(-(Rc0*uin*expl(Rc1*uin)/Re0 + 1.0L)*t*expl(-Rc1*uin)/Rc0) -
  (Rc0*Rc1*expl(Rc1*uin) - Re0/uin/uin)*Rc0*expl(Rc1*uin)/
  square(Rc0*expl(Rc1*uin) + Re0/uin));

  assert(ret>=0);
  return(ret);

  //sage orig. Cified.
  return (- (Rc0*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin) - cur)
          *((Rc0*uin*exp(Rc1*uin)/Re0 + 1)*Rc1*t*exp(-Rc1*uin)/Rc0 -
      (Rc0*Rc1*uin*exp(Rc1*uin)/Re0 +
       Rc0*exp(Rc1*uin)/Re0)*t*exp(-Rc1*uin)/Rc0)
          *exp(-(Rc0*uin*exp(Rc1*uin)/Re0 +
       1)*t*exp(-Rc1*uin)/Rc0) +  Rc0*Rc1*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin)
                               - (Rc0*Rc1*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin) -
      (Rc0*Rc1*exp(Rc1*uin) - Re0/uin/uin)*Rc0*exp(Rc1*uin)/square(Rc0*exp(Rc1*uin) +
        Re0/uin))*exp(-(Rc0*uin*exp(Rc1*uin)/Re0 + 1)*t*exp(-Rc1*uin)/Rc0) -
  (Rc0*Rc1*exp(Rc1*uin) - Re0/uin/uin)*Rc0*exp(Rc1*uin)/
  square(Rc0*exp(Rc1*uin) + Re0/uin));


}
/*--------------------------------------------------------------------------*/
// solve E(uin)-E==0
//long double MODEL_BUILT_IN_RCD_SYM_V3::__uin_iter(long double& uin, double
//    E_old, double E, COMPONENT* dd ) const { const
//  COMMON_BUILT_IN_RCD* c = dynamic_cast<const COMMON_BUILT_IN_RCD*>(dd->common());
//  assert(false);
//  return c->__uin_iter( uin, E_old ,E,0,0,dd ); }
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// old.
DEV_BUILT_IN_RCD_SYM_V3::DEV_BUILT_IN_RCD_SYM_V3()
  :DEV_BUILT_IN_RCD()
{
}
/*--------------------------------------------------------------------------*/
int  MODEL_BUILT_IN_RCD_SYM_V3::tt_region(const COMPONENT* brh) const
{
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;

  assert(c);
  return ( (c->_Ccgfill)->region() );
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_precalc_last(COMMON_COMPONENT* ccmp, const CARD_LIST*)const
{
  COMMON_BUILT_IN_RCD* cc = dynamic_cast<COMMON_BUILT_IN_RCD*>(ccmp);
  assert(cc);
  const MODEL_BUILT_IN_RCD_SYM_V3* m=this;

  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->Uref, cc->Recommon0, cc->Rccommon0);

  double up   =  cc->Recommon0;
  double down =  cc->Rccommon0;

  // RE = cc->_Re0/uin;
  cc->_Re0 = cc->Uref* (down  + up ) * up / down;

  // RC = cc->_Rc0 * exp( cc->_Rc1 * uin );
  cc->_Rc0 = down;
  cc->_Rc1 = log(up/down+1)/cc->Uref;

  double Eend_bad = (cc->Uref / (cc->__Re(cc->Uref) / cc->__Rc(cc->Uref) +1));

  cc->_wcorr = double ( cc->Uref / Eend_bad );
  cc->_weight = cc->weight;

  // sanity check.
  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->__tau_up(cc->Uref), cc->Recommon0, cc->Rccommon0);
  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->_Rc1, cc->_Re0, cc->_Rc0);
  trace2("MODEL_BUILT_IN_RCD::do_precalc_last", m->__tau(0,cc), down);

  if ( abs( m->__tau(cc->Uref,cc) - cc->Recommon0)/cc->Recommon0 > 1e-2 ){
    error(bDANGER, ("MODEL_BUILT_IN_RCD::do_precalc_last %s cc->Uref=%E Rec=%E mtau=%E rc0=%E\n"),
        (  long_label() ).c_str(),
        (double) cc->Uref,
        (double) cc->Recommon0, 
        m->__tau(cc->Uref,cc),
        (double)cc->Rccommon0  );

    assert(false);
  }


  assert( cc->weight != 0 );
  assert( cc->_weight != 0 );
  assert( abs( m->__tau(0,cc) - down)/down <1e-6 );
  assert( is_number( cc->_Rc1 ) );
  assert( is_number( cc->_Rc0 ) );
}
/*--------------------------------------------------------------------------*/
ADP_NODE_RCD* MODEL_BUILT_IN_RCD_SYM_V3::new_adp_node(const COMPONENT* c) const
{
  return new ADP_NODE_RCD(c);
}
