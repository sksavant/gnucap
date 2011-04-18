/*$Id: d_logic.cc,v 1.2 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
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
 * FIXME: merge extlib into COMMON
 */
//testing=script,sparse 2006.07.17
#include "d_subckt.h"
#include "u_xprobe.h"
// #include "d_vvp.h"
#include "d_logic.h"
#include "ap.h"
#include "u_lang.h"
#include "e_elemnt.h"
#include "m_interp.h"
#include "bm.h"
#include "l_lib.h"
#include "io_trace.h"
/*--------------------------------------------------------------------------*/

#include "extlib.h"

#include "schedule.cc"

namespace {
#include "d_vvp.h"

/*--------------------------------------------------------------------------*/
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC_VVP;
class COMMON_LOGIC_VVP;
//from extlib.cc
//???
static int getVoltage(ExtSig *xsig,void *,double *ret) 
{
  ret[0] = xsig->d->tr_outvolts();
  return 1;
}
/*--------------------------------------------------------------------------*/
void ExtContSim(ExtLib* ext, const char *analysis,double accpt_time) {
  trace0("ExtContSim");
  ExtLib *lib = ext;

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
  //bug
  lib->contsim(analysis,accpt_time);
}
/*--------------------------------------------------------------------------*/
class LOGIC_IN : public COMMON_LOGIC {
  // data from icarus
private:
  explicit LOGIC_IN(const LOGIC_IN& p) :COMMON_LOGIC(p){++_count;}
  COMMON_COMPONENT* clone()const	{return new LOGIC_IN(*this);}
public:
  ExtSig* _ext;
  explicit LOGIC_IN(int c=0)		  :COMMON_LOGIC(c) {}

  LOGICVAL logic_eval(const node_t* )const {
    // called by DEV_LOGIC::tr_accept
    return _ext->get_logic();
  }
  virtual std::string name()const	  {return "in";}
};
/*--------------------------------------------------------------------------*/
static LOGIC_IN Default_in(CC_STATIC);
/*--------------------------------------------------------------------------*/
class LOGIC_OUT : public COMMON_LOGIC {
  // just the outnode. data to icarus
private:
  explicit LOGIC_OUT(const LOGIC_OUT& p) :COMMON_LOGIC(p){++_count;}
  COMMON_COMPONENT* clone()const	{return new LOGIC_OUT(*this);}
public:
  explicit LOGIC_OUT(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t* )const {
    trace0("LOGIC_OUT::logic_eval noting to eval");
    return lvUNKNOWN; 
  }
  virtual std::string name()const	  {return "out";}
};
/*--------------------------------------------------------------------------*/
static LOGIC_OUT Default_out(CC_STATIC);
/*--------------------------------------------------------------------------*/
static inline double FixTable(double *dp,std::vector<DPAIR> *num_table,
                              double now)
{
  double t = -1,nxt = -1;
  unsigned int s;
  int rsz = 1;
  for (s = 0;; s++) {
    double t2 = *dp++;
//    trace3("Fix", *dp,s,t2);
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
/*--------------------------------------------------------------------------*/
class EVAL_BM_EXTPWL : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double>   _delta;
  PARAMETER<int>      _smooth;
  ExtSig* _ext;
  ExtRef* _extref; // unneeded?
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _raw_table;
  std::vector<DPAIR> _num_table;
  explicit	EVAL_BM_EXTPWL(const EVAL_BM_EXTPWL& p);
public:
  explicit      EVAL_BM_EXTPWL(int c=0);
		~EVAL_BM_EXTPWL()		{}

  void attach_sig(ExtSig* a) {assert(_ext==0); _ext=a; }

// delete this function (see bm.h), use measurement;
// to find related places use code "ICARCOSIMVOLT"
//  virtual double    voltage(ELEMENT *) const;

private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_EXTPWL(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  //void  	expand(const COMPONENT*);//COMPONENT_COMMON/nothing
  //COMMON_COMPONENT* deflate();	 //COMPONENT_COMMON/nothing
  void		precalc_last(const CARD_LIST*);
  /// gs - 2remove void		precalc(const CARD_LIST*);

  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  TIME_PAIR	tr_review(COMPONENT*);
  std::string	name()const		{return "extpwl2";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
  void		skip_type_tail(CS& cmd)const {cmd.umatch("(1)");}
};
/*--------------------------------------------------------------------------*/
COMMON_LOGIC_VVP::COMMON_LOGIC_VVP(int c)
      :COMMON_COMPONENT(c), 
      _extlib(0),
      incount(0),
      file(""),
      module(""),
      status(0)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_LOGIC_VVP::COMMON_LOGIC_VVP(const COMMON_LOGIC_VVP& p)
      :COMMON_COMPONENT(p),
      _extlib(p._extlib),
      incount(p.incount),
      file(p.file),
      module(p.module),
      status(p.status)
{
  trace0("COMMON_LOGIC_VVP::COMMON_LOGIC_VVP( COMMON_LOGIC_VVP )");
  ++_count;
}
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC_VVP::operator==(const COMMON_COMPONENT& x )const{
  const COMMON_LOGIC_VVP* p = dynamic_cast<const COMMON_LOGIC_VVP*>(&x);
  bool ret = (file==p->file);
     //       && (module==p->module); // bad idea...?
  bool cr = COMMON_COMPONENT::operator==(x);

  trace2("COMMON_LOGIC_VVP::operator==" + string( file ) + " " + string( p->file),ret,cr);
  return ret && cr;
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* COMMON_LOGIC_VVP::deflate()
{
  for( list<const COMMON_COMPONENT*>::iterator i = _commons.begin();
      i != _commons.end(); ++i ){
    if (*this == **i){
      return const_cast<COMMON_COMPONENT*>( *i );
    }
    trace0("COMMON_LOGIC_VVP::deflate miss");
  }
  _commons.push_back(this);
  return this;
}
/*--------------------------------------------------------------------------*/
EVAL_BM_EXTPWL::EVAL_BM_EXTPWL(int c)
  :EVAL_BM_ACTION_BASE(c),
   _delta(_default_delta),
   _smooth(_default_smooth),
   _ext(NULL),
   _extref(NULL), //unneeded?
   _raw_table(),
   _num_table()
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_EXTPWL::EVAL_BM_EXTPWL(const EVAL_BM_EXTPWL& p)
  :EVAL_BM_ACTION_BASE(p),
   _delta(p._delta),
   _smooth(p._smooth),
   _ext(p._ext),
   _extref(p._extref), //unneeded
   _raw_table(p._raw_table),
   _num_table(p._num_table)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXTPWL::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_EXTPWL* p = dynamic_cast<const EVAL_BM_EXTPWL*>(&x);
  bool rv = p
    && _delta == p->_delta
    && _smooth == p->_smooth
    && _ext == p->_ext
    && _extref == p->_extref
    && _raw_table == p->_raw_table
    && _num_table == p->_num_table
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  incomplete(); // remove obs cb.
  assert(lang);
  o << name() << '(';
  o << (long int)(void*)this <<  ')';
  print_pair(o, lang, "delta", _delta, _delta.has_hard_value());
  print_pair(o, lang, "smooth",_smooth,_smooth.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _delta.e_val(_default_delta, Scope);
  _smooth.e_val(_default_smooth, Scope);
  assert(_ext);

  trace0(("EVAL_BM_EXTPWL::precalc_first bound ")); 

  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::iterator
	 p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    p->first.e_val(0, Scope);
    p->second.e_val(0, Scope);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);
  trace0("EVAL_BM_EXTPWL::precalc_last ");

  double last = -BIGBIG;
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::iterator
	 p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    if (last > p->first) {
      throw Exception_Precalc("PWL is out of order: (" + to_string(last)
			      + ", " + to_string(p->first) + ")\n");
    }else{
      DPAIR x(p->first, p->second);
      _num_table.push_back(x);
    }
    last = p->first;
  }

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::tr_eval(ELEMENT* d)const
{
  ExtSig *xsig = _ext;
  assert (xsig);
  SpcIvlCB *cbd = xsig->cb_data;
  double time0 = CKT_BASE::_sim->_time0,*dp,nxt;
  dp=0;
  xsig->d= d; // not here.

  // to Ivl
  //if (d->id_letter()=='V')
  dp  = (*cbd->eval)(cbd,time0,CB_LOAD,getVoltage,xsig,0);

  // &_num_table is const? why?
  std::vector<DPAIR>* num_table = const_cast<std::vector<DPAIR>*>(&_num_table);
  nxt = FixTable(dp,num_table,time0);

  double ext = (d->is_source()) ? 0. : NOT_INPUT;
  assert(ext==0);

  if(d->id_letter()=='V')
  { // from ivl
    d->_y[0] = interpolate(_num_table.begin(), _num_table.end(), 
                         ioffset(d->_y[0].x), ext, ext);
    tr_final_adjust(&(d->_y[0]), d->f_is_value());
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_EXTPWL::tr_review(COMPONENT* comp)
{
  assert (_ext);
  double dtime = comp->_sim->_dtmin;
  double dt = dtime;

  ExtSig *xsig = _ext;

  if ('I' == xsig->iv) {
    assert(comp->id_letter()=='I');
    SpcIvlCB *cbd = xsig->cb_data;
    double time0 = CKT_BASE::_sim->_time0,accpt_time = time0+dtime,*dp,nxt;

    if (cbd->sig==NULL)
    {
      trace0("ExtSigTrCheck Warnung xsig->cb_data->sig noch nicht gesetzt: Keine Schrittweitenkontrolle\n");
    }
    else
    { 
      dp = (*cbd->eval)(cbd,accpt_time,CB_TRUNC,getVoltage,xsig,0);

      nxt = dp[2];
      if (nxt <= time0) {
        dt = CKT_BASE::_sim->_dtmin;
      } else if (nxt < accpt_time) {
        dt = accpt_time - nxt;
      }
    }
  }


  //--------------------
  if (dt < dtime) {
    comp->_time_by.min_event(dt + comp->_sim->_time0);
  }

  if (comp->is_source()) {
    ELEMENT* dd = prechecked_cast<ELEMENT*>(comp);
    assert(dd);
    double x = dd->_y[0].x + comp->_sim->_dtmin * .01;
    DPAIR here(x, BIGBIG);
    std::vector<DPAIR>::iterator begin = _num_table.begin();
    std::vector<DPAIR>::iterator end   = _num_table.end();
    std::vector<DPAIR>::iterator upper = upper_bound(begin, end, here);
    std::vector<DPAIR>::iterator lower = upper - 1;
    assert(x > lower->first);
    comp->_time_by.min_event((x < upper->first) ? upper->first : NEVER);
  }else{untested();
  }

  return comp->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXTPWL::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here  = cmd.cursor();
  std::pair<PARAMETER<double>, PARAMETER<double> > p;
  if (0 == _ext) {
    // _ext  = (some_int)bindExtSigInit(_ext.string(),cmd.fullstring().c_str());
    p.first  = 0;
    p.second = 0;
    _raw_table.push_back(p);
    p.first  = BIGBIG;
    p.second = 0;
    _raw_table.push_back(p);
  } else {
    // _ext = 0;
    for (;;) {
      unsigned start_of_pair = here;
      //cmd >> key >> value;
      cmd >> p.first; // key
      if (cmd.stuck(&here)) {
	// no more, graceful finish
	break;
      }else{
	cmd >> p.second; // value
	if (cmd.stuck(&here)) {
	  // ran out, but already have half of the pair
	  // back up one, hoping somebody else knows what to do with it
	  cmd.reset(start_of_pair);
	  break;
	}else{
	  _raw_table.push_back(p);
	}
      }
    }
    if (cmd.gotit(start)) {
    }else{
      untested();
    }
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool Getptr(CS& cmd, const std::string& key, PARAMETER<intptr_t>* val)  // consumes key and optional "="
{  // see for "iu_parameter.h" "Get" functions for example and reference
  std::string str;  
  if (cmd.umatch(key + ' ')) {
    if (cmd.skip1b('=')) {
      untested();
      cmd >> str;
    }else{
      str = "";
    }
    *val=0;
    *val=str;
    return true;
  }else {
    str = "";
    *val=str;
    return false;
    }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXTPWL::parse_params_obsolete_callback(CS& cmd)
{
  trace0("EVAL_BM_EXTPWL::parse_params_obsolete_callback");
  return ONE_OF
    || Get(cmd, "delta",    &_delta)
    || Get(cmd, "smooth",   &_smooth)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_EXTPWL pp1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL dd1(&bm_dispatcher, "extpwl2", &pp1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

inline OMSTREAM& operator<<(OMSTREAM& o, direction_t t) {
  const std::string s[] = {"", "input", "output", "bus"};
  return (o << s[t]);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TIME_PAIR  DEV_LOGIC_VVP::tr_review(){
  return  BASE_SUBCKT::tr_review();
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_VVP::do_tr(){
  assert(status);
  q_accept();
  return BASE_SUBCKT::do_tr();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::tr_accept(){
  //trace1("DEV_LOGIC_VVP::tr_accept", _sim->_time0);
  ExtContSim(extlib(),"TRAN",_sim->_time0);     

  subckt()->tr_accept();
  q_eval();
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::precalc_first(const CARD_LIST* par_scope)
{
  trace0("COMMON_LOGIC_VVP::precalc_first");
  assert(par_scope);

  COMMON_COMPONENT::precalc_first(par_scope);
  file.e_val_normal("UNSET" , par_scope);
  module.e_val_normal("UNSET" , par_scope);
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::expand(const  COMPONENT* d ){
  trace1("COMMON_LOGIC_VVP::expand" + d->long_label(), (intptr_t) d % PRIME);

  COMMON_COMPONENT::expand(d);
  attach_model(d);
  // COMMON_LOGIC_VVP* c = this;
  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), name());
  }

}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC_VVP::precalc_last(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_last(par_scope);

  file.e_val_normal("UNSET" , par_scope);
  module.e_val_normal("UNSET" , par_scope);

  trace1("COMMON_LOGIC_VVP::precalc_last " + file.string(), status);
  if(!_extlib){
    void* h = dlopen("libvvpg.so",RTLD_LAZY|RTLD_GLOBAL);
    if(h==NULL) throw Exception("cannot open libvvp: %s: ", dlerror());
    dlerror();
    trace1("dlopened libvvp.so", (intptr_t)h%PRIME);
    _extlib = new ExtLib("foo",h);
    int ret;
    ret=_extlib->init(("-M. -mbindsigs2 -v "+string(file)+".vvp").c_str());   
    if(dlerror()) throw Exception("cannot init vvp %s", dlerror());

    if(ret)
      error(bDANGER, "somethings wrong with vvp: %s\n", file.string().c_str() );

  } else {
    untested();
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
void MODEL_LOGIC_VVP::precalc_first(){
  unreachable();
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC_VVP::precalc_last(){
  unreachable();
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
bool COMMON_LOGIC_VVP::param_is_printable(int i) const{
    return (COMMON_COMPONENT::param_count() - 1 - i)  < param_count();
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC_VVP::param_name(int i) const{
    switch (COMMON_COMPONENT::param_count() - 1 - i) {
      case 0: return "file";
      case 1: return "module";
      default: return COMMON_COMPONENT::param_name(i);
    }
}
/*--------------------------------------------------------------------------*/
bool MODEL_LOGIC_VVP::param_is_printable(int i) const{
    return (MODEL_LOGIC::param_count() - 1 - i)  < param_count();
}
/*--------------------------------------------------------------------------*/
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
void COMMON_LOGIC_VVP::set_param_by_index(int i, std::string& value, int offset)
{
  switch (COMMON_COMPONENT::param_count() - 1 - i) {
  case 0: file = value;
          break;

  case 1: module = value; break;
  default: COMMON_COMPONENT::set_param_by_index(i, value, offset); break;
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
  case 0: return file.string();
  case 1: return module.string();
  default: return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* COMMON_LOGIC_VVP::clone()const
{
  return new COMMON_LOGIC_VVP(*this);
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
/*--------------------------------------------------------------------------*/

int MODEL_LOGIC_VVP::_count = -1;
int COMMON_LOGIC_VVP::_count = -1;
typeof(COMMON_LOGIC_VVP::_commons) COMMON_LOGIC_VVP::_commons;
int DEV_LOGIC_VVP::_count = -1;
static COMMON_LOGIC_VVP Default_Params(CC_STATIC);
/*--------------------------------------------------------------------------*/
namespace DEV_DISP {
static DEV_LOGIC_VVP p1;
static DISPATCHER<CARD>::INSTALL
d1(&device_dispatcher, "vvp", &p1);
}
/*--------------------------------------------------------------------------*/
namespace MOD_DISP{
static DEV_LOGIC_VVP p1;
static MODEL_LOGIC_VVP p2(&p1);
static DISPATCHER<MODEL_CARD>::INSTALL
d2(&model_dispatcher, "vvp", &p2);
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_VVP::DEV_LOGIC_VVP()
  :BASE_SUBCKT(),
  status(0)
{
  trace0("DEV_LOGIC_VVP::DEV_LOGIC_VVP attaching...");
  attach_common(&Default_Params);
  trace0("DEV_LOGIC_VVP::DEV_LOGIC_VVP nodes");
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_VVP::DEV_LOGIC_VVP(const DEV_LOGIC_VVP& p)
  :BASE_SUBCKT(p)
   ,status(0)
{
  for (uint_t ii = 0;  ii < max_nodes();  ++ii) {
    nodes[ii] = p.nodes[ii];
  }
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::expand()
{
  trace0("DEV_LOGIC_VVP::expand " + long_label());
  BASE_SUBCKT::expand(); // calls common->expand, attaches model
  assert(_n);

  assert(common());
  const COMMON_LOGIC_VVP* c = prechecked_cast<const COMMON_LOGIC_VVP*>(common());
  assert(c);

  // c->attach(  dynamic_cast<const MODEL_LOGIC_VVP*>(find_model(_modelname)) );
  //
  assert(c->model());

  assert(c->model()); // d_mos.cc has a model here...
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

    assert(((const COMMON_LOGIC_VVP*) common())->_extlib);

    //if (!(_n[0].n_())) {
   //  _n[0].new_model_node("." + long_label() + ".gnd", this);
    //}else{
    //  untested0(_n[0].short_label().c_str());
    //}
    //if (!(_n[1].n_())) {
   //  _n[1].new_model_node("." + long_label() + ".vdd", this);
    //}else{
    //  untested0(_n[1].short_label().c_str());
    trace0("DEV_LOGIC_VVP::expand "+ short_label() + " entering loop");
    //}
    for( ; n<net_nodes(); ++n){
      string name;
      char src;
      COMMON_COMPONENT* logic_common;

      // a hack...
      stringstream a;
      a << "p" << n;
      name=a.str();
      src='V';
      name="out"; // from_iv
      logic_common = &Default_out;
      node_t lnodes[] = {_n[n], _n[0], _n[1], _n[1], _n[0]};


      if (n==3){
          src='I';
          name="in"; // to verilog
          lnodes[0] = _n[0];
          lnodes[1] = _n[0];
          lnodes[2] = _n[1];
          lnodes[3] = _n[1];
          lnodes[4] = _n[n];
          logic_common = &Default_in;
      }
      assert(n<4);

      trace1("DEV_LOGIC_VVP::expand "+ name + " " + short_label(), n);
      assert(_n[n].n_());

      // _n[n].new_model_node(long_label() + "."+ name, this);
      const CARD* p;
      const CARD* logic_dev;
      //if (i->direction == pIN){

      //}else{
      //  src='V';
      //}
      
        //src=get_pin_type(n)
      p = device_dispatcher[string(&src,1)];
      logic_dev =  device_dispatcher["logic"];

      assert(p);
      COMPONENT* _I = dynamic_cast<COMPONENT*>(p->clone());
      assert(_I);
      subckt()->push_front(_I);
      node_t nodes[] = {_n[n], _n[0]};
      const COMMON_COMPONENT* e = bm_dispatcher["extpwl2"];
      EVAL_BM_EXTPWL*_e = dynamic_cast<EVAL_BM_EXTPWL*>(e->clone());

      // from SigInit
      ExtRef* extref = new ExtRef(extlib(),( name ).c_str(),(char)toupper(src));
      ExtRefList.push_back(extref);

      //
      // from SigConn
      //  ExtSig(COMMON_COMPONENT *_c,ExtLib *_l,char _iv,void *cbd) 
      int s;
      trace0("binding something "+ extref->spec );
      SpcIvlCB* foo = (SpcIvlCB*) (* extlib()->bindnet)(name.c_str(),extref->iv,
          &s,extlib()->handle,ExtSig::SetActive);
      trace3("cb " + (string)(foo->spec) , foo->last_error, foo->last_time, s);

      ExtSig* sig= new ExtSig(_e, extlib(), extref->iv, foo);
      // (*_extlib->bindnet)(name.c_str(),extref->iv,
      //    &s,_extlib->handle,ExtSig::SetActive));
      string path=short_label();
      trace3(("ExtSig *bindExtSigConnect p: " + path).c_str(), (intptr_t)sig, s, foo->last_value);
      sig->slots = s;
      extref->sigs.push_back(sig);

      _e->attach_sig(sig);

      _I->set_parameters("in", this, (COMMON_COMPONENT*) _e, 0, 0, NULL, 2, nodes);
    } 
  }

  std::string subckt_name(c->modelname()+"."+string(c->file));

  assert(!is_constant());
  subckt()->set_slave();
  subckt()->expand();

  trace0("DEV_LOGIC_VVP::expand done");

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

  trace2("DEV_LOGIC_VVP::tr_begin startsim", status, (intptr_t) extlib());
  extlib()->startsim("TRAN",extlib());

  status++;
  subckt()->tr_begin();
  q_eval();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::precalc_first()
{
  COMPONENT::precalc_first();
  //const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(common());

   // FIXME: get ports from file...
   //


  if(subckt()) subckt()->precalc_first();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_VVP::precalc_last()
{
  trace0("DEV_LOGIC_VVP::precalc_last");
/////////////////////////////////////////
  COMPONENT::precalc_last();

  if (subckt()) {subckt()->precalc_last();}

  assert(common()->model());
}
/*--------------------------------------------------------------------------*/
// DEV_LOGIC_VVP::schluss{
//    lib->endsim(end_time);
//    }
/*--------------------------------------------------------------------------*/
//void DEV_LOGIC_VVP::tr_queue_eval()
//{
//  switch (_gatemode) {
//  case moUNKNOWN: unreachable(); break;
//  case moMIXED:	  unreachable(); break;
//  case moDIGITAL: ELEMENT::tr_queue_eval(); break;
//  case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
//  }
//}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//bool DEV_LOGIC_VVP::want_analog()const
//{
//  return subckt() &&
//    ((OPT::mode == moANALOG) || (OPT::mode == moMIXED && _quality != qGOOD));
//}
///*--------------------------------------------------------------------------*/
//bool DEV_LOGIC_VVP::want_digital()const
//{
//  return !subckt() ||
//    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
//}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
