/*$Id$
 * vim:sw=2:et:ts=8:
 * Copyright (C) 2009 Kevin Cameron
 * Author: Kevin Cameron
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
 */
#include "u_lang.h"
#include "globals.h"
#include "e_card.h"
#include "e_cardlist.h"
#include "s_tr.h"
#include "extlib.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/

static std::list<ExtLib*> Libs;

void ExtLib::set_active(void *handle,double time)
{
  if (active) {
    if (time >= next_time) {
      return;
    }
  } else {
    active    = 1;
  }
  next_time = time;
  if (time > now) {
    CKT_BASE::_sim->new_event(time);
  }
}

void ExtLib::SetActive(void *dl,void *handle,double time)
{
  SpcDllData *spd = (SpcDllData *)dl;
  ExtLib *lib = ExtLib::Sdd2El(spd);
  lib->set_active(handle,time);
}

void ExtLib::init(const char *args) {
  int i=strlen(args)+1;
  char buff[i],*bp=buff;
  const char *argv[i];
  argv[0]=name.c_str();
  argv[i=1]=bp;
  while ((*bp=*args++)) {
    if (' ' == *bp) {
      *bp++ = 0;
      argv[++i]=bp;
    } else {
      bp++;
    }
  }

  trace0("ExtLib::init");

  startsim = (typeof(startsim))dlsym(handle,"startsim");
  endsim   = (typeof(endsim))dlsym(handle,"endsim");
  bindnet  = (typeof(bindnet))dlsym(handle,"bindnet");
  contsim  = (typeof(contsim))dlsym(handle,"contsim");
  so_main  = (typeof(so_main))dlsym(handle,"so_main");
  activate = (typeof(activate))SetActive;

  argv[++i] = 0;
  (*so_main)(i,argv);
  //int sts = (*so_main)(i,argv);
}

ExtRef *bindExtSigInit(const string &so__data__sigpath,const char *src) {
  char buff[so__data__sigpath.size()],*bp = buff,*bp2;
  const char *sp = so__data__sigpath.c_str();

//  +external="../icarus/verilog/vvp/libvvp.so:-M. -mbindsigs -v netlist-dkc.vvp:%m.out"

  while ((*bp = *sp++) && *bp != ':') bp++;
  *bp++ = 0; bp2 = bp;
  while ((*bp = *sp++) && *bp != ':') bp++;
  *bp = 0;
  ExtLib *lib;
  list<ExtLib*>::iterator scan=Libs.begin();
  for (; scan != Libs.end(); scan++) {
    if (0 == (lib = *scan)->name.compare(buff)){
      trace0("bindExtSigInit, already have it");
      goto have_it;
    }
  }
  trace0(("bindExtSigInit new "+ (string)buff).c_str());
  lib = new ExtLib(buff,dlopen(buff,RTLD_LAZY|RTLD_GLOBAL));
  scan = Libs.insert(scan,lib);
  lib->init(bp2);
 have_it:
  trace0(("bindExtSigInit new ExtRef src"+ (string)src).c_str());
  trace0(("bindExtSigInit new ExtRef sp "+ (string)sp).c_str());
  ExtRef *ref = new ExtRef(lib,sp,toupper(*src));
  return ref;
}

static int getVoltage(ExtSig *xsig,void *,double *ret) 
{
/* can remove this part
  EVAL_BM_ACTION_BASE *pwl = dynamic_cast<EVAL_BM_ACTION_BASE *>(xsig->cmpnt);
  if (pwl) ret[0] = pwl->voltage(xsig->d);
*/
  ret[0] = xsig->d->tr_outvolts();
  return 1;
}

static inline double FixTable(double *dp,std::vector<DPAIR> *num_table,
                              double now)
{
  double t = -1,nxt = -1;
  unsigned int s;
  int rsz = 1;
  for (s = 0;; s++) {
    double t2 = *dp++;
    if (t2 <= t) break;
      t = t2;
      if (nxt < now) nxt = t;
      double v = *dp++;
      if (s >= num_table->size()) {
        DPAIR p(t,v);
	num_table->push_back(p); rsz = 0;
      } else {
	DPAIR &p((*num_table)[s]);
	p.first = t;
	p.second = v;
      }
  }
  if (rsz) num_table->resize(s);

  return nxt;
}

double ExtSigTrCheck(intptr_t data,double dtime,
    std::vector<DPAIR> *num_table,COMPONENT *d) {
  ExtSig *xsig = (ExtSig *)data;

  if ('I' == xsig->iv) {
    SpcIvlCB *cbd = xsig->cb_data;
    double time0 = CKT_BASE::_sim->_time0,accpt_time = time0+dtime,*dp,nxt;

    if (cbd->sig==NULL)
    {
      trace0("ExtSigTrCheck Warnung xsig->cb_data->sig noch nicht gesetzt: Keine Schrittweitenkontrolle\n");
    }
    else
    { 
      // to ivl...
      dp = (*cbd->eval)(cbd,accpt_time,CB_TRUNC,getVoltage,xsig,0);

      nxt = dp[2];
      if (nxt <= time0) {
        dtime = CKT_BASE::_sim->_dtmin;
      } else if (nxt < accpt_time) {
        dtime = accpt_time - nxt;
      }
    }
  }

  return dtime;
}

void  ExtSigTrEval(intptr_t data,std::vector<DPAIR> *num_table,ELEMENT *d) {
  ExtSig *xsig = (ExtSig *)data;
  assert (xsig);
  SpcIvlCB *cbd = xsig->cb_data;
  double time0 = CKT_BASE::_sim->_time0,*dp,nxt;
  xsig->d = d;
  dp  = (*cbd->eval)(cbd,time0,CB_LOAD,getVoltage,xsig,0);
  nxt = FixTable(dp,num_table,time0);
}

void ExtSig::SetActive(void *lib,void *sig,double time) {
  ((ExtSig*)sig)->set_active(time);
  trace1("ExtSig::SetActive", (intptr_t)sig );
}

void ExtSig::set_active(double time) {
}

ExtSig *bindExtSigConnect(intptr_t handle,const string &spec,const CARD_LIST* Scope,COMMON_COMPONENT *cmpnt) {
  ExtRef *ref = (ExtRef*)handle;
  trace1(("bindExtSigConnect spec:"+(string)spec).c_str(), ref->id());
  ExtSig *sig = 0;
  switch (ref->id()) {
    case EXT_SIG:
      trace0("bindExtSigConnect EXT_SIG" + spec);
      sig = (ExtSig*)handle;
      break;
    case EXT_REF:
      ExtLib *lib = ref->lib;
      const CARD *scp = *Scope->begin();
      const char *sp = ref->spec.c_str();
      std::string path;
      // HACK?
      std::string scp_nm(scp->owner()->long_label());
      trace0("bindExtSigConnect scp_nm " + scp_nm);
      char ch;
      while ((ch = *sp++)) {
        switch (ch) {
          case '%': switch (ch = *sp++) {
                      case 'm': path += scp_nm; continue;
                    }
          default:  path += ch; 
        }
      }
      trace0(("bindExtSigConnect sp:"+(string)sp).c_str());
      list<ExtSig*>::iterator scan=ref->sigs.begin();
      for (; scan != ref->sigs.end(); scan++) {
        sig = *scan;
        if (0 == path.compare(sig->cb_data->spec)){
          trace0("have " + string(sig->cb_data->spec) );
          break;
        }
      }
      int s;
          
      SpcIvlCB* foo= (SpcIvlCB*) (*lib->bindnet)(path.c_str(),ref->iv,
            &s,lib->handle,ExtSig::SetActive);
      trace2("cb", foo->last_error, foo->last_time);
      sig = new ExtSig(cmpnt,lib,ref->iv, foo);
      trace3(("bindExtSigConnect new sig to ref: " + path).c_str(), (intptr_t)sig, s, foo->last_value);
      sig->slots = s;
      ref->sigs.push_back(sig);
  }
  trace1("bindExtSigConnect returning", (intptr_t)sig);
  return sig;
}

void ExtStartSim(const char *analysis) {
  trace0("ExtStartSim");
  list<ExtLib*>::iterator scan=Libs.begin();
  for (; scan != Libs.end(); scan++) {
    ExtLib *lib = *scan;
    lib->startsim(analysis,lib);
  }
}

void ExtContSim(const char *analysis,double accpt_time) {
  trace0("ExtContSim");
  list<ExtLib*>::iterator scan=Libs.begin();
  for (; scan != Libs.end(); scan++) {
    ExtLib *lib = *scan;
    list<ExtRef*>::iterator scan=lib->refs.begin();
    lib->now = accpt_time;
    for (; scan != lib->refs.end(); scan++) {
      ExtRef *ref = *scan;
      if ('I' == ref->iv) {
	list<ExtSig*>::iterator ss=ref->sigs.begin();
	for (; ss != ref->sigs.end(); ss++) {
	  ExtSig *sig = *ss; 
	  SpcIvlCB *cbd = sig->cb_data;
	  (*cbd->eval)(cbd,accpt_time,CB_ACCEPT,'I' == ref->iv ? getVoltage
                                                               : 0,
                                         sig,0);
	}
      }   
    }
    if (lib->active) {
      if (lib->next_time <= accpt_time) lib->active = 0;    
    }
    lib->contsim(analysis,accpt_time);
  }
}

void ExtEndSim(const char *analysis,double end_time) {
  list<ExtLib*>::iterator scan=Libs.begin();
  for (; scan != Libs.end(); scan++) {
    ExtLib *lib = *scan;
    lib->endsim(end_time);
  }
}
