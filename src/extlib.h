/*$Id$ 
 * vim:sw=2:et:ts=8:
 * Copyright (C) 2009 Kevin Cameron
 * Authors: Kevin Cameron 
 *
 * This file is a part of plugin "Gnucap-Icarus" to "Gnucap", 
 * the Gnu Circuit Analysis Package
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
 * Dynamic binding of PWL signal sources to external simulators 
 *
 * mostly obsolete for DEV_VVP
 * TODO: merge needed stuff into COMMON_VVP
 *
 */


#ifndef EXT_H_LIB
#define EXT_H_LIB
#include "vvp/vpi_priv.h"
//#include "extpwl.h"
#include "e_compon.h"
#include "e_elemnt.h" 
#include <dlfcn.h>

# include  "vvp/compile.h"
# include  "vvp/schedule.h"
# include  "vvp/vpi_priv.h"
# include  "vvp/statistics.h"
# include  "vvp/vvp_cleanup.h"
# include  <cstdio>
# include  <csignal>
# include  <cstdlib>
# include  <cstring>
# include  <unistd.h>

  // trace facility from gnucap
# include "io_trace.h"

#if defined(HAVE_SYS_RESOURCE_H)
# include  <sys/time.h>
# include  <sys/resource.h>
#endif // defined(HAVE_SYS_RESOURCE_H)

# define EXT_BAS 0
# define EXT_REF 1
# define EXT_SIG 2

// extern void schedule_simulate(void);
//extern void vpip_mcd_init(FILE *log);


class ExtBase {
 public:
  virtual int id() {return 0;}
  static void null_call() { unreachable(); }
  virtual ~ExtBase(){}
};

struct SpcDllData {
    char   active;
    double next_time;

    void* El;
};

// schnittstelle zu digisim
class ExtAPI : public SpcDllData {
 public:

  //void    *(*bindnet)(const char *,char,int *,void *,void (*)(void *,void *,double));
  double   (*startsim)(const char *);
  void     (*endsim)();
  double   (*contsim)(const char *,double);
  int      (*so_main)(const char*);
  void     (*activate)(void *,void *,double);


  ExtAPI() {
    startsim = (typeof(startsim))ExtBase::null_call;
    endsim   = (typeof(endsim))ExtBase::null_call;
    //bindnet  = (typeof(bindnet))ExtBase::null_call;
    contsim  = (typeof(contsim))ExtBase::null_call;
    activate = (typeof(activate))ExtBase::null_call;
    so_main  = (typeof(so_main))ExtBase::null_call;
  }
};

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

class ExtSig : public ExtBase {
 public:
  virtual int id() {return EXT_SIG;}

  COMMON_COMPONENT *cmpnt;

  ExtLib       *lib;
  void     *cb_data;
  int           slots;
  char          iv;
  ELEMENT      *d;

  void set_active(double time);
  static void SetActive(void *,void *,double);

  ExtSig(COMMON_COMPONENT *_c,ExtLib *_l,char _iv,void *) 
    : cmpnt(_c), lib(_l), iv(_iv){}

 private:
  ExtSig();

 public:
  LOGICVAL get_logic(){

   //  SpcIvlCB *cbd = cb_data;
    //double time0 = CKT_BASE::_sim->_time0,*dp,nxt;
    // assert(lib->d == d);
//    dp  = (*cbd->eval)(cbd,time0,CB_LOAD,getVoltage,xsig,0);
    return lvUNKNOWN;
  }

};

class ExtRef : public ExtBase {
 public:
  virtual int id() {return EXT_REF;}

  std::list<ExtSig*> sigs;

  ExtLib       *lib;
  std::string   spec;
  char          iv;

  ExtRef(ExtLib *_l,const char *sig_spec,char _iv) 
    : lib(_l), spec(sig_spec), iv(_iv) {
    trace1(("ExtRef() baue "+spec ).c_str(), iv );
    lib->refs.push_back(this);
  }
};


class ExtControl{
  ExtRef *bindExtSigInit(const string &,const char *);
  ExtSig *bindExtSigConnect(intptr_t,const string &,
                            const CARD_LIST* Scope,COMMON_COMPONENT *);
  void    ExtSigTrEval(intptr_t,std::vector<DPAIR>*,ELEMENT*);
  double  ExtSigTrCheck(intptr_t,double,std::vector<DPAIR>*,COMPONENT*);

  void ExtStartSim(const char *);
  void ExtContSim(const char *,double);
  void ExtEndSim(double);
};

void PrintInst(FILE *fp,struct __vpiScope *scope);

/*------------------------------------------------------------*/

#endif
