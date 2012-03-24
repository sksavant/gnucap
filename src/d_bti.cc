/* $Id: d_bti.cc,v 1.7 2010-09-07 07:46:19 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 *
 *
 * (c) 2010 Felix Salfelder
 *
 * GPLv3
 *
 */

#include "e_aux.h"
#include "e_storag.h"
#include "globals.h"
#include "e_elemnt.h"
// #include "e_compon.h"
#include "d_bti.h"
#include "io_trace.h"
#include "u_para_vec.h"
static bool dummy=false;
/*--------------------------------------------------------------------------*/
class COMMON_COMPONENT;
/*--------------------------------------------------------------------------*/
//int  MODEL_BUILT_IN_BTI_MATRIX::foo(){return 1;}
//int  MODEL_BUILT_IN_BTI_SINGLE::foo(){return 1;}
/*--------------------------------------------------------------------------*/
int DEV_BUILT_IN_BTI::_count = -1;
int COMMON_BUILT_IN_BTI::_count = -1;
static COMMON_BUILT_IN_BTI Default_BUILT_IN_BTI(CC_STATIC);
/*--------------------------------------------------------------------------*/

//const double NA(NOT_INPUT);
const double INF(BIGBIG);
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_BTI::_count = 0;
/*--------------------------------------------------------------------------*/
namespace { 
  static DEV_BUILT_IN_BTI p3d;
  static MODEL_BUILT_IN_BTI p3(&p3d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d3(&model_dispatcher, "bti_default", &p3);
}
/*--------------------------------------------------------------------------*/
void SDP_BUILT_IN_BTI::init(const COMMON_COMPONENT* cc)
{
  assert(cc);
  SDP_CARD::init(cc);
}
/*--------------------------------------------------------------------------*/
TDP_BUILT_IN_BTI::TDP_BUILT_IN_BTI(const DEV_BUILT_IN_BTI*)
{
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI::MODEL_BUILT_IN_BTI(const BASE_SUBCKT* p)
  :MODEL_CARD(p),
   gparallel(0.0),
   flags(int(USE_OPT)),
   mos_level(0),
   rcd_number(1),
   bti_type(0),
   bti_base(10.0),
   anneal(true),
   rcd_model_name( (std::string) "rcdmodel"),
   weight(0.199)
   //,rcd_model( MODEL_BUILT_IN_RCD() )
{
  if (ENV::run_mode != rPRE_MAIN) {
    ++_count;
  }else{
  }
  set_default(&_tnom_c, OPT::tnom_c);
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI::MODEL_BUILT_IN_BTI(const MODEL_BUILT_IN_BTI& p)
  :MODEL_CARD(p),
   gparallel(p.gparallel),
   flags(p.flags),
   mos_level(p.mos_level),
   rcd_number(p.rcd_number),
   bti_type(p.bti_type),
   bti_base(p.bti_base),
   anneal(p.anneal),
   rcd_model_name(p.rcd_model_name),
   weight(p.weight)
//,   rcd_model( MODEL_BUILT_IN_RCD() )
{
  if (ENV::run_mode != rPRE_MAIN) {
    ++_count;
  }else{
    untested();//194
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SINGLE::attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const
{
  trace0("MODEL_BUILT_IN_BTI_SINGLE::attach_rcds()");
  int i=0;
  assert((int)rcd_number==1);
  COMMON_BUILT_IN_RCD* RCD1 = new COMMON_BUILT_IN_RCD;
  RCD1->set_modelname( rcd_model_name ); // <= !
  RCD1->Uref = uref ;
  trace0(("MODEL_BUILT_IN_BTI_SINGLE::attach_rcds set_modelname " + (std::string) rcd_model_name).c_str());
  RCD1->attach(this); // ?
  COMMON_COMPONENT::attach_common(RCD1, (COMMON_COMPONENT**)&(_RCD[i]));
  // assert (RCD1 == _RCD[i]);
}
/*--------------------------------------------------------------------------*/
 MODEL_BUILT_IN_BTI_SINGLE::MODEL_BUILT_IN_BTI_SINGLE(const BASE_SUBCKT* p)
  :MODEL_BUILT_IN_BTI(p),
  fooo(0)
{

}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_SINGLE::MODEL_BUILT_IN_BTI_SINGLE(const
    MODEL_BUILT_IN_BTI_SINGLE& p)
  :MODEL_BUILT_IN_BTI(p),
  fooo(0)
{
}
/*--------------------------------------------------------------------------*/
ADP_CARD* MODEL_BUILT_IN_BTI::new_adp( COMPONENT* c)const
{
  assert(c);
  return MODEL_CARD::new_adp(c);
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI::dev_type()const
{
  return "btimodel ";
  if (dummy == true) {
    return "btimodel";
  }else{untested();//235
    return MODEL_CARD::dev_type();
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI::set_dev_type(const std::string& new_type)
{
  if (Umatch(new_type, "btimodel ")) {
    dummy = true;
  }else{
    MODEL_CARD::set_dev_type(new_type);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI::param_value(int i)const
{
  switch (MODEL_BUILT_IN_BTI::param_count() - 1 - i) {
  case 0:  unreachable(); return "";
  case 1:  return _tnom_c.string();
  case 2:  return gparallel.string();
  case 3:  return flags.string();
  case 4:  return mos_level.string();
  case 5:  return rcd_number.string();
  case 6:  return anneal.string();
  case 7:  return rcd_model_name.string();
  case 8:  return weight.string();
//  case 8:  return "some string"; // rcd_model.string();
  default: return "unknown";
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI::is_valid(const COMPONENT* d)const
{
  assert(d);
  return MODEL_CARD::is_valid(d);
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI::tr_eval(COMPONENT*)const
{untested();//425
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI::precalc_first()
{
  trace0(("MODEL_BUILT_IN_BTI::precalc_first" + rcd_model_name.string()).c_str());
    const CARD_LIST* par_scope = scope();
    assert(par_scope);
    MODEL_CARD::precalc_first();
    e_val(&(this->gparallel), 0.0, par_scope);
    e_val(&(this->flags), int(USE_OPT), par_scope);
    e_val(&(this->mos_level), 0, par_scope);
    rcd_number.e_val( 1, par_scope);
    e_val(&(this->anneal), true, par_scope);
    // final adjust: code_pre
    // final adjust: override
    // final adjust: raw
    e_val(&(this->gparallel), 0.0, par_scope);
    e_val(&(this->flags), int(USE_OPT), par_scope);
    e_val(&(this->mos_level), 0, par_scope);
    rcd_number.e_val(1, par_scope);
    e_val(&(this->anneal), true, par_scope);
    rcd_model_name.e_val_strange( std::string("rcd_model_hc"), par_scope);
    e_val(&(this->weight), 0.123, par_scope);
    e_val(&(this->uref), NOT_INPUT, par_scope);
    e_val(&(this->v2), false, par_scope);


}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_MATRIX::MODEL_BUILT_IN_BTI_MATRIX(const BASE_SUBCKT* p)
  :MODEL_BUILT_IN_BTI_SUM(p),
  cols(0),
  rows(0),
  base(10)
{
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_SUM::param_value(int i)const
{
  switch (MODEL_BUILT_IN_BTI::param_count() - 1 - i) {
    case 0:  unreachable(); return "";
    case 1:  return string(rcdparm);
  }
  return MODEL_BUILT_IN_BTI::param_value(i);
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SUM::precalc_first()
{
  trace0("MODEL_BUILT_IN_BTI_SUM::precalc_first");
  const CARD_LIST* par_scope = scope();
  dpvv x; // empty params.

  rcdparm.e_val( x, par_scope);
  MODEL_BUILT_IN_BTI::precalc_first();
  rcd_number = (int) rcdparm.size();

}
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_BTI_SUM::param_count( )const
  {return (1 + MODEL_BUILT_IN_BTI::param_count());}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SUM::set_dev_type(const std::string& new_type)
{
  if (Umatch(new_type, "matrix ")) {
  }
  MODEL_BUILT_IN_BTI::set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_MATRIX::set_dev_type(const std::string& new_type)
{
  trace0(("MODEL_BUILT_IN_BTI_MATRIX::set_dev_type " + new_type).c_str());
  if (Umatch(new_type, "matrix ")) {
  }
  MODEL_BUILT_IN_BTI_SUM::set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_MATRIX::precalc_first()
{
  trace0(("MODEL_BUILT_IN_BTI_MATRIX::precalc_first" + rcd_model_name.string()).c_str());
  const CARD_LIST* par_scope = scope();
  assert(par_scope);
  rows.e_val( 2, par_scope);
  cols.e_val( 3, par_scope);
  e_val(&(this->base), 11.0, par_scope);
  MODEL_BUILT_IN_BTI_SUM::precalc_first();
  rcd_number=rows*cols;
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI::precalc_last()
{
    MODEL_CARD::precalc_last();
}
/*--------------------------------------------------------------------------*/
SDP_CARD* MODEL_BUILT_IN_BTI::new_sdp(COMMON_COMPONENT* c)const
{
  assert(c);
  if (COMMON_BUILT_IN_BTI* cc = dynamic_cast<COMMON_BUILT_IN_BTI*>(c)) {
    if (cc->_sdp) {
      cc->_sdp->init(cc);
      return cc->_sdp;
    }else{
      delete cc->_sdp;
      return new SDP_BUILT_IN_BTI(c);
    }
  }else{
    return MODEL_CARD::new_sdp(c);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI::set_param_by_index(int i, std::string& value, int offset)
{
  trace2("MODEL_BUILT_IN_BTI::set_param_by_index", i, offset );
  switch (MODEL_BUILT_IN_BTI::param_count() - 1 - i) {
  case 0: untested(); break;
  case 1: _tnom_c = value; break;
  case 2: gparallel = value; break;
  case 3: flags = value; break;
  case 4: mos_level = value; break;
  case 5: rcd_number = value; break;
  case 6: anneal = value; break;
  case 7: rcd_model_name = value;
          trace0(("set rcd_model_name to " +  rcd_model_name.string()).c_str());
          break;
  case 8: weight = value; break;
  case 9: uref = value; break;
  case 10: v2 = value; break;
  default: throw Exception_Too_Many(i, MODEL_BUILT_IN_BTI::param_count(), offset);
           break;
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI::RCD_name(uint_t i) const
{
  stringstream a;
  a << "RCD" << i;
  return a.str();
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI::param_name(int i)const
{
  switch (MODEL_BUILT_IN_BTI::param_count() - 1 - i) {
  case 0:  return "=====";
  case 1:  return "tnom";
  case 2:  return "gparallel";
  case 3:  return "flags";
  case 4:  return "mos_level";
  case 5:  return "rcd_number";
  case 6:  return "anneal";
  case 7:  return "rcd_model_name";
  case 8:  return "weight";
  case 9:  return "uref";
  case 10:  return "v2";
  default: return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI::param_name(int i, int j)const
{
  trace2("MODEL_BUILT_IN_BTI::param_name", i, j );
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (MODEL_BUILT_IN_BTI::param_count() - 1 - i) {
    case 0:  return "=====";
    case 1:  return "";
    case 2:  return "gp";
    case 3:  return "";
    case 4:  return "";
    case 5:  return "";
    case 6:  return "";
    case 7:  return "rcd_model";
    case 8:  return "";
    case 9:  return "uref";
    case 10:  return "v2";
    default: return "";
    }
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_SUM::param_name(int i)const
{
  switch (MODEL_BUILT_IN_BTI_SUM::param_count() - 1 -i ){
        case 0: return "=====";
        case 1: return "params";
  }
  return MODEL_BUILT_IN_BTI::param_name( i  );
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_SUM::MODEL_BUILT_IN_BTI_SUM(const BASE_SUBCKT* p)
  :MODEL_BUILT_IN_BTI(p),
  rcdparm( PARAMETER< dpvv>() )
{
}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_SUM::MODEL_BUILT_IN_BTI_SUM(const MODEL_BUILT_IN_BTI_SUM& p)
  :MODEL_BUILT_IN_BTI(p),
   rcdparm(p.rcdparm)
{
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_SUM::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (MODEL_BUILT_IN_BTI_SUM::param_count() - 1 - i) {
    case 0:  return "=====";
    case 1:  return "parms";
    default: return "";
    }
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SUM::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_BUILT_IN_BTI_SUM::param_count() - 1 - i) {
    case 0: untested(); break;
    case 1: 
            trace1("MODEL_BUILT_IN_BTI_SUM::set_param_by_index", value);
            rcdparm = value; 
            break;
    default: MODEL_BUILT_IN_BTI::set_param_by_index(i,value,offset);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SUM::attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const
{
  trace0("MODEL_BUILT_IN_BTI_SUM()");
  trace0(rcd_model_name.string().c_str());

  // k
  // 1 2 3
  // 4 5 6
  // 7 8 9
//typedef vector<PARAMETER<double> > dpv;
//typedef vector<PARAMETER<dpv> > dpvv;

  size_t Nr = rcdparm.size();
  if(!Nr)
    untested();
//    throw Exception("no summands");

  trace1("MODEL_BUILT_IN_BTI_SUM::attach_rcds", Nr);

  for(size_t n=0; n<Nr; n++){
    dpvv P = dpvv(rcdparm);
    PARAMETER<dpv> Q = P[n];
    assert(Q.size()==5); // 5 parameters per rcd

    COMMON_BUILT_IN_RCD* RCD1 = new COMMON_BUILT_IN_RCD;
    RCD1->set_modelname( rcd_model_name ); // <= !
    RCD1->attach(this); // ?

    size_t Np = dpv(Q).size();
    for(uint_t m=0; m<Np ; m++ ){
      trace1("MODEL_BUILT_IN_BTI_SUM::attach_rcds ",  m ); 
      PARAMETER<double> pk = dpv(Q)[m];
      string s=pk.string();
      int index =  RCD1->param_count() - m - 2;
      trace3("MODEL_BUILT_IN_BTI_SUM::attach_rcds ",  n, index, s  ); 
      RCD1->set_param_by_index( index, s, 0 );
    }
    COMMON_COMPONENT::attach_common(RCD1, (COMMON_COMPONENT**)&(_RCD[n]));
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_MATRIX::MODEL_BUILT_IN_BTI_MATRIX(const MODEL_BUILT_IN_BTI_MATRIX& p)
  :MODEL_BUILT_IN_BTI_SUM(p),
   cols(1),
   rows(1),
   base(1)
{
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_MATRIX::attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const
{
  trace0("MODEL_BUILT_IN_BTI_MATRIX::attach_rcds()");
  trace0(rcd_model_name.string().c_str());
  uint_t row, col , k;
  assert ((uint_t)rcd_number==(rows*cols));
  trace3("attaching", rows, cols, rcd_number);

  long double up = 1;
  long double down = 1;
  //double lambda=1;
  //double mu=1;

  // k
  // 1 2 3
  // 4 5 6
  // 7 8 9

  for(col=0; col < cols; col++ ){
    up *= base;
    for(row=0; row < rows; row++ ){
      down *=base;
      k=rows*col+row;
      COMMON_BUILT_IN_RCD* RCD1 = new COMMON_BUILT_IN_RCD;
      RCD1->set_modelname( rcd_model_name ); // <= !
      RCD1->attach(this); // ?
//  assert ((double)uref != NOT_INPUT );
      RCD1->Uref = uref;
      RCD1->Recommon0 = double(up);
      RCD1->Rccommon0 = double(down);

      trace5("MODEL_BUILT_IN_BTI_MATRIX::attach_rcds ", row, col, k, up, down); 

      COMMON_COMPONENT::attach_common(RCD1, (COMMON_COMPONENT**)&(_RCD[k]));
      // assert (RCD1 == _RCD[i]);
    }
    down=1;

  }

}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_MATRIX::set_param_by_index(int i, std::string& value, int offset)
{
  trace2(("MATRIX::set_param_by_index" + value).c_str(), i, offset );
  switch (MODEL_BUILT_IN_BTI_MATRIX::param_count() - 1 - i) {
    case 0: untested(); break;
    case 1: rows = value; break;
    case 2: cols = value; break;
    case 3: base = value; break;
    default: MODEL_BUILT_IN_BTI_SUM::set_param_by_index(i,value,offset);
  }
}
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_BTI_MATRIX::param_count( )const
  {return (3 + MODEL_BUILT_IN_BTI_SUM::param_count());}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI_SUM::param_is_printable(int i)const
{
  switch (MODEL_BUILT_IN_BTI_SUM::param_count() - 1 - i) {
  case 0:  return (false);
  case 1:
           return (true);
  default: return MODEL_BUILT_IN_BTI::param_is_printable( i );
  }
}
/*--------------------------------------------------------------------------*/
namespace  { 
  static DEV_BUILT_IN_BTI p1d;
  static MODEL_BUILT_IN_BTI_SUM p1(&p1d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d1(&model_dispatcher, "bti_sum|btisum", &p1);
}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI::param_is_printable(int i)const
{
  trace1("MODEL_BUILT_IN_BTI::param_is_printable", i );
  switch (MODEL_BUILT_IN_BTI::param_count() - 1 - i) {
  case 0:  return (false);
  case 1:  return (true);
  case 2:  return (gparallel != 0.);
  case 3:  return (!(flags & USE_OPT));
  case 4:  return (mos_level.has_hard_value());
  case 5:  return (true);
  case 6:  return (true);
  case 7:  return (true);
  case 8:  return (true);
  default: return false;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI_MATRIX::param_is_printable(int i)const
{
  trace1("MODEL_BUILT_IN_BTI_MATRIX::param_is_printable", i );
  switch (MODEL_BUILT_IN_BTI_MATRIX::param_count() - 1 - i) {
  case 0:  return (false);
  case 1:
  case 2:
  case 3:
           return (true);
  default: return MODEL_BUILT_IN_BTI_SUM::param_is_printable( i );
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_MATRIX::param_name(int i)const
{
  trace1("MODEL_BUILT_IN_BTI_MATRIX::param_name",i);
  switch (MODEL_BUILT_IN_BTI_MATRIX::param_count() - 1 -i ){
        case 0: return "=====";
        case 1: return "rows";
        case 2: return "cols";
        case 3: return "base";
  }
  return MODEL_BUILT_IN_BTI::param_name( i  );

}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_MATRIX::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (MODEL_BUILT_IN_BTI_MATRIX::param_count() - 1 - i) {
    case 0:  return "=====";
    default: return "";
    }
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_MATRIX::param_value(int i)const
{
  switch (MODEL_BUILT_IN_BTI_MATRIX::param_count() - 1 - i) {
    case 0:  return "???";
    case 1:  return rows.string();
    case 2:  return cols.string();
    case 3:  return base.string();
  }
  return MODEL_BUILT_IN_BTI_SUM::param_value(i);
}
/*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_BTI_MATRIX_DISPATCHER { 
  static DEV_BUILT_IN_BTI p1d;
  static MODEL_BUILT_IN_BTI_MATRIX p1(&p1d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d1(&model_dispatcher, "bti_matrix_old", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_SINGLE::param_value(int i)const
{
  switch (MODEL_BUILT_IN_BTI_SINGLE::param_count() - 1 - i) {
  case 0:  return fooo.string();
  }
  return MODEL_BUILT_IN_BTI::param_value(i);
}
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_BTI_SINGLE::param_count( )const
  {return (1 + MODEL_BUILT_IN_BTI::param_count());}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SINGLE::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_BUILT_IN_BTI_SINGLE::param_count() - 1 - i) {
    case 0: untested(); break;
    case 1: fooo = value; break;
    default: MODEL_BUILT_IN_BTI::set_param_by_index(i,value,offset);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_SINGLE::param_name(int i)const
{
  switch (MODEL_BUILT_IN_BTI_SINGLE::param_count() - 1 -i ){
        case 0: return "======";
        case 1: return "fooo";
  }
  return MODEL_BUILT_IN_BTI::param_name(i );

}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI_SINGLE::param_is_printable(int i)const
{
  switch (MODEL_BUILT_IN_BTI_SINGLE::param_count() - 1 - i) {
  case 0:  return (false);
  case 1:  return (true);
  default: return MODEL_BUILT_IN_BTI::param_is_printable( i );
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_SINGLE::precalc_first()
{
    const CARD_LIST* par_scope = scope();
    assert(par_scope); USE(par_scope);
    MODEL_BUILT_IN_BTI::precalc_first();
}
/*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_BTI_SINGLE_DISPATCHER { 
  static DEV_BUILT_IN_BTI p2d;
  static MODEL_BUILT_IN_BTI_SINGLE p2(&p2d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d2(&model_dispatcher, "bti_single", &p2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_BTI::COMMON_BUILT_IN_BTI(int c)
  :COMMON_COMPONENT(c),
   lambda(1.0),
   number(0),
   weight(1.0),
   polarity(pN),
   _sdp(0),
   _RCD(0)
{
  trace1("COMMON_BUILT_IN_BTI::COMMON_BUILT_IN_BTI new bti", c);
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_BTI::COMMON_BUILT_IN_BTI(const COMMON_BUILT_IN_BTI& p)
  :COMMON_COMPONENT(p),
   lambda(p.lambda),
   number(p.number),
   weight(p.weight),
   polarity(p.polarity),
   _sdp(0),
   _RCD(0)
{
   trace0("some bti copy");      
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_BTI::~COMMON_BUILT_IN_BTI()
{
// assert (_RCD1 == _RCD[0]);
//  assert(_RCD1);
if (_RCD) {  detach_common(&(_RCD[0]));
  //detach_common(&_RCD2);
  //detach_common(&_RCD3);
  delete []  _RCD;
  _RCD=NULL;

}
  --_count;
  delete _sdp;
}
/*--------------------------------------------------------------------------*/
bool COMMON_BUILT_IN_BTI::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_BUILT_IN_BTI* p = dynamic_cast<const COMMON_BUILT_IN_BTI*>(&x);
  trace0("COMMON_BUILT_IN_BTI::operator==");

  return (p
    && lambda == p->lambda
    && number == p->number
    && 0
    && weight == p->weight
    && _sdp == p->_sdp
    && COMMON_COMPONENT::operator==(x));
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_BTI::set_param_by_index(int I, std::string& Value, int Offset)
{
  trace1("COMMON_BUILT_IN_BTI::set_param_by_index " + Value , I);
  switch (COMMON_BUILT_IN_BTI::param_count() - 1 - I) {
    case 0:  lambda = Value; break;
    case 1:  number = Value; break;
    case 2:  weight = Value; break;
    default: COMMON_COMPONENT::set_param_by_index(I, Value, Offset);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_BUILT_IN_BTI::param_is_printable(int i)const
{
  switch (COMMON_BUILT_IN_BTI::param_count() - 1 - i) {
    case 0:  return (true);
    case 1:  return (true);
    case 2:  return (true);
    default: return COMMON_COMPONENT::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_BTI::param_name(int i)const
{
  switch (COMMON_BUILT_IN_BTI::param_count() - 1 - i) {
  case 0:  return "lambda";
  case 1:  return "num";
  case 2:  return "weight";
  default: return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_BTI::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (COMMON_BUILT_IN_BTI::param_count() - 1 - i) {
    case 0:  return "";
    case 1:  return "";
    case 2:  return "";
    default: return "";
    }
  }else{untested();//281
    return COMMON_COMPONENT::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_BTI::param_value(int i)const
{
  switch (COMMON_BUILT_IN_BTI::param_count() - 1 - i) {
  case 0:  return lambda.string();
  case 1:  return number.string();
  case 2:  return weight.string();
  default: return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_BTI::precalc_first(const CARD_LIST* par_scope)
{
//  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(model());
//  assert(m); not yet?
  assert(par_scope);
  COMMON_COMPONENT::precalc_first(par_scope);

  e_val(&(this->lambda), 1.0, par_scope);
  e_val(&(this->number), uint_t(0), par_scope);
  e_val(&(this->weight), 17.0, par_scope);

}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_BTI::expand(const COMPONENT* d)
{
  trace0("COMMON_BUILT_IN_BTI::expand");
  COMMON_COMPONENT::expand(d);
  trace0(("COMMON_BUILT_IN_BTI::expand attaching " + modelname() ).c_str());
  attach_model(d);
  COMMON_BUILT_IN_BTI* c = this; USE(c);
  const MODEL_BUILT_IN_BTI* m = dynamic_cast<const MODEL_BUILT_IN_BTI*>(model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "bti");
  }
  trace1("COMMON_BUILT_IN_BTI ", m->rcd_number);
  
  if (!_RCD) {
    trace1("alloc", m->rcd_number);
    assert(!_RCD);
    _RCD = new COMMON_COMPONENT*[m->rcd_number];
    uint_t i;
    for(i=0; i<(uint_t)m->rcd_number; i++ ) _RCD[i]=NULL;
  }else{
    trace1("allocd", m->rcd_number);
  }
  // size dependent
  //delete _sdp;
  _sdp = m->new_sdp(this);
  assert(_sdp);
  const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(_sdp);
  assert(s); USE(s);



  // subcircuit commons, recursive
  //

  m->attach_rcds( (COMMON_BUILT_IN_RCD**) _RCD);
  trace0("COMMON_BUILT_IN_BTI::expand attached rcds");

  //weight=m->weight;

  assert(c == this);
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_BTI::precalc_last(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_last(par_scope);
  COMMON_BUILT_IN_BTI* c = this; USE(c);
  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(model());
  assert(m);
  e_val(&(this->lambda), 1.0, par_scope);
  e_val(&(this->number), uint_t(0), par_scope);
  e_val(&weight, m->weight, par_scope);
  _sdp = m->new_sdp(this);
  assert(_sdp);
  const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(_sdp);
  assert(s); USE(s);

  //
  m->attach_rcds( (COMMON_BUILT_IN_RCD**) _RCD);

  assert(c == this);
  return;
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI::attach_rcds(COMMON_BUILT_IN_RCD** _RCDc) const
{
  trace1("MODEL_BUILT_IN_BTI::attach_rcds()", (long int)_RCDc[0]);
  uint_t i;
  trace1(("attach_rcds "+ std::string(rcd_model_name)).c_str() , rcd_number);
  for(i=0; i < rcd_number; i++ ){
    COMMON_BUILT_IN_RCD* RCD1 = new COMMON_BUILT_IN_RCD;
    trace0(("MODEL_BUILT_IN_BTI::attach_rcds rcd modelname set to " 
          + std::string(rcd_model_name) ).c_str());
    RCD1->set_modelname( rcd_model_name ); // <= !
    RCD1->attach(this); // ?
    RCD1->weight = 1;
    RCD1->Recommon0 = hoch(i);
    RCD1->Rccommon0 = runter(i);
    RCD1->Rccommon1 = hoch(i);
    COMMON_COMPONENT::attach_common(RCD1, (COMMON_COMPONENT**)&(_RCDc[i]));
    trace1("", i );
    // assert (RCD1 == _RCD[i]);
  }

}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_BTI::hoch(int i) const {
  uint_t n = rcd_number;
  //int type = m->bti_type;
  int a = int(sqrt(n));
  int col=i % a;
  // int row=i / a;

  return 100 * pow(10,col);
}
/*--------------------------------------------------------------------------*/
double MODEL_BUILT_IN_BTI::runter(int i) const{
  uint_t n = rcd_number;
  //int type = m->bti_type;
  int a = int(sqrt(n));
  // int col=i % a;
  int row=i / a;

  return 100 * pow(10,row);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace DEV_BUILT_IN_BTI_DISPATCHER { 
  static DEV_BUILT_IN_BTI p0;
  static DISPATCHER<CARD>::INSTALL
    dd0(&device_dispatcher, "bti", &p0);
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_BTI::DEV_BUILT_IN_BTI()
  :BASE_SUBCKT(),
   // input parameters,
   // calculated parameters,
   cutoff(false),
   // netlist,
   _Ubti(0),
   _RCD(0)
//   _RCD2(0),
//   _RCD3(0)
{
  _n = _nodes;
  trace1("DEV_BUILT_IN_BTI::DEV_BUILT_IN_BTI calling attach", hp(&Default_BUILT_IN_BTI));
  trace1("DEV_BUILT_IN_BTI::DEV_BUILT_IN_BTI" , Default_BUILT_IN_BTI.attach_count());
  trace0("DEV_BUILT_IN_BTI::DEV_BUILT_IN_BTI" + Default_BUILT_IN_BTI.modelname());
  attach_common(&Default_BUILT_IN_BTI);
  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_BTI::DEV_BUILT_IN_BTI(const DEV_BUILT_IN_BTI& p)
  :BASE_SUBCKT(p),
   // input parameters,
   // calculated parameters,
   cutoff(p.cutoff),
   // netlist,
   _Ubti(0),
   _RCD(0)
{
  _n = _nodes;
  for (uint_t ii = 0; ii < max_nodes() + int_nodes(); ++ii) {
    _n[ii] = p._n[ii];
  }
  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
bool DEV_BUILT_IN_BTI::do_tr() { // untested0(long_label().c_str());
  assert(subckt());set_converged(subckt()->do_tr());
  
  q_accept();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_BTI::expand()
{
  trace1("DEV_BUILT_IN_BTI::expand", long_label());
  BASE_SUBCKT::expand(); // calls common->expand, attaches model
  assert(_n);
  assert(common());
  const COMMON_BUILT_IN_BTI* c = static_cast<const COMMON_BUILT_IN_BTI*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(c->model());
  assert(m);
  assert(c->sdp());
  const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(c->sdp());
  assert(s); USE(s);

  if (!subckt()) {
    new_subckt();
  }else{
  }

  if (!_RCD) {
    trace1("alloc COMP", m->rcd_number);
    _RCD = new COMPONENT*[m->rcd_number];
    uint_t i;
    for(i=0; i<m->rcd_number; i++ ) _RCD[i]=NULL;
  }else{
    trace1("allocd", m->rcd_number);
  }


  if (_sim->is_first_expand()) {
    trace0("DEV_BUILT_IN_BTI::expand, first");
    precalc_first();
    precalc_last();
    // local nodes
    //assert(!(_n[n_iu].n_()));
    //BUG// this assert fails on a repeat elaboration after a change.
    //not sure of consequences when new_model_node called twice.
    if (!(_n[n_iu].n_())) {
      if (false) {
        _n[n_iu] = _n[n_b];
      }else{
        _n[n_iu].new_model_node("iu", this);
      }
    }else{
      if (false) {
        assert(_n[n_iu] == _n[n_b]);
      }else{
        //_n[n_iu].new_model_node("iu." + long_label(), this);
      }
    }

    // clone subckt elements
    double Ubti_par = 0;
    if (true) {
      if (!_Ubti) {
        const CARD* p ;
        if (!m->is_v2()){
          p = device_dispatcher["vcvs"];
          if (m->uref==NOT_INPUT )
            Ubti_par=1;
          else
            Ubti_par=m->uref;
        }else{
          p = device_dispatcher["vcvs"];
          Ubti_par = 1.0;
        }
        assert(p);
        _Ubti = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Ubti);
        subckt()->push_front(_Ubti);
      }else{
      }
      {
        node_t nodes[] = {_n[n_iu], _n[n_b], _n[n_g], _n[n_b]};
        // exp( v uref ) - 1 =! uref
        _Ubti->set_parameters("Ubti", this, NULL, Ubti_par, 0, NULL, 4, nodes);
      }
    }

    uint_t i;
//    assert(m->rcd_number == 1);
    for(i=0; i<m->rcd_number; i++ ) {
      trace0(".");
      if (!_RCD[i]) {
        const CARD* p = device_dispatcher["rcd"];
        assert(p);
        _RCD[i] = dynamic_cast<COMPONENT*>(p->clone());
        assert(_RCD[i]);
        subckt()->push_front(_RCD[i]);
      }else{
      }
      {
        node_t nodes[] = {_n[n_iu], _n[n_b]};
        std::string a; 
        a = m->RCD_name(i);
        _RCD[i]->set_parameters(a , this, c->_RCD[i], 0, 0, NULL, 2, nodes);
        trace1(("setpar " + a ).c_str(),i);
      }
    }
  }else{
    //precalc();
  }
  //precalc();
  subckt()->expand();
  //subckt()->precalc();
  assert(!is_constant());
  //if ( adp() == NULL ){
  //  attach_adp( m->new_adp( (COMPONENT*) this ) );
  //}else{
  //}
  //assert(adp());
  assert(subckt()->size() == size_t (m->rcd_number +1));
  trace0("DEV_BUILT_IN_BTI::expand");
}
/*--------------------------------------------------------------------------*/
// resultung dvth.
double DEV_BUILT_IN_BTI::vw()const{
  assert(_n);
  const COMMON_BUILT_IN_BTI* c = prechecked_cast<const COMMON_BUILT_IN_BTI*>(common());
  assert(c);
  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(c->model());
  assert(m);
  const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(c->sdp());
  assert(s); USE(s);
  const ADP_BUILT_IN_BTI* a = prechecked_cast<const ADP_BUILT_IN_BTI*>(adp()); a=a;
  double buf = 0;
  int i = m->rcd_number;
  for(; i-->0;  buf += CARD::probe(_RCD[i],"vw") );
  return buf*c->weight;
}
/*--------------------------------------------------------------------------*/
// voltage on cap. weighted.
double DEV_BUILT_IN_BTI::dvth()const{
  assert(_n);
  const COMMON_BUILT_IN_BTI* c = prechecked_cast<const COMMON_BUILT_IN_BTI*>(common());
  assert(c);
  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(c->model());
  assert(m);
  //const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(c->sdp());
  //assert(s);
  const ADP_BUILT_IN_BTI* a = prechecked_cast<const ADP_BUILT_IN_BTI*>(adp()); a=a;
  double buf = 0;
  int i = m->rcd_number;
  //for(; i-->0;  buf += CARD::probe(_RCD[i],"P") );
  for(; i-->0; ){
    const DEV_BUILT_IN_RCD* R = asserted_cast< const DEV_BUILT_IN_RCD* > ( _RCD[i] );
    double cont = R->P();
    if(cont<0){
      error(bWARNING, "Strange, %s contributes negative %E\n", _RCD[i]->long_label().c_str(), cont);
      cont=0;
    }
    buf += cont;
  }
  
  return buf * c->weight;
}
/*--------------------------------------------------------------------------*/
double DEV_BUILT_IN_BTI::tr_probe_num(const std::string& x)const
{
  assert(_n);
  const COMMON_BUILT_IN_BTI* c = prechecked_cast<const COMMON_BUILT_IN_BTI*>(common());
  assert(c);
  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(c->model());
  assert(m);
//  const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(c->sdp());
//  assert(s);
  const ADP_BUILT_IN_BTI* a = prechecked_cast<const ADP_BUILT_IN_BTI*>(adp()); a=a;

  if (Umatch(x, "vc{v} |fill ")) { 
    double buf = 0;
    int i=m->rcd_number;
    for(; i-->0;  buf += CARD::probe(_RCD[i],"vc") );
    return buf;
  }else if (Umatch(x, "rcdnumber ")) {
    return m->rcd_number;
  }else if (Umatch(x, "dvth |vth ")) {
    double buf = 0;
    int i=m->rcd_number;
    while ( i-->0 )   buf += CARD::probe(_RCD[i],"P");
    assert(is_number(buf));
    return buf * c->weight;
  }else if (Umatch(x, "vw ")) {
    return vw();
  }else if (Umatch(x, "wt ")) {
    return  c->weight;
  }else if (Umatch(x, "uin|v|vin ")) {
    return  _n[n_g].v0() - _n[n_b].v0();
  }else if (Umatch(x, "ueff ")) {
    return  _n[n_iu].v0() - _n[n_b].v0();
  }else if (Umatch(x, "pol{arity} ")) {
    return c->polarity;
  }else {
    return BASE_SUBCKT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
double DEV_BUILT_IN_BTI::tt_probe_num(const std::string& x)const
{
  assert(_n);
  const COMMON_BUILT_IN_BTI* c = prechecked_cast<const COMMON_BUILT_IN_BTI*>(common());
  assert(c);
  const MODEL_BUILT_IN_BTI* m = prechecked_cast<const MODEL_BUILT_IN_BTI*>(c->model());
  assert(m);
  const SDP_BUILT_IN_BTI* s = prechecked_cast<const SDP_BUILT_IN_BTI*>(c->sdp());
  assert(s); USE(s);
//  const ADP_BUILT_IN_BTI* a = prechecked_cast<const ADP_BUILT_IN_BTI*>(adp());
//  if(!a)untested0(("no a"+long_label()).c_str());

  if (Umatch(x, "vc{v} |fill ")) {
    double buf = 0;
    int i=m->rcd_number;
    while ( i-->0 )   buf += CARD::probe(_RCD[i],"vc");
    return buf * c->weight;
  }else if (Umatch(x, "dvth |vth ")) {
    double buf = 0;
    int i=m->rcd_number;
    while ( i-->0 )   buf += CARD::probe(_RCD[i],"P");
    return buf * c->weight;
  }else if (Umatch(x, "pol{arity} ")) {
    return  c->polarity;
  }else if (Umatch(x, "num ")) {
    return  m->rcd_number;
  }else if (Umatch(x, "uin ")) {
    return  _n[n_b].v0() - _n[n_g].v0();
  }else if (Umatch(x, "ueff ")) {
    return  _n[n_iu].v0() - _n[n_b].v0();
  }else {
    return BASE_SUBCKT::tt_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_BTI::tr_stress_last()
{
  trace0("DEV_BUILT_IN_BTI::tr_stress_last");
  assert(subckt()); 
  subckt()->do_forall( &CARD::tr_stress_last );
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_BTI::tr_stress() {
  trace0("DEV_BUILT_IN_BTI::tr_stress()");
  subckt()->do_forall( &CARD::tr_stress );
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_BTI::tt_commit() { unreachable();
  //FIXME, subckt default
  //        RCD reicht!
  subckt()->do_forall( &CARD::tt_commit ); // sort of tt_prepare?
}
/*--------------------------------------------------------------------------*/
//void DEV_BUILT_IN_BTI::tt_prepare() {
//  trace0("DEV_BUILT_IN_BTI::tt_prepare");
//  //FIXME, subckt default
//  //        RCD reicht!
//  subckt()->do_forall( &CARD::tt_prepare ); // sort of tt_prepare?
//}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_BTI::stress_apply() {
  itested();
  //FIXME, subckt default
  //        RCD reicht!
  subckt()->stress_apply();
}
/*--------------------------------------------------------------------------*/
// cc_direct

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_BTI::init(const COMPONENT* ) { }
void ADP_BUILT_IN_BTI::tt_commit() {}
void ADP_BUILT_IN_BTI::tt_accept() {}
double ADP_BUILT_IN_BTI::tt_probe_num(const std::string& )const {untested(); return 888;}
double ADP_BUILT_IN_BTI::tr_probe_num(const std::string& )const {untested(); return 888;}
