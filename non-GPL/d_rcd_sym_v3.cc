/* vim:ts=8:sw=2:et:
 *
 * (c)2010 felix salfelder
 * nonGPL ?
 */


#include "e_aux.h"
#include "e_storag.h"
// #include "d_mos_base.h"

#include "globals.h"
#include "e_elemnt.h"
#include "u_adp.h"
#include "d_rcd_sym_v3.h"

/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_stress_apply( COMPONENT*  ) const
{

}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V3::tr_stress() const
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
    d3(&model_dispatcher, "rcdsym_v3", &p4);
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
  uref=1; 
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
double MODEL_BUILT_IN_RCD_SYM_V3::__tau(double uin, const COMMON_COMPONENT* cc)const{
        
  double tau_e_here = __Re( uin, cc);
  // double tau_e_inv = __Ge( uin, cc);
  double tau_c_here = __Rc( uin, cc);
  return tau_c_here  / ( tau_c_here/tau_e_here +1 );
}
/*--------------------------------------------------------------------------*/

void MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress( const COMPONENT* brh) const
{
  const MODEL_BUILT_IN_RCD_SYM_V3* m = this;
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(c->common());
  //double  fill = _n[n_ic].v0();

  double fill = c->_Ccgfill->get_total();
  double uin = c->involts();

  trace2("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress ", fill, uin );
  trace1("MODEL_BUILT_IN_RCD_SYM_V3::do_tr_stress ", c->involts() );
  assert (fill>=0);
  if (fill > 1.000001 ){
    error(bDANGER, " RCD_V3 fill %f too big\n", fill );
    fill = 1;
  }

  if( m->positive) {
    if ( c->_Ccgfill->get_total() < 0 ){
      trace1(("DEV_BUILT_IN_RCD_SYM_V3::tr_stress fill is negative: " +
            short_label()).c_str() ,  c->_Ccgfill->get_total() );
    }
    if (  c->involts() < -1e-10 ){
      trace1(("DEV_BUILT_IN_RCD_SYM_V3::tr_stress input is negative: " + 
            short_label()).c_str() ,   c->involts() );
      assert (false );
    }
  }
  assert(!m->use_net());

  //double  fill = _n[n_ic].v0();
  assert (fill==fill);
  assert (uin==uin);

  //trace3("DEV_BUILT_IN_RCD_SYM_V3::tr_stress ", _n[n_b].v0(), _n[n_u].v0(), _n[n_ic].v0() );
  // use positive values for pmos

  double h = _sim->_dt0;

  double tau_e_here = m->__Re( uin, cc);
  //double tau_e_inv = m->__Ge( uin, cc);
  double tau_c_here = m->__Rc( uin, cc);

  double uend = tau_c_here / ( tau_e_here + tau_c_here );
  double tau = __tau(uin, cc);
  double tau_up = cc->__tau_up(uin);

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

  // double bulkpot=_n[n_b].v0();

  trace5("MODEL_BUILT_IN_RCD_SYM_V3::tr_stress ", tau_c_here, tau_e_here, _sim->_Time0, _sim->_time0, tau_up );
  trace6("MODEL_BUILT_IN_RCD_SYM_V3::tr_stress ", fill, h, tau, newfill, uin, uend );

//  _sim->_v0[_n[n_ic]->m_()] = newfill ; 
//  _sim->_vdc[_n[n_ic]->m_()] = newfill ; 

  c->_Ccgfill->tr_add(newfill-fill);
  trace4("DEV_BUILT_IN_RCD_SYM_V3::tr_stress ", fill, h, tau, (newfill-fill)/h );
  assert(newfill >= 0 );

  if (newfill > 1.000001 ){
    error(bDANGER, ("* RCD_V3 %f too big\n" + long_label() ).c_str() , newfill );
    newfill=1;
  }
  assert(newfill==newfill);
  assert(h>0);
  assert(is_number(tau));
  assert(is_number(uend));
}
/*--------------------------------------------------------------------------*/
// precalc doesnt know device!!
//void MODEL_BUILT_IN_RCD_SYM_V3::do_tt_prepare( COMPONENT* brh) const{
//  const DEV_BUILT_IN_RCD* c = prechecked_cast<const DEV_BUILT_IN_RCD*>(brh);
//  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(c->common());
//  //std::cout << "* " << cc->_wcorr << "\n";
//  c->_Ccgfill->tt_set( -cc->_wcorr );
//
//  trace1( ( "MODEL_BUILT_IN_RCD_SYM_V3::do_tt_prepare" + brh->short_label()).c_str(),  -cc->_wcorr );
//}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::dvth(const COMPONENT* brh) const{
  const DEV_BUILT_IN_RCD* c = prechecked_cast<const DEV_BUILT_IN_RCD*>(brh);
  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(c->common());

//  stupid hack.
  if (_sim->analysis_is_tt()){
    return (c->_Ccgfill->get_tt()+ cc->_wcorr ) * cc->_weight;
  } else {
    return (c->_Ccgfill->get_total()+ cc->_wcorr ) * cc->_weight;
  }
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::__Re(double uin, const COMMON_COMPONENT* c) const {
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Re0/uin;
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::__Rc(double uin, const COMMON_COMPONENT* c ) const {
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return cc->_Rc0 * exp( cc->_Rc1 * uin );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V3::__Ge(double uin, const COMMON_COMPONENT* c ) const {
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return uin/cc->_Re0; 
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_RCD_SYM_V3::DEV_BUILT_IN_RCD_SYM_V3()
  :DEV_BUILT_IN_RCD()
{
//  _n = _nodes;
//  attach_common(&Default_BUILT_IN_RCD);

//  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
int  MODEL_BUILT_IN_RCD_SYM_V3::tt_region(const COMPONENT* brh) const{
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;

  assert(c);
  return ( (c->_Ccgfill)->region() );
}


/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V3::do_precalc_last(COMMON_COMPONENT* ccmp, const CARD_LIST* par_scope)const
{
  COMMON_BUILT_IN_RCD* cc = dynamic_cast<COMMON_BUILT_IN_RCD*>(ccmp);
  assert(cc);
  const MODEL_BUILT_IN_RCD_SYM_V3* m=this;

  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->Uref, cc->Recommon, cc->Rccommon0);
  //long double ueff = cc->Uref; // ( exp ( lambda * Uref ) - 1 );

  double up   =  cc->Recommon;
  double down =  cc->Rccommon0;

  cc->_Re0 = (down  + up ) * up / down;
  cc->_Rc0 = down;
  cc->_Rc1 = log(up/down+1);


  double uend_bad = (cc->Uref / (cc->__Re(cc->Uref) / cc->__Rc(cc->Uref) +1));

  cc->_wcorr = double ( cc->Uref / uend_bad );

  cc->_weight = cc->weight;
  // sanity check.
  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->__tau_up(cc->Uref), cc->Recommon, cc->Rccommon0);
  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->_Rc1, cc->_Re0, cc->_Rc0);
  trace2("MODEL_BUILT_IN_RCD::do_precalc_last", m->__tau(0,cc), down);

  assert( cc->weight != 0 );
  assert( cc->_weight != 0 );
  assert( abs( m->__tau(cc->Uref,cc) - cc->Recommon)/cc->Recommon <1e-6 );
  assert( abs( m->__tau(0,cc) - down)/down <1e-6 );
  assert( is_number( cc->_Rc1 ) );
  assert( is_number( cc->_Rc0 ) );
}
/*--------------------------------------------------------------------------*/
ADP_NODE* MODEL_BUILT_IN_RCD_SYM_V3::new_adp_node(const COMPONENT* c) const{
  return new ADP_NODE_RCD(c);
}
