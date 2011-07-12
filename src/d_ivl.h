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
// using namespace std;
inline double getdtime(struct event_time_s *et)
{
  return (double) et->delay * pow(10.0,vpip_get_time_precision());
}
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
enum {PORTS_PER_IVL = 100};
enum direction_t {pIN=0, pOUT, pBUS};

PLI_INT32 callback(t_cb_data*x);

struct port_info_t {
  string name;
  direction_t direction;
};
class MODEL_IVL_BASE;
class COMMON_IVL;
class DEV_IVL_BASE;

class ExtLib; // obsolete (vvp?)
class VVP;
class COMPILE;
/*--------------------------------------------------------------------------*/
enum sim_mode {SIM_ALL,
               SIM_INIT,SIM_CONT0,SIM_CONT1,
               SIM_PREM,SIM_DONE};
/*------------------------------------------------------------*/
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
    static  int	count()	{return _count;}
    COMMON_COMPONENT* deflate();
  private:
    static int _count;
    static list<const COMMON_COMPONENT*> _commons;
    int vvpinit();
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
    COMMON_COMPONENT* _logic_none;
  protected:
    COMPILE* compile;

    // VVP things...
  public:
    mutable double SimTimeD;
    mutable double SimTimeA; 
    mutable double SimTimeDlast;
    mutable double SimDelayD;
    mutable sim_mode SimState;
    // Provide dummies
    inline static void my_getrusage(struct rusage *);
    inline static void print_rusage(struct rusage *, struct rusage *);

    //from vvp_vpi.cc
    static void vvp_vpi_init();
    static int init(const char* design_path);
    int compile_design(COMPILE* c, COMPONENT*) const;

    static void signals_capture(void);
    static void signals_revert(void);
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/
    /*--------------------------------------------------------------------*/
    //static double getdtime(struct event_time_s *et);
    sim_mode schedule_simulate_m(sim_mode mode) const;
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

    // double   (*startsim)(const char *);
    double   startsim() const;

    void     (*endsim)();
    //double   (*contsim)(const char *,double);
    double   contsim(const char *,double) const;


    void     (*activate)(void *,void *,double);
    vpiHandle(*vhbn)(const char *name, vpiHandle scope);
    COMPILE* (*get_compiler)();

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
  public: // override virtuals
    virtual std::string	dev_type()const		{return "none_yet";}
    // virtual CARD*	clone()const	{return new MODEL_IVL_BASE(*this);}
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
    virtual int compile_design(COMPILE* c, const string) const = 0;
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
    void expand_nodes(){ incomplete(); }
    void precalc_last();
    void tr_accept();
    bool do_tr();
    bool tr_needs_eval()const{return true;}
    TIME_PAIR tr_review();
  private:
   // VVP* vvp; // common??
   //


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
    //void qe() { q_eval(); }
    void register_port(vpiHandle); // data from ivl

    bool has_common()const {return true;}
    virtual std::string port_name(uint_t)const ;
    void tr_begin();
    ExtLib* extlib()const; //{return (((COMMON_IVL*) common())->_extlib);}
};

/*--------------------------------------------------------------------*/
#endif
