/* $Id: d_rcd.cc,v 1.9 2010-09-07 07:46:21 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 *
 * RCD device ... 
 *
 * (c) 2010 Felix Salfelder
 *
 * GPLv3
 */

#include "e_aux.h"
#include "e_adp.h"
#include "e_storag.h"
#include "globals.h"
#include "e_elemnt.h"
#include "d_rcd.h"

/*--------------------------------------------------------------------------*/
class ADP_NODE;
static bool dummy=false;
const double NA(NOT_INPUT);
const double INF(BIGBIG);
/*--------------------------------------------------------------------------*/
int DEV_BUILT_IN_RCD::_count = -1;
int COMMON_BUILT_IN_RCD::_count = -1;
static COMMON_BUILT_IN_RCD Default_BUILT_IN_RCD(CC_STATIC);
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_RCD::_count = 0;
/*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_RCD_DISPATCHER { 
  static DEV_BUILT_IN_RCD p1d;
  static MODEL_BUILT_IN_RCD_NET p1(&p1d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d1(&model_dispatcher, "rcdnet|rcdmodel", &p1);
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::tr_accept() {
  trace3(("DEV_BUILT_IN_RCD::tr_accept (stress) " + long_label()).c_str(),
      _sim->_time0, _sim->_Time0, involts());
  // assert(subckt()); subckt()->tr_accept();
  assert(is_number((double)_tr_fill));
  tr_stress();
  assert(is_number((double)_tr_fill));
  q_eval();
}
/*--------------------------------------------------------------------------*/
void SDP_BUILT_IN_RCD::init(const COMMON_COMPONENT* cc)
{
  assert(cc);
  SDP_CARD::init(cc);
}
/*--------------------------------------------------------------------------*/
TDP_BUILT_IN_RCD::TDP_BUILT_IN_RCD(const DEV_BUILT_IN_RCD*)
{
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD::MODEL_BUILT_IN_RCD(const BASE_SUBCKT* p)
  :MODEL_CARD(p),
   anneal(true),
   Remodel(1e6),
   Re1(1),
   Re0(1e6),
   Rc1(1),
   Rc0(1e6),
   flags(int(USE_OPT)),
   uref(0),
   modelparm(0),
   positive(true),
   norm_uin(false)
{
  if (ENV::run_mode != rPRE_MAIN) {
    ++_count;
  }else{
  }
  set_default(&_tnom_c, OPT::tnom_c);
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::do_tr_stress( const COMPONENT* ) const {
  unreachable(); // stress done  by device.
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD::MODEL_BUILT_IN_RCD(const MODEL_BUILT_IN_RCD& p)
  :MODEL_CARD(p),
   anneal(p.anneal),
   Remodel(p.Remodel),
   Re1(p.Re1),
   Re0(p.Re0),
   Rc1(p.Rc1),
   Rc0(p.Rc0),
   flags(p.flags),
   uref(p.uref),
   modelparm(p.modelparm),
   positive(p.positive),
   norm_uin(p.norm_uin)
{
  if (ENV::run_mode != rPRE_MAIN) {
    ++_count;
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD::P(const COMPONENT* ) const{
  assert(false);
  return ( NAN ); //c->_Ccgfill->get()+ _tr_fill ) * cc->_weight * cc->_wcorr;
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD::__Rc(double uin, const COMMON_COMPONENT* ccmp)const {
  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(ccmp);
  double ret = ( cc->_Rc0 + uin * cc->_lambda * cc->_Rc1 ); 
  assert (is_number(cc->_lambda));
  assert (is_number(cc->_Rc0));
  assert (is_number(cc->_Rc1));
  assert (is_number(ret));
  return ret;
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD::__Re(double , const COMMON_COMPONENT* cc)const {
  const COMMON_BUILT_IN_RCD* c = prechecked_cast<const COMMON_BUILT_IN_RCD*>(cc);

  return c->_Re0;
//  return c->__Re(uin);
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD::__Ge(double, const COMMON_COMPONENT* )const {
  return NAN;
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD_NET::dev_type()const
{
  return "rcdnet";
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD::dev_type()const
{
  unreachable();
  if (dummy == true) {
    return "rcdmodel?";
  }else{untested();//235
    return MODEL_CARD::dev_type();
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::set_dev_type(const std::string& new_type)
{
  untested();
  if (Umatch(new_type, "rcdmodel ")) {
    dummy = true;
  }else{
    MODEL_CARD::set_dev_type(new_type);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::precalc_first()
{
    const CARD_LIST* par_scope = scope();
    assert(par_scope);
    MODEL_CARD::precalc_first();
    e_val(&(this->anneal), true, par_scope);
    e_val(&(this->Remodel), 1e6, par_scope);
    e_val(&(this->Re0), 1.0, par_scope);
    e_val(&(this->Rc0), 1.0, par_scope);
    e_val(&(this->Re1), 1.0, par_scope);
    e_val(&(this->Rc1), 1.0, par_scope);
    e_val(&(this->flags), int(USE_OPT), par_scope);
    e_val(&(this->uref), NA, par_scope);
    e_val(&(this->modelparm), 0, par_scope);
    e_val(&(this->positive), true, par_scope);
    e_val(&(this->norm_uin), false, par_scope);
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::precalc_last()
{
    MODEL_CARD::precalc_last();
}
/*--------------------------------------------------------------------------*/
SDP_CARD* MODEL_BUILT_IN_RCD::new_sdp(COMMON_COMPONENT* c)const
{
  assert(c);
  if (COMMON_BUILT_IN_RCD* cc = dynamic_cast<COMMON_BUILT_IN_RCD*>(c)) {
    if (cc->_sdp) {
      cc->_sdp->init(cc);
      assert(cc->_sdp);
      return cc->_sdp;
    }else{
      delete cc->_sdp;
      return new SDP_BUILT_IN_RCD(c);
    }
  }else{
    trace0("MODEL_BUILT_IN_RCD::new_sdp, MODEL_CARD");
    return MODEL_CARD::new_sdp(c);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0: untested(); break;
  case 1: _tnom_c = value; break;
  case 2: anneal = value; break;
  case 3: Remodel = value; break;
  case 4: Re0 = value; break; // former Re
  case 5: Rc0 = value; break;
  case 6: flags = value; break;
  case 7: uref = value; break;
  case 8: modelparm = value; break;
  case 9: positive = value; break;
  case 10: norm_uin = value; break;
  default: throw Exception_Too_Many(i, 7, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_RCD::param_is_printable(int i)const
{
  switch (MODEL_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0:  return (false);
  case 1:  return (true);
  case 2:  return (true);
  case 3:  return (true);
  case 4:  return (true); // Re
  case 5:  return (true); //Rc
  case 6:  return (!(flags & USE_OPT));
  case 7:  return (uref.has_hard_value());
  case 8:  return (modelparm.has_hard_value());
  case 9:  return (positive.has_hard_value());
  case 10:  return (norm_uin.has_hard_value());
  default: return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD::param_name(int i)const
{
  switch (MODEL_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0:  return "=====";
  case 1:  return "tnom";
  case 2:  return "anneal";
  case 3:  return "rem";
  case 4:  return "Re";
  case 5:  return "Rc";
  case 6:  return "flags";
  case 7:  return "uref";
  case 8:  return "modelparm";
  case 9:  return "positive";
  case 10:  return "norm_uin";
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (MODEL_BUILT_IN_RCD::param_count() - 1 - i) {
    case 4:  return "Re";
    case 5:  return "Rc";
    case 9:  return "pos";
    default: return "";
    }
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_RCD::param_value(int i)const
{
  switch (MODEL_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0:  unreachable(); return "";
  case 1:  return _tnom_c.string();
  case 2:  return anneal.string();
  case 3:  return Remodel.string();
  case 4:  return Re0.string(); // former Re
  case 5:  return Rc0.string(); // former Rc
  case 6:  return flags.string();
  case 7:  return uref.string();
  case 8:  return modelparm.string();
  case 9:  return positive.string();
  case 10:  return norm_uin.string();
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::do_precalc_last(COMMON_COMPONENT* ccc, const CARD_LIST* )const
{
  COMMON_BUILT_IN_RCD* cc = dynamic_cast<COMMON_BUILT_IN_RCD*>(ccc);
  assert(cc);
  //const MODEL_BUILT_IN_RCD* m = this;
  //
  if (is_number(cc->Uref)){
  assert( is_number( cc->Rccommon0 ) && (double)cc->Rccommon0 != NA );
  assert( is_number( cc->Rccommon1 ) );
  }

  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->Uref, cc->Recommon0, cc->Rccommon0);
  long double ueff = cc->Uref; // ( exp ( lambda * Uref ) - 1 );

  double up   =  cc->Recommon0;
  double down =  cc->Rccommon0;

  double rad = double(ueff*ueff*up*up + 2.0*(up*up + up*down)*ueff + up*up - 2*up*down + down*down);
  //double s = ueff*up + up - down;
  double up_res = double ( 1.0/2.0*(ueff*up + up - down + sqrt(rad))/ueff );
  double down_res = down;

  cc->_Re0 = up_res;
  cc->_Rc0 = down_res;
  cc->_Rc1 = up_res;
  // double _rr = _rr_.subs(runter=runter, u_gate_=uref)

  // double _rh = _rh_.subs(runter=runter, u_gate_=uref)  
  double Eend_bad = (cc->Uref / (cc->__Re(cc->Uref) / cc->__Rc(cc->Uref) +1));

  cc->_wcorr = double ( cc->Uref / Eend_bad );
  cc->_weight = cc->weight;
  // sanity check.
  trace3("MODEL_BUILT_IN_RCD::do_precalc_last", cc->__tau_up(cc->Uref), cc->Recommon0, cc->Rccommon0);
  trace3("MODEL_BUILT_IN_RCD::do_precalc_last",cc->_Rc1, cc->_Re0, cc->_Rc0);
  assert( cc->weight != 0 );
  assert( cc->_weight != 0 );
  assert( abs( cc->__tau_up(cc->Uref) - cc->Recommon0)/cc->Recommon0 <1e-6 );
  assert( is_number( cc->_Rc1 ) );
  assert( is_number( cc->_Rc0 ) );
}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_RCD::is_valid(const COMPONENT* d)const
{
  assert(d);
  return MODEL_CARD::is_valid(d);
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::tr_eval(COMPONENT*)const
{untested();//425
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_RCD::COMMON_BUILT_IN_RCD(int c)
  :COMMON_COMPONENT(c),
   perim(0.0),
   weight(1.0),
   Recommon0(NA),
   Recommon1(NA),
   Rccommon0(NA),
   Rccommon1(NA),
   Uref(0.0),
   mu(1.0),
   lambda(1.0),
   dummy_capture(false),
   dummy_emit(false),
   _sdp(0),
   cj_adjusted(NA)
{
  trace1("COMMON_BUILT_IN_RCD::COMMON_BUILT_IN_RCD", (double) Uref);
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_RCD::COMMON_BUILT_IN_RCD(const COMMON_BUILT_IN_RCD& p)
  :COMMON_COMPONENT(p),
   perim(p.perim),
   weight(p.weight),
   Recommon0(p.Recommon0),
   Recommon1(p.Recommon1),
   Rccommon0(p.Rccommon0),
   Rccommon1(p.Rccommon1),
   Uref(p.Uref),
   mu(p.mu),
   lambda(p.lambda),
   dummy_capture(p.dummy_capture),
   dummy_emit(p.dummy_emit),
   _sdp(0),
   cj_adjusted(p.cj_adjusted)
{
  trace1("COMMON_BUILT_IN_RCD::COMMON_BUILT_IN_RCD(copy)", (double) Uref);
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_RCD::~COMMON_BUILT_IN_RCD()
{
  --_count;
  delete _sdp;
}
/*--------------------------------------------------------------------------*/
bool COMMON_BUILT_IN_RCD::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_BUILT_IN_RCD* p = dynamic_cast<const COMMON_BUILT_IN_RCD*>(&x);
  return (p
    && perim == p->perim
    && weight == p->weight
    && Recommon0 == p->Recommon0
    && Recommon1 == p->Recommon1
    && Rccommon0 == p->Rccommon0
    && Rccommon1 == p->Rccommon1
    && Uref == p->Uref
    && mu == p->mu
    && lambda == p->lambda
    && dummy_capture == p->dummy_capture
    && dummy_emit == p->dummy_emit
    && _sdp == p->_sdp
    && COMMON_COMPONENT::operator==(x));
}
/*--------------------------------------------------------------------------*/
int COMMON_BUILT_IN_RCD::param_count()const
    {return (3 + COMMON_COMPONENT::param_count());}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_RCD::set_param_by_index(int I, std::string& Value, int Offset)
{
  trace3("COMMON_BUILT_IN_RCD::set_param_by_index ",I, COMMON_BUILT_IN_RCD::param_count() - 1 - I, Value );
  switch (COMMON_BUILT_IN_RCD::param_count() - 1 - I) {
  case 0:  perim = Value; break;
  case 1:  weight = Value; 
//  order          double _hl, double _hc, double _rl, double _rc)
           trace1("wt", Value);
           break;
  case 2:  Recommon1 = Value;
           break;
  case 3:  Recommon0 = Value; 
           break;
  case 4:  Rccommon1 = Value; break;
           break;
  case 5:  Rccommon0 = Value; break;
           break;
  case 6:  Uref = Value;
           break;
  case 7:  mu = Value; break;
  case 8:  lambda = Value; break;
  case 9:  Uref = Value; break;
  case 10:  dummy_emit = Value; break;
  default: COMMON_COMPONENT::set_param_by_index(I, Value, Offset);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_BUILT_IN_RCD::param_is_printable(int i)const
{
  switch (COMMON_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0:  return (perim != 0.);
  case 1:  return (true);
  case 2:  return (true);
  case 3:  return (true);
  case 4:  return (true);
  case 5:  return (true);
  case 6:  return (true);
  case 7:  return (true);
  case 8:  return (true);
  default: return COMMON_COMPONENT::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_RCD::param_name(int i)const
{
  switch (COMMON_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0:  return "perim";
  case 1:  return "weight";
  case 2:  return "re1";
  case 3:  return "re0";
  case 4:  return "rc1";
  case 5:  return "rc0";
  case 6:  return "mu";
  case 7:  return "lam";
  case 8:  return "rcdummy";
  case 9:  return "uref";
  //case 10:  return "norm_uin";
  default: return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_RCD::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (COMMON_BUILT_IN_RCD::param_count() - 1 - i) {
    case 0:  return "";
    case 1:  return "";
    case 2:  return "";
    case 3:  return "";
    case 4:  return "";
    case 5:  return "";
    case 6:  return "";
    case 7:  return "";
    case 8:  return "";
    default: return "";
    }
  }else{untested();//281
    return COMMON_COMPONENT::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_RCD::param_value(int i)const
{
  switch (COMMON_BUILT_IN_RCD::param_count() - 1 - i) {
  case 0:  return perim.string();
  case 1:  return weight.string();
  case 2:  return Recommon1.string();
  case 3:  return Recommon0.string();
  case 4:  return Rccommon1.string();
  case 5:  return Rccommon0.string();
  case 6:  return Uref.string();
  case 7:  return mu.string();
  case 8:  return lambda.string();
  case 9:  return Uref.string();
  case 10:  return dummy_emit.string();
  default: return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_RCD::expand(const COMPONENT* d)
{
  COMMON_COMPONENT::expand(d);
  attach_model(d);
  COMMON_BUILT_IN_RCD* c = this;
  const MODEL_BUILT_IN_RCD* m = dynamic_cast<const MODEL_BUILT_IN_RCD*>(model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "rcd");
  }else{
  }

  if ((double)Recommon0 == NA) { Recommon0 = m->Re0;}
  if ((double)Rccommon0 == NA) { Rccommon0 = m->Rc0;}

  trace6(("COMMON_BUILT_IN_RCD::expand" + d->short_label()).c_str(), Rccommon0,
      Recommon0, m->Re0, m->Rc0, Uref, m->uref );
  // size dependent
  //delete _sdp;
  _sdp = m->new_sdp(this);
  assert(_sdp);
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(_sdp);
  assert(s);

  // subcircuit commons, recursive
  assert(c == this);
}
/*--------------------------------------------------------------------------*/
/* calculate before model? */
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_RCD::precalc_first(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_first(par_scope);
    e_val(&(this->perim), 0.0, par_scope);
    e_val(&(this->weight), 1.0, par_scope);
    e_val(&(this->Recommon0), 1.0, par_scope);
    e_val(&(this->Recommon1), 1.0, par_scope);
    e_val(&(this->Rccommon0), 1.0, par_scope);
    e_val(&(this->Rccommon1), 1.0, par_scope);
    e_val(&(this->Uref), 0.00001, par_scope);
    trace3("uref...",  Uref, NOT_INPUT, Uref );
    e_val(&(this->mu), 1.0, par_scope);
    e_val(&(this->lambda), 1.0, par_scope);
    e_val(&(this->dummy_capture), false, par_scope);
    e_val(&(this->dummy_emit), false, par_scope);
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_RCD::precalc_last(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_last(par_scope);
  COMMON_BUILT_IN_RCD* cc = this;
  COMMON_BUILT_IN_RCD* c = this;
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(model());
  // final adjust: code_pre
  trace2("COMMON_BUILT_IN_RCD::precalc_last", m->v2(), m->uref);
  // final adjust: override
  // final adjust: raw
  e_val(&(this->perim), 0.0, par_scope);
  e_val(&(this->weight), 1.0, par_scope);
  e_val(&(this->Recommon0), m->Re0, par_scope);
  e_val(&(this->Recommon1), m->Re1, par_scope);
  e_val(&(this->Rccommon0), m->Rc0, par_scope);
  e_val(&(this->Rccommon1), m->Rc1, par_scope);
  e_val(&(this->Uref), m->uref, par_scope);
  
  trace4("uref...",  m->uref, NOT_INPUT, Uref , double (Uref));
  Uref.e_val( (double)m->uref, par_scope);
  trace3("uref...",  m->uref, NOT_INPUT, Uref );

  e_val(&(this->mu), 1.0, par_scope);
  e_val(&(this->lambda), 1.0, par_scope);
  e_val(&(this->dummy_capture), false, par_scope);
  e_val(&(this->dummy_emit), false, par_scope);
  // final adjust: mid
  // final adjust: calculated
  cj_adjusted = 19.0;

  _lambda = 1;
  lambda=1;

  if((double)Uref == NOT_INPUT) { 
    trace2("nin",  m->uref, NOT_INPUT );
    Uref = 0.0;
  }
  if((double)Uref == NA) { 
    trace0("na");
    Uref = 0.0;
  }

  // size dependent
  //delete _sdp;
  _sdp = m->new_sdp(this);
  assert(_sdp);
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(_sdp);
  assert(s);

  // subcircuit commons, recursive
  c->_wcorr=0;
  c->_zero=0;


  if(m->v2()){
    m->do_precalc_last(cc, par_scope );
  } else if (Uref!=0.0 ){
    m->do_precalc_last(cc, par_scope );
  } else { // no uref...
    cc->_Re0 = Recommon0;
    cc->_Rc0 = Rccommon0;
    cc->_Rc1 = _Re0;

    if(Rccommon1 != NA && Rccommon1 != NOT_INPUT ) _Rc1 = Rccommon1;

    _weight = weight;
    _wcorr = 1;
    assert (weight != 0);
    trace5("COMMON_BUILT_IN_RCD::precalc_last no uref. simple model", _Re0, _Rc0, _Rc1, __Re(1), __Rc(0));
    Uref=1;
  }
  trace3("COMMON_BUILT_IN_RCD::precalc_last", Uref*weight, _Re0, _Rc0 );
  trace1("COMMON_BUILT_IN_RCD::precalc_last", cc->_Re0);
  trace1("COMMON_BUILT_IN_RCD::precalc_last done", m->v2());
}
/*--------------------------------------------------------------------------*/
//double MODEL_BUILT_IN_RCD::__tau_up ( double uin, const COMMON_BUILT_IN_RCD* cc ) const{
//        return cc->__tau_up(uin);
//}
/*--------------------------------------------------------------------------*/
double COMMON_BUILT_IN_RCD::__tau_up ( double uin ) const{
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(model());
  double  rc = m->__Rc(uin, this);
  double  re = m->__Re(uin, this);
  return float( rc / ( 1 + rc/re )  ) ;
} 
/*--------------------------------------------------------------------------*/
namespace DEV_BUILT_IN_RCD_DISPATCHER { 
  static DEV_BUILT_IN_RCD p0;
  static DISPATCHER<CARD>::INSTALL
    d0(&device_dispatcher, "Z|rcd", &p0);
}
/*--------------------------------------------------------------------------*/
double COMMON_BUILT_IN_RCD::__Re(double uin) const
{
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(model());
  return m->__Re(uin,this);
}
/*--------------------------------------------------------------------------*/
double COMMON_BUILT_IN_RCD::__Rc(double uin) const
{
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(model());
  return m->__Rc(uin,this);

   //double ret = ( _Rc0 + uin * _lambda * _Rc1 ); 
   //assert (ret==ret);
   //return ret;
}
/*--------------------------------------------------------------------------*/
static EVAL_BUILT_IN_RCD_GRc Eval_GRc(CC_STATIC);
void EVAL_BUILT_IN_RCD_GRc::tr_eval(ELEMENT* d)const
{
  assert(d);
  DEV_BUILT_IN_RCD* p = prechecked_cast<DEV_BUILT_IN_RCD*>(d->owner());
  assert(p);
  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(p->common());
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(cc->sdp());
  assert(s);
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(cc->model());
  assert(m);

  // FIXME: merge with __Rc somehow
  double _c[3] = { cc->_Rc0, cc->_Rc1 * cc->_lambda, 0 };
  double x = (d->_y[0].x);
  //    trace1("Rc", x);
  double f0 = 0.;
  double f1 = 0.;
  for (size_t i=1; i>0; --i) {
    f0 += _c[i];
    f0 *= x;
    f1 *= x;
    f1 += _c[i]*int(i);
  }
  f0 += _c[0];
  d->_y[0] = FPOLY1(x, f0, f1);

  //  d->set_converged(d->conv_check());
}
/*--------------------------------------------------------------------------*/
static EVAL_BUILT_IN_RCD_Ye Eval_Ye(CC_STATIC);
void EVAL_BUILT_IN_RCD_Ye::tr_eval(ELEMENT* d)const
{
  assert(d);
  DEV_BUILT_IN_RCD* p = prechecked_cast<DEV_BUILT_IN_RCD*>(d->owner());
  assert(p);
  const COMMON_BUILT_IN_RCD* c = prechecked_cast<const COMMON_BUILT_IN_RCD*>(p->common());
  assert(c);
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(c->sdp());
  assert(s);
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);

  trace0("eval Ye");
  region_t oldregion = p->_region;
  hp_float_t& volts = d->_y[0].x;

  if ( volts >= 0 ){
    p->_region = FORWARD;
    d->_y[0].f1 = 1 / c->__Re(volts);
    d->_y[0].f0 = d->_y[0].x / c->__Re(volts);
  }else{               
    p->_region = REVERSE;
    d->_y[0].f1 = 0;
    d->_y[0].f0 = 0;
  }
  if (oldregion != p->_region  )
  {
    trace0("switching region");
    d->_sim->_fulldamp = true;
  }
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_RCD::DEV_BUILT_IN_RCD()
  :BASE_SUBCKT(),
   // input parameters,
   // calculated parameters,
   lasts(-inf),
   _region(UNKNOWN),
   // netlist,
   _Ccg(0),
   _Ye(0),
   _Re(0),
   _Rc(0),
   _GRc(0),
   _Ccgfill(0),
   _tr_fill(0)
{
  _n = _nodes;
  attach_common(&Default_BUILT_IN_RCD);

  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_RCD::DEV_BUILT_IN_RCD(const DEV_BUILT_IN_RCD& p)
  :BASE_SUBCKT(p),
   // input parameters,
   // calculated parameters,
   lasts(-inf),
   _region(p._region),
   // netlist,
   _Ccg(0),
   _Ye(0),
   _Re(0),
   _Rc(0),
   _GRc(0),
   _Ccgfill(0),
   _tr_fill(p._tr_fill)
{
  _n = _nodes;
  for (uint_t ii = 0; ii < max_nodes() + int_nodes(); ++ii) {
    _n[ii] = p._n[ii];
  }
  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::expand()
{
  BASE_SUBCKT::expand(); // calls common->expand, attaches model
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

  if (!subckt()) {
    new_subckt();
  }else{
  }
  if (_sim->is_first_expand()) {
    assert (!_Ccgfill);
    _Ccgfill = new ADP_NODE((const COMPONENT*) this, "C");
  }

// idee: _Ccgfill:: tr_value <= udc
//                  tt_value <= E
//
 // _Udc = new ADP_NODE_UDC((const COMPONENT*) common()); //, _Ccgfill);
 // ADP_NODE_LIST::adp_node_list.push_back( _Udc );

  expand_net();

  //precalc();
  subckt()->expand();
  //subckt()->precalc();
  assert(!is_constant());
  if ( adp() == NULL ){
  //  attach_adp( m->new_adp( (COMPONENT*) this ) );
  }else{
    untested(); // rebuild circuit??
  }

  if (m->v2()){
    // incomplete();
    // do_tt?
    // _Ccgfill->tt_set( -c->_wcorr );
  }
  // _tr_fill = _Ccgfill->get();
}
/*==================================*/
ADP_NODE_RCD* MODEL_BUILT_IN_RCD::new_adp_node(const COMPONENT*) const{
  unreachable();
  return NULL;
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::expand_sym() {
  assert(_n);
  assert(common());
  const COMMON_BUILT_IN_RCD* cc = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(cc);
  assert(cc->model());
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(cc->model());
  assert(m);
  assert(cc->sdp());
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(cc->sdp());
  assert(s);
  if (_sim->is_first_expand()) {
    precalc_first();
    precalc_last();

    trace4(("DEV_BUILT_IN_RCD::expand_sym" + short_label()).c_str(), cc->Rccommon0, cc->Recommon0,
        m->Re0, m->Rc0 );
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
double DEV_BUILT_IN_RCD::P()const {
  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  return m->P(this);
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::expand_net() {
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

    // clone subckt elements
    if (m->use_net()) 
    { 
      if (!_Ccg) {
        const CARD* p = device_dispatcher["capacitor"];
        assert(p);
        _Ccg = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Ccg);
        subckt()->push_front(_Ccg);
      }else{
      }
      {
        node_t nodes[] = {_n[n_ic], _n[n_b]};
        _Ccg->set_parameters("Ccg", this, NULL, 1.0, 0, NULL, 2, nodes);
      }
      if (c->dummy_emit) {
        if (_Ye) {
          subckt()->erase(_Ye);
          _Ye = NULL;
        }else{
        }
      }else{
        if (!_Ye) {
          const CARD* p = device_dispatcher["admittance"];
          assert(p);
          _Ye = dynamic_cast<COMPONENT*>(p->clone());
          assert(_Ye);
          subckt()->push_front(_Ye);
        }else{
        }
        {
          node_t nodes[] = {_n[n_u], _n[n_ic]};
          _Ye->set_parameters("Ye", this, &Eval_Ye, 0, 0, NULL, 2, nodes);
        }
      }
      if (!c->dummy_emit) {
      if (_Re) {
        subckt()->erase(_Re);
        _Re = NULL;
      }else{
      }
      }else{
        if (!_Re) {
          const CARD* p = device_dispatcher["resistor"];
          assert(p);
          _Re = dynamic_cast<COMPONENT*>(p->clone());
          assert(_Re);
          subckt()->push_front(_Re);
        }else{
        }
        {
          node_t nodes[] = {_n[n_u], _n[n_ic]};
          _Re->set_parameters("Re", this, NULL, 10, 0, NULL, 2, nodes);
        }
      }
      if (!c->dummy_capture) {
        if (_Rc) {
          subckt()->erase(_Rc);
          _Rc = NULL;
        }else{
        }
      }else{
        if (!_Rc) {
          const CARD* p = device_dispatcher["resistor"];
          assert(p);
          _Rc = dynamic_cast<COMPONENT*>(p->clone());
          assert(_Rc);
          subckt()->push_front(_Rc);
        }else{
        }
        {
          node_t nodes[] = {_n[n_b], _n[n_ic]};
          _Rc->set_parameters("Rc", this, NULL, 10, 0, NULL, 2, nodes);
        }
      }
      if (c->dummy_capture) {
        if (_GRc) {
          subckt()->erase(_GRc);
          _GRc = NULL;
        }else{
        }
      }else{
        if (!_GRc) {
          const CARD* p = device_dispatcher["vcr"];
          assert(p);
          _GRc = dynamic_cast<COMPONENT*>(p->clone());
          assert(_GRc);
          subckt()->push_front(_GRc);
        }else{
        }
        {
          node_t nodes[] = {_n[n_ic], _n[n_b], _n[n_u], _n[n_b]};
          _GRc->set_parameters("GRc", this, &Eval_GRc, 0, 0, NULL, 4, nodes);
        }
      }
    }else{
      //precalc();
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double DEV_BUILT_IN_RCD::tr_probe_num(const std::string& x)const
{
  assert(_n);
  const COMMON_BUILT_IN_RCD* c = prechecked_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(c->sdp());
  assert(s);
  assert(_Ccgfill);

  if (Umatch(x, "region ")) {
    return  static_cast<double>(region());
  }else if (Umatch(x, "ulo |vlo ")) {
    return  ( _Ccgfill->tr_lo );
  }else if (Umatch(x, "uhi |vhi ")) {
    return  ( _Ccgfill->tr_hi );
  }else if (Umatch(x, "tra ")) {
    return  ( _Ccgfill->tr_abs_err() );
  }else if (Umatch(x, "noise ")) {
    assert(_Ccgfill);
    return 0; // ( _Ccgfill->get_tr_noise() );
  }else if (Umatch(x, "udc |ueff")) {
    return  ( _Ccgfill->tr() );
  }else if (Umatch(x, "trr ")) {
    return  ( _Ccgfill->tr_rel_err() );
  }else if (Umatch(x, "E0|zero "    )) {
    return  ( c->_zero );
  }else if (Umatch(x, "dE ")) {
    return (double)_tr_fill - c->_zero ;
  }else if (Umatch(x, "E ")) {
    return (double)_tr_fill;
  }else if (Umatch(x, "te ")) {
      return  ( c->__tau_up(c->Uref) );
  }else if (Umatch(x, "tc ")) {
      return  ( m->__Rc(0, c) );
  }else if (Umatch(x, "re ")) {
      return  ( m->__Re(c->Uref, c) );
  }else if (Umatch(x, "rc ")) {
      return  ( m->__Rc(0 , c) );
  }else if (Umatch(x, "wdt ")) {
    return  ( _Ccgfill->wdT() );
  }else if (Umatch(x, "tau ")) {
    return  ( m->__tau(involts(),c) );
  }else if (Umatch(x, "net ")) {
    return  ( m->use_net() );
  }else if (Umatch(x, "tra ")) {
    return  ( _Ccgfill->tr_abs_err() );
  }else if (Umatch(x, "uref ")) {
    return  ( c->Uref );
  }else if (Umatch(x, "P ")) {
    return  ( P() );
  }else if (Umatch(x, "dvth |vth ")) {
    unreachable();
    assert(is_number(  P() ));
    return  ( P() );
  }else if (Umatch(x, "vw{v} ")) {
    assert (c->_weight != .0);
    if (m->use_net()){
      assert(false);
      assert ( c->_wcorr ==  c->_wcorr );
      assert ( c->_weight==  c->_weight);
      return  ( _n[n_ic].v0() - _n[n_b].v0() ) * c->_weight * c->_wcorr;
    } else
      return _Ccgfill->get_total() * c->_weight * c->_wcorr;
  }else if (Umatch(x, "fill ")) {
    if (m->use_net())
      return _n[n_ic].v0() - _n[n_b].v0();
    else
      return (double)_tr_fill;
  }else if (Umatch(x, "v{c} ")) {
    if (m->use_net())
      return _n[n_ic].v0() - _n[n_b].v0();
    else
      return (double)_tr_fill;
  }else if (Umatch(x, "wt ")) {
    return  c->_weight;
  }else if (Umatch(x, "vin ")) {
    assert (involts() <= _Ccgfill->tr_hi || _sim->_time0==0);
    assert (involts() >= _Ccgfill->tr_lo || _sim->_time0==0);
    return  involts();
  }else if (Umatch(x, "status ")) {
    return  static_cast<double>(converged() * 2);
  }else if (Umatch(x, "_region ")) {
    return _region;
  }else {
    return BASE_SUBCKT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
double DEV_BUILT_IN_RCD::tt_probe_num(const std::string& x)const
{
  assert(_n);
  const COMMON_BUILT_IN_RCD* c = prechecked_cast<const COMMON_BUILT_IN_RCD*>(common());
  const COMMON_BUILT_IN_RCD* cc = prechecked_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(cc->model());
  assert(m);
  const SDP_BUILT_IN_RCD* s = prechecked_cast<const SDP_BUILT_IN_RCD*>(cc->sdp());
  assert(s);
  // const ADP_BUILT_IN_RCD* a = prechecked_cast<const ADP_BUILT_IN_RCD*>(adp());
  //
  // FIXME 

  if (Umatch(x, "vw{v} |dvth "))
  { unreachable(); // deprecated
    return  ( P() );
  } else if (Umatch(x, "P "))  {
    return P();
  } else if (Umatch(x, "vc "))  {
    if( m->use_net()){
      assert(false);
      return  ( _n[n_ic].v0() - _n[n_b].v0() );
    }else{
      //  return _Ccgfill->get_tt();
      //  depends on model!
      assert(is_number(  ( P() )));
      return  ( P() );
    }
  }
  else if (Umatch(x, "net "   )) { return( (double ) m->use_net()); }
  else if (Umatch(x, "tr "    )) { return( _Ccgfill->tr_get() ); }
  else if (Umatch(x, "tt "    )) { return( _Ccgfill->get_tt() ); }
  else if (Umatch(x, "RE0 "    )) { return( c->_Re1 );}
  else if (Umatch(x, "RE1 "    )) { return( c->_Re0 );}
  else if (Umatch(x, "RC0 "    )) { return( c->_Rc1 );}
  else if (Umatch(x, "RC1 "    )) { return( c->_Rc0 );}
  else if (Umatch(x, "ttr "   )) { return( _Ccgfill->tt_rel_err() ); }
  else if (Umatch(x, "trr "   )) { return( _Ccgfill->tr_rel_err() ); }
  else if (Umatch(x, "iter "  )) { return( _iter_count ); }
  else if (Umatch(x, "tra "   )) { return( _Ccgfill->tr_abs_err() ); }
  else if (Umatch(x, "order " )) { return( _Ccgfill->order() ); }
  else if (Umatch(x, "Rc "    )) { return( c->_Rc0 ); }
  else if (Umatch(x, "wt "    )) { return( c->_weight ); }
  else if (Umatch(x, "wdt "   )) { return( _Ccgfill->wdT() ); }
  else if (Umatch(x, "region ")) { return( m->tt_region( this ) ); }
  else if (Umatch(x, "uref "  )) { return( c->Uref ); }
  else if (Umatch(x, "udc "))    { return( _Ccgfill->tr() ); }

  else if (Umatch(x, "uend "  )) { return( c->Uref / (c->__Re(c->Uref) / c->__Rc(c->Uref) +1) * c->_wcorr ) * c->_weight; }
  else if (Umatch(x, "tc "    )) { return( c->__Rc(0) ); }
  else if (Umatch(x, "te ")) {
    if (m->v2())
            return( m->__Re(cc->Uref,cc));
    else
    return  ( c->__tau_up( c->Uref ) );
  }
  else if (Umatch(x, "vwtr ")) {
    return  ( _Ccgfill->tr_get() * c->_weight );
  }else {
    return BASE_SUBCKT::tt_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// cc_direct

double ADP_BUILT_IN_RCD::tt_probe_num(const std::string& )const
 {untested(); return 888;}
double ADP_BUILT_IN_RCD::tr_probe_num(const std::string& )const
{untested(); return 888;}
void ADP_BUILT_IN_RCD::init(const COMPONENT* )
{
  untested();
}
/*--------------------------------------------------------------------------*/
region_t MODEL_BUILT_IN_RCD::region( const COMPONENT* )const
{
  return UNKNOWN;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_RCD::tt_region( const COMPONENT* )const
{
  return 179;
}
/*--------------------------------------------------------------------------*/
region_t DEV_BUILT_IN_RCD::region(  )const
{
  return UNKNOWN;
}
/*--------------------------------------------------------------------------*/
int  DEV_BUILT_IN_RCD::tt_region(  )const
{
  return 14;
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::tt_eval(COMPONENT* )const
{
  untested();
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD::do_tt_prepare(COMPONENT* c)const
{
  trace0(("MODEL_BUILT_IN_RCD::tt_prepare. " + c->short_label()).c_str());
  c=c;
}
///*--------------------------------------------------------------------------*/
ADP_CARD* MODEL_BUILT_IN_RCD::new_adp( COMPONENT* c)const
{
  trace0("MODEL_BUILT_IN_RCD::new_adp");
  assert(c);
  return MODEL_CARD::new_adp(c);
}
/*--------------------------------------------------------------------------*/
bool DEV_BUILT_IN_RCD::tr_needs_eval()const  // not defined...
{
  // subckt()->tr_queue_eval();
  BASE_SUBCKT::tr_needs_eval();
  return true;

}

///*--------------------------------------------------------------------------*/
//long double COMMON_BUILT_IN_RCD::__step(long double uin, long double cur,  double deltat ) const 
//{
//  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(model());
//  assert(m);
//
//  return m->__step(uin,cur,deltat, this);
//}
///*--------------------------------------------------------------------------*/
//
// v3 implementation.
// FIXME: put generic impl here.
long double MODEL_BUILT_IN_RCD::__step(long double uin, long double cur,  double deltat, const COMMON_COMPONENT* c ) const 
{
  cout << "v3\n";
  if (!is_number(uin)){
    error(bDANGER, "DEV_BUILT_IN_RCD::tr_stress no number %E ", uin);
    throw(Exception("no number in __step"));
  }

  assert(is_number(cur));
  const MODEL_BUILT_IN_RCD* m = this; //prechecked_cast<const MODEL_BUILT_IN_RCD*>(cc->model());
  const COMMON_BUILT_IN_RCD* cc = asserted_cast<const COMMON_BUILT_IN_RCD*>(c);

  assert (uin >= 0 || !m->positive);
  
  long double Rc0 = cc->_Rc0;
  long double Re0 = cc->_Re0;
  long double Rc1 = cc->_Rc1;
  long double t = deltat;

  // long double Eend = 1.0L/(1.0L + expl(-Rc1*uin)/uin/Rc0*Re0 );
  // (__Rc(s) / (__Re(s) + __Rc(s) ));
  // (1/uin) / (exp(zeux) + 1/uin)
  //  1/ (1+ e/c)
  //
  long double Eend = uin / ( uin + expl(-Rc1*uin)/Rc0*Re0 );

  long double tauinv = (uin / Re0 + expl(-Rc1*uin)/Rc0);
  long double ret =  (cur-Eend) * expl( -t*tauinv ) + Eend;

  trace6("COMMON_BUILT_IN_RCD::__step ", Eend, deltat, uin, Rc0, ret,  logl(fabsl(cur-Eend ) ) );
//  assert(is_almost(retalt ,ret));

  if (!is_number(ret) || ret < -0.1){
    trace7("COMMON_BUILT_IN_RCD::__step ", Eend, deltat, uin, Rc0, Rc1, Re0, logl(fabsl(cur-Eend ) ) );
    trace7("COMMON_BUILT_IN_RCD::__step ", cur-Eend, deltat, uin, Rc0, Rc1, Re0, fabsl(cur-Eend )  );
    trace6("COMMON_BUILT_IN_RCD::__step ", cur, deltat, uin, Rc0, Rc1, Re0  );
    assert(false);
  }

  return(ret);

// old  sage output
  return ( -(Rc0*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin)
        - cur)*exp(-(Rc0*uin*exp(Rc1*uin)/Re0 + 1)*t*exp(-Rc1*uin)/Rc0) +
      Rc0*exp(Rc1*uin)/(Rc0*exp(Rc1*uin) + Re0/uin));

}
///*--------------------------------------------------------------------------*/
//bool DEV_BUILT_IN_RCD::tr_needs_eval()const
//{
//        untested();
//  return true;
//}

// dashier funktioniert (aus e_subckt.h) 
//   void tr_queue_eval() {assert(subckt()); subckt()->tr_queue_eval();}
// dashier macht der modelgen:
//   void tr_queue_eval()      {if(tr_needs_eval()){q_eval();}}
///*--------------------------------------------------------------------------*/
//
// tt_load...
void DEV_BUILT_IN_RCD::stress_apply()
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

  assert(_sim->_time0 == 0 || _sim->_mode==s_TRAN ); //?

  trace2("DEV_BUILT_IN_RCD::stress_apply ",  _sim->_dT0, _Ccgfill->tt() );
  
  if(_sim->_dT0==0){
    _tr_fill = _Ccgfill->tt();
    return;
  }
  double Time1 = _sim->_Time0 - _sim->_dT0;

  trace4("DEV_BUILT_IN_RCD::stress_apply ", _sim->_Time0, _sim->_dT0,
      tt_iteration_number(), _sim->_Time0);
  trace4("DEV_BUILT_IN_RCD::stress_apply ", 
      _Ccgfill->tr() , _Ccgfill->tr(_sim->_Time0 ), _Ccgfill->order(), _sim->_time0);

  if (! ( is_almost( _Ccgfill->tr1() , _Ccgfill->tr(Time1) )))
  {
      error(bDANGER, "DEV_BUILT_IN_RCD::tr_stress !almost tr1 %E tr(T1) %E \n",
          _Ccgfill->tr1() , _Ccgfill->tr(Time1) );
  } 
  assert ( is_almost( _Ccgfill->tr() , _Ccgfill->tr_rel(_sim->_dT0 + _sim->_time0) ));
  long double E_old = _Ccgfill->tt1();
  long double eff   = _Ccgfill->tr();
  if(m->positive) eff= max(eff,0.0L);

  assert (is_number(eff));
  assert (is_number(E_old));

  long double fill_new  = E_old;
  long double fill_new2 = E_old;

  double ex_time = _sim->_dT0 - _sim->_last_time;
  assert(ex_time = _sim->_last_Time);

  assert ( eff >= 0 || !m->positive);
  fill_new = m->__step( eff , fill_new, ex_time, c );

  long double eff1 = _Ccgfill->tr( Time1 + ex_time/3.0 );
  long double eff2 = _Ccgfill->tr( Time1 + ex_time*2.0/3.0 );
  if(m->positive) eff1= max(eff1,0.0L);
  if(m->positive) eff2= max(eff2,0.0L);


//  better 2 steps.
  assert ( eff1 >= 0 || !m->positive);
  fill_new2 = m->__step( eff1, fill_new2, ex_time/2.0, c );
  assert(is_number(fill_new2));
  assert ( eff2 >= 0 || !m->positive);
  fill_new2 = m->__step( eff2, fill_new2, ex_time/2.0, c );
  assert(is_number(fill_new2));


  fill_new = fill_new2;

  trace4("DEV_BUILT_IN_RCD::stress_apply ", fill_new, E_old, eff, fill_new-_tr_fill );

  assert(is_number(_tr_fill));
  assert(is_number(fill_new));
  assert(is_number(_tr_fill));

  trace3("DEV_BUILT_IN_RCD::stress_apply", eff, _tr_fill, _sim->tt_iteration_number() );

//  hp_float_t fv = (hp_float_t) _tr_fill;

  {
    assert(is_number(fill_new));
    _Ccgfill->tt() = (double) fill_new;
    _tr_fill = fill_new;
    trace2("DEV_BUILT_IN_RCD::stress_apply done ", fill_new, _tr_fill );
  }
}
///*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_RCD_NET::do_stress_apply( COMPONENT*  ) const
{
}
///*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::tr_stress() // called from accept
{
  const DEV_BUILT_IN_RCD* rcd = this;
  double h = _sim->_dt0;
  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_RCD* m = asserted_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);
  assert(c->sdp());
  assert(_Ccgfill);

  trace2("DEV_BUILT_IN_RCD::tr_stress " + long_label(), _sim->_time0, lasts );

  if( _sim->_time0 > lasts ){
    lasts = _sim->_time0;
  }else {
    trace1("DEV_BUILT_IN_RCD::tr_stress again?? bug??", _sim->_time0 );
    if (! (_sim->_time0 == lasts) ){

      error(bDANGER,"DEV_BUILT_IN_RCD::tr_stress unequal now: %E lasts: %E at %E\n",
          _sim->_time0, lasts, _sim->_Time0 );
      throw(Exception("time mismatch in " + long_label()));
    }
    return;
  }


  assert(is_number(involts()));

  if( _sim->_time0==0 ){
    assert( _Ccgfill->tr_lo == inf );
    assert( _Ccgfill->tr_hi == -inf );
    // assert !tran_dynamic()?
  } else { // do not update tr_lo, tr_hi. could be wrong!

    // assert tran_dynamic()?

    _Ccgfill->tr_lo = min(involts(), _Ccgfill->tr_lo);
    _Ccgfill->tr_hi = max(involts(), _Ccgfill->tr_hi);

    assert( is_number(_Ccgfill->tr_lo) );
    assert( is_number(_Ccgfill->tr_hi) );
    assert(fabs(_Ccgfill->tr_lo) < 1e5);
    assert(fabs(_Ccgfill->tr_hi) < 1e5);

    if( -15<  _Ccgfill->tr_hi &&  _Ccgfill->tr_hi < 100  &&
        -15<  _Ccgfill->tr_lo &&  _Ccgfill->tr_lo < 100 ){
    }else{
      error(bDANGER, "%s something wrong with input range %f %f at time %E\n",
          long_label().c_str(), _Ccgfill->tr_lo, _Ccgfill->tr_hi, _sim->_time0 );
      assert(false);
      throw(Exception("boundary error in " + long_label()));
    }
  }

  if (!h) {
    trace1("not h\n", _tr_fill);
    return;
  }
  double uin = rcd->involts();
  trace5("DEV_BUILT_IN_RCD::tr_stress ", long_label(), _tr_fill, h, m->positive, (_tr_fill-c->_zero)*c->_weight );
  if(uin>0) assert(m->__E_end(uin,c) > c->_zero);

  trace1("DEV_BUILT_IN_RCD::tr_stress" ,  m->__E_end_0(c)-  m->__E_end(0.l,c)  );
//  assert( m->__E_end_0(c) == c->_zero );
//  assert( m->__E_end(0.l,c) == c->_zero );

#ifdef DO_TRACE
  double lim=m->__E_end(uin,c);
#endif
  trace3("DEV_BUILT_IN_RCD::tr_stress ", uin, (_tr_fill-c->_zero)*c->_weight, (lim-c->_zero)*c->_weight) ;

  if( (bool)m->positive) {
    if ( _tr_fill < 0 ){
      trace1(("DEV_BUILT_IN_RCD::tr_stress fill is negative: " +
            short_label()).c_str() ,  _Ccgfill->get_total() );
    }
    if (  involts() < -2e-1 ){ // overshoot!
      error(bDANGER, "DEV_BUILT_IN_RCD::tr_stress input %s is negative: %f."
          " overshoot?\n", long_label().c_str(), involts() );
    }
  }

  if( m->use_net()) { incomplete(); return; }
  // m->do_tr_stress(this);
  /*----------------------------------------------------------------------------*/

  long double fill=_tr_fill;
  if (m->positive){
    uin=max(uin,0.0);
    itested();
  }

  trace2("DEV_BUILT_IN_RCD::tr_stress ", fill, uin );
  trace2("DEV_BUILT_IN_RCD::tr_stress ", rcd->involts(), iteration_number() );
  if (fill >= 1.001 ){
    error(bDANGER, "DEV_BUILT_IN_RCD::tr_stress %s fill %LE big uin=%f\n", long_label().c_str(), _tr_fill, uin );
    fill = 1;
  }

  assert(!m->use_net());

  //double  fill = _n[n_ic].v0();
  assert (fill==fill);
  assert (uin==uin);

  //tr_stress...
  long double newfill;
  switch(_sim->_stepno){ incomplete();
    case 0:
    case 1:
    default:
      trace0("DEV_BUILT_IN_RCD::tr_stress calling __step");
      assert ( uin >= 0 || !m->positive);
      newfill = m->__step(uin, fill, h, c);
  }
  assert( newfill > -0.01 || !m->positive);
  if( newfill <= 0 && m->positive  ){
    untested();
    newfill = 0.0;
  }

  if(newfill > 1.000001){
    error(bDANGER, ("* RCD_V3 %f too big\n" + long_label() ).c_str() , newfill );
    newfill=1;
  }
  assert(newfill==newfill);

  _tr_fill = newfill;
  trace5("DEV_BUILT_IN_RCD::tr_stress ", fill, h, (newfill-fill)/h, newfill, h );

  assert(is_number(_tr_fill));
  assert(h > 0);
  /*----------------------------------------------------------------------------*/

  // _Ccgfill->set_tt(_tr_fill);
}
/*----------------------------------------------------------------------------*/
hp_float_t DEV_BUILT_IN_RCD::involts() const {
  hp_float_t v = _n[n_u].v0()  - _n[n_b].v0();
  assert(is_number(v));
  assert(fabs(v) < 1e5);
  return v;
}
/*----------------------------------------------------------------------------*/
// FIXME: move pred/corr to here.
/*----------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::tr_stress_last()
{
  ADP_NODE* cap=_Ccgfill;
  ADP_NODE* _c=_Ccgfill;
  trace2(("tr_stress_last " + short_label()).c_str(), _tr_fill, _sim->tt_iteration_number()  );
  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);
  assert(c->sdp());

  if(m->positive){
        _c->tr_lo = max(.0,_c->tr_lo);
        _c->tr_hi = max(.0,_c->tr_hi);
  } 
  assert( _c->tr_lo <= _c->tr_hi );
  if(m->use_net()){
    assert(false);
    incomplete();
    _tr_fill = ((ELEMENT*)_Ccg)->tr_involts();


    trace3("DEV_BUILT_IN_RCD::tr_stress_last n ", _Ccgfill->get_tt(),
        _Ccgfill->get_tr(), _Ccgfill->get_total() );
  }else{
    trace3("DEV_BUILT_IN_RCD::tr_stress_last s ", _Ccgfill->get_tt(),
        _Ccgfill->get_tr(), _Ccgfill->get_total() );
    // fixme. move to common.
    assert(is_number(_tr_fill));
    try{
      // calculate udc
      m->do_tr_stress_last(_tr_fill,_Ccgfill, this);
    }catch( Exception &e) {
      error(bDANGER, "%s\n", long_label().c_str());
      throw(e);
    }
    assert(is_number(_tr_fill));
  }

  double uin_eff = cap->tr(); 

  if ((uin_eff < cap->tr_lo) || (uin_eff > cap->tr_hi ) ){
    error(bDANGER, "DEV_BUILT_IN_RCD::tr_stress_last Time %E \n    "
        " %s order broken, should be %E < %E < %E, is %i%i\n",
        (double)_sim->_Time0,
        long_label().c_str(),
        cap->tr_lo, uin_eff, cap->tr_hi, 
        (cap->tr_lo < uin_eff), (cap->tr_hi > uin_eff) );
  }

  assert( cap->tr_lo <= cap->tr_hi );

  if (cap->tr_lo >  cap->tr() ) {
    untested();
    cap->tr() = cap->tr_lo ;
    cap->set_order(0);
   
  }else if ( cap->tr() > cap->tr_hi ) {
    untested();
    cap->set_order(0);
    cap->tr() = cap->tr_hi ;
  }

  trace3("DEV_BUILT_IN_RCD::tr_stress_last done",  cap->tr_lo ,  cap->tr(), cap->tr_hi ) ;
  // assert( cap->tr_hi > cap->tr() || fabs( cap->tr() - cap->tr_hi ) < OPT::abstol || fabs((cap->tr() - cap->tr_hi)/ cap->tr_hi  ) < OPT::reltol );
  // assert( cap->tr_lo < cap->tr() || fabs( cap->tr() - cap->tr_lo ) < OPT::abstol || fabs((cap->tr() - cap->tr_lo)/ cap->tr()  ) < OPT::reltol );

  // tt_value not needed for rollback.
  _Ccgfill->set_tt(double(_tr_fill));
}
///*--------------------------------------------------------------------------*/
double COMMON_BUILT_IN_RCD::__tau(double uin)const
{
  const COMMON_COMPONENT* cc=this;
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(cc->model());
  assert(m);
  double tau_e = m->__Re( uin, cc );
  double tau_c = m->__Rc( uin, cc );
  if (tau_c < tau_e){
    return tau_c  / ( tau_c/tau_e + 1 );
  } else{
    return tau_e  / ( tau_e/tau_c + 1 );
  }
}
///*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::tt_commit() { unreachable();
  // untested();
  return;
}
///*--------------------------------------------------------------------------*/
//
// tt_begin??
void DEV_BUILT_IN_RCD::tt_prepare()
{
  unreachable();
  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  //const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());

  _tr_fill = _Ccgfill->tt();

  assert(_sim->_time0 == 0 );

  trace0(("DEV_BUILT_IN_RCD::tt_prepare " + short_label()).c_str());

  lasts = -inf;
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::tt_begin()  {
  const COMMON_BUILT_IN_RCD* c = prechecked_cast<const COMMON_BUILT_IN_RCD*>(common());
  trace1("DEV_BUILT_IN_RCD::tt_begin", c->_zero);
  _tr_fill = c->_zero ;

  _Ccgfill->set_tt(c->_zero);  // tt_load??
  _Ccgfill->set_tr(-inf);


  const MODEL_BUILT_IN_RCD* m = asserted_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  m->do_tt_prepare(this);

  q_eval();

}
///*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::tr_begin(){
  BASE_SUBCKT::tr_begin();

  q_accept();
}
///*--------------------------------------------------------------------------*/
bool DEV_BUILT_IN_RCD::do_tr()
{
  const COMMON_BUILT_IN_RCD* c = static_cast<const COMMON_BUILT_IN_RCD*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_RCD* m = prechecked_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  assert(m);
  assert(c->sdp());
  q_accept(); // multiple times??

  if(m->use_net()){
    bool     s = true;
    s = s && _Ye->do_tr();
    s = s && _GRc->do_tr();
    s = s && _Ccg->do_tr();
    set_converged(s);
    trace1("do_tr",s);
    return converged();
  }
  return true;
}
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_RCD::precalc_last()  {
  trace0("DEV_BUILT_IN_RCD::precalc_last");
  COMPONENT::precalc_last(); assert(subckt()); subckt()->precalc_last();
}
/*--------------------------------------------------------------------------*/
/* Newton iterator. finding effective DC value */
// should be part of MODEL?
long double COMMON_BUILT_IN_RCD::__uin_iter(long double& uin, double E_old, double E_in, double bound_lo, double bound_hi, COMPONENT* dd ) const
{
  const MODEL_BUILT_IN_RCD* m = dynamic_cast<const MODEL_BUILT_IN_RCD*>(model());
  return m->__uin_iter(uin,E_old, E_in,  bound_lo, bound_hi, dd );
}
/*--------------------------------------------------------------------------*/
long double MODEL_BUILT_IN_RCD::__uin_iter(long double& uin, double E_old, double E_in, double bound_lo, double bound_hi, COMPONENT* dd ) const
{
  long double E=(long double) E_in;
  const COMMON_BUILT_IN_RCD* c = asserted_cast<const COMMON_BUILT_IN_RCD*>(dd->common());
  const MODEL_BUILT_IN_RCD* m = dynamic_cast<const MODEL_BUILT_IN_RCD*>(c->model());
  DEV_BUILT_IN_RCD* d = asserted_cast<DEV_BUILT_IN_RCD*>(dd);
  trace5("COMMON_BUILT_IN_RCD::__uin_iter: ", uin, E_old, E, E-E_old,  CKT_BASE::_sim->_last_time );
  assert (E<1.000001);
  
  if (E>1) {
    untested0("COMMON_BUILT_IN_RCD::__uin_iter aligned E");
    E=1;
  }
  double h = BASE_SUBCKT::_sim->_last_time;

  long double Euin = 0;
  long double Euin_alt = 0;
  if (E < 0.0) { E=0.0;}
  //  E = max(E,0.0);
  //if(E<1e-12) return 0;

  assert(( -1 < E ) && (E < 2) ) ;

  long double res=1;   // dx
  long double deltaE =1; // df 
  long double deltaE_alt = 0; // dfold
  long double damp=1;
  long double fu =1; // function we try to find zero of, at u
  long double df_fres =1; // df 
  long double cres; // cut res
  long double dx_res=1;
  unsigned i=0;
  int hhack=0;
  long double Edu=0;
  long double Q=1;
  bool putres=false;

  double ustart = (double) uin;
  long double delta_u=.0L;
  bool edge=false;
  bool A=true;
  bool B=true;
  bool C=true;
  bool D=true;
  bool U=true;



  assert(uin>=-0.001 || !m->positive);
  Euin = m->__step( uin, E_old, h, c );
  if(!is_number(Euin)) {
    error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter pl cannot evaluate E "
        "at uin=%LE (E_old=%E) %i\n", uin, E_old, CKT_BASE::_sim->tt_iteration_number());
    assert(false);
  }
  double reltol = pow(OPT::reltol,1.2);
  double abstol = OPT::abstol/10.0;
  unsigned iterlimit=50;
  if (!_sim->tt_iteration_number()){
    iterlimit=500;
  }


  trace2("COMMON_BUILT_IN_RCD::__uin_iter rel_tol", reltol, OPT::abstol);
  //while( ( A || ( B && C && D ) ) ) { // && fabs(Euin-E)>1e-40 )  }
  while( U || (( A && C ) || ( B && C && D ) ) ) { // && fabs(Euin-E)>1e-40 ) 
    i++;
    edge = false;
    trace7("COMMON_BUILT_IN _RCD::__uin_iter loop", (double)uin, (double)res, (double)deltaE, Edu, E, i,Euin);
    trace3(" ",dx_res,Q,df_fres);
    if( i> iterlimit){
      untested();
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter no converge uin="
          "%LE, E=%LE, res=%LE, lres=%Lg, Q=%Lg\n", uin, E, res, log(fabs(res)), Q);
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter no converge bounds %E, %E\n",
          bound_lo, bound_hi);

      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter LQ=%Lf>%Lf. h%i deltaE=%LE\n", 
          log(Q), logl(reltol), hhack, deltaE);
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter start=%E\n", ustart);
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter Edu=%LE Euin=%LE E=%LE, fu= %LE, delta_u=%LE\n",
          Edu, Euin, E, fu, delta_u);
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter s=%i%i%i%i%i putres=%i\n",
          A, B, C, D, U, putres);
      throw(Exception("does not converge: %s\n", dd->long_label().c_str()));
      break;
    }
    if(!is_number(uin)){
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter uin wrong %E diff "
          "%E loking for %E \n", Euin, Edu, E );
      assert(false);
      return( inf );
    }
    Edu = m->__dstepds(uin, E_old, c); // ??
    if(!is_number(Edu) ){
      untested();
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter step %i:%i Edu nan at %LE Euin=%LE C=%LE diff "
          "%LE looking for %E, start %E res %LE\n", CKT_BASE::_sim->tt_iteration_number(),i,
             uin, Euin, 1-Euin, Edu, E, ustart, res  );
      putres = true;
      // assert(false);
      if (i==1) // first iteration no reliale res
      {
        trace2("Guessing the nan reason",Euin,uin);
        if ( Euin >= 0.5 )
        {
          uin=uin/2; // Guess: uin probably much too large  
        } else {
          uin=uin*2; // 
        }
      } else {  // next iterations, try half step size 
        res /= 2.0;
        uin += res;
        uin= std::max(uin,0.0L);
        untested();
        hhack++;
      }
      continue;
    }
    if((Edu==0 || fabs(Edu) < 1E-150 ) ){
     // error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter step %i:%i Edu 0 at %LE Euin=%LE C=%LE diff "
     //     "%LE looking for %LE, start %E res %LE\n", CKT_BASE::_sim->tt_iteration_number(),i,
     //     uin, Euin, 1-Euin, Edu, E, ustart, res  );
      trace0("COMMON_BUILT_IN_RCD::__uin_iter Edu 0");
      putres=true;
// assert(false);
      Edu=1;
    }
    assert (is_number (Euin));
    assert (is_number (Edu));
    fu =(Euin-E);
    res  = damp*fu/Edu;       // dx, Das ist die Differenz in x also: delta x 

    if(!is_number(res) && fabs(Edu) < 1E-150 ) {
      trace2("COMMON_BUILT_IN_RCD::__uin_iter",res, Edu);
      putres=true;     // falls Edu sehr klein aber nicht 0 ist kann es 
                       // auch zu Fehlern  kommen 
      Edu=Edu*1E10;
      res  = fu/Edu; 
    }

    assert(is_number(res));

    cres= std::min( 1.L,res);
    cres= std::max(-1.L,res);

    dx_res=uin;
    uin -= cres;
    if(( (double) uin > bound_hi )){
      edge=true;
      uin=bound_hi;
    }else if(( (double) uin < bound_lo )){
      edge=true;
      uin=bound_lo;
    }

    assert(is_number(uin));
    if( (uin<-0.0000) && m->positive ) {
      untested();
      trace1( "COMMON_BUILT_IN_RCD::__uin_iter neg uin ", uin );
      putres=true;
      cres = uin;
      uin = .00;
    }
    dx_res-=uin; // Effektives dx da es durch Numerik kaputt gehen kann

    Q = fabs( dx_res / uin );

    assert(uin>=-0.001 || !m->positive);
    Euin = m->__step( uin, E_old, h, c );
    deltaE_alt = deltaE;
    deltaE = Euin - Euin_alt; // Effektive Veraenderung
    Euin_alt = Euin;

    if(deltaE * deltaE_alt < 0){
      damp *= 0.8;
    }


    if( !is_number( Euin ) ){
      error( bDANGER, "COMMON_BUILT_IN_RCD::__uin_iter cannot evaluate E "
          "at uin=%LE (E_old=%E) %i:%i\n", uin, E_old, CKT_BASE::_sim->tt_iteration_number(), i);
      assert(is_number(Euin)); break;
    }

    // FIXME: use bounds to zero step and break.
    if (Euin > E && uin==0 && m->positive ){
      break;
    }

    delta_u = .5 * max ( OPT::abstol, double( fabs(uin) * (OPT::reltol) ) );

    U = ( delta_u * Edu < fabs(( E - Euin)) ) && !edge;
    A = Q > reltol;
    B = ( fabs(dx_res) > 1e-30 );   // dx failed
    C = ( fabs(deltaE) > abstol ); // df zu Zielwert failed
    C = false;  // was soll C bringen?
    D = ( fabs(df_fres) > 1e-125); // df zu Altwert  failed <= hack.

// hab ein paar numerische Hacks eingebaut, die es jetzt durchlaufen lassen...
// 1. der reine dx Fehelr wird irgendwann , besonders wenn Edu klein wird nicht mehr kleiner. => 
//    df Fehler als zusaetliches Kriterium (C) 
// 2. Irgendwann wird auch der df Fehler nicht mehr kleiner, da die Ableitung (wahrscheinlich numerisch falsch)
//    so ausgerechnet wird, dass man nur noch um kleine dx voranschreitet und sich effektiv am Funktionswert
//    gar nichts mehr tut (Kriterium D) 
// Bei einer anderen Implementierung kann man da auch so Kombis mit
// der Haelfte von abstol machen, das hab ich mir mal verkniffen, koennte bei weiterem Fehlschlagen 
// noch eingebaut werden.
// 3. kein Hack, der nur bei monoton steigenden Funktionen geht: Wenn er bei der ersten Iteration
//     mit uin= 3681 anfaengt kann er gar kein Edu ausrechnen dann halbier ich nicht res  sondern uin.
    trace6(" Ende Loop ",A,B,C,D,U,i);
    
  }

  d->_iter_count=i;
  trace7("COMMON_BUILT_IN_RCD::__uin_iter done loop", uin, res,
      deltaE, df_fres, Edu, E, E_old);
  trace5("COMMON_BUILT_IN_RCD::__uin_iter done", (E-Euin), ustart,i, putres, delta_u );



  assert(uin>=-0.001 || !m->positive);
  return uin;
}
/*-----------------*/
void DEV_BUILT_IN_RCD::tt_next(){
  lasts = -inf;
  _Ccgfill->tr_hi = -inf;
  _Ccgfill->tr_lo = inf;
  q_accept();
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_RCD_NET::P(const COMPONENT* c) const{
  const DEV_BUILT_IN_RCD* d = asserted_cast<const DEV_BUILT_IN_RCD*>(c);
  const COMMON_BUILT_IN_RCD* cc = asserted_cast<const COMMON_BUILT_IN_RCD*>(d->common());
  // read from CAP. _Ccgfill is only updated after tr
  const  ELEMENT* cap = dynamic_cast<const ELEMENT*> (d->_Ccg);
  assert(cap);
  assert(false);
  assert(is_number( cap->tr_involts() ) * cc->_weight * cc->_wcorr );

  return  ( cap->tr_involts() ) * cc->_weight * cc->_wcorr;
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_NET::MODEL_BUILT_IN_RCD_NET(const BASE_SUBCKT* p)  
  : MODEL_BUILT_IN_RCD(p){ }
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_RCD_NET::MODEL_BUILT_IN_RCD_NET(const MODEL_BUILT_IN_RCD_NET& p) 
  : MODEL_BUILT_IN_RCD(p){ }
