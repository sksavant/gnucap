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
#include "d_rcd_sym_v2.h"

/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V2::do_stress_apply( COMPONENT*  ) const
{

}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V2::tr_stress() const
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
namespace MODEL_BUILT_IN_RCD_DISPATCHER { 
  static DEV_BUILT_IN_RCD_SYM_V2 p3d;
  static MODEL_BUILT_IN_RCD_SYM_V2 p3(&p3d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d2(&model_dispatcher, "rcdsym_v2", &p3);
}
///*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_SYM_V2::do_expand(const  COMPONENT* ) const
{
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD_SYM_V2::dev_type()const
{
  return "rcd";
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V2::MODEL_BUILT_IN_RCD_SYM_V2(const BASE_SUBCKT* p) 
  : MODEL_BUILT_IN_RCD(p){ }
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_SYM_V2::MODEL_BUILT_IN_RCD_SYM_V2(const MODEL_BUILT_IN_RCD_SYM_V2& p)  
  : MODEL_BUILT_IN_RCD(p){ }
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD_SYM_V2::expand() {
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
void MODEL_BUILT_IN_RCD_SYM_V2::do_tr_stress( const COMPONENT* brh) const {

  const MODEL_BUILT_IN_RCD_SYM_V2* m = this;
  const DEV_BUILT_IN_RCD* c = (const DEV_BUILT_IN_RCD*) brh;
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(c->common());
  //double  fill = _n[n_ic].v0();
  //

  double  fill = c->_Ccgfill->get_total();
  double  uin = c->involts();

  trace1("MODEL_BUILT_IN_RCD_SYM_V2::tr_stress ", c->involts() );

  bool use_1_uend = m -> norm_uin;

  if( m->positive) {
    if ( c->_Ccgfill->get_total() < 0 ){
      trace1(("DEV_BUILT_IN_RCD::tr_stress fill is negative: " +
            short_label()).c_str() ,  c->_Ccgfill->get_total() );
    }
    if (  c->involts() < -1e-10 ){
      trace1(("DEV_BUILT_IN_RCD::tr_stress input is negative: " + 
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
  double re = 1/m->__Ge(uin,cc);
  double ge = m->__Ge(uin,cc);
  double rc = m->__Rc(uin,cc);
  double uend = 1 / (1/ge/rc +1) ;

  double tau = ( rc / ( 1+rc*ge )  ) ;

  double newfill;
  switch(_sim->_stepno){
        case 0:
        case 1:
        default:
        newfill = (fill - uend) * exp(-h/tau) + uend;
  }
  assert(newfill >= 0);
  assert(newfill <= 1);

  // double bulkpot=_n[n_b].v0();

  trace6("DEV_BUILT_IN_RCD_SYM_V2::tr_stress ", fill, h, tau, newfill, uin, uend );

//  _sim->_v0[_n[n_ic]->m_()] = newfill ; 
//  _sim->_vdc[_n[n_ic]->m_()] = newfill ; 

  c->_Ccgfill->tr_add(newfill-fill);
  assert(newfill==newfill);
  trace4("DEV_BUILT_IN_RCD::tr_stress ", fill, h, tau, (newfill-fill)/h );
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::__Rc(double uin, const COMMON_COMPONENT* c) const {
  assert(c);
   const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
   double ret = ( cc->_Rc0 + uin* cc->_Rc1 ); 
   return ret;
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::__Re(double uin, const COMMON_COMPONENT* c ) const {
  return 1/__Ge(uin,c);
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_SYM_V2::__Ge(double uin, const COMMON_COMPONENT* c ) const {
  const COMMON_BUILT_IN_RCD* cc = dynamic_cast<const COMMON_BUILT_IN_RCD*>(c) ;
  return uin/cc->X; 
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_RCD_SYM_V2::DEV_BUILT_IN_RCD_SYM_V2()
  :DEV_BUILT_IN_RCD()
{
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
ADP_NODE* MODEL_BUILT_IN_RCD_SYM_V2::new_adp_node(const COMPONENT* c) const{
  return new ADP_NODE_RCD(c);
}
