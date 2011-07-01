/*$Id: d_logic.cc,v 1.2 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2011 Felix Salfelder
 * Authors: Albert Davis <aldavis@gnu.org>
 *          Felix Salfelder
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
 *------------------------------------------------------------------
 *
 * gnucap device interface to Icarus Verilog
 *
 * FIXME: event sent out to ivl should be crosstime rather than time0
 * TODO: get model parms from Icarus (like gnucap-icarus spiceDLL)
 *
 */
//testing=script,sparse 2006.07.17
#include "d_subckt.h"
#include "u_xprobe.h"
#include "d_vvp.h"
#include "ap.h"
#include "u_lang.h"
#include "e_elemnt.h"
#include "m_interp.h"
#include "bm.h"
#include "l_lib.h"
#include "io_trace.h"
#include "vvp/vvp_net.h"
#include "extlib.h"
#include "d_ivl_ports.h"

//#include "schedule.cc"
/*--------------------------------------------------------------------------*/
inline double event_(struct event_time_s *et)
{
  return  double ( et->delay * (long double)
      pow(10.0,vpip_get_time_precision()) );
}
inline double event_absolute(struct event_time_s *et)
{
  return  double ( (et->delay+schedule_simtime() )  
      * (long double) pow(10.0,vpip_get_time_precision()) );
}
inline double digital_time(void)
{
  return double(schedule_simtime() * (long double)
      pow(10.0,vpip_get_time_precision())) ;
}
inline double prec(){
  return double(pow(10.0,vpip_get_time_precision()));
}
/*--------------------------------------------------------------------------*/
inline OMSTREAM& operator<<(OMSTREAM& o, direction_t t) {
  const std::string s[] = {"", "input", "output", "inout"};
  return (o << s[t]);
}
/*--------------------------------------------------------------------------*/
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
class COMMON_LOGIC_VVP;
/*--------------------------------------------------------------------------*/
int MODEL_LOGIC_VVP::_count = -1;
int COMMON_LOGIC_VVP::_count = -1;
int DEV_LOGIC_VVP::_count = -1;
typeof(COMMON_LOGIC_VVP::_commons) COMMON_LOGIC_VVP::_commons;
static COMMON_LOGIC_VVP Default_Logic_Params(CC_STATIC);
static LOGIC_NONE Default_LOGIC(CC_STATIC);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//   MODEL (not in use)
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC_VVP::precalc_first(){
  MODEL_LOGIC::precalc_first();
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC_VVP::precalc_last(){
  MODEL_LOGIC::precalc_last();
}
/*--------------------------------------------------------------------------*/
MODEL_LOGIC_VVP::MODEL_LOGIC_VVP(const DEV_LOGIC_VVP* p)
  :MODEL_LOGIC((const COMPONENT*) p){
    trace0("MODEL_LOGIC_VVP::MODEL_LOGIC_VVP");
  }
/*--------------------------------------------------------------------------*/
MODEL_LOGIC_VVP::MODEL_LOGIC_VVP(const MODEL_LOGIC_VVP& p)
  :MODEL_LOGIC(p){
    file=p.file;
    output=p.output;
    input=p.input;
    trace0("MODEL_LOGIC_VVP::MODEL_LOGIC_VVP");
}
/*--------------------------------------------------------------------------*/
bool MODEL_LOGIC_VVP::param_is_printable(int i) const{
    return (MODEL_LOGIC::param_count() - 1 - i)  < param_count();
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC_VVP::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= MODEL_LOGIC::param_count()) {
    return "";
  }else{
    return MODEL_LOGIC::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC_VVP::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_LOGIC_VVP::param_count() - 1 - i) {
  case 0: file = value; break;
  case 1: output = value; break;
  case 2: input = value; break;
  default: MODEL_LOGIC::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC_VVP::param_name(int i) const{
    switch (MODEL_LOGIC_VVP::param_count() - 1 - i) {
      case 0: return "file_unused";
      case 1: return "input";
      case 2: return "output";
      default: return MODEL_LOGIC::param_name(i);
    }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC_VVP::param_value(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: return file.string();
  case 1: return output.string();
  case 2: return input.string();
  default: return MODEL_LOGIC::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
// COMMON
/*--------------------------------------------------------------------------*/
COMMON_LOGIC_VVP::COMMON_LOGIC_VVP(int c)
      :COMMON_COMPONENT(c), 
      _extlib(0),
      incount(0),
      vvpfile(""),
      module(""),
      status(0),
      _logic_none(0)
{
  trace1("COMMON_LOGIC_VVP::COMMON_LOGIC_VVP", c);
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_LOGIC_VVP::COMMON_LOGIC_VVP(const COMMON_LOGIC_VVP& p)
      :COMMON_COMPONENT(p),
      _extlib(p._extlib),
      incount(p.incount),
      vvpfile(p.vvpfile),
      module(p.module),
      status(p.status),
      _logic_none(0)
{
  trace0("COMMON_LOGIC_VVP::COMMON_LOGIC_VVP( COMMON_LOGIC_VVP )");
  ++_count;
}
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC_VVP::operator==(const COMMON_COMPONENT& x )const{
  const COMMON_LOGIC_VVP* p = dynamic_cast<const COMMON_LOGIC_VVP*>(&x);
  bool ret = (vvpfile==p->vvpfile)
            && (module==p->module); // bad idea...?
  bool cr = COMMON_COMPONENT::operator==(x);

  return ret && cr;
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* COMMON_LOGIC_VVP::deflate()
{
  trace0("COMMON_LOGIC_VVP::deflate");
  // return this;
  for( list<const COMMON_COMPONENT*>::iterator i = _commons.begin();
      i != _commons.end(); ++i ){
    if (*this == **i){
      trace0("COMMON_LOGIC_VVP::deflate hit");
      return const_cast<COMMON_COMPONENT*>( *i );
    }
    trace0("COMMON_LOGIC_VVP::deflate miss");
  }
  _commons.push_back(this);
  return this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// get next event time from icarus queue.
TIME_PAIR  DEV_LOGIC_VVP::tr_review(){
   _time_by =  BASE_SUBCKT::tr_review();

  event_time_s* ctim = schedule_list();

  if (ctim){
    double delay = event_absolute(ctim);
    trace1("DEV_LOGIC_VVP::tr_review", delay );
    _time_by.min_event(delay + _sim->_time0);
  }
  q_accept();
  return _time_by;
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_VVP::do_tr(){
  assert(status);
  //q_accept();
  return BASE_SUBCKT::do_tr();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::tr_accept(){
  static double lastaccept;

  if ( lastaccept == _sim->_time0 && _sim->_time0!=0){
    return;
  }

  trace2("DEV_LOGIC_VVP::tr_accept", _sim->_time0,  lastaccept);
  lastaccept = _sim->_time0;

  // first queue events.
  // FIXME: just outports.
  subckt()->tr_accept();

  // then execute anything until now.
  trace1("DEV_LOGIC_VVP::tr_accept calling cont", _sim->_time0);
  vvp::contsim("",_sim->_time0);

  // accept again (the other nodes might have changed.)
  // FIXME: just inports
  subckt()->tr_accept();
  //uint_t incount=1;
  // node_t* n=&_n[2];
  
  // copy next event to master queue
  event_time_s* ctim = schedule_list();
  double evt;
  if (ctim){
    evt = event_(ctim);
    double eventtime = event_absolute(ctim);
    trace3("DEV_LOGIC_VVP::tr_accept,  fetching event",_sim->_time0, ctim->delay, eventtime);
    assert(evt>=0);
    if(evt==0) { untested(); };
    _sim->new_event(eventtime);
  }

  // CHECK: implement tr_needs_eval?
  q_eval();
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::precalc_first(const CARD_LIST* par_scope)
{
  trace0("COMMON_LOGIC_VVP::precalc_first " + (string) module + " " + (string) vvpfile );
  assert(par_scope);

  COMMON_COMPONENT::precalc_first(par_scope);
  vvpfile.e_val("UNSET", par_scope);
  module.e_val("UNSET", par_scope);

  //something hosed here.
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::expand(const COMPONENT* dev ){
  trace1("COMMON_LOGIC_VVP::expand" + dev->long_label(), (intptr_t) dev % PRIME);

  COMMON_COMPONENT::expand(dev);
  attach_model(dev);

  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(dev->long_label(), modelname(), name());
  }

  COMMON_LOGIC* logic_none = new LOGIC_NONE;
  logic_none->set_modelname(modelname());
  logic_none->attach(model());

  attach_common(logic_none, &_logic_none);
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::precalc_last(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_last(par_scope);

  trace0("COMMON_LOGIC_VVP::precalc_last " + (string) module + " " + (string) vvpfile );

  vvpfile.e_val("UNSET" , par_scope);
  module.e_val("UNSET" , par_scope);

  if(!_extlib){
#ifdef SOMETHING_
    /// dlopen has been overwritten!!1
    void* h = dlopen("libvvpg.so",RTLD_LAZY|RTLD_GLOBAL);
    if(h==NULL) throw Exception("cannot open libvvp: %s: ", dlerror());
    trace0("=========== LOADED libvvpg.so ==========");
    dlerror();
#else
    void* h = NULL;
#endif
    _extlib = new ExtLib("foo",h);
    int ret;
    ret=_extlib->init((string(vvpfile)+".vvp").c_str());   
    if(dlerror()) throw Exception("cannot init vvp %s", dlerror());

    if(ret)
      error(bDANGER, "somethings wrong with vvp: %s\n", vvpfile.string().c_str() );

  } else {
    trace0("COMMON_LOGIC_VVP::precalc_last already done extlib");
  }
  status++;
}
/*--------------------------------------------------------------------------*/
std::string DEV_LOGIC_VVP::port_name(uint_t i)const{
  stringstream a;
  a << "port" << i;
  return a.str();
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_VVP::~DEV_LOGIC_VVP() {
  --_count;

  trace0("~DEV_LOGIC_VVP");

/*  for( vector<COMMON_COMPONENT*>::iterator i = _subcommons.begin();
      i!=_subcommons.end(); ++i){
  //   COMMON_COMPONENT::detach_common(&(*i));
  }
  */
}
/*--------------------------------------------------------------------------*/
COMMON_LOGIC_VVP::~COMMON_LOGIC_VVP()	{
  --_count;

  detach_common(&_logic_none);

  for( vector<COMMON_COMPONENT*>::iterator i = _subcommons.begin();
      i!=_subcommons.end(); ++i){
    delete (&(*i));
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC_VVP::param_name(int i) const{
  trace1("COMMON_LOGIC_VVP::param_name ", i);
  switch (COMMON_COMPONENT::param_count() - 1 - i) {
    case 0: return "file";
    case 1: return "module";
    default: return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC_VVP::param_is_printable(int i) const{
    return (COMMON_COMPONENT::param_count() - 1 - i)  < param_count();
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC_VVP::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= COMMON_COMPONENT::param_count()) {
    return "";
  }else{
    return COMMON_COMPONENT::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC_VVP::param_value(int i)const
{
  switch (COMMON_COMPONENT::param_count() - 1 - i) {
    case 0: return vvpfile.string();
    case 1: return module.string();
    default: return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::set_param_by_index(int i, std::string& value, int offset)
{
  switch (COMMON_COMPONENT::param_count() - 1 - i) {
    case 0: vvpfile = value; 
            break;
    case 1: module = value;
            break;
    default: COMMON_COMPONENT::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* COMMON_LOGIC_VVP::clone()const
{
  return new COMMON_LOGIC_VVP(*this);
}
/*--------------------------------------------------------------------------*/

namespace {
static DEV_LOGIC_VVP p1;
static DISPATCHER<CARD>::INSTALL x1(&device_dispatcher, "vvp", &p1);
}
/*--------------------------------------------------------------------------*/
namespace MOD_DISP{
static DEV_LOGIC_VVP p1;
static MODEL_LOGIC_VVP p2(&p1);
static DISPATCHER<MODEL_CARD>::INSTALL
d2(&model_dispatcher, "vvpm", &p2);
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_VVP::DEV_LOGIC_VVP()
  :BASE_SUBCKT(),
  status(0)
{
  trace0("DEV_LOGIC_VVP::DEV_LOGIC_VVP attaching...");
  attach_common(&Default_Logic_Params); // as in mos.cc
  trace0("DEV_LOGIC_VVP::DEV_LOGIC_VVP nodes");
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_VVP::DEV_LOGIC_VVP(const DEV_LOGIC_VVP& p)
  :BASE_SUBCKT(p)
   ,status(0)
{
  for (uint_t ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
PLI_INT32 callback(t_cb_data*x){
  DEV_LOGIC_IN* c= reinterpret_cast<DEV_LOGIC_IN*>(x->user_data);
  assert(c);

  __vpiSignal* signal = (__vpiSignal*) x->obj;
  vvp_sub_pointer_t<vvp_net_t> ptr(signal->node,0);    

  struct t_vpi_value argval;  
  argval.format = vpiIntVal; 
  vpi_get_value(x->obj, &argval);
  const vvp_vector4_t bit(1,(vvp_bit4_t)argval.value.integer);

  trace5("callback called", bit.value(0), digital_time() ,
      x->value->value.integer, c->lvfromivl, CKT_BASE::_sim->_time0);
  assert(bit.value(0)==0 || bit.value(0)==1);

  if (c->lvfromivl == lvUNKNOWN){
    trace0("callback init.");
    c->lvfromivl  = _LOGICVAL(3*bit.value(0)) ;
    assert(CKT_BASE::_sim->_time0 == 0 );
    return 0 ;
  }

  CKT_BASE::_sim->new_event( double( digital_time()) );

  switch (bit.value(0)) {
    case 0: c->lvfromivl = lvFALLING; break;
    case 1: c->lvfromivl = lvRISING;  break;
    default: 
             error(bDANGER, "got bogus value %i from ivl\n", bit.value(0));
             break;
  }
  trace1("callback done ", c->lvfromivl);
  c->qe();
  return 0;
}
/*--------------------------------------------------------------------------*/
// TODO: cleanup
void DEV_LOGIC_VVP::expand()
{
  BASE_SUBCKT::expand();
  assert(_n);
  const COMMON_LOGIC_VVP* c = prechecked_cast<const COMMON_LOGIC_VVP*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);

  if (!subckt()) {
    new_subckt();
  }else{
  }
  
  if (_sim->is_first_expand()) {
    trace1 ("first expanding " +long_label(), net_nodes());
    precalc_first();
    precalc_last();
    uint_t n=2;

    ExtLib*el=((const COMMON_LOGIC_VVP*) common())->_extlib;
    assert(el);

    // else Icarus wont let me rigister callbacks 
    assert(vpi_mode_flag == VPI_MODE_NONE);
    vpi_mode_flag = VPI_MODE_COMPILETF;

    vpiHandle item;
    const char* modulename = ((string) c->module).c_str();

    vpiHandle module = vpi_handle_by_name(modulename,NULL);
    assert(module);

    vpiHandle vvp_device = vpi_handle_by_name(short_label().c_str(),module);
    assert(vvp_device);

    vpiHandle vvp_device_module = vpi_handle(vpiModule,vvp_device);
    assert(vvp_device_module);

    assert ((_n[0].n_()));
    assert ((_n[1].n_()));
    assert ((_n[2].n_()));
    assert ((_n[3].n_()));
    char src;
    COMMON_COMPONENT* logic_common;

    node_t lnodes[] = {_n[n], _n[0], _n[1], _n[1], _n[0]};
//  vpiParameter  holds fall, rise etc.
    string name;
    // not implemented
    // vpiHandle net_iterator = vpi_iterate(vpiPorts,vvp_device);
    vpiHandle net_iterator = vpi_iterate(vpiScope,vvp_device);
    assert(net_iterator);
    CARD* logicdevice;
    node_t* x;
    trace1("DEV_LOGIC_VVP::expand "+ short_label() + " entering loop", net_nodes());
    while ((item = vpi_scan(net_iterator))) {
      int type= vpi_get(vpiType,item);
      name = vpi_get_str(vpiName,item);
      COMPONENT* P;

      trace2("==> "+ short_label() + " " + string(name), item->vpi_type->type_code, n );

      switch(type){
        case vpiNet: // <- ivl
          {
          src='V';
          logic_common = c->_logic_none;
          x = new node_t();
          x->new_model_node("i_"+name, this);
          lnodes[0] = _n[n];
          lnodes[1] = _n[0];
          lnodes[2] = _n[1];
          lnodes[3] = _n[1];
          lnodes[4] = *x;
          logicdevice = device_dispatcher["port_from_ivl"];

          P = dynamic_cast<COMPONENT*>(logicdevice->clone());

          t_cb_data cbd = {
            cbValueChange, //reason
            callback, //cb_rtn
            item, //obj
            0, //time
            0, //value
            0, //index
            (char*)P //user_data
          };

          vpi_register_cb(&cbd);

          break;
          }
        case vpiReg: // -> ivl
          src='I';
          x = new node_t();
          x->new_model_node("i_"+name, this);
          lnodes[0] = *x;
          lnodes[1] = _n[0];
          lnodes[2] = _n[1];
          lnodes[3] = _n[1];
          lnodes[4] = _n[n];
          logic_common = c->_logic_none;
          logicdevice = device_dispatcher["port_to_ivl"];
          ((DEV_LOGIC_OUT*) logicdevice)->H = item;
          P = dynamic_cast<COMPONENT*>(logicdevice->clone());
          break;

        default:
          // which other types would make sense?
          continue;
      }

      trace2("DEV_LOGIC_VVP::expand "+ name + " " + short_label(), n, (intptr_t)logic_common);
      assert(_n[n].n_());

      assert(P);
      subckt()->push_front(P);

      trace3("setting parameters", intptr_t(logic_common), logic_common->attach_count(), n );
      trace0("modelname: " + logic_common->modelname());

      if (src=='I'){
        // to ivl
        COMMON_LOGIC* L = (COMMON_LOGIC*) logic_common;
        P->set_parameters(name, this, L, 0, 0, NULL, 5 , lnodes);
      }else{
        // from ivl.
        COMMON_LOGIC* L = (COMMON_LOGIC*) logic_common;
        P->set_parameters(name, this, L, 0, 0, NULL, 5 , lnodes);
      }

      trace1("loop end for "+name, n);
      n++;
    } 
    vpi_mode_flag = VPI_MODE_NONE;
  }

  std::string subckt_name(c->modelname()+"."+string(c->vvpfile));

  assert(!is_constant());
// is this an option?
// subckt()->set_slave();
  subckt()->expand();
}
/*--------------------------------------------------------------------------*/
ExtLib* DEV_LOGIC_VVP::extlib()const{
  return (((const COMMON_LOGIC_VVP*) common())->_extlib);
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::tr_begin()
{
  const COMMON_LOGIC_VVP* c = prechecked_cast<const COMMON_LOGIC_VVP*>(common());
  assert(c);

  trace1("DEV_LOGIC_VVP::tr_begin " + short_label(), status);
  vvp::startsim("TRAN");
  vvp::contsim("TRAN",0);

  // exchange initial conditions?
  // maybe not necessary (done during dc)

  status++;
  subckt()->tr_begin();
  q_eval();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::precalc_first()
{
  COMPONENT::precalc_first();
  assert(common());

  if(subckt()){
    subckt()->precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::precalc_last()
{
  COMPONENT::precalc_last();
  if (subckt()) {subckt()->precalc_last();}
  assert(common()->model());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// logic port devices.

/*--------------------------------------------------------------------------*/
#define INNODE BEGIN_IN
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int DEV_LOGIC_OUT::_count = -1;
int DEV_LOGIC_IN::_count = -1;
/*--------------------------------------------------------------------------*/
namespace logicports {
  static LOGIC_NONE Default_LOGIC(CC_STATIC);
  static DEV_LOGIC_OUT p1;
  static DISPATCHER<CARD>::INSTALL
    d1(&device_dispatcher, "port_to_ivl", &p1);

  static DEV_LOGIC_IN p2;
  static DISPATCHER<CARD>::INSTALL
    d2(&device_dispatcher, "port_from_ivl", &p2);
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_OUT::DEV_LOGIC_OUT()
  :DEV_LOGIC(),
  _lastchangenode(0),
  _quality(qGOOD),
  _failuremode("ok"),
  _oldgatemode(moUNKNOWN),
  _gatemode(moUNKNOWN),
  H(0)
{
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_OUT::DEV_LOGIC_OUT(const DEV_LOGIC_OUT& p)
  :DEV_LOGIC(p),
  _lastchangenode(0),
  _quality(qGOOD),
  _failuremode("ok"),
  _oldgatemode(moUNKNOWN),
  _gatemode(moUNKNOWN),
  H(p.H)
{
  for (uint_t ii = 0;  ii < max_nodes();  ++ii) {
    nodes[ii] = p.nodes[ii];
  }
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::expand()
{
  ELEMENT::expand();
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);

  attach_model();

  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(c->model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(long_label(), c->modelname(), "logic family (LOGIC)");
  }else{
  }

  std::string subckt_name(c->modelname()+c->name()+to_string(c->incount));
  try {
    const CARD* model = find_looking_out(subckt_name);

    if(!dynamic_cast<const MODEL_SUBCKT*>(model)) {untested();
      error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)),
          long_label() + ": " + subckt_name + " is not a subckt, forcing digital\n");
    }else{
      _gatemode = OPT::mode;    
      renew_subckt(model, this, scope(), NULL/*&(c->_params)*/);    
      subckt()->expand();
    }
  }catch (Exception_Cant_Find&) {
    error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)), 
        long_label() + ": can't find subckt: " + subckt_name + ", forcing digital\n");
  }

  assert(!is_constant()); /* is a BUG */
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_iwant_matrix()
{
  if (subckt()) {
    subckt()->tr_iwant_matrix();
  }else{
  }
  tr_iwant_matrix_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_begin()
{
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  ELEMENT::tr_begin();
  trace5("DEV_LOGIC_OUT::tr_begin", lvtoivl, _n[INNODE]->lv(), _n[INNODE]->v0(),
                                             _n[OUTNODE]->lv(), _n[OUTNODE]->v0());
  if (!subckt()) {
    _gatemode = moDIGITAL;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
    untested();
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
    subckt()->tr_begin();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_restore()
{untested();
  ELEMENT::tr_restore();
  if (!subckt()) {untested();
    _gatemode = moDIGITAL;
  }else{untested();
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    subckt()->tr_restore();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::dc_advance()
{
  trace0("DEV_LOGIC_OUT::dc_advance");
  ELEMENT::dc_advance();

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:
                    assert(subckt());
                    subckt()->dc_advance();
                    break;
    case moDIGITAL:
                    if ( _n[OUTNODE]->in_transit()) {
                      _n[OUTNODE]->propagate();
                    }else{
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_LOGIC_OUT::tr_advance()
{
  trace0("DEV_LOGIC::tr_advance");
  ELEMENT::tr_advance();

  if (_gatemode != _oldgatemode) {
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:
                    untested();
                    assert(subckt());
                    subckt()->tr_advance();
                    break;
    case moDIGITAL: 
                    if (_n[OUTNODE]->in_transit()) {
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {
                        _n[OUTNODE]->propagate();
                      }else{
                      }
                    }else{
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_regress()
{untested();
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{untested();
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:  untested();
                    assert(subckt());
                    subckt()->tr_regress();
                    break;
    case moDIGITAL: untested();
                    if (_n[OUTNODE]->in_transit()) {untested();
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {untested();
                        _n[OUTNODE]->propagate();
                      }else{untested();
                      }
                    }else{untested();
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC_OUT::tr_needs_eval()const
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL:
                    //assert(!is_q_for_eval());
                    if (_sim->analysis_is_restore()) {untested();
                    }else if (_sim->analysis_is_static()) {
                    }else{
                    }
                    return (_sim->analysis_is_static() || _sim->analysis_is_restore());
    case moANALOG:
                    //untested();
                    assert(!is_q_for_eval());
                    assert(subckt());
                    return subckt()->tr_needs_eval();
  }
  unreachable();
  return false;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_queue_eval()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:	  unreachable(); break;
    case moDIGITAL: ELEMENT::tr_queue_eval(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_OUT::tr_eval_digital()
/*{
  assert(_gatemode == moDIGITAL);
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
    tr_accept();
  }else{
    assert(_sim->analysis_is_tran_dynamic());
  }

  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  untested();
  
  _n[OUTNODE]->to_logic(m);
  set_converged(conv_check());
  store_values();
  q_load();

  return converged();
}*/
{
  trace0("DEV_LOGIC_OUT::tr_eval_digital");
  assert(_gatemode == moDIGITAL);
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
    tr_accept();
  }else{
    assert(_sim->analysis_is_tran_dynamic());
  }

  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  _y[0].x = 0.;
  _y[0].f1 = _n[OUTNODE]->to_analog(m);
  _y[0].f0 = 0.;
  _m0.x = 0.;
  _m0.c1 = 1./m->rs;
  _m0.c0 = _y[0].f1 / -m->rs;
  set_converged(conv_check());
  store_values();
  q_load();

  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_OUT::do_tr()
{  
  trace0("DEV_LOGIC_OUT::do_tr");
 //bm tr_eval
 //  ExtSigTrEval(_ext,const_cast<std::vector<DPAIR>*>(&_num_table),d);
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: set_converged(tr_eval_digital()); break;
    case moANALOG:  assert(subckt()); set_converged(subckt()->do_tr()); break;
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tt_next()
{
  assert(subckt());
  subckt()->tt_next();
  ELEMENT::tt_next();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_load()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: tr_load_passive(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_LOGIC_OUT::tr_review()
{
  //propagate change of  input state here?
//  q_accept();

  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL:
                    _time_by.reset(); 
                    tr_review_digital();
                    break;
    case moANALOG:  assert(subckt()); _time_by = subckt()->tr_review(); break;
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
// tell icarus about state change, before we accept
void DEV_LOGIC_OUT::tr_review_digital(){ // to ivl
  trace1("DEV_LOGIC_OUT::tr_review_digital()", _sim->_time0);
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  //_n[OUTNODE]->to_logic(m);

  _n[OUTNODE]->to_logic(m);
  _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = "ok"; // _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
  int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */

  // LOOP
  _n[INNODE]->to_logic(m);
  if (_n[INNODE]->quality() < _quality) {
    _quality = _n[INNODE]->quality();
    _failuremode = _n[INNODE]->failure_mode();
  }else{
  }
  if (_n[INNODE]->d_iter() >= lastchangeiter) {
    trace2("DEV_LOGIC_OUT::tr_accept inchange", _sim->_time0, _n[INNODE]->d_iter());
    lastchangeiter = _n[INNODE]->d_iter();
    _lastchangenode = INNODE;
  }else{
  }

  trace7("DEV_LOGIC_OUT::tr_review_digital", _sim->_time0,
      _n[INNODE]->quality(), 
      _n[OUTNODE]->quality(), 
      _n[INNODE]->v0(),
      _n[INNODE]->lv(),
      _n[OUTNODE]->v0(),
      _n[OUTNODE]->lv()
      );
/* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
   * Otherwise, an input changed.  Need to evaluate.
   * If all quality are good, can evaluate as digital.
   * Otherwise need to evaluate as analog.
   */
  trace3("DEV_LOGIC_OUT::tr_accept " + _failuremode, _lastchangenode, lastchangeiter, _quality);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  assert(want_digital());
  assert(_gatemode == moDIGITAL);

  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (!_sim->_bypass_ok
      || _lastchangenode != OUTNODE
      || _sim->analysis_is_static()
      || _sim->analysis_is_restore()) {
    LOGICVAL future_state = _n[INNODE]->lv();
    //		         ^^^^^^^^^^
    if ((_n[OUTNODE]->is_unknown() 
        ) &&
        (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
      trace1("DEV_LOGIC_OUT::tr_accept, Forcing", future_state);


      switch (future_state) {
        case lvSTABLE0:	/*nothing*/		break;
        case lvRISING:  future_state=lvSTABLE1;	break;
        case lvFALLING: future_state=lvSTABLE0;	break;
        case lvSTABLE1:	/*nothing*/		break;
        case lvUNKNOWN: unreachable();		break;
      }

      _n[OUTNODE]->force_initial_value(future_state);
      //time0==0

      //prop immediately, its just a shadow value.
      __vpiSignal* HS = (__vpiSignal*) H;
      vvp_sub_pointer_t<vvp_net_t> ptr(HS->node,0);
      int l=future_state;
      if (l==3) l=1;

      vvp_time64_t  dly=0;
      vvp_vector4_t val(1,l);
      trace2("DEV_LOGIC_OUT, forcing ivl", _sim->_time0, 0);
      schedule_assign_plucked_vector(ptr,dly,val,0,1);

      /* This happens when initial DC is digital.
       * Answers could be wrong if order in netlist is reversed 
       */
    }
                //else removed!
    if (future_state != _n[OUTNODE]->lv()) {
      trace0("DEV_LOGIC_OUT::tr_accept ...");
      assert(future_state != lvUNKNOWN);
      switch (future_state) {
        case lvSTABLE0:	/*nothing*/		break;
        case lvRISING:  future_state=lvSTABLE0;	break;
        case lvFALLING: future_state=lvSTABLE1;	break;
        case lvSTABLE1:	/*nothing*/		break;
        case lvUNKNOWN: unreachable();		break;
      }
      /* This handling of rising and falling may seem backwards.
       * These states occur when the value has been contaminated 
       * by another pending action.  The "old" value is the
       * value without this contamination.
       * This code is planned for replacement as part of VHDL/Verilog
       * conversion, so the kluge stays in for now.
       */
      assert(future_state.lv_old() == future_state.lv_future());
      if (_n[OUTNODE]->lv() == lvUNKNOWN
          || future_state.lv_future() != _n[OUTNODE]->lv_future()) {

        trace5("DEV_LOGIC_OUT::tr_accept, setting event... ", 
            _sim->_time0, future_state, _n[OUTNODE]->lv(), digital_time(), vvp::SimTimeD);
        _n[OUTNODE]->set_event(0, future_state);
        trace0("DEV_LOGIC_OUT::tr_accept, prop " );
        _n[OUTNODE]->propagate();
        q_eval();

        _sim->new_event(_n[OUTNODE]->final_time());
        lvtoivl = future_state;

        int l=future_state;
        if (l==3) l=1;

        assert(l==0 || l ==1 );
        assert(H);

        __vpiSignal* HS = (__vpiSignal*) H;
        vvp_sub_pointer_t<vvp_net_t> ptr(HS->node,0);

        // BUG: sim->time0 is not necessarily the time of this event

        //need floor. event needs to be executed during DEV_LOGIC_VVP::tr_accept
        vvp_time64_t  i_dly = (int)floor(_sim->_time0 /
            pow(10.0,vpip_get_time_precision())) -  schedule_simtime();
        vvp_time64_t  dly(i_dly);
        vvp_vector4_t val(1,l);
        trace2("DEV_LOGIC_OUT, scheduling ivl event", _sim->_time0, i_dly);
        schedule_assign_plucked_vector(ptr,dly,val,0,1);

        // CLEAR: what if step is rejected?

        assert (_lastchangenode != OUTNODE);
      }else{
      }
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC_OUT::tr_accept() // to ivl
{
  return;

  trace1("DEV_LOGIC_OUT::tr_accept()", _sim->_time0);
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  //_n[OUTNODE]->to_logic(m);

  _n[OUTNODE]->to_logic(m);
  _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = "ok"; // _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
  int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */

  // LOOP
  _n[INNODE]->to_logic(m);
  if (_n[INNODE]->quality() < _quality) {
    _quality = _n[INNODE]->quality();
    _failuremode = _n[INNODE]->failure_mode();
  }else{
  }
  if (_n[INNODE]->d_iter() >= lastchangeiter) {
    trace2("DEV_LOGIC_OUT::tr_accept inchange", _sim->_time0, _n[INNODE]->d_iter());
    lastchangeiter = _n[INNODE]->d_iter();
    _lastchangenode = INNODE;
  }else{
  }

  trace7("DEV_LOGIC_OUT::tr_accept", _sim->_time0,
      _n[INNODE]->quality(), 
      _n[OUTNODE]->quality(), 
      _n[INNODE]->v0(),
      _n[INNODE]->lv(),
      _n[OUTNODE]->v0(),
      _n[OUTNODE]->lv()
      );
/* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
   * Otherwise, an input changed.  Need to evaluate.
   * If all quality are good, can evaluate as digital.
   * Otherwise need to evaluate as analog.
   */
  trace3("DEV_LOGIC_OUT::tr_accept " + _failuremode, _lastchangenode, lastchangeiter, _quality);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  assert(want_digital());
  assert(_gatemode == moDIGITAL);

  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (!_sim->_bypass_ok
      || _lastchangenode != OUTNODE
      || _sim->analysis_is_static()
      || _sim->analysis_is_restore()) {
    LOGICVAL future_state = _n[INNODE]->lv();
    //		         ^^^^^^^^^^
    if ((_n[OUTNODE]->is_unknown() 
        ) &&
        (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
      trace1("DEV_LOGIC_OUT::tr_accept, Forcing", future_state);


      switch (future_state) {
        case lvSTABLE0:	/*nothing*/		break;
        case lvRISING:  future_state=lvSTABLE1;	break;
        case lvFALLING: future_state=lvSTABLE0;	break;
        case lvSTABLE1:	/*nothing*/		break;
        case lvUNKNOWN: unreachable();		break;
      }

      _n[OUTNODE]->force_initial_value(future_state);
      //time0==0

      //prop immediately, its just a shadow value.
      __vpiSignal* HS = (__vpiSignal*) H;
      vvp_sub_pointer_t<vvp_net_t> ptr(HS->node,0);
      int l=future_state;
      if (l==3) l=1;

      //need floor. event needs to be executed during DEV_LOGIC_VVP::tr_accept
      vvp_time64_t  dly=0;
      vvp_vector4_t val(1,l);
      trace2("DEV_LOGIC_OUT, forcing ivl", _sim->_time0, 0);
      schedule_assign_plucked_vector(ptr,dly,val,0,1);

      /* This happens when initial DC is digital.
       * Answers could be wrong if order in netlist is reversed 
       */
    }
                //else removed!
    if (future_state != _n[OUTNODE]->lv()) {
      trace0("DEV_LOGIC_OUT::tr_accept ...");
      assert(future_state != lvUNKNOWN);
      switch (future_state) {
        case lvSTABLE0:	/*nothing*/		break;
        case lvRISING:  future_state=lvSTABLE0;	break;
        case lvFALLING: future_state=lvSTABLE1;	break;
        case lvSTABLE1:	/*nothing*/		break;
        case lvUNKNOWN: unreachable();		break;
      }
      /* This handling of rising and falling may seem backwards.
       * These states occur when the value has been contaminated 
       * by another pending action.  The "old" value is the
       * value without this contamination.
       * This code is planned for replacement as part of VHDL/Verilog
       * conversion, so the kluge stays in for now.
       */
      assert(future_state.lv_old() == future_state.lv_future());
      if (_n[OUTNODE]->lv() == lvUNKNOWN
          || future_state.lv_future() != _n[OUTNODE]->lv_future()) {

        trace5("DEV_LOGIC_OUT::tr_accept, setting event... ", 
            _sim->_time0, future_state, _n[OUTNODE]->lv(), digital_time(), vvp::SimTimeD);
        _n[OUTNODE]->set_event(0, future_state);
        trace0("DEV_LOGIC_OUT::tr_accept, prop " );
        _n[OUTNODE]->propagate();
        q_eval();

        _sim->new_event(_n[OUTNODE]->final_time());
        lvtoivl = future_state;

        int l=future_state;
        if (l==3) l=1;

        assert(l==0 || l ==1 );
        assert(H);

        __vpiSignal* HS = (__vpiSignal*) H;
        vvp_sub_pointer_t<vvp_net_t> ptr(HS->node,0);

        //need floor. event needs to be executed during DEV_LOGIC_VVP::tr_accept
        vvp_time64_t  i_dly = (int)floor(_sim->_time0 /
            pow(10.0,vpip_get_time_precision())) -  schedule_simtime();
        vvp_time64_t  dly(i_dly);
        vvp_vector4_t val(1,l);
        trace2("DEV_LOGIC_OUT, scheduling ivl event", _sim->_time0, i_dly);
        schedule_assign_plucked_vector(ptr,dly,val,0,1);

        assert (_lastchangenode != OUTNODE);
      }else{
      }
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::tr_unload()
{
  if (subckt()) {
    subckt()->tr_unload();
  }else{untested();
  }
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::ac_iwant_matrix()
{
  if (subckt()) {
    subckt()->ac_iwant_matrix();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_OUT::ac_begin()
{untested();
  if (subckt()) {untested();
    subckt()->ac_begin();
  }else{untested();
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC_OUT::tr_probe_num(const std::string& what)const
{
  if (Umatch(what, "ivl ")) { 
    return lvtoivl;
  } 
  return _n[OUTNODE]->tr_probe_num(what);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC_OUT::ac_probe_ext(const std::string& what)const
{untested();
  return _n[OUTNODE]->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_OUT::want_analog()const
{
  untested();
  return false;
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_OUT::want_digital()const
{
  return !subckt() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_LOGIC_IN::DEV_LOGIC_IN()
  :DEV_LOGIC(),
  _lastchangenode(0),
  _quality(qGOOD),
  _failuremode("ok"),
  _oldgatemode(moUNKNOWN),
  _gatemode(moUNKNOWN)   
{
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_IN::DEV_LOGIC_IN(const DEV_LOGIC_IN& p)
  :DEV_LOGIC(p),
  _lastchangenode(0),
  _quality(qGOOD),
  _failuremode("ok"),
  _oldgatemode(moUNKNOWN),
  _gatemode(moUNKNOWN)   
{
  for (uint_t ii = 0;  ii < max_nodes();  ++ii) {
    nodes[ii] = p.nodes[ii];
  }
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::expand()
{
  ELEMENT::expand();
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);

  attach_model();

  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(c->model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(long_label(), c->modelname(), "logic family (LOGIC)");
  }else{
  }

  std::string subckt_name(c->modelname()+c->name()+to_string(c->incount));
  try {
    const CARD* model = find_looking_out(subckt_name);

    if(!dynamic_cast<const MODEL_SUBCKT*>(model)) {untested();
      error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)),
          long_label() + "DEV_LOGIC_IN::expand: " + subckt_name + " is not a subckt, forcing digital\n");
    }else{
      _gatemode = OPT::mode;    
      renew_subckt(model, this, scope(), NULL/*&(c->_params)*/);    
      subckt()->expand();
    }
  }catch (Exception_Cant_Find&) {
    error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)), 
        long_label() + ": can't find subckt: " + subckt_name + ", forcing digital\n");
  }

  trace1("DEV_LOGIC_IN::expand",_gatemode);

  assert(!is_constant()); /* is a BUG */
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_iwant_matrix()
{
  if (subckt()) {
    subckt()->tr_iwant_matrix();
  }else{
  }
  tr_iwant_matrix_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_begin()
{
  ELEMENT::tr_begin();
  if (!subckt()) {
    _gatemode = moDIGITAL;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
    subckt()->tr_begin();
  }
  q_eval();

}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_restore()
{untested();
  ELEMENT::tr_restore();
  if (!subckt()) {untested();
    _gatemode = moDIGITAL;
  }else{untested();
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    subckt()->tr_restore();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::dc_advance()
{
  ELEMENT::dc_advance();
  trace5("DEV_LOGIC_IN::dc_advance", 
      lvfromivl,
      _n[OUTNODE]->is_digital(),
      _n[INNODE]->v0(),
      _gatemode,
      _oldgatemode );

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:
                    assert(subckt());
                    subckt()->dc_advance();
                    break;
    case moDIGITAL:
                    if (_n[OUTNODE]->in_transit()) {
                      //q_eval(); evalq is not used for DC
                      _n[OUTNODE]->propagate();
                    }else{
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_LOGIC_IN::tr_advance()
{
  trace0("DEV_LOGIC_IN::tr_advance");
  ELEMENT::tr_advance();

  if (_gatemode != _oldgatemode) {
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:
                    assert(subckt());
                    subckt()->tr_advance();
                    break;
    case moDIGITAL: 
                    if (_n[OUTNODE]->in_transit()) {
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {
                        _n[OUTNODE]->propagate();
                      }else{
                      }
                    }else{
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_regress()
{untested();
  trace0("DEV_LOGIC_IN::tr_regress");
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{untested();
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:  untested();
                    assert(subckt());
                    subckt()->tr_regress();
                    break;
    case moDIGITAL: untested();
                    if (_n[OUTNODE]->in_transit()) {untested();
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {untested();
                        _n[OUTNODE]->propagate();
                      }else{untested();
                      }
                    }else{untested();
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC_IN::tr_needs_eval()const
{
  trace1("DEV_LOGIC_IN::tr_needs_eval",_gatemode);
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL:
                    //assert(!is_q_for_eval());
                    if (_sim->analysis_is_restore()) {untested();
                    }else if (_sim->analysis_is_static()) {
                    }else{
                    }
                    return (_sim->analysis_is_static() || _sim->analysis_is_restore());
    case moANALOG:
                    //untested();
                    assert(!is_q_for_eval());
                    assert(subckt());
                    return subckt()->tr_needs_eval();
  }
  unreachable();
  return false;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_queue_eval()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:	  unreachable(); break;
    case moDIGITAL: ELEMENT::tr_queue_eval(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_IN::tr_eval_digital()
{
  trace0("DEV_LOGIC_IN::tr_eval_digital");
  assert(_gatemode == moDIGITAL);
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
    tr_accept();
  }else{
    assert(_sim->analysis_is_tran_dynamic());
  }

  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  _y[0].x = 0.;
  _y[0].f1 = _n[OUTNODE]->to_analog(m);
  trace1("DEV_LOGIC_IN::tr_eval_digital", _y[0].f1);
  _y[0].f0 = 0.;
  _m0.x = 0.;
  _m0.c1 = 1./m->rs;
  _m0.c0 = _y[0].f1 / -m->rs;
  set_converged(conv_check());
  store_values();
  q_load();

  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_IN::do_tr()
{  
 //bm tr_eval
 //  ExtSigTrEval(_ext,const_cast<std::vector<DPAIR>*>(&_num_table),d);

  trace2("DEV_LOGIC_IN::do_tr " + long_label(),_sim->_time0, _gatemode);
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: set_converged(tr_eval_digital()); break;
    case moANALOG:  assert(subckt()); set_converged(subckt()->do_tr()); break;
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tt_next()
{
  assert(subckt());
  subckt()->tt_next();
  ELEMENT::tt_next();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_load()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: tr_load_passive(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_LOGIC_IN::tr_review()
{
  // not calling ELEMENT::tr_review();
  //
  //
  //double dt_s = d->_sim->_dtmin;
  //double dt = ExtSigTrCheck(_ext,dt_s,
  //    const_cast<std::vector<DPAIR>*>(&_num_table),d);
  //if (dt < dt_s) {
  //  d->_time_by.min_event(dt + d->_sim->_time0);
 // }

  // q_accept();
  //digital mode queues events explicitly in tr_accept

  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: _time_by.reset(); break;
    case moANALOG:  assert(subckt()); _time_by = subckt()->tr_review(); break;
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
// stupid hack. doesnt do the right thing.
// need: mark_for_q_eval (or something like that.)
void DEV_LOGIC_IN::qe() 
{
  trace1("DEV_LOGIC_IN::qe",_n[OUTNODE]->is_digital());
  //  tr_queue_eval();
  //  q_eval();
  _sim->_evalq_uc->push_back(owner());
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC_IN::tr_accept() // from ivl.
{
  trace4("DEV_LOGIC_IN::tr_accept() " +long_label(),
      _gatemode,  _n[OUTNODE]->is_digital(),_sim->_time0,lvfromivl);
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  _n[OUTNODE]->to_logic(m);
  _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
//  int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */
  trace0(long_label().c_str());
  trace2(_n[OUTNODE]->failure_mode().c_str(), OUTNODE, _n[OUTNODE]->quality());

  if (lvfromivl == lvRISING || lvfromivl == lvFALLING)
  {
    trace1("DEV_LOGIC_IN::tr_accept", lvfromivl);
    _lastchangenode=1;
  }

  trace2(_failuremode.c_str(), _lastchangenode, _quality);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  if (want_analog()) {
    trace0("DEV_LOGIC_IN want_analog");
    untested();
    if (_gatemode == moDIGITAL) {untested();
      error(bTRACE, "%s:%u:%g switch to analog, %s\n", long_label().c_str(),
          _sim->iteration_tag(), _sim->_time0, _failuremode.c_str());
      _oldgatemode = _gatemode;
      _gatemode = moANALOG;
    }else{
    }
    assert(_gatemode == moANALOG);
  }else{
    assert(want_digital());
    trace0("DEV_LOGIC_IN want_digital");
    if (_gatemode == moANALOG) {
      error(bTRACE, "%s:%u:%g switch to digital\n",
          long_label().c_str(), _sim->iteration_tag(), _sim->_time0);
      _oldgatemode = _gatemode;
      _gatemode = moDIGITAL;
    }else{
    }
    assert(_gatemode == moDIGITAL);
    if (_sim->analysis_is_restore()) {untested();
    }else if (_sim->analysis_is_static()) {
    }else{
    }
    trace2("DEV_LOGIC_IN, transition?", _lastchangenode, _sim->_time0);
    if (!_sim->_bypass_ok
        || _lastchangenode != OUTNODE
        || _sim->analysis_is_static()
        || _sim->analysis_is_restore()) {
      trace1("DEV_LOGIC_IN transition",lvfromivl);
      LOGICVAL future_state = lvfromivl;

// enum smode_t   {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};
// enum _LOGICVAL {lvSTABLE0,lvRISING,lvFALLING,lvSTABLE1,lvUNKNOWN};

      if ((_n[OUTNODE]->is_unknown()) &&
          (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
        trace3("DEV_LOGIC_IN force",future_state, _sim->_time0, _gatemode);
        _n[OUTNODE]->force_initial_value(future_state);
        /* This happens when initial DC is digital.
         * Answers could be wrong if order in netlist is reversed 
         */
      }else if (future_state != _n[OUTNODE]->lv()) {
        trace4("DEV_LOGIC_IN, new state", future_state, _n[OUTNODE]->lv(),
            _sim->_time0, future_state.lv_future());
        assert(future_state != lvUNKNOWN);
        switch (future_state) {
          case lvSTABLE0:	/*nothing*/		break;
          case lvRISING:  future_state=lvSTABLE1;	break;
          case lvFALLING: future_state=lvSTABLE0;	break;
          case lvSTABLE1:	/*nothing*/		break;
          case lvUNKNOWN: unreachable();		break;
        }
        lvfromivl = future_state;
        /* This handling of rising and falling may seem backwards.
         * These states occur when the value has been contaminated 
         * by another pending action.  The "old" value is the
         * value without this contamination.
         * This code is planned for replacement as part of VHDL/Verilog
         * conversion, so the kluge stays in for now.
         */
        trace3("DEV_LOGIC_IN debugging",  future_state.lv_future() ,
            _n[OUTNODE]->lv_future(),_n[OUTNODE]->lv() );
        assert(future_state.lv_old() == future_state.lv_future());

        if (_n[OUTNODE]->lv() == lvUNKNOWN
            || future_state.lv_future() != _n[OUTNODE]->lv_future()) {

          double delay;
          switch(future_state) {
            case lvSTABLE0: delay=m->fall; break;
            case lvSTABLE1: delay=m->rise; break;
            default:
                            assert(false); break;
          }
          _n[OUTNODE]->set_event(delay, future_state);
          trace1("DEV_LOGIC_IN new event", _n[OUTNODE]->final_time());
          _sim->new_event(_n[OUTNODE]->final_time());

          //assert(future_state == _n[OUTNODE].lv_future());
          if (_lastchangenode == OUTNODE) {
            unreachable();
            error(bDANGER, "%s:%u:%g non-event state change\n",
                long_label().c_str(), _sim->iteration_tag(), _sim->_time0);
          }else{
          }
        }else{
        }
      }else{
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_unload()
{
  if (subckt()) {
    subckt()->tr_unload();
  }else{untested();
  }
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::ac_iwant_matrix()
{
  if (subckt()) {
    subckt()->ac_iwant_matrix();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::ac_begin()
{untested();
  if (subckt()) {untested();
    subckt()->ac_begin();
  }else{untested();
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC_IN::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "ivl ")) { 
    return lvfromivl;
  }
  return _n[OUTNODE]->tr_probe_num(x);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC_IN::ac_probe_ext(const std::string& what)const
{untested();
  return _n[OUTNODE]->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_IN::want_analog()const { return false; }
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_IN::want_digital()const
{
  return !subckt() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
}
