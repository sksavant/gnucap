/*$Id: d_logic.cc,v 1.2 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2011 Felix Salfelder
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
 *
 */
#ifndef D_IVL_H__
#define D_IVL_H__
//testing=script,sparse 2006.07.17
#include "e_subckt.h"
//#include "u_xprobe.h"
// #include "d_vvp.h"
// #include "extlib.h"
#include "d_logic.h"
//#include "ap.h"
//#include "u_lang.h"
//#include "m_interp.h"
#include "md.h"
//#include "bm.h"
//#include "l_lib.h"
//#include "io_trace.h"
using namespace std;
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_IVL = 100};
enum direction_t {pIN=0, pOUT, pBUS};

struct port_info_t {
  string name;
  direction_t direction;
};
class MODEL_LOGIC_IVL;
class COMMON_LOGIC_IVL;
class DEV_IVL_BASE;
/*--------------------------------------------------------------------------*/
class COMMON_LOGIC_IVL : public COMMON_COMPONENT {
  public:
    explicit	COMMON_LOGIC_IVL(int c=0);
    explicit	COMMON_LOGIC_IVL(const COMMON_LOGIC_IVL& p);
    ~COMMON_LOGIC_IVL();
    bool operator==(const COMMON_COMPONENT&)const; 
    COMMON_COMPONENT* clone()const; //  { return new COMMON_LOGIC_IVL(*this);}
    void set_param_by_index(int, std::string&, int);
    bool param_is_printable(int)const;
    string param_name(int)const;
    string param_name(int,int)const;
    string param_value(int)const;
    int	param_count()const	{return (2 + COMMON_COMPONENT::param_count());}
    void precalc_first(const CARD_LIST*);
    void expand(const COMPONENT* d);
    void precalc_last(const CARD_LIST*);
    std::string name()const {itested();return "logic_vvp";}
    static  int	count()	{return _count;}
    COMMON_COMPONENT* deflate();
  private:
    static int _count;
    static list<const COMMON_COMPONENT*> _commons;
  public:
    // virtual LOGICVAL logic_eval(const node_t*)const	= 0;
    void startsim();
    //private:?
    ExtLib* _extlib;
  public: // input parameters
    int		incount; //?
    PARAMETER<std::string> vvpfile;
    PARAMETER<std::string> module;
  private:
   uint_t status;

  public:
   // doesnt make sense probably
    std::vector< COMMON_COMPONENT* > _subcommons;
    COMMON_COMPONENT* _logic_none;
};
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC_IVL : public MODEL_LOGIC {
private:
  explicit	MODEL_LOGIC_IVL(const MODEL_LOGIC_IVL& p);
public:
  explicit MODEL_LOGIC_IVL(const DEV_IVL_BASE* );
	   ~MODEL_LOGIC_IVL()		{ --_count; }
private: // override virtuals
  static int _count;
  std::string	dev_type()const		{return "logic_vvp";}
  CARD*	clone()const		{return new MODEL_LOGIC_IVL(*this);}
  void precalc_first();
  void precalc_last();
  void set_param_by_index(int, std::string&, int);
  bool param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int		param_count()const	{return (3 + MODEL_LOGIC::param_count());}
public:
  static int	count()			{return _count;}
  std::string port_name(uint_t)const;
public:
  PARAMETER<string> file;
  PARAMETER<string> input;
  PARAMETER<string> output;
};
/*--------------------------------------------------------------------------*/
class ExtLib : public ExtAPI , public COMPONENT {
  public:
    std::list<class ExtRef*> refs;
    std::string name;
    void *handle;
    double now;
    ExtLib(const char *_nm,void *_hndl) : name(_nm), handle(_hndl), now(-1) {
      El=this;
    }
    int init(const char *);
    static void SetActive(void *dl,void *handle,double time);
    void        set_active(void *handle,double time);
    virtual std::string value_name() const {return name;}
    virtual bool print_type_in_spice() const {return false;}
    static ExtLib *Sdd2El(SpcDllData *spd) {
      return (ExtLib*) spd->El;
      //intptr_t p = (intptr_t)spd;
      //p -= offsetof(ExtLib,active);   
      //return (ExtLib *)p;
    }
  private:
    ExtLib();

  public:
    virtual std::string port_name(uint_t)const {return "";}
};
/*--------------------------------------------------------------------------*/
class DEV_IVL_BASE : public BASE_SUBCKT  {
  private:
    explicit	DEV_IVL_BASE(const DEV_IVL_BASE& p);
  public:
    explicit	DEV_IVL_BASE();
    ~DEV_IVL_BASE();// {--_count;}
  private: // override virt.
    char	   id_letter()const	{return '\0';}
    bool print_type_in_spice()const {return true;}
    string value_name()const	{return "#";}
    uint_t max_nodes()const     {return 10;}
    uint_t min_nodes()const     {return 2;}
    //int     matrix_nodes()const; //BASE_SUBCKT
//    uint_t net_nodes()const     {return 5;} //depends...
    uint_t int_nodes()const     {return 0;}
    uint_t tail_size()const     {return 1;}
    node_t _nodes[PORTS_PER_IVL];	/* PORTS_PER_IVL <= PORTSPERSUBCKT */
    CARD* clone()const { return new DEV_IVL_BASE(*this); }
    void precalc_first();
    void expand();
    void precalc_last();
    void tr_accept();
    bool do_tr();
    bool tr_needs_eval()const{return true;}
    TIME_PAIR tr_review();

  public:
    static int count()			{return _count;}

  private:
    static int	_count;
    uint_t status;
    std::vector<port_info_t> _ports;
    ExtRef* _extref; //initialized Extlib...
    std::vector< ExtRef* > ExtRefList;
    std::vector< COMPONENT* > _subdevices;

    vector<vpiHandle> _outport;
    vector<vpiHandle> _inport;

  public:
    //void qe() { q_eval(); }
    void register_port(vpiHandle); // data from ivl

    bool has_common()const {return true;}
    std::string port_name(uint_t)const ;
    void tr_begin();
    ExtLib* extlib()const; //{return (((COMMON_LOGIC_IVL*) common())->_extlib);}
};
/*------------------------------------------------------------*/
class vvp{
  public:
  static double SimTimeD;
  static double SimTimeA; 
  static double SimTimeDlast;
  static double SimDelayD;
  static sim_mode SimState;
  // Provide dummies
  inline static void my_getrusage(struct rusage *);
  inline static void print_rusage(struct rusage *, struct rusage *);

  //from vvp_vpi.cc
  static void vvp_vpi_init();
  static int init(const char* design_path);
  static int compile();

  static void signals_capture(void);
  static void signals_revert(void);
  /*--------------------------------------------------------------------*/
  /*--------------------------------------------------------------------*/
  /*--------------------------------------------------------------------*/
  static double getdtime(struct event_time_s *et);
  static sim_mode schedule_simulate_m(sim_mode mode);
  /*--------------------------------------------------------------------*/
  static double startsim(const char *analysis);
  static double contsim(const char *analysis,double time);
  static void endsim();
  static void *bindnet(const char *,char ,int *, void *,void (*)(void *,void *,double));

};

/*--------------------------------------------------------------------*/

inline void DEV_LOGIC_IVL::tr_begin()
{
  const COMMON_LOGIC_IVL* c = prechecked_cast<const COMMON_LOGIC_IVL*>(common());
  assert(c);

  trace1("DEV_LOGIC_IVL::tr_begin " + short_label(), status);
  vvp::startsim("TRAN");
  vvp::contsim("TRAN",0);

  // exchange initial conditions?
  // maybe not necessary (done during dc)

  status++;
  subckt()->tr_begin();
  q_eval();
}
/*--------------------------------------------------------------------------*/
inline void DEV_LOGIC_IVL::precalc_first()
{
  COMPONENT::precalc_first();
  assert(common());

  if(subckt()){
    subckt()->precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
inline void DEV_LOGIC_IVL::precalc_last()
{
  COMPONENT::precalc_last();
  if (subckt()) {subckt()->precalc_last();}
  assert(common()->model());
}
/*--------------------------------------------------------------------------*/
inline void DEV_LOGIC_IVL::expand()
{
  BASE_SUBCKT::expand();
  assert(_n);
  const COMMON_LOGIC_IVL* c = prechecked_cast<const COMMON_LOGIC_IVL*>(common());
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

    ExtLib*el=((const COMMON_LOGIC_IVL*) common())->_extlib;
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
    trace1("DEV_LOGIC_IVL::expand "+ short_label() + " entering loop", net_nodes());

    expand_nodes();

    // old
    while ((item = vpi_scan(net_iterator))) {
      int type = vpi_get(vpiType,item);
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
int vvp::init(const char *args) {

  trace0("ExtLib::init");

#if 1
  const char* e;
  vhbn  = (typeof(vhbn))dlsym(handle,"vpi_handle_by_name");
  assert(vhbn);
  bindnet  = (typeof(bindnet))dlsym(handle,"bindnet");
  assert(bindnet);
  startsim = (typeof(startsim))dlsym(handle,"startsim");
  assert(startsim);
  endsim   = (typeof(endsim))dlsym(handle,"endsim");
  assert(endsim);
  contsim  = (typeof(contsim))dlsym(handle,"contsim");
  assert(contsim);
//  so_main =  (typeof(so_main))dlsym(handle,"so_main");
//   if(!so_main){
//     error(bDANGER, "so: %s\n", dlerror());
//   }
  activate = (typeof(activate))SetActive;
  assert(activate);
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

  return( (*so_main)(args) );
}
#endif
