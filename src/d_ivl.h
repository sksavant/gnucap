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

DEV_IVL_name has a COMMON_IVL with a MODEL_IVL_name
it expands to a subnet of DEV_LOGIC_AD/DA devices with a common EVAL_IVL
(derived from COMMON_LOGIC), which has a dummy MODEL.

MODEL_IVL_name is derived from MODEL_LOGIC, the logic ports need to use
their parents model, otherwise all devices would have to use the same logic
model...


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
    static COMPILE_WRAP* _comp;
  public:
    static COMPILE_WRAP* get_comp(){
      if (_comp) return _comp;
      return _comp= new COMPILE_WRAP();
    }

    int vvpinit(COMPILE_WRAP*);
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

    void some_tr_begin_stuff()const;


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
    static int _time_prec;
    static int _log_time_prec;
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
    { assert (_time_prec); 
      return _time_prec; }
    double time_precision()const
    { return  pow(10,_time_prec); }

  private: // vvp stuff. eventually obsolete.


    void schedule_list(event_time_s* x) const
    { set_schedule_list(x); }

    void schedule_time( vvp_time64_t x ) const
    { set_schedule_time( x); }

   public: // some helpers... cleanup needed :|



    void schedule_transition(vpiHandle H,  vvp_time64_t  dly,
       vvp_vector4_t val, int a, int b)const;
    inline double prec() const {
      return double(pow(10.0,_time_prec));
    }

    inline double getdtime(struct event_time_s *et) const
    {
      return (double) et->delay * prec();
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
    virtual int compile_design(COMPILE_WRAP* c, const string, COMPONENT** da, 
        const COMMON_COMPONENT*) const = 0;
    virtual unsigned da_nodes()const = 0;
  public:
    PARAMETER<string> file;
    PARAMETER<string> input;
    PARAMETER<string> output;
};
/*--------------------------------------------------------------------------*/
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
    //uint_t max_nodes()const     {return 10;}
    //uint_t min_nodes()const     {return 2;}
    //int     matrix_nodes()const; //BASE_SUBCKT
//    uint_t net_nodes()const     {return 5;} //depends...
    //uint_t int_nodes()const     {return 0;}
    uint_t tail_size()const     {return 1;}
    node_t _nodes[PORTS_PER_IVL];	/* PORTS_PER_IVL <= PORTSPERSUBCKT */
    // CARD* clone()const { return new DEV_IVL_BASE(*this); }
    void precalc_first();
    void expand();
    void expand_nodes(){ incomplete(); }
    void precalc_last();
    void tr_accept();
    bool do_tr();
    bool tr_needs_eval()const{return true;}
    TIME_PAIR tr_review();
  public:
    const COMMON_COMPONENT* subcommon()const{
     assert(_subcommon); return _subcommon; }

    double tr_probe_num(const std::string& x)const;
  private: // COMPILE_WRAP stuff
    void init_vvp();
    void* _vvpso;
    COMPILE_WRAP* (*g_c)();
    const COMMON_COMPONENT* _subcommon;
    COMPILE_WRAP* _comp; // REMOVE

    // virtual int foo()=0;

  public:
    static int count()			{return _count;}

  private:
    static int	_count;
    uint_t status;
    std::vector<port_info_t> _ports;
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
