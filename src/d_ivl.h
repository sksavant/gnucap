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

DEV_IVL_name has a dummy COMMON_IVL with a MODEL_IVL_name
it expands to a subnet of DEV_LOGIC_AD/DA devices with a common EVAL_IVL
(derived from COMMON_LOGIC), which has a MODEL derived from MODEL_LOGIC.

EVAL_IVL contains the vvp-stuff. it would be nice to have a static vvp, this
would need a way to clone/reallocate the device instances within the runtime...

 */

#ifndef D_IVL_H__
#define D_IVL_H__
#define DLINKED 1
#include "e_subckt.h"
//#include "u_xprobe.h"
// #include "d_vvp.h"
// #include "extlib.h"
#include "d_logic.h"
#include "d_ivl_ports.h"
//#include "ap.h"
//#include "u_lang.h"
//#include "m_interp.h"
#include "md.h"
//#include "bm.h"
//#include "l_lib.h"
//#include "io_trace.h"
#include "vvp/schedule.h" // event_time_s
#include "e_ivl_compile.h" // vpi_mode_t
# include  <iostream> 
//# include  <cstdio>

// #include "vvp/config.h"// vvp_time64_t etc.
//#include "vvp/vvp_net.h"// vvp_sub_pointer_t ?
using namespace std;
//
/*--------------------------------------------------------------------------*/
// to make libvvp happy
void yyerror(const char*msg);
extern ofstream debug_file;
/*--------------------------------------------------------------------------*/
#ifdef DO_TRACE
inline void trace_queue(event_time_s* x, int depth = 0){
    cerr << ":::";
  if(x){
    for(int i=0;i++<=depth;cerr<< " ");
    cerr << x->delay << "("<< hp(x)<< ")\n";
    trace_queue(x->next,depth+1);
  }else{
    cerr<<"\n";
  }
}
#else
#define trace_queue(a) 
#endif

class COMMON_IVL;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_IVL = 100};
enum direction_t {pIN=0, pOUT, pBUS};

// PLI_INT32 callback(t_cb_data*x);

struct port_info_t {
  string name;
  direction_t direction;
};
class MODEL_IVL_BASE;
class COMMON_IVL;
class DEV_IVL_BASE;

class ExtLib; // obsolete (vvp?)
class VVP;
class COMPILE_WRAP;
/*--------------------------------------------------------------------------*/
enum sim_mode {SIM_ALL,
               SIM_INIT,
               SIM_CONT0,
               SIM_CONT1,
               SIM_PREM,
               SIM_DONE};
/*------------------------------------------------------------*/
class EVAL_IVL : public COMMON_COMPONENT {
  public:
    explicit	EVAL_IVL(int c=0);
    explicit	EVAL_IVL(const EVAL_IVL& p);
    ~EVAL_IVL();
    bool operator==(const COMMON_COMPONENT&)const; 
    COMMON_COMPONENT* clone()const  { return new EVAL_IVL(*this);}
    //void set_param_by_index(int, std::string&, int);
    //bool param_is_printable(int)const;
    //string param_name(int)const;
    //string param_name(int,int)const;
    //string param_value(int)const;
    //int	param_count()const	{return (2 + COMMON_COMPONENT::param_count());}
    void precalc_first(const CARD_LIST*);
    void expand(const COMPONENT* d);
    void precalc_last(const CARD_LIST*);
    std::string name()const {itested();return "eval_ivl";}
    static  int	count()	{return _count;}
    COMMON_COMPONENT* deflate();
  private:
    static int _count;
    static list<const COMMON_COMPONENT*> _commons;
  public:
    int vvpinit(COMPILE_WRAP*);
    COMPILE_WRAP* comp;
  public: // input parameters
    int		incount; //?
  private: // simulator stuff
    uint_t status;

    vvp_time64_t (*g_s_t)();
    void (*s_s_t)(vvp_time64_t);

#ifdef SOME_OLD_STUFF
    event_time_s* (*g_s_l)();
    void (*s_s_l)(event_time_s*);
    vpi_mode_t& (*_vpi_mode)();
    void (*s_a_p_v) (vvp_sub_pointer_t<vvp_net_t>,vvp_time64_t,vvp_vector4_t,int, int);
    void (*s_st)();
    void (*n_s)();
    void (*r_r)(event_time_s*);
    void (*s_e)(event_time_s*);
    void (*e_c)();
    void (*e_i)();
    void (*v_s)();
    void (*v_p)();
    bool (*s_x)();
    bool (*s_r)();
    void (*v_t)(int);
    void (*s_l)();
#endif

    void do_some_precalc_last_stuff()const;


  public:
    // doesnt make sense probably
    std::vector< COMMON_COMPONENT* > _subcommons;
    // COMMON_COMPONENT* _logic_none;
  protected:
    COMPILE_WRAP* compile;

    // VVP things...
  public:
    mutable double SimTimeD;
    mutable double SimTimeA; 
    mutable double SimTimeDlast;
    mutable double SimDelayD;
    mutable sim_mode SimState;
    int _time_prec;
    // Provide dummies
    inline static void my_getrusage(struct rusage *);
    inline static void print_rusage(struct rusage *, struct rusage *);

    double event_absolute(struct event_time_s *et) const;
    vvp_time64_t discrete_floor(double abs_t) const;


  private: // for now, move everything to the external vars
    event_time_s* my_schedule_list;
    vvp_time64_t my_schedule_time;

  public: // potentially local?
#ifdef OLD_SUTUF
    event_time_s* schedule_list() const 
    { assert(g_s_l); return (*g_s_l)(); }
    vvp_time64_t schedule_time( ) const
    { assert(g_s_t); return (*g_s_t)(); }

#else
    event_time_s* schedule_list() const 
    { return get_schedule_list(); }
    vvp_time64_t schedule_time( ) const
    { return get_schedule_time(); }
#endif

    int log_time_precision()const
    { return _time_prec; }
    double time_precision()const
    { return  pow(10,_time_prec); }

  private: // vvp stuff. eventually obsolete.


#ifdef SOME_OLD_APPROACH
    void schedule_list(event_time_s* x) const
    { assert(s_s_l); return (*s_s_l)(x); }
    void schedule_time( vvp_time64_t x ) const
    { trace1("IVL::schedule_time", x);
      assert(g_s_t); return (*s_s_t)(x); }
    vpi_mode_t& vpi_mode_flag() const 
    { assert(_vpi_mode); return (*_vpi_mode)(); }
    void vpiNextSimTime() const 
    { assert(n_s); return(n_s)(); }
    void schedule_start() const 
    { assert(s_st); return (*s_st)(); }
    void run_rosync(struct event_time_s*ctim) const
    { assert(r_r); return (*r_r)(ctim); }
    void schedule_enlist(event_time_s*t) const
    { assert(s_e); return (*s_e)(t); }
    void vpiEndOfCompile(void) const
    { assert(e_c); return (*e_c)(); }
    void exec_init_list() const 
    { assert(e_i); return (*e_i)(); }
    void vpiStartOfSim(void) const
    { assert(v_s); return (*v_s)(); }
    void vpiPostsim(void) const
    { assert(v_p); return (*v_p)(); }
    bool schedule_stopped(void) const
    { assert(s_x); return (*s_x)(); }
    bool schedule_runnable(void) const
    { assert(s_r); return (*s_r)(); }
    void vpip_set_time_precision(int pres) const
    { assert(v_t); (*v_t)(pres); }
#else
    void schedule_list(event_time_s* x) const
    { set_schedule_list(x); }

    void schedule_time( vvp_time64_t x ) const
    { set_schedule_time( x); }
#endif

   public: // some helpers... cleanup needed :|



    void schedule_transition(vpiHandle H,  vvp_time64_t  dly,
       vvp_vector4_t val, int a, int b)const;

    inline double getdtime(struct event_time_s *et) const
    {
      return (double) et->delay * pow(10.0,_time_prec);
    }
    inline double event_(struct event_time_s *et) const
    {
      return  double ( et->delay * (long double)
          pow(10.0,_time_prec) );
    }
    inline double digital_time(void) const
    {
      trace1("",_time_prec);
      return double(this->schedule_time() * (long double)
          pow(10.0,_time_prec)) ;
    }
    inline double prec() const {
      return double(pow(10.0,_time_prec));
    }


    //from vvp_vpi.cc
    static void vvp_vpi_init();
    static int init(const char* design_path);
    int compile_design(COMPILE_WRAP* c, COMPONENT*, COMPONENT**) const;

    static void signals_capture(void);
    static void signals_revert(void);
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/
    //static double getdtime(struct event_time_s *et);
    sim_mode schedule_simulate_m(sim_mode mode) const;
    sim_mode schedule_cont0() const;
    sim_mode schedule_cont1() const;
    /*--------------------------------------------------------------------*/

#if 0
    static double startsim(const char *analysis);
    static double *contsim(const char *analysis,double time);

    static void endsim();
    static void *bindnet(const char *,char ,int *, void *,void (*)(void *,void *,double));
#else
    // not implemented...
#endif

    // former extlib.h
    void    *(*bindnet)(const char *,char,int *,void *,void (*)(void *,void *,double));
    double   tr_begin() const;

    void     (*endsim)();
    double   contsim(const char *,double) const;
    void   contsim_set(double) const;
    vvp_time64_t contsim(vvp_time64_t until_rel) const;

    void catch_up(double time) const;

//    void     (*activate)(void *,void *,double);
//    vpiHandle(*vhbn)(const char *name, vpiHandle scope);

};
/*--------------------------------------------------------------------------*/
class COMMON_IVL : public COMMON_COMPONENT {
  public:
    explicit	COMMON_IVL(int c=0);
    explicit	COMMON_IVL(const COMMON_IVL& p);
    ~COMMON_IVL();
    bool operator==(const COMMON_COMPONENT&)const; 
    COMMON_COMPONENT* clone()const; //  { return new COMMON_IVL(*this);}
    void set_param_by_index(int, std::string&, int);
    bool param_is_printable(int)const;
    string param_name(int)const;
    string param_name(int,int)const;
    string param_value(int)const;
    int	param_count()const	{return (2 + COMMON_COMPONENT::param_count());}
    void precalc_first(const CARD_LIST*);
    void expand(const COMPONENT* d);
    void precalc_last(const CARD_LIST*);
    std::string name()const {itested();return "common_ivl";}
    static  int	count()	{return _count;}\

    // no deflate...
    COMMON_COMPONENT* no_deflate();
  private:
    static int _count;
    static list<const COMMON_COMPONENT*> _commons;
     // int vvpinit();
  public:
    // virtual LOGICVAL logic_eval(const node_t*)const	= 0;
    // void startsim();
    //private:?
    void* vvpso;
    // dlopen("libvvpg.so",RTLD_LAZY|RTLD_GLOBAL);
    //    ExtLib* _extlib;
  public: // input parameters
    int		incount; //?
    PARAMETER<std::string> vvpfile;
    PARAMETER<std::string> module;
  private:
    uint_t status;

  public:
    // doesnt make sense probably
    std::vector< COMMON_COMPONENT* > _subcommons;
    COMMON_COMPONENT* _eval_ivl;
  protected:
    COMPILE_WRAP* compile;

    // VVP things...
  public:
    unsigned _time_prec;
    // Provide dummies
    inline static void my_getrusage(struct rusage *);
    inline static void print_rusage(struct rusage *, struct rusage *);


    //from vvp_vpi.cc
    static void vvp_vpi_init();
    static int init(const char* design_path);
    int compile_design(COMPILE_WRAP* c, COMPONENT*, COMPONENT**da) const;

    static void signals_capture(void);
    static void signals_revert(void);
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/

#if 0
#else
    // not implemented...
#endif

    // double   (*startsim)(const char *);

    void     (*endsim)();


    void     (*activate)(void *,void *,double);
    vpiHandle(*vhbn)(const char *name, vpiHandle scope);

};
/*--------------------------------------------------------------------------*/
class MODEL_IVL_BASE : public MODEL_LOGIC {
  protected:
    explicit	MODEL_IVL_BASE(const MODEL_IVL_BASE& p);
    explicit	MODEL_IVL_BASE(const BASE_SUBCKT* p);
  public:
    explicit MODEL_IVL_BASE(const DEV_IVL_BASE* );
    ~MODEL_IVL_BASE()		{ --_count; }
  private:
    static int _count;
    CARD* _logic_model;
  public: // override virtuals
    virtual std::string	dev_type()const		{return "none_yet";}
    // no need to (virtual class)
    // virtual CARD*	clone()const	{return new MODEL_IVL_BASE(*this);}
    CARD* logic_model() const
    { assert(_logic_model); return( _logic_model ); }
    void precalc_first();
    void precalc_last();
    //void set_param_by_index(int, std::string&, int);
    //bool param_is_printable(int)const;
    //std::string	param_name(int)const;
    //std::string	param_name(int,int)const;
    //std::string	param_value(int)const;
    //int		param_count()const	{return (3 + MODEL_LOGIC::param_count());}
  public:
    //static int	count()			{return _count;}
    virtual std::string port_name(uint_t)const;
    virtual int compile_design(COMPILE_WRAP* c, const string, COMPONENT** da) const = 0;
    virtual unsigned da_nodes()const = 0;
  public:
    PARAMETER<string> file;
    PARAMETER<string> input;
    PARAMETER<string> output;
};
/*--------------------------------------------------------------------------
class ExtLib : public COMPONENT {
  public:
    std::list<class ExtRef*> refs;
    std::string name;
    void *handle;
    double now;
    ExtLib(const char *_nm,void *_hndl) : name(_nm), handle(_hndl), now(-1) {
      // El=this;
    }
    int init(const char *);
    static void SetActive(void *dl,void *handle,double time);
    void        set_active(void *handle,double time);
    virtual std::string value_name() const {return name;}
    virtual bool print_type_in_spice() const {return false;}
  private:
    ExtLib();

  public:
    virtual std::string port_name(uint_t)const {return "";}
};
--------------------------------------------------------------------------*/
class DEV_IVL_BASE : public BASE_SUBCKT {
  protected:
    explicit	DEV_IVL_BASE(const DEV_IVL_BASE& p);
  public:
    explicit	DEV_IVL_BASE();
    ~DEV_IVL_BASE();
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
    void expand_nodes(){ incomplete(); }
    void precalc_last();
    void tr_accept();
    bool do_tr();
    bool tr_needs_eval()const{return true;}
    TIME_PAIR tr_review();
  public:
    const COMMON_COMPONENT* subcommon()const{ return _subcommon; }

    double tr_probe_num(const std::string& x)const;
  private: // COMPILE_WRAP stuff
    void init_vvp();
    void* _vvpso;
    COMPILE_WRAP* _comp;
    COMPILE_WRAP* (*g_c)();
    const COMMON_COMPONENT* _subcommon;


  public:
    static int count()			{return _count;}

  private:
    static int	_count;
    uint_t status;
    std::vector<port_info_t> _ports;
    //ExtRef* _extref; //initialized Extlib...
    // std::vector< ExtRef* > ExtRefList;
    std::vector< COMPONENT* > _subdevices;

    vector<vpiHandle> _outport;
    vector<vpiHandle> _inport;

  public:
    void compile_design(COMPILE_WRAP* c, COMPONENT** da);
    //void qe() { q_eval(); }
    void register_port(vpiHandle); // data from ivl

    bool has_common()const {return true;}
    virtual std::string port_name(uint_t)const ;
    void tr_begin();
    ExtLib* extlib()const; //{return (((COMMON_IVL*) common())->_extlib);}
};

/*--------------------------------------------------------------------*/
inline double EVAL_IVL::event_absolute(struct event_time_s *et) const
{
  trace1("event_absolute", _time_prec);
  assert(is_number(  pow(10.0, _time_prec ))) ;
  return  double ( (et->delay + this->schedule_time() )  
      * (long double) pow(10.0, _time_prec ));
}
/*--------------------------------------------------------------------*/
#endif
