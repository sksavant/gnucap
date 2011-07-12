/*$Id: d_logic.cc,v 1.2 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *         Felix Salfelder 2011
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
 *------------------------------------------------------------------*/
#include "d_subckt.h"
#include "u_xprobe.h"
#include "d_ivl.h"
#include "d_ivl_ports.h"
#include "d_logic.h"
#include <dlfcn.h>
// #include "vvp/vvp_net.h"
#include "vvp/compile.h"

// COMMON_IVL::~COMMON_IVL(){}
int DEV_IVL_BASE::_count;

// enum smode_t   {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};

DEV_IVL_BASE::DEV_IVL_BASE(): BASE_SUBCKT() 
{
  _n = _nodes;
  ++_count;
}
DEV_IVL_BASE::DEV_IVL_BASE(const DEV_IVL_BASE& p): BASE_SUBCKT(p){
  for (uint_t ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  ++_count;
}

//bug?
DEV_IVL_BASE::~DEV_IVL_BASE(){
  --_count;
}

void DEV_IVL_BASE::tr_accept(){
  static double lastaccept;
  const COMMON_IVL* c = prechecked_cast<const COMMON_IVL*>(common());
  assert(c);

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
  c->contsim("",_sim->_time0);

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

/*------------------------------------------------------------------*/
void DEV_IVL_BASE::tr_begin()
{
  const COMMON_IVL* c = prechecked_cast<const COMMON_IVL*>(common());
  assert(c);

  trace0("DEV_IVL_BASE::tr_begin " + short_label());

  // fixme. only once per common
  c->startsim();
  c->contsim("TRAN",0);

  // exchange initial conditions?
  // maybe not necessary (done during dc)

  subckt()->tr_begin();
  q_eval();
}
/*--------------------------------------------------------------------------*/
void DEV_IVL_BASE::precalc_first()
{
  trace0("DEV_IVL_BASE::precalc_first");
  COMPONENT::precalc_first();
  assert(common());

  if(subckt()){
    subckt()->precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_IVL_BASE::precalc_last()
{
  COMPONENT::precalc_last();
  if (subckt()) {subckt()->precalc_last();}
  assert(common()->model());
}
/*--------------------------------------------------------------------------*/
std::string DEV_IVL_BASE::port_name(uint_t i)const{
  const COMMON_IVL* c = prechecked_cast<const COMMON_IVL*>(common());
  assert(c);
  const MODEL_IVL_BASE* m = prechecked_cast<const MODEL_IVL_BASE*>(c->model());
  assert(m);
  return m->port_name(i);
}
/*--------------------------------------------------------------------------*/
void DEV_IVL_BASE::expand()
{
  trace0("DEV_IVL_BASE::expand " + short_label());
  BASE_SUBCKT::expand();
  assert(_n);
  const COMMON_IVL* c = prechecked_cast<const COMMON_IVL*>(common());
  assert(c);
  const MODEL_IVL_BASE* m = prechecked_cast<const MODEL_IVL_BASE*>(c->model());
  assert(m);

  if (!subckt()) {
    new_subckt();
  }else{
  }
  
  if (_sim->is_first_expand()) {
    trace1 ("First expanding " +long_label(), net_nodes());
    precalc_first();
    precalc_last();
    uint_t n=2;

    void*el=((const COMMON_IVL*) common())->vvpso;
    assert(el);

    COMPILE* comp = c->get_compiler();
    assert(comp);
    trace0("compiling...");
    comp->ca();
    trace0("compiling...");
    // c->init();
    c->compile_design(comp, this);
    trace0("done compiling...");


    assert(vpi_mode_flag == VPI_MODE_NONE);
    vpi_mode_flag = VPI_MODE_COMPILETF;

    vpiHandle item;

    trace0("looking up in vhbn");
    vpiHandle module = (c->vhbn)(short_label().c_str(),NULL);
    assert(module);

    vpiHandle vvp_device = (c->vhbn)(short_label().c_str(),module);
    assert(vvp_device);

    vvp_device=module;

    /// old code from d_vvp

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

    CARD* logicdevice;
    node_t* x;
    trace1("DEV_IVL_BASE::expand "+ short_label() + " entering loop", net_nodes());

    // expand_nodes(); ?

    // vpiHandle net_iterator = vpi_iterate(vpiScope,vvp_device);
    //while ((item = vpi_scan(net_iterator))) {
    //
    while ( n < m->net_nodes() ) {
      item = vpi_handle_by_name( port_name(n).c_str() , vvp_device );

      int type = vpi_get(vpiType,item);
      name = vpi_get_str(vpiName,item);
      COMPONENT* P;


      trace2("==> "+ short_label() + " item name: " + string(name), item->vpi_type->type_code, n );
      trace0("==>  looking for " + port_name(n));

      switch(type){
        case vpiNet: // <- ivl
          {
          trace0("  ==> Net: " + string(name) );
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
          trace0("  ==> Reg: " + string(name) );
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

      trace2("DEV_LOGIC_IVL::expand "+ name + " " + short_label(), n, (intptr_t)logic_common);
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
/*---------------------*/
bool DEV_IVL_BASE::do_tr(){
  trace0("DEV_IVL_BASE::do_tr");
  //q_accept();
  return BASE_SUBCKT::do_tr();
}
/*--------------------------------------------------------------------------*/
TIME_PAIR  DEV_IVL_BASE::tr_review(){
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int COMMON_IVL::_count = -1;
/*--------------------------------------------------------------------------*/
COMMON_IVL::COMMON_IVL(int c)
      :COMMON_COMPONENT(c), 
      vvpso(0),
      incount(0),
      vvpfile(""),
      module(""),
      _logic_none(0)
{
  trace1("COMMON_IVL::COMMON_IVL", c);
  ++_count;
}
/*--------------------------------------------------------------------------*/
typeof(COMMON_IVL::_commons) COMMON_IVL::_commons;
/*--------------------------------------------------------------------------*/
COMMON_IVL::COMMON_IVL(const COMMON_IVL& p)
      :COMMON_COMPONENT(p),
      vvpso(0), // correct?
      incount(p.incount),
      vvpfile(p.vvpfile),
      module(p.module),
      _logic_none(0)
{
  trace0("COMMON_IVL::COMMON_IVL( COMMON_IVL )");
  ++_count;
}
/*--------------------------------------------------------------------------*/
double COMMON_IVL::contsim(const char *analysis,double time) const
{
	trace0("contsim");
	SimTimeA  = time;
	SimDelayD = -1;

	if (0 == strcmp("TRAN",analysis)) {
		SimState = SIM_CONT0;
		while (SimTimeDlast < time) {
			SimState = schedule_simulate_m(SimState);
			SimTimeDlast = SimTimeD;
			if (SIM_PREM <= SimState) break;
		}
	}

	return SimDelayD;
} 
/*--------------------------------------------------------------------------*/
double COMMON_IVL::startsim()const
{
  trace0("startsim -> schedule_simulate_m(SIM_INIT)");
  SimDelayD  = -1;

  SimState = schedule_simulate_m(SIM_INIT);

  SimTimeDlast = SimTimeD;

  return SimDelayD;
} 
/*--------------------------------------------------------------------------*/
sim_mode COMMON_IVL::schedule_simulate_m(sim_mode mode) const
{
  trace1("schedule_simulate_m", mode);
  struct event_s      *cur  = 0;
  struct event_time_s *ctim = 0;
  double               d_dly;

  switch (mode) {
    case SIM_CONT0: if ((ctim = schedule_list())) goto sim_cont0;
                      goto done;
    case SIM_CONT1: goto sim_cont1;
    default:
                    break;

  }

  assert(schedule_simtime()==0);

  // Execute end of compile callbacks
  vpiEndOfCompile();
  trace0("Done EOC");

  // Execute initialization events.
  exec_init_list();
  trace0("Done init events...");

  // Execute start of simulation callbacks

  trace0("calling vpiStartOfSim");
  vpiStartOfSim();


  // do i need signals here?
  // signals_capture();
  // trace1("signals_capture Done", schedule_runnable());

  trace0("schedule_list?");
  if (schedule_runnable())
    while (schedule_list()) {
      trace0("schedule_list");

      if (schedule_stopped()) {
        schedule_start();
        stop_handler(0);
        // You can finish from the debugger without a time change.
        if (!schedule_runnable()) break;
        goto cycle_done;
      }

      /* ctim is the current time step. */
      ctim = schedule_list();

      /* If the time is advancing, then first run the
         postponed sync events. Run them all. */
      if (ctim->delay > 0) {
        switch (mode) {
          case SIM_CONT0:
          case SIM_CONT1:
          case SIM_INIT:

            trace0("SIM_SOME");
            d_dly = getdtime(ctim);
            if (d_dly > 0) {
              trace0("skip ExtPWL");
              //doExtPwl(sched_list->nbassign,ctim);
              SimDelayD = d_dly; return SIM_CONT0; 
sim_cont0:
              double dly = getdtime(ctim),
                     te  = SimTimeDlast + dly;
              if (te > SimTimeA) {
                SimDelayD = te - SimTimeA;
                return SIM_PREM; 
              }
              SimTimeD  = SimTimeDlast + dly;
            }
            break;
          default:
            fprintf(stderr,"deffault?\n");
        }

        if (!schedule_runnable()) break;
        schedule_sim(schedule_simtime() + ctim->delay);
        ctim->delay = 0;

        vpiNextSimTime();
      }


      /* If there are no more active events, advance the event
         queues. If there are not events at all, then release
         the event_time object. */
      if (ctim->active == 0) {
        ctim->active = ctim->nbassign;
        ctim->nbassign = 0;

        if (ctim->active == 0) {
          ctim->active = ctim->rwsync;
          ctim->rwsync = 0;

          /* If out of rw events, then run the rosync
             events and delete this time step. This also
             deletes threads as needed. */
          if (ctim->active == 0) {
            run_rosync(ctim);
            schedule_enlist( ctim->next);
            switch (mode) {
              case SIM_CONT0:
              case SIM_CONT1:
              case SIM_INIT: 

                d_dly = getdtime(ctim);
                if (d_dly > 0) {
                  trace0("noextPWL again");
                  // doExtPwl(sched_list->nbassign,ctim);
                  SimDelayD = d_dly;
                  delete ctim;
                  return SIM_CONT1;
sim_cont1:
                  // SimTimeD += ???;
                  goto cycle_done;
                }
              default:
                fprintf(stderr,"default 2\n");
            }
            delete ctim;
            goto cycle_done;
          }
        }
      }

      /* Pull the first item off the list. If this is the last
         cell in the list, then clear the list. Execute that
         event type, and delete it. */
      cur = ctim->active->next;
      if (cur->next == cur) {
        ctim->active = 0;
      } else {
        ctim->active->next = cur->next;
      }
      assert(cur);

      cur->run_run();

      delete (cur);

cycle_done:;
    }

  if (SIM_ALL == mode) {


    // signals_revert();

    // Execute post-simulation callbacks
    vpiPostsim();
  }

done:
  return SIM_DONE;
}
/*--------------------------------------------------------------------------*/
int COMMON_IVL::vvpinit() {
  trace0("COMMON_IVL::vvpinit");
  SimTimeD = 0;
  SimTimeA = 0;
  SimTimeDlast = 0;
  SimDelayD = 0;


#if 1
  // const char* e;
  vhbn  = (typeof(vhbn))dlsym(vvpso,"vpi_handle_by_name");
  if(!vhbn){
    error(bDANGER, "so: %s\n", dlerror());
  }
  assert(vhbn);
  //bindnet  = (typeof(bindnet))dlsym(vvpso,"bindnet");
  //assert(bindnet);
  //startsim = (typeof(startsim))dlsym(vvpso,"startsim");
  //assert(startsim);
  //endsim   = (typeof(endsim))dlsym(vvpso,"endsim");
  //assert(endsim);
  //contsim  = (typeof(contsim))dlsym(vvpso,"contsim");
  //ssert(contsim);
  get_compiler  = (typeof(get_compiler))dlsym(vvpso,"get_compiler");
  if(!get_compiler){
    error(bDANGER, "so: %s\n", dlerror());
  }
  assert(get_compiler);

  



  //  so_main =  (typeof(so_main))dlsym(vvpso,"so_main");
  //   if(!so_main){
  //     error(bDANGER, "so: %s\n", dlerror());
  //   }
  //activate = (typeof(activate))SetActive;
  //assert(activate);
#else
  startsim = &vvp::startsim;
  bindnet = &vvp::bindnet;
  assert(bindnet);
  assert(startsim);
  endsim   = &vvp::endsim;
  assert(endsim);
  contsim  = &vvp::contsim;
  assert(contsim);
  so_main =  &vvp::init;
  if(!so_main){
    error(bDANGER, "so: %s\n", dlerror());
  }
  assert(so_main);
  //activate = &vvp::SetActive;
  //assert(activate);
#endif

  return 0;
  //return( (*so_main)(args) );
}
/*--------------------------------------------------------------------------*/
void COMMON_IVL::precalc_first(const CARD_LIST* par_scope)
{
  trace0("COMMON_IVL::precalc_first " + (string) module + " " + (string) vvpfile );
  assert(par_scope);

  COMMON_COMPONENT::precalc_first(par_scope);
  vvpfile.e_val("UNSET", par_scope);
  module.e_val("UNSET", par_scope);

  //something hosed here.
}
/*--------------------------------------------------------------------------*/

int COMMON_IVL::compile_design(COMPILE*c, COMPONENT* p)const{
  const MODEL_IVL_BASE* m = dynamic_cast<const MODEL_IVL_BASE*>(model());
  trace0("COMMON_IVL::compile_design");

  //c->init();
  c->ca();
  return m->compile_design(c, p->short_label());
}
/*--------------------------------------------------------------------------*/
void COMMON_IVL::expand(const COMPONENT* dev ){
  trace1("COMMON_IVL::expand" + dev->long_label(), (intptr_t) dev % PRIME);

  COMMON_COMPONENT::expand(dev);
  attach_model(dev);

  const MODEL_IVL_BASE* m = dynamic_cast<const MODEL_IVL_BASE*>(model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(dev->long_label(), modelname(), name());
  }

  COMMON_LOGIC* logic_none = new LOGIC_NONE;
  logic_none->set_modelname(modelname());
  logic_none->attach(model());

  attach_common(logic_none, &_logic_none);
}
/*--------------------------------------------------------------------------*/
void COMMON_IVL::precalc_last(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_last(par_scope);

  trace0("COMMON_IVL::precalc_last " + (string) module + " " + (string) vvpfile );

  vvpfile.e_val("UNSET" , par_scope);
  module.e_val("UNSET" , par_scope);

#define USE_DLM 1
 
  if(!vvpso){

    /// dlopen has been redeclared. md.h
#ifdef USE_DLM
    vvpso = dlmopen(LM_ID_NEWLM,"libvvp.so",RTLD_LAZY); 
    // vvpso = dlmopen(LM_ID_BASE,"libvvp.so",RTLD_LAZY); 
#else
    vvpso = dlopen("libvvp.so",RTLD_LAZY|RTLD_LOCAL); //|RTLD_GLOBAL);
#endif
    if(vvpso == NULL) throw Exception("cannot open libvvp: %s: ", dlerror());
    trace0("=========== LOADED libvvp.so ==========");
    dlerror();

    int ret;
    incomplete();
    ret = vvpinit();
    //(string(vvpfile)+".vvp").c_str());   
    if(dlerror()) throw Exception("cannot init vvp %s", dlerror());

    if(ret)
      error(bDANGER, "somethings wrong with vvp: %s\n", vvpfile.string().c_str() );

  } else {
    trace0("COMMON_IVL::precalc_last already done extlib");
  }
  trace0("COMMON_IVL::precalc_last done");
}
/*--------------------------------------------------------------------------*/
COMMON_IVL::~COMMON_IVL()	{
  --_count;

  detach_common(&_logic_none);

  for( vector<COMMON_COMPONENT*>::iterator i = _subcommons.begin();
      i!=_subcommons.end(); ++i){
    delete (&(*i));
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_IVL::param_name(int i) const{
  trace1("COMMON_IVL::param_name ", i);
  switch (COMMON_COMPONENT::param_count() - 1 - i) {
    case 0: return "file";
    case 1: return "module";
    default: return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_IVL::param_is_printable(int i) const{
  return (COMMON_COMPONENT::param_count() - 1 - i)  < param_count();
}
/*--------------------------------------------------------------------------*/
std::string COMMON_IVL::param_name(int i, int j)const
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
std::string COMMON_IVL::param_value(int i)const
{
  switch (COMMON_COMPONENT::param_count() - 1 - i) {
    case 0: return vvpfile.string();
    case 1: return module.string();
    default: return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_IVL::set_param_by_index(int i, std::string& value, int offset)
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
COMMON_COMPONENT* COMMON_IVL::clone()const
{
  return new COMMON_IVL(*this);
}
/*--------------------------------------------------------------------------*/
bool COMMON_IVL::operator==(const COMMON_COMPONENT& x )const{
  trace0("COMMON_IVL::operator==");
  const COMMON_IVL* p = dynamic_cast<const COMMON_IVL*>(&x);
  bool ret = (vvpfile==p->vvpfile)
    && (module==p->module); // bad idea...?
  bool cr = COMMON_COMPONENT::operator==(x);

  return false;
  return ret && cr;
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* COMMON_IVL::deflate()
{
  trace0("COMMON_IVL::deflate");
  // return this;
  for( list<const COMMON_COMPONENT*>::iterator i = _commons.begin();
      i != _commons.end(); ++i ){
    if (*this == **i){
      trace0("COMMON_IVL::deflate hit");
      return const_cast<COMMON_COMPONENT*>( *i );
    }
    trace0("COMMON_IVL::deflate miss");
  }
  _commons.push_back(this);
  return this;
}
/*--------------------------------------------------------------------------*/
std::string MODEL_IVL_BASE::port_name(uint_t i)const{
  stringstream a;
  a << "port" << i;
  return a.str();
}
/*--------------------------------------------------------------------------*/
void MODEL_IVL_BASE::precalc_first(){
  MODEL_LOGIC::precalc_first();
}
/*--------------------------------------------------------------------------*/
void MODEL_IVL_BASE::precalc_last(){
  MODEL_LOGIC::precalc_last();
}
/*--------------------------------------------------------------------------*/
MODEL_IVL_BASE::MODEL_IVL_BASE(const BASE_SUBCKT* p)
  :MODEL_LOGIC((const COMPONENT*) p){
    trace0("MODEL_IVL_BASE::MODEL_IVL_BASE");
  }
/*--------------------------------------------------------------------------*/
MODEL_IVL_BASE::MODEL_IVL_BASE(const MODEL_IVL_BASE& p)
  :MODEL_LOGIC(p){
    file=p.file;
    output=p.output;
    input=p.input;
    trace0("MODEL_IVL_BASE::MODEL_IVL_BASE");
  }
/*--------------------------------------------------------------------------*/
int MODEL_IVL_BASE::_count = -1;
