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

#include "d_rcd_sym.h"

class DEV_BUILT_IN_RCD;

class MODEL_BUILT_IN_RCD_SYM_V4 : public MODEL_BUILT_IN_RCD_SYM {
  protected:
    explicit MODEL_BUILT_IN_RCD_SYM_V4(const MODEL_BUILT_IN_RCD_SYM_V4& p);
  public:
    explicit MODEL_BUILT_IN_RCD_SYM_V4(const BASE_SUBCKT* p);
    //virtual void do_tt_prepare(COMPONENT*)const;
    virtual void do_precalc_last(COMMON_COMPONENT* ccmp, const CARD_LIST* par_scope)const;
    virtual bool v2() const{return false;}
    // ~MODEL_BUILT_IN_RCD_SYM_V4() : ~MODEL_BUILT_IN_RCD {}
    bool use_net() const { return(0); }
    void do_stress_apply( COMPONENT* d ) const;
    void do_tr_stress( const COMPONENT*) const;        
    std::string dev_type()const ;
    void      set_dev_type(const std::string& nt )
    {
      assert(&nt);
     trace0(("MODEL_BUILT_IN_RCD_SYM_V4::set_dev_type() " + nt).c_str()); 
    };
    CARD* clone()const {return new MODEL_BUILT_IN_RCD_SYM_V4(*this);}
    void do_expand( COMPONENT*) const;
    ADP_NODE_RCD* new_adp_node(const COMPONENT*) const;
//    region_t region(const COMPONENT*) const;
    int  tt_region(const COMPONENT*) const;
    double __Re(double uin, const COMMON_COMPONENT* cc)const;
    double __Rc(double uin, const COMMON_COMPONENT* cc)const;
    double __Ge(double uin, const COMMON_COMPONENT* cc)const;
    double __tau(double uin, const COMMON_COMPONENT* cc)const;
    virtual void do_tr_stress_last(long double fill, ADP_NODE* , const COMMON_COMPONENT* cc ) const;
  private:
    double __uin_iter(double& uin, double E, const COMMON_COMPONENT* cc)const;
    double __E(double uin, const COMMON_COMPONENT* cc)const;

    long double __uin_iter(long double& uin,  double cur, double E, const COMMON_COMPONENT* cc)const;
    long double __E(double uin, long double cur, const COMMON_COMPONENT* cc)const;
    long double __Edu(long double uin, long double cur, const COMMON_COMPONENT* cc)const;
};
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V4::do_stress_apply( COMPONENT*  ) const
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
//void DEV_BUILT_IN_RCD_SYM_V4::tr_stress()
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
  static MODEL_BUILT_IN_RCD_SYM_V4 p4(&p4d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d3(&model_dispatcher, "rcdsym_v4|rcdsym", &p4);
}
///*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V4::do_expand(  COMPONENT* c) const
{
  DEV_BUILT_IN_RCD_SYM* d = dynamic_cast<DEV_BUILT_IN_RCD_SYM*>(c);
  assert(d);
  // d->expand();
  
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD_SYM_V4::dev_type()const
{
  return "rcd";
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V4::MODEL_BUILT_IN_RCD_SYM_V4(const BASE_SUBCKT* p) 
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  // uref=1; 
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V4::MODEL_BUILT_IN_RCD_SYM_V4(const MODEL_BUILT_IN_RCD_SYM_V4& p)  
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V4::__tau(double uin, const COMMON_COMPONENT* cc)const
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
void MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress( const COMPONENT* brh) const
{
  const MODEL_BUILT_IN_RCD_SYM_V4* m = this;
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(c->common());

  double fill = c->_Ccgfill->get_total();
  double uin = c->involts();

  unreachable();
  trace2("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress ", fill, uin );
  trace2("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress ", c->involts(), iteration_number() );
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

  trace4("MODEL_BUILT_IN_RCD_SYM_V4::tr_stress ", tau_c_here, tau_e_here, _sim->_Time0, _sim->_time0 );
  trace6("MODEL_BUILT_IN_RCD_SYM_V4::tr_stress ", fill, h, tau, newfill, uin, uend );

  c->_Ccgfill->tr_add(newfill-fill); // ERROR?
  assert(newfill-fill < 1 );

  trace4("DEV_BUILT_IN_RCD_SYM_V4::tr_stress ", fill, h, tau, (newfill-fill)/h );

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
void MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last( long double E, ADP_NODE* _c,
    const COMMON_COMPONENT* ccmp ) const
{
  // ADP_NODE_UDC* udc= dynamic_cast<ADP_NODE_UDC*>(a);
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(ccmp);
  const MODEL_BUILT_IN_RCD* m =   static_cast<const MODEL_BUILT_IN_RCD*>(this);
  assert(m);
  assert(is_number(_c->tt()));
  double E_old = _c->tt();


  if (_c->tr()==-inf) _c->tr() = ( _c->tr_lo+_c->tr_hi)/2.0;
  if (!is_number(_c->tr())) _c->tr() = (_c->tr_lo+_c->tr_hi)/2.0;

  long double uin_eff=_c->tr(); // 0 == current estimate

  trace3(("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last " +
        _c->label()).c_str(), E_old, tt_iteration_number(), uin_eff);

  assert(E_old<E_max);
  assert(E<=E_max);

  if (E>1) {
    trace0("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last aligned E");
    untested();
    E=1;
  }

  long double E_high; 
  long double E_low; 
  long double uin_high = max ( uin_eff + OPT::abstol, uin_eff * (1+OPT::reltol) );
  long double uin_low  = min ( uin_eff - OPT::abstol, uin_eff * (1-OPT::reltol) );
  assert (uin_high>=uin_low);

  E_high = cc->__step( uin_high, E_old, CKT_BASE::_sim->_last_time );
  E_low  = cc->__step( uin_low,  E_old, CKT_BASE::_sim->_last_time ); 

  assert (E_low>=0);

  if(!((double)E_high >= (double)E_low)){
    error(bDANGER,"MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last uin_high=%LE uin_low=%LE deltaE= %LE; %LE>%LE\n",
        uin_high, uin_low, E_high - E_low, E_high, E_low );
    throw(Exception("false"));
  }

  bool linear_inversion=false;

  if ( E_low <= E && E <= E_high ) {
    // invert linearly
    uin_eff =uin_low + (uin_high - uin_low) * ( E -E_low)/ (E_high-E_low);
    trace5("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last linv", 1-E_low, 1-E, E_high-E_low, uin_low, uin_high );
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
      uin_eff = cc->__uin_iter(uin_eff, E_old, (double)E, _c->tr_lo, _c->tr_hi );
    } catch (Exception &e) {
      error(bDANGER, "Exception in %s\n", long_label().c_str());
      throw(e);
    }
    trace3("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last iteration -> ", E_old,
        E, uin_eff);
  }

  // sanitycheck (monotonicity)
  long double E_test=cc->__step( uin_eff, E_old, CKT_BASE::_sim->_last_time );
  double trvorher= _c->tr();
  long double E_vorher=cc->__step( trvorher , E_old, CKT_BASE::_sim->_last_time );
  if (fabs(E_test - E) < fabs(E_vorher-E)){
    trace6("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last new uin better ",
        uin_eff, _c->tr(), E_test - E, E_vorher-E,1-E , linear_inversion);
    trace5("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last                ",
        uin_eff, _c->tr(), E_test - E, E_vorher-E,1-E );
  }else{
    trace3("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last new uin not better ",
        uin_eff, _c->tr(), linear_inversion);
    if ( (E_test - E) * (E_vorher-E) < 0 ){
      uin_eff = (_c->tr() + uin_eff)/2.0;
      untested();
    }
    _c->set_order(0);
  }

  if ( linear_inversion && ((E_low > E_test) || ( E_test > E_high) )){
    trace3("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last monotonicity check ",
        uin_eff-uin_low, uin_high-uin_eff, uin_eff );
    assert( uin_eff<=uin_high && uin_low<=uin_eff);
    trace5("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last ", 1-E, 1-E_test,
        E_test-E_low, E_high-E_test, linear_inversion );
    assert( E_test <= E_high && E_low <= E_test); 
  }

  uin_high = max ( uin_eff + OPT::abstol, uin_eff * (1 + OPT::reltol) );
  uin_low  = min ( uin_eff - OPT::abstol, uin_eff * (1-OPT::reltol) );
  trace3(("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last E" + _c->label()).c_str(),
      uin_low, uin_eff, uin_high);

  E_high = cc->__step( uin_high, E_old, CKT_BASE::_sim->_last_time  );
  E_low  = cc->__step( uin_low,  E_old, CKT_BASE::_sim->_last_time  ); 
  trace6(("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last E" + _c->label()).c_str(),
      uin_eff,  E-E_low, E_high - E_low, E, 1-E, E_high-E );
  trace3(("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last E" + _c->label()).c_str(), 
      E-E_low , E_high-E, linear_inversion  );

  if( ( E_old < E_high ) && ( E_low <= E_old ))
    _c->set_order(0);

  if ((double)E_high<(double)E_low){
    error( bDANGER, "COMMON_BUILT_IN_RCD:: sanitycheck ( %LE < %LE )", E_high, E_low);
    assert(false);
  }

  if (linear_inversion)  _c->set_order(0);
  //assert ( uin_eff == _c->get_tr());
  //
  //
  //
  assert(is_number(E_high-E_low));
  assert( (double) E_high-(double) E_low >= 0);
  assert((double) E_high>= (double) E_low);
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

  trace1("MODEL_BUILT_IN_RCD_SYM_V4::do_tr_stress_last done", _c->get_tr_noise());

}

/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V4::__Re(double uin, const COMMON_COMPONENT* c) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Re0 * exp( - cc->_Re1 * uin );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V4::__Rc(double uin, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Rc0 * exp( cc->_Rc1 * uin );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V4::__Ge(double uin, const COMMON_COMPONENT* c ) const
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return exp( cc->_Re1 * uin )/cc->_Re0;
}
/*--------------------------------------------------------------------------*/
/* E(t=inf) */
double MODEL_BUILT_IN_RCD_SYM_V4::__E(double uin, const COMMON_COMPONENT* c ) const 
{
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  double tau_e_here = __Re( uin, cc);
  double tau_c_here = __Rc( uin, cc);
  return( tau_c_here / ( tau_e_here + tau_c_here ));
}
/*--------------------------------------------------------------------------*/
long double MODEL_BUILT_IN_RCD_SYM_V4::__Edu(long double uin, long double cur, const COMMON_COMPONENT* cc ) const
{
  const COMMON_BUILT_IN_RCD* c = dynamic_cast<const COMMON_BUILT_IN_RCD*>(cc) ;
  long double Rc0=c->_Rc0;
  long double Re0=c->_Re0;
  long double Rc1=c->_Rc1;
  long double Re1=c->_Re1;
  long double t=_sim->_last_time;

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
long double MODEL_BUILT_IN_RCD_SYM_V4::__uin_iter(long double& uin, double
    E_old, double E, const COMMON_COMPONENT* ccmp ) const { const
  COMMON_BUILT_IN_RCD* c = dynamic_cast<const COMMON_BUILT_IN_RCD*>(ccmp);
  assert(false);
  return c->__uin_iter( uin, E_old ,E,0,0 ); 
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// old.
int  MODEL_BUILT_IN_RCD_SYM_V4::tt_region(const COMPONENT* brh) const
{
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;

  assert(c);
  return ( (c->_Ccgfill)->region() );
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V4::do_precalc_last(COMMON_COMPONENT* ccmp, const CARD_LIST*)const
{
  COMMON_BUILT_IN_RCD* cc = dynamic_cast<COMMON_BUILT_IN_RCD*>(ccmp);
  assert(cc);
  const MODEL_BUILT_IN_RCD_SYM_V4* m=this;

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
ADP_NODE_RCD* MODEL_BUILT_IN_RCD_SYM_V4::new_adp_node(const COMPONENT* c) const
{
  return new ADP_NODE_RCD(c);
}
