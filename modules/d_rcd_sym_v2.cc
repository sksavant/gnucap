/* vim:ts=8:sw=2:et:
 *
 * (c)2010 felix salfelder
 * nonGPL ?
 */

// das ist die version, die es nicht geschafft hat, weil ein sprung in tau ist.


#include "e_aux.h"
#include "e_storag.h"
// #include "d_mos_base.h"

#include "globals.h"
#include "e_elemnt.h"
#include "e_adp.h"
#include "d_rcd_sym_v2.h"

/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V2::do_stress_apply( COMPONENT*  ) const
{

}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V2::tr_stress() 
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
  static DEV_BUILT_IN_RCD_SYM_V2 p3d;
  static MODEL_BUILT_IN_RCD_SYM_V2 p3(&p3d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d2(&model_dispatcher, "rcdsym_v2", &p3);
}
///*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V2::do_expand(  COMPONENT* c) const
{
  DEV_BUILT_IN_RCD_SYM_V2* d = dynamic_cast<DEV_BUILT_IN_RCD_SYM_V2*>(c);
  assert(d);
  // d->expand();
  
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD_SYM_V2::dev_type()const
{
  return "rcd";
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V2::MODEL_BUILT_IN_RCD_SYM_V2(const BASE_SUBCKT* p) 
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  uref=1; 
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V2::MODEL_BUILT_IN_RCD_SYM_V2(const MODEL_BUILT_IN_RCD_SYM_V2& p)  
  : MODEL_BUILT_IN_RCD_SYM(p)
{ 
  
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V2::expand()
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

  trace0("DEV_BUILT_IN_RCD_SYM_V2::expand()");

  if (_sim->is_first_expand()) {
    precalc_first();
    precalc_last();
    // local nodes
    //assert(!(_n[n_ic].n_()));
    //BUG// this assert fails on a repeat elaboration after a change.
    //not sure of consequences when new_model_node called twice.
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
void MODEL_BUILT_IN_RCD_SYM_V2::do_tr_stress( const COMPONENT* brh) const
{
  const MODEL_BUILT_IN_RCD_SYM_V2* m = this;
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(c->common());
  //double  fill = _n[n_ic].v0();

  double fill = c->_Ccgfill->get_total();
  double uin = c->involts();

  trace2("MODEL_BUILT_IN_RCD_SYM_V2::do_tr_stress ", fill, uin );
  trace1("MODEL_BUILT_IN_RCD_SYM_V2::tr_stress ", c->involts() );
  assert (fill>=0);
  if (fill > 1.000001 ){
    error(bDANGER, " RCD_V2 fill %f too big\n", fill );
    fill = 1;
  }

  if( m->positive) {
    if ( c->_Ccgfill->get_total() < 0 ){
      trace1(("DEV_BUILT_IN_RCD_SYM_V2::tr_stress fill is negative: " +
            short_label()).c_str() ,  c->_Ccgfill->get_total() );
    }
    if (  c->involts() < -1e-10 ){
      trace1(("DEV_BUILT_IN_RCD_SYM_V2::tr_stress input is negative: " + 
            short_label()).c_str() ,   c->involts() );
      assert (false );
    }
  }
  assert(!m->use_net());

  //double  fill = _n[n_ic].v0();
  assert (fill==fill);
  assert (uin==uin);

  //trace3("DEV_BUILT_IN_RCD_SYM_V2::tr_stress ", _n[n_b].v0(), _n[n_u].v0(), _n[n_ic].v0() );
  // use positive values for pmos

  double h = _sim->_dt0;

  double tau_e_here = m->__Re( uin, cc);
  double tau_c_here = m->__Rc( uin, cc);

  double uend=1/(1+tau_e_here/tau_c_here);
  double tau;

  if (fill < uend){
    tau= tau_e_here;
  }else{
    tau= tau_c_here;
  }

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

  trace4("DEV_BUILT_IN_RCD_SYM_V2::tr_stress ", tau_c_here, tau_e_here, _sim->_Time0, _sim->_time0 );
  trace6("DEV_BUILT_IN_RCD_SYM_V2::tr_stress ", fill, h, tau, newfill, uin, uend );

  c->_Ccgfill->tr_add(newfill-fill);
  trace4("DEV_BUILT_IN_RCD_SYM_V2::tr_stress ", fill, h, tau, (newfill-fill)/h );
  assert(newfill >= 0 );

  if (newfill > 1.000001 ){
    error(bDANGER, ("* RCD_V2 %f too big\n" + long_label() ).c_str() , newfill );
    newfill=1;
  }
  assert(newfill==newfill);
  assert(h>0);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::dvth(const COMPONENT* brh) const{
  const DEV_BUILT_IN_RCD* c = prechecked_cast<const DEV_BUILT_IN_RCD*>(brh);
  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(c->common());

//  stupid hack.
  if (_sim->analysis_is_tt()){
    return (c->_Ccgfill->get_tt()+ cc->_wcorr )* cc->_weight;
  } else {
    assert(is_number( c->_Ccgfill->get_total()+ cc->_wcorr )* cc->_weight);
    return (c->_Ccgfill->get_total()+ cc->_wcorr )* cc->_weight;
  }
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::__Re(double uin, const COMMON_COMPONENT* c) const {
  assert(c);
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  double Y = cc->Recommon0 * exp(cc->Uref* cc->_lambda) ;
        trace2(" " , Y , cc->_Re0);
  assert( Y = cc->_Re0);
  return cc->_Re0 / exp( cc->_lambda * uin );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::__Rc(double uin, const COMMON_COMPONENT* c ) const {
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  double X = cc->Rccommon0;
  return X * exp( cc->_lambda * uin );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::__Ge(double uin, const COMMON_COMPONENT* c ) const {
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return uin/cc->X; 
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_RCD_SYM_V2::DEV_BUILT_IN_RCD_SYM_V2()
  :DEV_BUILT_IN_RCD() {
//  _n = _nodes;
//  attach_common(&Default_BUILT_IN_RCD);

//  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
int  MODEL_BUILT_IN_RCD_SYM_V2::tt_region(const COMPONENT* brh) const{
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;

  assert(c);
  return ( (c->_Ccgfill)->region() );
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V2::do_precalc_last(COMMON_COMPONENT* ccmp, const CARD_LIST*)const{
  COMMON_BUILT_IN_RCD* cc = dynamic_cast<COMMON_BUILT_IN_RCD*>(ccmp);
  const MODEL_BUILT_IN_RCD_SYM_V2* m=this;
  assert(cc);
  cc->_Rc0 = cc->Rccommon0;

  cc->X = cc->Rccommon0;
  cc->_Re0 = cc->Recommon0 * exp(cc->Uref* cc->_lambda) ;

  double re_0 = m->__Re(0.0,cc);
  double rc_0 = m->__Rc(0.0,cc);
  double rc_uref = m->__Rc(cc->Uref,cc);
  double re_uref = m->__Re(cc->Uref,cc);

  double uend_uref = (1 / ( re_uref / rc_uref +1));

  // vth(e ) = (e+_wcorr) * _wt
  //_wcorr = -uend(0) 
  cc->_wcorr = - 1/ (1+re_0/rc_0);
  cc->_zero = 1/ (1+re_0/rc_0);

  // vth(e(uref))=! Uref*weight
  // vth( uend_uref ) = (  uend_uref+_wcorr ) * wt;
  cc->_weight = cc->weight * cc->Uref / (  uend_uref+cc->_wcorr );

  //double test = ( rc / ( 1+rc*ge )  ) ;

  //trace6("COMMON_BUILT_IN_RCD::precalc_last v2", Recommon, Rccommon0, rc, X, _wcorr, test);
  trace6("MODEL_BUILT_IN_RCD_SYM_V2::do_precalc_last" , uend_uref, cc->weight, cc->_weight, re_0, re_uref, m->uref );
  trace2("MODEL_BUILT_IN_RCD_SYM_V2::do_precalc_last", re_0, re_uref);
  trace5("MODEL_BUILT_IN_RCD_SYM_V2::do_precalc_last", rc_0, rc_uref,  cc->Recommon0, cc->__Re(cc->Uref), cc->Uref);
  assert(rc_0 <= rc_uref); // incresging...
  assert(re_0 >= re_uref); // decreasing...
  assert( fabs(cc->Recommon0 - cc->__Re(cc->Uref))/cc->Recommon0 < 1e-12);
  assert( abs(cc->Uref*cc->weight  - (uend_uref + cc->_wcorr ) * cc->_weight) < 1e-12);
}

/*--------------------------------------------------------------------------*/
ADP_NODE_RCD* MODEL_BUILT_IN_RCD_SYM_V2::new_adp_node(const COMPONENT* c) const{
  return new ADP_NODE_RCD(c);
}
