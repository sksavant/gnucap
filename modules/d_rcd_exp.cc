/*
 *
 * (c)2010 felix salfelder
 */

// generic RCD cell (exponential taus)

#include "e_aux.h"
#include "e_storag.h"
// #include "d_mos_base.h"

#include "globals.h"
#include "e_elemnt.h"
#include "e_adp.h"

#include "d_rcd_sym.h"

class DEV_BUILT_IN_RCD;

class MODEL_BUILT_IN_RCD_EXP : public MODEL_BUILT_IN_RCD_SYM {
  protected:
    explicit MODEL_BUILT_IN_RCD_EXP(const MODEL_BUILT_IN_RCD_EXP& p);
  public:
    explicit MODEL_BUILT_IN_RCD_EXP(const BASE_SUBCKT* p);
    //virtual void do_tt_prepare(COMPONENT*)const;
    virtual void do_precalc_last(COMMON_COMPONENT* , const CARD_LIST* par_scope)const;
    virtual bool v2() const{return true;}
    // ~MODEL_BUILT_IN_RCD_EXP() : ~MODEL_BUILT_IN_RCD {}
    void do_stress_apply( COMPONENT* d ) const;
    void do_tr_stress( const COMPONENT*) const;        
    std::string dev_type()const ;
    void      set_dev_type(const std::string& nt )
    {
      assert(&nt);
     trace0(("MODEL_BUILT_IN_RCD_EXP::set_dev_type() " + nt).c_str()); 
    };
    CARD* clone()const {return new MODEL_BUILT_IN_RCD_EXP(*this);}
    void do_expand( COMPONENT*) const;
    ADP_NODE_RCD* new_adp_node(const COMPONENT*) const;
//    region_t region(const COMPONENT*) const;
    double dvth( const COMPONENT* brh) const;
  private:
    double __dRe(double uin, const COMMON_COMPONENT* cc)const; // unneeded?
    double __dRc(double uin, const COMMON_COMPONENT* cc)const;
    double __Re(double uin, const COMMON_COMPONENT* cc)const;
    double __Rc(double uin, const COMMON_COMPONENT* cc)const;
    double __Ge(double uin, const COMMON_COMPONENT* cc)const;
    double __tau(double uin, const COMMON_COMPONENT* cc)const;
  public:
    virtual void do_tr_stress_last(long double fill, ADP_NODE*, COMPONENT*  ) const;
  private:
    //double __uin_iter(double& uin, double E, const COMMON_COMPONENT* cc)const;
    double __E(double uin, const COMMON_COMPONENT* cc)const;

   // long double __uin_iter(long double& uin,  double cur, double E, COMPONENT* )const;
    long double __E(double uin, long double cur, const COMMON_COMPONENT* cc)const;
    long double __dstepds(long double uin, long double cur, const COMMON_COMPONENT* cc)const;
//    template<class T>
//    BUG: _step in MODEL_RCD not generic.
  protected:
    long double __step(long double s, long double cur,  double dt, const COMMON_COMPONENT* ) const ; // MODEL_RCD
};
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_EXP::do_stress_apply( COMPONENT*  ) const
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
//void DEV_BUILT_IN_RCD_EXP::tr_stress()
//{
//  unreachable(); // obsolet....
//
//  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
//  assert(c);
//  assert(c->model());
//  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
//  assert(m);
//  assert(c->sdp());
//
//  assert(false);
//
//}
///*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_RCD_DISPATCHER
{ 
  static DEV_BUILT_IN_RCD_SYM p4d;
  static MODEL_BUILT_IN_RCD_EXP p4(&p4d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d3(&model_dispatcher, "rcd_exp", &p4);
}
///*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_EXP::do_expand(  COMPONENT* c) const
{
  DEV_BUILT_IN_RCD_SYM* d = dynamic_cast<DEV_BUILT_IN_RCD_SYM*>(c);
  assert(d);
  // d->expand();
  
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD_EXP::dev_type()const
{
  return "rcd";
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_EXP::MODEL_BUILT_IN_RCD_EXP(const BASE_SUBCKT* p) 
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  // uref=1; 
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_EXP::MODEL_BUILT_IN_RCD_EXP(const MODEL_BUILT_IN_RCD_EXP& p)  
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::__tau(double uin, const COMMON_COMPONENT* cc)const
{
  double tau_e = __Re( uin, cc);
  double tau_c = __Rc( uin, cc);

  if (tau_c < tau_e){
    return tau_c  / ( tau_c/tau_e +1 );
  } else{
    return tau_e  / ( tau_e/tau_c +1 );
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_EXP::do_tr_stress( const COMPONENT* brh) const
{
  const MODEL_BUILT_IN_RCD_EXP* m = this;
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(c->common());

  double fill = c->_Ccgfill->get_total();
  double uin = c->involts();

  unreachable();
  trace2("MODEL_BUILT_IN_RCD_EXP::do_tr_stress ", fill, uin );
  trace2("MODEL_BUILT_IN_RCD_EXP::do_tr_stress ", c->involts(), iteration_number() );
  assert (fill>=E_min);
  if (fill >= 1.0 ){
    error(bDANGER, " RCD_V4 %s fill %E too big\n", brh->long_label().c_str(), fill );
    fill = 1;
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

  trace4("MODEL_BUILT_IN_RCD_EXP::tr_stress ", tau_c_here, tau_e_here, _sim->_Time0, _sim->_time0 );
  trace6("MODEL_BUILT_IN_RCD_EXP::tr_stress ", fill, h, tau, newfill, uin, uend );

  c->_Ccgfill->tr_add(newfill-fill); // ERROR?
  assert(newfill-fill < 1 );

  trace4("DEV_BUILT_IN_RCD_EXP::tr_stress ", fill, h, tau, (newfill-fill)/h );

  if(newfill > 1.000001){
    error(bDANGER, ("* RCD_V4 %f too big\n" + long_label() ).c_str() , newfill );
    newfill=1;
  }
  assert(newfill==newfill);
  assert(h > 0);
  assert(is_number(tau));
  assert(is_number(uend));
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last( long double E, ADP_NODE* _c,
    COMPONENT* dd ) const
{
  // ADP_NODE_UDC* udc= dynamic_cast<ADP_NODE_UDC*>(a);
  DEV_BUILT_IN_RCD* d = asserted_cast<DEV_BUILT_IN_RCD*>(dd);
  const COMMON_BUILT_IN_RCD* cc = asserted_cast<const COMMON_BUILT_IN_RCD*>(d->common());
  const COMMON_BUILT_IN_RCD* c = cc;
  const MODEL_BUILT_IN_RCD* m = this; //  asserted_cast<const MODEL_BUILT_IN_RCD*>(this);
  assert(m);
  assert(is_number(_c->tt()));
  double E_old = _c->tt();


  if (_c->tr()==-inf) _c->tr() = ( _c->tr_lo+_c->tr_hi)/2.0;
  if (!is_number(_c->tr())) _c->tr() = (_c->tr_lo+_c->tr_hi)/2.0;

  long double uin_eff=_c->tr(); // 0 == current estimate

  trace3(("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last " +
        _c->label()).c_str(), E_old, tt_iteration_number(), uin_eff);

  assert(E_old<E_max);
  assert(E<=E_max);

  if (E>1) {
    trace0("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last aligned E");
    untested();
    E=1;
  }

  long double E_high; 
  long double E_low; 
  long double uin_high = max ( uin_eff + OPT::abstol, uin_eff * (1+OPT::reltol) );
  long double uin_low  = min ( uin_eff - OPT::abstol, uin_eff * (1-OPT::reltol) );
  assert (uin_high>=uin_low);

  E_high = __step( uin_high, E_old, CKT_BASE::_sim->_last_time, c );
  E_low  = __step( uin_low,  E_old, CKT_BASE::_sim->_last_time, c ); 

  assert (E_low>=0);

  if(((double)E_high < (double)E_low - 1e-18)){
    error(bDANGER,"MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last uin_high=%LE uin_low=%LE deltaE= %LE; %LE>%LE\n",
        uin_high, uin_low, E_high - E_low, E_high, E_low );
    throw(Exception("order mismatch in exp"));
  }

  bool linear_inversion=false;

  if ( E_low <= E && E <= E_high ) {
    // invert linearly
    uin_eff =uin_low + (uin_high - uin_low) * ( E -E_low)/ (E_high-E_low);
    trace5("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last linv", 1-E_low, 1-E, E_high-E_low, uin_low, uin_high );
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
      uin_eff = cc->__uin_iter(uin_eff, E_old, (double)E, _c->tr_lo, _c->tr_hi, d );
    } catch (Exception &e) {
      error(bDANGER, "Exception in %s\n", long_label().c_str());
      throw(e);
    }
    trace3("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last iteration -> ", E_old,
        E, uin_eff);
  }

  // sanitycheck (monotonicity)
  long double E_test = this->__step( uin_eff, E_old, CKT_BASE::_sim->_last_time, c );
  double trvorher= _c->tr();
  long double E_vorher = this->__step( trvorher , E_old, CKT_BASE::_sim->_last_time, c );
  if (fabs(E_test - E) < fabs(E_vorher-E)){
    trace6("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last new uin better ",
        uin_eff, _c->tr(), E_test - E, E_vorher-E,1-E , linear_inversion);
    trace5("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last                ",
        uin_eff, _c->tr(), E_test - E, E_vorher-E,1-E );
  }else{
    trace3("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last new uin not better ",
        uin_eff, _c->tr(), linear_inversion);
    if ( (E_test - E) * (E_vorher-E) < 0 ){
      uin_eff = (_c->tr() + uin_eff)/2.0;
      untested();
    }
    _c->set_order(0);
  }

  if ( linear_inversion && ((E_low > E_test) || ( E_test > E_high) )){
    trace3("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last monotonicity check ",
        uin_eff-uin_low, uin_high-uin_eff, uin_eff );
    assert( uin_eff<=uin_high && uin_low<=uin_eff);
    trace5("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last ", 1-E, 1-E_test,
        E_test-E_low, E_high-E_test, linear_inversion );
    assert( E_test <= E_high && E_low <= E_test); 
  }

  uin_high = max ( uin_eff + OPT::abstol, uin_eff * (1 + OPT::reltol) );
  uin_low  = min ( uin_eff - OPT::abstol, uin_eff * (1-OPT::reltol) );
  trace3(("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last E" + _c->label()).c_str(),
      uin_low, uin_eff, uin_high);

  E_high = __step( uin_high, E_old, CKT_BASE::_sim->_last_time, c );
  E_low  = __step( uin_low,  E_old, CKT_BASE::_sim->_last_time, c ); 
  trace6(("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last E" + _c->label()).c_str(),
      uin_eff,  E-E_low, E_high - E_low, E, 1-E, E_high-E );
  trace3(("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last E" + _c->label()).c_str(), 
      E-E_low , E_high-E, linear_inversion  );

  if( ( E_old < E_high ) && ( E_low <= E_old ))
    _c->set_order(0);

  if ((double)E_high<(double)E_low - 1e-19){
    error( bDANGER, "MODEL_BUILT_IN_RCD_EXP:: sanitycheck ( %LE ) in %s\n", E_high - E_low, dd->long_label().c_str());
    assert(false);
  }

  if (linear_inversion)  _c->set_order(0);
  //assert ( uin_eff == _c->get_tr());
  //
  //
  //
  assert(is_number(E_high-E_low));
  //assert( (double) E_high-(double) E_low >= 0);
  ///assert((double) E_high>= (double) E_low);
  assert(E_low <= E || double(E)==1.0 || double(E_high)==1.0 || !linear_inversion);

  if(E > E_high && E!=1){
    untested1("MODEL_IN_RCD_EXP::", linear_inversion);
    error( bDANGER, "MODEL_BUILT_IN_RCD_EXP:: sanitycheck ( %LE =E >  E_high=%LE ) del %LE\n", E_high, E, E_high-E);
  }

  _c->set_tr_noise (0); //(double)E_high-(double)E_low);
  _c->set_tr((double)uin_eff);
  
  if ( CKT_BASE::_sim->tt_iteration_number()>1 ){
    if((_c->tr()-_c->tr1())*(_c->tr1()-_c->tr2())<=0 ){
      _c->set_order(0);
    }
  }

  trace2("MODEL_BUILT_IN_RCD_EXP::do_tr_stress_last done", _c->get_tr_noise(), uin_eff);
  assert(is_number(uin_eff));

}

/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::__Re(double s, const COMMON_COMPONENT* c) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return exp( cc->_Re1 * s + cc->_Re0 );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::__dRe(double s, const COMMON_COMPONENT* c) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Re1 * exp( cc->_Re1 * s + cc->_Re0 );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::__Rc(double s, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return exp( cc->_Rc1 * s  + cc->_Rc0 );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::__dRc(double s, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Rc1 * exp( cc->_Rc1 * s  + cc->_Rc0 );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::__Ge(double s, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return exp( - cc->_Re1 * s - cc->_Re0 );
}
/*--------------------------------------------------------------------------*/
//template<class T>
//T MODEL_BUILT_IN_RCD_EXP::__step(T s, T cur,  double t, const COMMON_COMPONENT* c ) const 
long double MODEL_BUILT_IN_RCD_EXP::__step(long double s, long double cur, double deltat, const COMMON_COMPONENT* c ) const 
{
  //cout << "exp step\n";
  assert(is_number(s));
  assert(is_number(cur));
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  const MODEL_BUILT_IN_RCD* m =   static_cast<const MODEL_BUILT_IN_RCD*>(this);
  assert ( s >= 0 || !m->positive);
  
  long double Eend = __E_end(s,cc);
  long double tauinv = __tau_inv(s,cc);
  long double ret = (cur-Eend) * expl( -deltat*tauinv ) + Eend;

  trace5("MODEL_BUILT_IN_RCD_EXP::__step ", Eend, deltat, s,  ret,  logl(fabsl(cur-Eend ) ) );
//  assert(is_almost(retalt ,ret));

  if (!is_number(ret) || ret < -0.1){
    trace4("COMMON_BUILT_IN_RCD::__step ", Eend, deltat, s, logl(fabsl(cur-Eend ) ) );
    trace4("COMMON_BUILT_IN_RCD::__step ", cur-Eend, deltat, s, fabsl(cur-Eend )  );
    trace3("COMMON_BUILT_IN_RCD::__step ", cur, deltat, s  );
    assert(false);
  }

  return(ret);

}
/* E(t=inf) */
/*--------------------------------------------------------------------------*/
long double MODEL_BUILT_IN_RCD_EXP::__dstepds(long double uin, long double cur, const COMMON_COMPONENT* cc ) const
{
  const COMMON_BUILT_IN_RCD* c = dynamic_cast<const COMMON_BUILT_IN_RCD*>(cc) ;
  long double Rc0=c->_Rc0;
  long double Re0=c->_Re0;
  long double Rc1=c->_Rc1;
  long double Re1=c->_Re1;
  long double t=_sim->_last_time;

  return 1;

  long double ret =
    -((cur - 1.L)*Rc0*Rc0*Rc0*Re1*t*expl(3.L*(Rc1 + Re1)*uin) - Rc1*Re0*Re0*Re0*cur*t - ((cur - 1.L)*Rc0*Rc0*Rc1*Re0 - 
          (2.L*cur - 1.L)*Rc0*Rc0*Re0*Re1)*t*expl(2.L*(Rc1 + Re1)*uin) - (((2.L*cur - 1.L)*Rc0*Rc1*Re0*Re0
                - Rc0*Re0*Re0*Re1*cur)*t*expl(Rc1*uin) - (Rc0*Rc0*Rc1*Re0*Re0 +
                  Rc0*Rc0*Re0*Re0*Re1)*expl(2.L*Rc1*uin))*expl(Re1*uin) - (Rc0*Rc0*Rc1*Re0*Re0 +
                  Rc0*Rc0*Re0*Re0*Re1)*expl(((2*Rc0*Rc1*Re0 + Rc0*Re0*Re1)*uin*expl(Rc1*uin) +
                      Rc0*t*expl((Rc1 + Re1)*uin) +
                      Re0*t)*expl(-Rc1*uin)/(Rc0*Re0)))
    *expl(-(Rc0*t*expl((Rc1 + Re1)*uin) + Re0*t)*expl(-Rc1*uin)/(Rc0*Re0))
    /(Rc0*Rc0*Rc0*Re0*expl((3.L*Rc1 + 2.L*Re1)*uin) +
                      2.L*Rc0*Rc0*Re0*Re0*expl((2.L*Rc1 + Re1)*uin) + Rc0*Re0*Re0*Re0*expl(Rc1*uin));

  
  assert(ret>=0);
  return(ret);
  /*sage
  ret =
    -((cur - 1)*Rc0^3*Re1*t*e^(3*(Rc1 +
            Re1)*uin) - Rc1*Re0^3*cur*t - ((cur - 1)*Rc0^2*Rc1*Re0 - (2*cur -
              1)*Rc0^2*Re0*Re1)*t*e^(2*(Rc1 + Re1)*uin) - (((2*cur - 1)*Rc0*Rc1*Re0^2
                - Rc0*Re0^2*Re1*cur)*t*e^(Rc1*uin) - (Rc0^2*Rc1*Re0^2 +
                  Rc0^2*Re0^2*Re1)*e^(2*Rc1*uin))*e^(Re1*uin) - (Rc0^2*Rc1*Re0^2 +
                  Rc0^2*Re0^2*Re1)*e^(((2*Rc0*Rc1*Re0 + Rc0*Re0*Re1)*uin*e^(Rc1*uin) +
                      Rc0*t*e^((Rc1 + Re1)*uin) +
                      Re0*t)*e^(-Rc1*uin)/(Rc0*Re0)))*e^(-(Rc0*t*e^((Rc1 + Re1)*uin) +
                      Re0*t)*e^(-Rc1*uin)/(Rc0*Re0))/(Rc0^3*Re0*e^((3*Rc1 + 2*Re1)*uin) +
                      2*Rc0^2*Re0^2*e^((2*Rc1 + Re1)*uin) + Rc0*Re0^3*e^(Rc1*uin))
*/

}
/*--------------------------------------------------------------------------*/
// solve E(uin)-E==0
//long double MODEL_BUILT_IN_RCD_EXP::__uin_iter(long double& uin, double
//    E_old, double E, COMPONENT* dd ) const { const
//  COMMON_BUILT_IN_RCD* c = dynamic_cast<const COMMON_BUILT_IN_RCD*>(dd->common());
//  assert(false);
//  return c->__uin_iter( uin, E_old ,E,0,0,dd ); 
//}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_EXP::do_precalc_last(COMMON_COMPONENT* ccc, const CARD_LIST*)const
{
  COMMON_BUILT_IN_RCD* cc = asserted_cast<COMMON_BUILT_IN_RCD*>(ccc);
  COMMON_BUILT_IN_RCD* c=cc; 
  assert(cc);
  //const MODEL_BUILT_IN_RCD_EXP* m=this;

  cc->Uref=0;

  trace5("MODEL_BUILT_IN_RCD_EXP::do_precalc_last", cc->Uref,
      c->Recommon1,
      c->Recommon0,
      c->Rccommon1,
      c->Rccommon0);

  //double up   =  cc->Recommon0;

  c->_Re1 = cc->Recommon1;
  c->_Re0 = cc->Recommon0;
  c->_Rc1 = cc->Rccommon1;
  c->_Rc0 = cc->Rccommon0;
  // double Eend_bad = (cc->Uref / (cc->__Re(cc->Uref) / cc->__Rc(cc->Uref) +1));

  c->_zero = (__Rc(0,c) / (__Re(0,c) + __Rc(0,c) ));
  c->_zero = (double)__E_end_0(c);

  cerr.precision(150);
  trace1("MODEL_BUILT_IN_RCD_EXP::do_precalc_last", cc->_zero);
  cerr.precision(16);
  

  cc->_wcorr = 1;
  cc->_weight = cc->weight;


  assert( cc->weight != 0 );
  assert( cc->_weight != 0 );
  assert( is_number( cc->_Rc1 ) );
  assert( is_number( cc->_Rc0 ) );
}
/*-------------------------------------------------*/
double MODEL_BUILT_IN_RCD_EXP::dvth( const COMPONENT* brh) const
{
  const DEV_BUILT_IN_RCD* d = prechecked_cast<const DEV_BUILT_IN_RCD*>(brh);
  const COMMON_BUILT_IN_RCD* c = prechecked_cast<const COMMON_BUILT_IN_RCD*>(brh->common());
  if ( _sim->analysis_is_tt() ){
    if(positive && (d->_Ccgfill->get_tt() < c->_zero)){
      error(bDANGER,"not positive %f %f\n", double(d->_Ccgfill->get_tt()), double(c->_zero));
    }
    return (d->_Ccgfill->get_tt() - c->_zero) * c->_weight;
  }else{
    assert(is_number( d->_Ccgfill->get_tt() * c->_weight));
    // return c->_Ccgfill->get_tt() * c->_weight * c->_wcorr;
    //
    //FIXME. _tr_fill must be part of an ADP_NODE
    trace2("MODEL_BUILT_IN_RCD_EXP::dvth",  d->_tr_fill,  c->_weight  );
    assert( d->_Ccgfill->get_tt() <=1 );
    assert( d->_tr_fill <=1 );
    return double((d->_tr_fill -c->_zero) * c->_weight);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ADP_NODE_RCD* MODEL_BUILT_IN_RCD_EXP::new_adp_node(const COMPONENT* c) const
{
  return new ADP_NODE_RCD(c);
}

// vim:ts=8:sw=2:et:
