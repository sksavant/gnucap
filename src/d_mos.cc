/* $Id: d_mos.cc,v 1.18 2010-09-07 07:46:20 felix Exp $ -*- C++ -*-
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
 * data structures and defaults for mos model.
 * internal units all mks (meters)
 * but some user input parameters are in cm.
 *
 * netlist syntax:
 * device:  mxxxx d g s b mname <device args> <model card args>
 * model:   .model mname NMOS <args>
 *	or  .model mname PMOS <args>
 */
/* This file is not automatically generated */

#include "u_limit.h"
#include "io_trace.h"
#include "e_storag.h"
#include "d_mos_base.h"
#include "e_adp.h"
#include "e_adplist.h"
#include "globals.h"
#include "e_elemnt.h"
#include "d_mos.h"
#include "d_bti.h"

// doesnt work without yet.
#define BTI_IN_SUBCKT
/*--------------------------------------------------------------------------*/
const double NA(NOT_INPUT);
const double INF(BIGBIG);
/*--------------------------------------------------------------------------*/
int DEV_BUILT_IN_MOS::_count = -1;
int COMMON_BUILT_IN_MOS::_count = -1;
static COMMON_BUILT_IN_MOS Default_BUILT_IN_MOS(CC_STATIC);
/*--------------------------------------------------------------------------*/
#ifndef BTI_IN_SUBCKT
#define BTI_LATE_EVAL
#endif
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_MOS::COMMON_BUILT_IN_MOS(int c)
  :COMMON_COMPONENT(c),
   l_in(OPT::defl),
   w_in(OPT::defw),
   ad_in(OPT::defad),
   as_in(OPT::defas),
   pd(0.0),
   ps(0.0),
   nrd(1.0),
   nrs(1.0),
   _sdp(0),
   _bu(0),
   _db(0),
   _sb(0),
   _bti(0)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_MOS::COMMON_BUILT_IN_MOS(const COMMON_BUILT_IN_MOS& p)
  :COMMON_COMPONENT(p),
   l_in(p.l_in),
   w_in(p.w_in),
   ad_in(p.ad_in),
   as_in(p.as_in),
   pd(p.pd),
   ps(p.ps),
   nrd(p.nrd),
   nrs(p.nrs),
   _sdp(0),
   _bu(0),
   _db(0),
   _sb(0),
   _bti(0)
{
//  trace0("some mos copy");
  ++_count;
}
/*--------------------------------------------------------------------------*/
COMMON_BUILT_IN_MOS::~COMMON_BUILT_IN_MOS()
{
  detach_common(&_bu);
  detach_common(&_db);
  detach_common(&_sb);
  detach_common(&_bti);
  --_count;
  delete _sdp;
}
/*--------------------------------------------------------------------------*/
bool COMMON_BUILT_IN_MOS::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_BUILT_IN_MOS* p = dynamic_cast<const COMMON_BUILT_IN_MOS*>(&x);
  return (p
    && l_in == p->l_in
    && w_in == p->w_in
    && ad_in == p->ad_in
    && as_in == p->as_in
    && pd == p->pd
    && ps == p->ps
    && nrd == p->nrd
    && nrs == p->nrs
    && _sdp == p->_sdp
    && COMMON_COMPONENT::operator==(x));
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_MOS::set_param_by_index(int I, std::string& Value, int Offset)
{
  switch (COMMON_BUILT_IN_MOS::param_count() - 1 - I) {
  case 0:  l_in = Value; break;
  case 1:  w_in = Value; break;
  case 2:  ad_in = Value; break;
  case 3:  as_in = Value; break;
  case 4:  pd = Value; break;
  case 5:  ps = Value; break;
  case 6:  nrd = Value; break;
  case 7:  nrs = Value; break;
  default: COMMON_COMPONENT::set_param_by_index(I, Value, Offset);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_BUILT_IN_MOS::param_is_printable(int i)const
{
  switch (COMMON_BUILT_IN_MOS::param_count() - 1 - i) {
  case 0:  return (true);
  case 1:  return (true);
  case 2:  return (has_hard_value(ad_in));
  case 3:  return (has_hard_value(as_in));
  case 4:  return (has_hard_value(pd));
  case 5:  return (has_hard_value(ps));
  case 6:  return (has_hard_value(nrd));
  case 7:  return (has_hard_value(nrs));
  default: return COMMON_COMPONENT::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_MOS::param_name(int i)const
{
  switch (COMMON_BUILT_IN_MOS::param_count() - 1 - i) {
  case 0:  return "l";
  case 1:  return "w";
  case 2:  return "ad";
  case 3:  return "as";
  case 4:  return "pd";
  case 5:  return "ps";
  case 6:  return "nrd";
  case 7:  return "nrs";
  default: return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_MOS::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (j == 1) {
    switch (COMMON_BUILT_IN_MOS::param_count() - 1 - i) {
    case 0:  return "";
    case 1:  return "";
    case 2:  return "";
    case 3:  return "";
    case 4:  return "";
    case 5:  return "";
    case 6:  return "";
    case 7:  return "";
    default: return "";
    }
  }else{untested();//281
    return COMMON_COMPONENT::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_BUILT_IN_MOS::param_value(int i)const
{
  switch (COMMON_BUILT_IN_MOS::param_count() - 1 - i) {
  case 0:  return l_in.string();
  case 1:  return w_in.string();
  case 2:  return ad_in.string();
  case 3:  return as_in.string();
  case 4:  return pd.string();
  case 5:  return ps.string();
  case 6:  return nrd.string();
  case 7:  return nrs.string();
  default: return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_MOS::expand(const COMPONENT* d)
{
  trace0(("COMMON_BUILT_IN_MOS::expand "+ modelname()).c_str());
  COMMON_COMPONENT::expand(d);
  attach_model(d);
  COMMON_BUILT_IN_MOS* c = this;
  const MODEL_BUILT_IN_MOS_BASE* m = dynamic_cast<const MODEL_BUILT_IN_MOS_BASE*>(model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "mosfet");
  }else{
  }
  // size dependent
  //delete _sdp;
  _sdp = m->new_sdp(this);
  assert(_sdp);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(_sdp);
  assert(s);

  // subcircuit commons, recursive
  COMMON_BUILT_IN_DIODE* db = new COMMON_BUILT_IN_DIODE;
  db->area = double(s->ad);
  db->perim = double(c->pd);
  db->is_raw = double(s->idsat);
  db->cj_raw = double(m->cbd);
  db->cjsw_raw = NA;
  db->off = true;
  db->set_modelname(modelname());
  db->attach(model());
  attach_common(db, &_db);

  COMMON_BUILT_IN_DIODE* sb = new COMMON_BUILT_IN_DIODE;
  sb->area = double(s->as);
  sb->perim = double(c->ps);
  sb->is_raw = double(s->issat);
  sb->cj_raw = double(m->cbs);
  sb->cjsw_raw = NA;
  sb->off = true;
  sb->set_modelname(modelname());
  sb->attach(model());
  attach_common(sb, &_sb);


  trace1(("bti_model_name: " + m->bti_model_name.value()).c_str(), m->use_bti());
// none  assert( (m->bti_model_name.value() != std::string("")) == m->use_bti );
  if(m->use_bti()){
    trace0("setting up bti common");

    COMMON_BUILT_IN_BTI* bti = new COMMON_BUILT_IN_BTI; // nix single. hier kein model
    bti->number = 1;
    bti->set_modelname( m->bti_model_name.value()  );
    bti->polarity =  m->polarity   ;
    bti->attach(model());
    attach_common(bti, &_bti);

    assert(c == this);
    trace0("done setting up bti common");
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_MOS::precalc_first(const CARD_LIST* par_scope)
{
  assert(par_scope);
  COMMON_COMPONENT::precalc_first(par_scope);
    e_val(&(this->l_in), OPT::defl, par_scope);
    e_val(&(this->w_in), OPT::defw, par_scope);
    e_val(&(this->ad_in), OPT::defad, par_scope);
    e_val(&(this->as_in), OPT::defas, par_scope);
    e_val(&(this->pd), 0.0, par_scope);
    e_val(&(this->ps), 0.0, par_scope);
    e_val(&(this->nrd), 1.0, par_scope);
    e_val(&(this->nrs), 1.0, par_scope);
}
/*--------------------------------------------------------------------------*/
void COMMON_BUILT_IN_MOS::precalc_last(const CARD_LIST* par_scope)
{
  untested0("prec mos");
  assert(par_scope);
  COMMON_COMPONENT::precalc_last(par_scope);
  COMMON_BUILT_IN_MOS* c = this;
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(model());
    // final adjust: code_pre
    // final adjust: override
    // final adjust: raw
    e_val(&(this->l_in), OPT::defl, par_scope);
    e_val(&(this->w_in), OPT::defw, par_scope);
    e_val(&(this->ad_in), OPT::defad, par_scope);
    e_val(&(this->as_in), OPT::defas, par_scope);
    e_val(&(this->pd), 0.0, par_scope);
    e_val(&(this->ps), 0.0, par_scope);
    e_val(&(this->nrd), 1.0, par_scope);
    e_val(&(this->nrs), 1.0, par_scope);
    // final adjust: mid
    // final adjust: calculated
    // final adjust: post
    // final adjust: done

  // size dependent
  //delete _sdp;
  _sdp = m->new_sdp(this);
  assert(_sdp);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(_sdp);
  assert(s);

  // subcircuit commons, recursive
  COMMON_BUILT_IN_DIODE* db = new COMMON_BUILT_IN_DIODE;
  db->area = double(s->ad);
  db->perim = double(c->pd);
  db->is_raw = double(s->idsat);
  db->cj_raw = double(m->cbd);
  db->cjsw_raw = NA;
  db->off = true;
  db->set_modelname(modelname());
  db->attach(model());
  attach_common(db, &_db);

  COMMON_BUILT_IN_DIODE* sb = new COMMON_BUILT_IN_DIODE;
  sb->area = double(s->as);
  sb->perim = double(c->ps);
  sb->is_raw = double(s->issat);
  sb->cj_raw = double(m->cbs);
  sb->cjsw_raw = NA;
  sb->off = true;
  sb->set_modelname(modelname());
  sb->attach(model());
  attach_common(sb, &_sb);

  assert(c == this);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace DEV_BUILT_IN_MOS_DISPATCHER { 
  static DEV_BUILT_IN_MOS p0;
  static DISPATCHER<CARD>::INSTALL
    d0(&device_dispatcher, "M|mosfet", &p0);
}
/*--------------------------------------------------------------------------*/
static EVAL_BUILT_IN_MOS_Cgb Eval_Cgb(CC_STATIC);
void EVAL_BUILT_IN_MOS_Cgb::tr_eval(ELEMENT* d)const
{
  assert(d);
  DEV_BUILT_IN_MOS* p = prechecked_cast<DEV_BUILT_IN_MOS*>(d->owner());
  assert(p);
  const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(p->common());
  assert(c);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(c->sdp());
  assert(s);
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);

    STORAGE* brh = prechecked_cast<STORAGE*>(d);
    assert(brh);

    double cap = brh->value();

    if (m->cmodel != 0) {
      if (p->vgst < - s->phi) { 		/* accumulation */
	cap += s->cgate;
      }else if (p->vgst < 0.) {			/* depletion */
	cap += s->cgate * (-p->vgst) / s->phi;
      }else{					/* active, overlap only */
      }
    }
    brh->_y[0].f1 = cap;
    if (d->_sim->analysis_is_tran_dynamic()) {
      cap = (brh->_y[0].f1 + brh->_y[1].f1) / 2;
      brh->_y[0].f0 = (brh->_y[0].x - brh->_y[1].x) * cap + brh->_y[1].f0;
    }else{
      assert(d->_sim->analysis_is_static() || d->_sim->analysis_is_restore());
      brh->_y[0].f0 = brh->_y[0].x * brh->_y[0].f1;
    }
    trace3(brh->long_label().c_str(), brh->_y[0].x, brh->_y[0].f0, brh->_y[0].f1);
}
/*--------------------------------------------------------------------------*/
static EVAL_BUILT_IN_MOS_Cgd Eval_Cgd(CC_STATIC);
void EVAL_BUILT_IN_MOS_Cgd::tr_eval(ELEMENT* d)const
{
  assert(d);
  DEV_BUILT_IN_MOS* p = prechecked_cast<DEV_BUILT_IN_MOS*>(d->owner());
  assert(p);
  const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(p->common());
  assert(c);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(c->sdp());
  assert(s);
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);

    STORAGE* brh = prechecked_cast<STORAGE*>(d);
    assert(brh);

    double cap = 0;
    if (m->cmodel != 0) {
      assert(p->vdsat >= 0.);
      assert(p->vds >= 0.);
      double vbs    = (m->cmodel == 3) ? 0. : p->vbs;
      double vdbsat = p->vdsat - vbs;
      double vdb    = p->vds   - vbs;
      double ddif   = 2. * vdbsat - vdb;
      
      if (!p->reversed) { // treat as Cgs
	if (p->vgst >= 0.) {
	  if (p->vdsat > p->vds) {		/* linear */
	    cap = (2./3.) * s->cgate * (1. - (vdbsat*vdbsat)/(ddif*ddif));
	    if (p->vgst <= .1) {
	      cap *= 10. * p->vgst;	// smooth discontinuity
	    }
	  }
	}
      }else{ // treat as Cgs
	if (p->vgst >= -s->phi/2.) {		/* depletion  or active */
	  cap = (2./3.) * s->cgate;
	  if (p->vdsat > p->vds) {			/* linear */
	    double ndif   = p->vdsat - p->vds;
	    cap *= 1. - (ndif*ndif)/(ddif*ddif);
	  }
	  if (p->vgst <= 0) {
	    cap *= 1. + p->vgst / (s->phi);
	    cap *= 1. + p->vgst / (s->phi);
	  }
	}
      }
    }
    cap += brh->value();		/* else overlap only */
    
    brh->_y[0].f1 = cap;
    if (d->_sim->analysis_is_tran_dynamic()) {
      cap = (brh->_y[0].f1 + brh->_y[1].f1) / 2;
      brh->_y[0].f0 = (brh->_y[0].x - brh->_y[1].x) * cap + brh->_y[1].f0;
    }else{
      assert(d->_sim->analysis_is_static() || d->_sim->analysis_is_restore());
      brh->_y[0].f0 = brh->_y[0].x * brh->_y[0].f1;
    }
    trace3(brh->long_label().c_str(), brh->_y[0].x, brh->_y[0].f0, brh->_y[0].f1);
}
/*--------------------------------------------------------------------------*/
static EVAL_BUILT_IN_MOS_Cgs Eval_Cgs(CC_STATIC);
void EVAL_BUILT_IN_MOS_Cgs::tr_eval(ELEMENT* d)const
{
  assert(d);
  DEV_BUILT_IN_MOS* p = prechecked_cast<DEV_BUILT_IN_MOS*>(d->owner());
  assert(p);
  const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(p->common());
  assert(c);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(c->sdp());
  assert(s);
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);

    STORAGE* brh = prechecked_cast<STORAGE*>(d);
    assert(brh);

    double cap = 0;
    if (m->cmodel != 0) {
      assert(p->vdsat >= 0.);
      assert(p->vds >= 0.);
      double vbs    = (m->cmodel == 3) ? 0. : p->vbs;
      double vdbsat = p->vdsat - vbs;
      double vdb    = p->vds   - vbs;
      double ddif   = 2. * vdbsat - vdb;
      
      if (p->reversed) { // treat as Cgd
	if (p->vgst >= 0.) {
	  if (p->vdsat > p->vds) {		/* linear */
	    cap = (2./3.) * s->cgate * (1. - (vdbsat*vdbsat)/(ddif*ddif));
	    if (p->vgst <= .1) {
	      cap *= 10. * p->vgst;	// smooth discontinuity
	    }
	  }
	}
      }else{ // treat as Cgs
	if (p->vgst >= -s->phi/2.) {		/* depletion  or active */
	  cap = (2./3.) * s->cgate;
	  if (p->vdsat > p->vds) {			/* linear */
	    double ndif   = p->vdsat - p->vds;
	    cap *= 1. - (ndif*ndif)/(ddif*ddif);
	  }
	  if (p->vgst <= 0) {
	    cap *= 1. + p->vgst / (s->phi);
	    cap *= 1. + p->vgst / (s->phi);
	  }
	}
      }
    }
    cap += brh->value();		/* else overlap only */
    
    brh->_y[0].f1 = cap;
    if (d->_sim->analysis_is_tran_dynamic()) {
      cap = (brh->_y[0].f1 + brh->_y[1].f1) / 2;
      brh->_y[0].f0 = (brh->_y[0].x - brh->_y[1].x) * cap + brh->_y[1].f0;
    }else{
      assert(d->_sim->analysis_is_static() || d->_sim->analysis_is_restore());
      brh->_y[0].f0 = brh->_y[0].x * brh->_y[0].f1;
    }
    trace3(brh->long_label().c_str(), brh->_y[0].x, brh->_y[0].f0, brh->_y[0].f1);
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_MOS::DEV_BUILT_IN_MOS()
  :BASE_SUBCKT(),
   // input parameters,
   // calculated parameters,
   ids(0.),
   idsxxx(NA),
   gds(0.),
   gmf(0.),
   gmr(0.),
   gmbf(0.),
   gmbr(0.),
   idb(0.),
   idbxxx(0.),
   gdbdb(0.),
   gdbds(0.),
   gdbgs(0.),
   gdbbs(0.),
   isb(0.),
   isbxxx(0.),
   gsbsb(0.),
   gsbsd(0.),
   gsbgd(0.),
   gsbbd(0.),
   qgate(0.),
   cgs(0.),
   cggb(0.),
   cgsb(0.),
   cgdb(0.),
   qgs(0.),
   cgsgs(0.),
   cgsgb(0.),
   cgssb(0.),
   cgsdb(0.),
   qgd(0.),
   cgdgd(0.),
   cgdgb(0.),
   cgdsb(0.),
   cgddb(0.),
   qdrn(0.),
   cdsds(0.),
   cdgb(0.),
   cdsb(0.),
   cddb(0.),
   qbulk(0.),
   cbs(0.),
   cbgb(0.),
   cbsb(0.),
   cbdb(0.),
   qbs(0.),
   cbsbs(0.),
   cbsgb(0.),
   cbssb(0.),
   cbsdb(0.),
   qbd(0.),
   cbdbd(0.),
   cbdgb(0.),
   cbdsb(0.),
   cbddb(0.),
   gtau(0.),
   cqgb(0.),
   cqsb(0.),
   cqdb(0.),
   cqbb(0.),
   vgs(0.),
   vds(0.),
   vbs(0.),
   vdsat(0.),
   vgst(0.),
   von(0.),
   reversed(false),
   cutoff(false),
   subthreshold(false),
   saturated(false),
   sbfwd(false),
   punchthru(false),
   // netlist,
   _Rs(0),
   _Rd(0),
   _Ddb(0),
   _Dsb(0),
   _Cgs(0),
   _Cgd(0),
   _Cgb(0),
   _BTI(0),
   _Ids(0),
   _Idb(0),
   _Isb(0)
{
  _n = _nodes;
  attach_common(&Default_BUILT_IN_MOS);
  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
DEV_BUILT_IN_MOS::DEV_BUILT_IN_MOS(const DEV_BUILT_IN_MOS& p)
  :BASE_SUBCKT(p),
   // input parameters,
   // calculated parameters,
   ids(p.ids),
   idsxxx(p.idsxxx),
   gds(p.gds),
   gmf(p.gmf),
   gmr(p.gmr),
   gmbf(p.gmbf),
   gmbr(p.gmbr),
   idb(p.idb),
   idbxxx(p.idbxxx),
   gdbdb(p.gdbdb),
   gdbds(p.gdbds),
   gdbgs(p.gdbgs),
   gdbbs(p.gdbbs),
   isb(p.isb),
   isbxxx(p.isbxxx),
   gsbsb(p.gsbsb),
   gsbsd(p.gsbsd),
   gsbgd(p.gsbgd),
   gsbbd(p.gsbbd),
   qgate(p.qgate),
   cgs(p.cgs),
   cggb(p.cggb),
   cgsb(p.cgsb),
   cgdb(p.cgdb),
   qgs(p.qgs),
   cgsgs(p.cgsgs),
   cgsgb(p.cgsgb),
   cgssb(p.cgssb),
   cgsdb(p.cgsdb),
   qgd(p.qgd),
   cgdgd(p.cgdgd),
   cgdgb(p.cgdgb),
   cgdsb(p.cgdsb),
   cgddb(p.cgddb),
   qdrn(p.qdrn),
   cdsds(p.cdsds),
   cdgb(p.cdgb),
   cdsb(p.cdsb),
   cddb(p.cddb),
   qbulk(p.qbulk),
   cbs(p.cbs),
   cbgb(p.cbgb),
   cbsb(p.cbsb),
   cbdb(p.cbdb),
   qbs(p.qbs),
   cbsbs(p.cbsbs),
   cbsgb(p.cbsgb),
   cbssb(p.cbssb),
   cbsdb(p.cbsdb),
   qbd(p.qbd),
   cbdbd(p.cbdbd),
   cbdgb(p.cbdgb),
   cbdsb(p.cbdsb),
   cbddb(p.cbddb),
   gtau(p.gtau),
   cqgb(p.cqgb),
   cqsb(p.cqsb),
   cqdb(p.cqdb),
   cqbb(p.cqbb),
   vgs(p.vgs),
   vds(p.vds),
   vbs(p.vbs),
   vdsat(p.vdsat),
   vgst(p.vgst),
   von(p.von),
   reversed(p.reversed),
   cutoff(p.cutoff),
   subthreshold(p.subthreshold),
   saturated(p.saturated),
   sbfwd(p.sbfwd),
   punchthru(p.punchthru),
   // netlist,
   _Rs(0),
   _Rd(0),
   _Ddb(0),
   _Dsb(0),
   _Cgs(0),
   _Cgd(0),
   _Cgb(0),
   _BTI(0),
   _Ids(0),
   _Idb(0),
   _Isb(0)
{
  _n = _nodes;
  for (int ii = 0; ii < max_nodes() + int_nodes(); ++ii) {
    _n[ii] = p._n[ii];
  }
  ++_count;
  // overrides
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::expand()
{
  BASE_SUBCKT::expand(); // calls common->expand, attached model
  assert(_n);
  assert(common());
  const COMMON_BUILT_IN_MOS* c = static_cast<const COMMON_BUILT_IN_MOS*>(common());
  assert(c);
  assert(c->model());
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);
  assert(c->sdp());
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(c->sdp());
  assert(s);
  if (!subckt()) {
    new_subckt();
  }else{
  }

  if (_sim->is_first_expand()) {
    precalc_first();
    precalc_last();
    // local nodes
    //assert(!(_n[n_id].n_()));
    //BUG// this assert fails on a repeat elaboration after a change.
    //not sure of consequences when new_model_node called twice.
    if (!(_n[n_id].n_())) {
      if (!OPT::rstray || s->rd == 0.) {
        _n[n_id] = _n[n_d];
      }else{
        _n[n_id].new_model_node("." + long_label() + ".id", this);
      }
    }else{
      if (!OPT::rstray || s->rd == 0.) {
        assert(_n[n_id] == _n[n_d]);
      }else{
        //_n[n_id].new_model_node("id." + long_label(), this);
      }
    }
    //assert(!(_n[n_is].n_()));
    //BUG// this assert fails on a repeat elaboration after a change.
    //not sure of consequences when new_model_node called twice.
    if (!(_n[n_is].n_())) {
      if (!OPT::rstray || s->rs == 0.) {
        _n[n_is] = _n[n_s];
      }else{
        _n[n_is].new_model_node("." + long_label() + ".is", this);
      }
    }else{
      if (!OPT::rstray || s->rs == 0.) {
        assert(_n[n_is] == _n[n_s]);
      }else{
        //_n[n_is].new_model_node("is." + long_label(), this);
      }
    }

    // clone subckt elements
    if (!OPT::rstray || s->rs == 0.) {
      if (_Rs) {
        subckt()->erase(_Rs);
        _Rs = NULL;
      }else{
      }
    }else{
      if (!_Rs) {
        const CARD* p = device_dispatcher["resistor"];
        assert(p);
        _Rs = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Rs);
        subckt()->push_front(_Rs);
      }else{
      }
      {
        node_t nodes[] = {_n[n_s], _n[n_is]};
        _Rs->set_parameters("Rs", this, NULL, s->rs, 0, NULL, 2, nodes);
      }
    }
    if (!OPT::rstray || s->rd == 0.) {
      if (_Rd) {
        subckt()->erase(_Rd);
        _Rd = NULL;
      }else{
      }
    }else{
      if (!_Rd) {
        const CARD* p = device_dispatcher["resistor"];
        assert(p);
        _Rd = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Rd);
        subckt()->push_front(_Rd);
      }else{
      }
      {
        node_t nodes[] = {_n[n_d], _n[n_id]};
        _Rd->set_parameters("Rd", this, NULL, s->rd, 0, NULL, 2, nodes);
      }
    }
    if (_n[n_b].n_() == _n[n_d].n_() || s->idsat == 0.) {
      if (_Ddb) {
        subckt()->erase(_Ddb);
        _Ddb = NULL;
      }else{
      }
    }else{
      if (!_Ddb) {
        const CARD* p = device_dispatcher["diode"];
        assert(p);
        _Ddb = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Ddb);
        subckt()->push_front(_Ddb);
      }else{
      }
      if (m->polarity==pP) {
        node_t nodes[] = {_n[n_id], _n[n_b]  };
        _Ddb->set_parameters("Ddb", this, c->_db, 0., 0, NULL, 2, nodes);
      }else{
        node_t nodes[] = {_n[n_b], _n[n_id]};
        _Ddb->set_parameters("Ddb", this, c->_db, 0., 0, NULL, 2, nodes);
      }
    }
    if (_n[n_b].n_() == _n[n_s].n_() || s->issat == 0.) {
      if (_Dsb) {
        subckt()->erase(_Dsb);
        _Dsb = NULL;
      }else{
      }
    }else{
      if (!_Dsb) {
        const CARD* p = device_dispatcher["diode"];
        assert(p);
        _Dsb = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Dsb);
        subckt()->push_front(_Dsb);
      }else{
      }
      if (m->polarity==pP) {
        node_t nodes[] = {_n[n_is], _n[n_b]  };
        _Dsb->set_parameters("Dsb", this, c->_sb, 0., 0, NULL, 2, nodes);
      }else{
        node_t nodes[] = {_n[n_b], _n[n_is]};
        _Dsb->set_parameters("Dsb", this, c->_sb, 0., 0, NULL, 2, nodes);
      }
    }
    if (!OPT::cstray || _n[n_g].n_() == _n[n_s].n_()) {
      if (_Cgs) {
        subckt()->erase(_Cgs);
        _Cgs = NULL;
      }else{
      }
    }else{
      if (!_Cgs) {
        const CARD* p = device_dispatcher["capacitor"];
        assert(p);
        _Cgs = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Cgs);
        subckt()->push_front(_Cgs);
      }else{
      }
      {
        node_t nodes[] = {_n[n_g], _n[n_is]};
        _Cgs->set_parameters("Cgs", this, &Eval_Cgs, s->cgso, 0, NULL, 2, nodes);
      }
    }
    if (!OPT::cstray || _n[n_g].n_() == _n[n_d].n_()) {
      if (_Cgd) {
        subckt()->erase(_Cgd);
        _Cgd = NULL;
      }else{
      }
    }else{
      if (!_Cgd) {
        const CARD* p = device_dispatcher["capacitor"];
        assert(p);
        _Cgd = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Cgd);
        subckt()->push_front(_Cgd);
      }else{
      }
      {
        node_t nodes[] = {_n[n_g], _n[n_id]};
        _Cgd->set_parameters("Cgd", this, &Eval_Cgd, s->cgdo, 0, NULL, 2, nodes);
      }
    }
    if (!OPT::cstray || _n[n_b].n_() == _n[n_g].n_()) {
      if (_Cgb) {
        subckt()->erase(_Cgb);
        _Cgb = NULL;
      }else{
      }
    }else{
      if (!_Cgb) {
        const CARD* p = device_dispatcher["capacitor"];
        assert(p);
        _Cgb = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Cgb);
        subckt()->push_front(_Cgb);
      }else{
      }
      {
        node_t nodes[] = {_n[n_g], _n[n_b]};
        _Cgb->set_parameters("Cgb", this, &Eval_Cgb, s->cgbo, 0, NULL, 2, nodes);
      }
    }

    assert( (m->bti_model_name.value() != std::string("")) == m->use_bti() );
    if( m->bti_model_name.value() == std::string("") )
    {
      trace2("DEV_BUILT_IN_MOS::expand no bti expand", m->polarity, m->use_bti());
    }else{
      trace2("DEV_BUILT_IN_MOS::expand", m->polarity, m->use_bti());
      if (!_BTI) {
        const CARD* p = device_dispatcher["bti"];
        assert(p);
        _BTI = dynamic_cast<COMPONENT*>(p->clone());
        assert(_BTI);
#ifdef BTI_IN_SUBCKT
        subckt()->push_front(_BTI);
#endif
      }else{
      }
      {
        if (m->polarity==pP) { // stupid hack. use voltage source?
          node_t nodes[] = {_n[n_b], _n[n_g]};
          _BTI->set_parameters("BTI", this, c->_bti, 1.0, 0, NULL, 2, nodes);
        } else {
          node_t nodes[] = {_n[n_g], _n[n_b]};
          std::cout << "polan" << m->polarity <<"\n";
          _BTI->set_parameters("BTI", this, c->_bti, 1.0, 0, NULL, 2, nodes);
        }
      }
    }
    {
      if (!_Ids) {
        const CARD* p = device_dispatcher["cpoly_g"];
        assert(p);
        _Ids = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Ids);
        subckt()->push_front(_Ids);
      }else{
      }
      {
        node_t nodes[] = {_n[n_id], _n[n_is], _n[n_g], _n[n_is], _n[n_id],
          _n[n_g], _n[n_b], _n[n_is], _n[n_id], _n[n_b]};
        _Ids->set_parameters("Ids", this, NULL, 0., 6, &idsxxx, 10, nodes);
      }
    }
    if (!(m->needs_isub) || _n[n_d].n_() == _n[n_b].n_()) {
      if (_Idb) {
        subckt()->erase(_Idb);
        _Idb = NULL;
      }else{
      }
    }else{
      if (!_Idb) {
        const CARD* p = device_dispatcher["cpoly_g"];
        assert(p);
        _Idb = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Idb);
        subckt()->push_front(_Idb);
      }else{
      }
      {
        node_t nodes[] = {_n[n_id], _n[n_b], _n[n_id], _n[n_is], _n[n_g], _n[n_is], _n[n_b], _n[n_is]};
        _Idb->set_parameters("Idb", this, NULL, 0., 5, &idbxxx, 8, nodes);
      }
    }
    if (!(m->needs_isub) || _n[n_s].n_() == _n[n_b].n_()) {
      if (_Isb) {
        subckt()->erase(_Isb);
        _Isb = NULL;
      }else{
      }
    }else{
      if (!_Isb) {
        const CARD* p = device_dispatcher["cpoly_g"];
        assert(p);
        _Isb = dynamic_cast<COMPONENT*>(p->clone());
        assert(_Isb);
        subckt()->push_front(_Isb);
      }else{
      }
      {
        node_t nodes[] = {_n[n_is], _n[n_b], _n[n_is], _n[n_id], _n[n_g], _n[n_id], _n[n_b], _n[n_id]};
        _Isb->set_parameters("Isb", this, NULL, 0., 5, &isbxxx, 8, nodes);
      }
    }
  }else{
    //precalc();
  }
  //precalc();
  subckt()->expand();

#ifndef BTI_IN_SUBCKT
  trace0("DEV_BUILT_IN_MOS::expand bti not in subckt");
  assert( (m->bti_model_name.value() != std::string("")) == m->use_bti() );
  if( m->use_bti() ){
    trace1("DEV_BUILT_IN_MOS::expanding bti", polarity);
    _BTI->expand();
    // _BTI->set_slave();
  }
#endif
  // subckt()->precalc();
  assert(!is_constant());
  subckt()->set_slave();


  trace0("DEV_BUILT_IN_MOS::expand, ADP things");
  if ( adp() == NULL ){
    attach_adp( m->new_adp( (COMPONENT*) this ) );
        //subckt()->push_front(adp());
  }else{
    std::cerr << short_label() << "\n";
    assert(false);
  }
}
/*--------------------------------------------------------------------------*/
double DEV_BUILT_IN_MOS::tr_probe_num(const std::string& x)const
{
  const DEV_BUILT_IN_MOS* d = this;
  assert(_n);
  const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(common());
  assert(c);
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(c->sdp());
  assert(s);
  const ADP_BUILT_IN_MOS* a = prechecked_cast<const ADP_BUILT_IN_MOS*>(adp());
  assert(a);

  const DEV_BUILT_IN_BTI* B = dynamic_cast<const DEV_BUILT_IN_BTI*>(_BTI);

  if (Umatch(x, "bti |dvth_bti ")) {
    if (B)
      return  (B->dvth());
    else
      return NA;
  } else if (Umatch(x, "v ")) {
    return  _n[n_d].v0() - _n[n_s].v0();
#ifdef BTI_HACK
  }else if (Umatch(x, "ugbti ")) {
    return  ( CARD::probe(_BTI,"vin"));
#endif
  }else if (Umatch(x, "vds ")) {
    return  _n[n_d].v0() - _n[n_s].v0();
  }else if (Umatch(x, "dvth ")) { // hci???
    return a->delta_vth;
  }else if (Umatch(x, "dv_bti ")) { // hci???
    if (d->_BTI) return  ((const DEV_BUILT_IN_BTI*)(d->_BTI))->dvth();
    return(NA);
  }else if (Umatch(x, "bti_stress ")) { // hci???
    return  a->bti_stress->tr_get();
  }else if (Umatch(x, "hci |bti ")) { // hci???
    return  a->tr_probe_num(x);
  }else if (Umatch(x, "vgs ")) {
    return  _n[n_g].v0() - _n[n_s].v0();
  }else if (Umatch(x, "vbs ")) {
    return  _n[n_b].v0() - _n[n_s].v0();
  }else if (Umatch(x, "vdsi{nt} ")) {
    return  vds;
  }else if (Umatch(x, "vgsi{nt} ")) {
    return  vgs;
  }else if (Umatch(x, "vbsi{nt} ")) {
    return  vbs;
  }else if (Umatch(x, "use_bti ")) {
    return  m->use_bti();
  }else if (Umatch(x, "vgd ")) {
    return  _n[n_g].v0() - _n[n_d].v0();
  }else if (Umatch(x, "vbd ")) {
    return  _n[n_b].v0() - _n[n_d].v0();
  }else if (Umatch(x, "vsd ")) {
    return  _n[n_s].v0() - _n[n_d].v0();
  }else if (Umatch(x, "vdm ")) {
    return  ( _n[n_d].v0() - _n[n_s].v0() + _n[n_d].v0() - _n[n_d].v0() ) / 2.;
  }else if (Umatch(x, "vgm ")) {
    return  ( _n[n_g].v0() - _n[n_s].v0() + _n[n_g].v0() - _n[n_d].v0() ) / 2.;
  }else if (Umatch(x, "vbm ")) {
    return  ( _n[n_b].v0() - _n[n_s].v0() + _n[n_b].v0() - _n[n_d].v0() ) / 2.;
  }else if (Umatch(x, "vsm ")) {
    return  ( _n[n_s].v0() - _n[n_s].v0() + _n[n_s].v0() - _n[n_d].v0() ) / 2.;
  }else if (Umatch(x, "vdg ")) {
    return  _n[n_d].v0() - _n[n_g].v0();
  }else if (Umatch(x, "vbg ")) {
    return  _n[n_b].v0() - _n[n_g].v0();
  }else if (Umatch(x, "vsg ")) {
    return  _n[n_s].v0() - _n[n_g].v0();
  }else if (Umatch(x, "vdb ")) {
    return  _n[n_d].v0() - _n[n_b].v0();
  }else if (Umatch(x, "vgb ")) {
    return  _n[n_g].v0() - _n[n_b].v0();
  }else if (Umatch(x, "vsb ")) {
    return  _n[n_s].v0() - _n[n_b].v0();
  }else if (Umatch(x, "vd ")) {
    return  _n[n_d].v0();
  }else if (Umatch(x, "vg ")) {
    return  _n[n_g].v0();
  }else if (Umatch(x, "vb ")) {
    return  _n[n_b].v0();
  }else if (Umatch(x, "vs ")) {
    return  _n[n_s].v0();
  }else if (Umatch(x, "i{d} ")) {
    return  (_Rd) ? CARD::probe(_Rd,"I") : ( CARD::probe(_Ids,"I") - CARD::probe(_Cgd,"I") - CARD::probe(_Ddb,"I") * m->polarity);
  }else if (Umatch(x, "is ")) {
    return  (_Rs) ? CARD::probe(_Rs,"I") : (- CARD::probe(_Ids,"I") - CARD::probe(_Cgs,"I") - CARD::probe(_Dsb,"I") * m->polarity);
  }else if (Umatch(x, "ig ")) {
    return  CARD::probe(_Cgs,"I") + CARD::probe(_Cgd,"I") + CARD::probe(_Cgb,"I");
  }else if (Umatch(x, "ib ")) {
    return  - CARD::probe(_Ddb,"I") * m->polarity - CARD::probe(_Dsb,"I") * m->polarity - CARD::probe(_Cgb,"I");
  }else if (Umatch(x, "ibd ")) {
    return  CARD::probe(_Ddb,"I");
  }else if (Umatch(x, "ibs ")) {
    return  CARD::probe(_Dsb,"I");
  }else if (Umatch(x, "cgso{vl} ")) {
    return  CARD::probe(_Cgs,"NV");
  }else if (Umatch(x, "cgdo{vl} ")) {
    return  CARD::probe(_Cgd,"NV");
  }else if (Umatch(x, "cgbo{vl} ")) {
    return  CARD::probe(_Cgb,"NV");
  }else if (Umatch(x, "cgst ")) {
    return  CARD::probe(_Cgs,"EV");
  }else if (Umatch(x, "cgdt ")) {
    return  CARD::probe(_Cgd,"EV");
  }else if (Umatch(x, "cgbt ")) {
    return  CARD::probe(_Cgb,"EV");
  }else if (Umatch(x, "cgs{m} ")) {
    return  CARD::probe(_Cgs,"EV") - CARD::probe(_Cgs,"NV");
  }else if (Umatch(x, "cgd{m} ")) {
    return  CARD::probe(_Cgd,"EV") - CARD::probe(_Cgd,"NV");
  }else if (Umatch(x, "cgb{m} ")) {
    return  CARD::probe(_Cgb,"EV") - CARD::probe(_Cgb,"NV");
  }else if (Umatch(x, "cbd ")) {
    return  CARD::probe(_Ddb,"Cap");
  }else if (Umatch(x, "cbs ")) {
    return  CARD::probe(_Dsb,"Cap");
  }else if (Umatch(x, "cgate ")) {
    return  s->cgate;
  }else if (Umatch(x, "gm ")) {
    return  (reversed) ? gmr : gmf;
  }else if (Umatch(x, "gmb{s} ")) {
    return  (reversed) ? gmbr : gmbf;
  }else if (Umatch(x, "gbd ")) {
    return  CARD::probe(_Ddb,"G");
  }else if (Umatch(x, "gbs ")) {
    return  CARD::probe(_Dsb,"G");
  }else if (Umatch(x, "vth ")) {
    return  von * m->polarity;
  }else if (Umatch(x, "ids ")) {
    return  m->polarity * ((reversed) ? -ids : ids);
  }else if (Umatch(x, "idst{ray} ")) {
    return  - CARD::probe(_Cgd,"I") + CARD::probe(_Ddb,"I") * m->polarity;
  }else if (Umatch(x, "p ")) {
    return  CARD::probe(_Rs,"P") + CARD::probe(_Rd,"P") + CARD::probe(_Ddb,"P") + CARD::probe(_Dsb,"P") + CARD::probe(_Cgs,"P") + CARD::probe(_Cgd,"P") + CARD::probe(_Cgb,"P") + CARD::probe(_Ids,"P");
  }else if (Umatch(x, "pd ")) {
    return  CARD::probe(_Rs,"PD") + CARD::probe(_Rd,"PD") + CARD::probe(_Ddb,"PD") + CARD::probe(_Dsb,"PD") + CARD::probe(_Cgs,"PD") + CARD::probe(_Cgd,"PD") + CARD::probe(_Cgb,"PD") + CARD::probe(_Ids,"PD");
  }else if (Umatch(x, "ps ")) {
    return  CARD::probe(_Rs,"PS") + CARD::probe(_Rd,"PS") + CARD::probe(_Ddb,"PS") + CARD::probe(_Dsb,"PS") + CARD::probe(_Cgs,"PS") + CARD::probe(_Cgd,"PS") + CARD::probe(_Cgb,"PS") + CARD::probe(_Ids,"PS");
  }else if (Umatch(x, "REgion ")) {
    return  static_cast<double>((!cutoff) + (!subthreshold * 2) + (saturated * 4) + (sbfwd * 10) + ((vbs > vds) * 20) + (punchthru * 40)) * ((reversed)? -1 : 1);
  }else if (Umatch(x, "SUBthreshold ")) {
    return  static_cast<double>(subthreshold);
  }else if (Umatch(x, "CUToff ")) {
    return  static_cast<double>(cutoff);
  }else if (Umatch(x, "SATurated ")) {
    return  static_cast<double>(saturated);
  }else if (Umatch(x, "TRIode ")) {
    return  static_cast<double>(!saturated && !subthreshold);
  }else if (Umatch(x, "SBFwd ")) {
    return  static_cast<double>(sbfwd);
  }else if (Umatch(x, "DBFwd ")) {
    return  static_cast<double>(vbs > vds);
  }else if (Umatch(x, "REVersed ")) {
    return  static_cast<double>(reversed);
  }else if (Umatch(x, "status ")) {
    return  static_cast<double>(converged() * 2);
  }else if (Umatch(x, "ids ")) {
    return ids;
  }else if (Umatch(x, "idsxxx ")) {
    return idsxxx;
  }else if (Umatch(x, "gds ")) {
    return gds;
  }else if (Umatch(x, "gmf ")) {
    return gmf;
  }else if (Umatch(x, "gmr ")) {
    return gmr;
  }else if (Umatch(x, "gmbf ")) {
    return gmbf;
  }else if (Umatch(x, "gmbr ")) {
    return gmbr;
  }else if (Umatch(x, "idb ")) {
    return idb;
  }else if (Umatch(x, "idbxxx ")) {
    return idbxxx;
  }else if (Umatch(x, "gdbdb ")) {
    return gdbdb;
  }else if (Umatch(x, "gdbds ")) {
    return gdbds;
  }else if (Umatch(x, "gdbgs ")) {
    return gdbgs;
  }else if (Umatch(x, "gdbbs ")) {
    return gdbbs;
  }else if (Umatch(x, "isb ")) {
    return isb;
  }else if (Umatch(x, "isbxxx ")) {
    return isbxxx;
  }else if (Umatch(x, "gsbsb ")) {
    return gsbsb;
  }else if (Umatch(x, "gsbsd ")) {
    return gsbsd;
  }else if (Umatch(x, "gsbgd ")) {
    return gsbgd;
  }else if (Umatch(x, "gsbbd ")) {
    return gsbbd;
  }else if (Umatch(x, "qgate ")) {
    return qgate;
  }else if (Umatch(x, "cgs ")) {
    return cgs;
  }else if (Umatch(x, "cggb ")) {
    return cggb;
  }else if (Umatch(x, "cgsb ")) {
    return cgsb;
  }else if (Umatch(x, "cgdb ")) {
    return cgdb;
  }else if (Umatch(x, "qgs ")) {
    return qgs;
  }else if (Umatch(x, "cgsgs ")) {
    return cgsgs;
  }else if (Umatch(x, "cgsgb ")) {
    return cgsgb;
  }else if (Umatch(x, "cgssb ")) {
    return cgssb;
  }else if (Umatch(x, "cgsdb ")) {
    return cgsdb;
  }else if (Umatch(x, "qgd ")) {
    return qgd;
  }else if (Umatch(x, "cgdgd ")) {
    return cgdgd;
  }else if (Umatch(x, "cgdgb ")) {
    return cgdgb;
  }else if (Umatch(x, "cgdsb ")) {
    return cgdsb;
  }else if (Umatch(x, "cgddb ")) {
    return cgddb;
  }else if (Umatch(x, "qdrn ")) {
    return qdrn;
  }else if (Umatch(x, "cdsds ")) {
    return cdsds;
  }else if (Umatch(x, "cdgb ")) {
    return cdgb;
  }else if (Umatch(x, "cdsb ")) {
    return cdsb;
  }else if (Umatch(x, "cddb ")) {
    return cddb;
  }else if (Umatch(x, "qbulk ")) {
    return qbulk;
  }else if (Umatch(x, "cbs ")) {
    return cbs;
  }else if (Umatch(x, "cbgb ")) {
    return cbgb;
  }else if (Umatch(x, "cbsb ")) {
    return cbsb;
  }else if (Umatch(x, "cbdb ")) {
    return cbdb;
  }else if (Umatch(x, "qbs ")) {
    return qbs;
  }else if (Umatch(x, "cbsbs ")) {
    return cbsbs;
  }else if (Umatch(x, "cbsgb ")) {
    return cbsgb;
  }else if (Umatch(x, "cbssb ")) {
    return cbssb;
  }else if (Umatch(x, "cbsdb ")) {
    return cbsdb;
  }else if (Umatch(x, "qbd ")) {
    return qbd;
  }else if (Umatch(x, "cbdbd ")) {
    return cbdbd;
  }else if (Umatch(x, "cbdgb ")) {
    return cbdgb;
  }else if (Umatch(x, "cbdsb ")) {
    return cbdsb;
  }else if (Umatch(x, "cbddb ")) {
    return cbddb;
  }else if (Umatch(x, "gtau ")) {
    return gtau;
  }else if (Umatch(x, "cqgb ")) {
    return cqgb;
  }else if (Umatch(x, "cqsb ")) {
    return cqsb;
  }else if (Umatch(x, "cqdb ")) {
    return cqdb;
  }else if (Umatch(x, "cqbb ")) {
    return cqbb;
  }else if (Umatch(x, "vgs ")) {
    return vgs;
  }else if (Umatch(x, "vds ")) {
    return vds;
  }else if (Umatch(x, "vbs ")) {
    return vbs;
  }else if (Umatch(x, "vdsat ")) {
    return vdsat;
  }else if (Umatch(x, "vgst ")) {
    return vgst;
  }else if (Umatch(x, "von ")) {
    return von;
  }else if (Umatch(x, "reversed ")) {
    return reversed;
  }else if (Umatch(x, "cutoff ")) {
    return cutoff;
  }else if (Umatch(x, "subthreshold ")) {
    return subthreshold;
  }else if (Umatch(x, "saturated ")) {
    return saturated;
  }else if (Umatch(x, "sbfwd ")) {
    return sbfwd;
  }else if (Umatch(x, "punchthru ")) {
    return punchthru;
  }else {
    return BASE_SUBCKT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
double DEV_BUILT_IN_MOS::tt_probe_num(const std::string& x)const
{
  assert(_n);
  const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(common());
  assert(c);
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);
  const SDP_BUILT_IN_MOS_BASE* s = prechecked_cast<const SDP_BUILT_IN_MOS_BASE*>(c->sdp());
  assert(s);
  const ADP_BUILT_IN_MOS* a = prechecked_cast<const ADP_BUILT_IN_MOS*>(adp());
  const DEV_BUILT_IN_BTI* B = dynamic_cast<const DEV_BUILT_IN_BTI*>(_BTI);

  if (Umatch(x, "bti |dvth_bti ")) {
    if (B)
      return  (B->dvth());
    else
      return NA;
  }else if (Umatch(x, "wdt ")) {
    return  a->wdT();
  }else if (Umatch(x, "hci |dvth_hci ")) {
    return  a->tt_probe_num(x);
  }else if (Umatch(x, "use_bti ")) {
    return  m->use_bti();
  }else if (Umatch(x, "dvth ")) { // hci???
    if (a) return a->delta_vth;
  }else if (Umatch(x, "stress ")) {
    return 19999;
  }
  return BASE_SUBCKT::tt_probe_num(x);
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::reverse_if_needed()
{
    if (vds < 0) {
      error(bTRACE, long_label() + ": reversing\n");
      error(bTRACE, "before: vds=%g vgs=%g vbs=%g\n", vds, vgs, vbs);
      reversed = !reversed;
      vgs -= vds;
      vbs -= vds;
      vds = -vds;
      error(bTRACE, "after: vds=%g vgs=%g vbs=%g\n", vds, vgs, vbs);
      if (OPT::dampstrategy & dsREVERSE) {
	_sim->_fulldamp = true;
	untested();
	error(bTRACE, long_label() + ":reverse damp\n");
      }
      if (!(OPT::mosflags & 0040)) {
	vbs = std::min(vbs,(hp_float_t)0.);
      }else{
	untested();
      }
    }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
bool DEV_BUILT_IN_MOS::tr_needs_eval()const
{
  if (is_q_for_eval()) {
    untested();
    return false;
  }else if (!converged()) {
    return true;
  }else{
    const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(common());
    assert(c);
    const MODEL_BUILT_IN_MOS_BASE* m=prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
    assert(m);
    polarity_t polarity = m->polarity;
    node_t& eff_s((reversed) ? _n[n_id] : _n[n_is]);
    node_t& eff_d((reversed) ? _n[n_is] : _n[n_id]);
    if( m->use_bti() ) if ( _BTI->tr_needs_eval()) return true;
    return !(conchk(vds,polarity*(eff_d.v0()-eff_s.v0()),OPT::vntol)
	     && conchk(vgs, polarity*(_n[n_g].v0()-eff_s.v0()),
		       OPT::vntol)
	     && conchk(vbs, polarity*(_n[n_b].v0()-eff_s.v0()),
		       OPT::vntol));
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_BUILT_IN_MOS::do_tr()
{
  const COMMON_BUILT_IN_MOS* c = prechecked_cast<const COMMON_BUILT_IN_MOS*>(common());
  assert(c);
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(c->model());
  assert(m);

  bool was_cutoff = cutoff;
  bool was_subthreshold = subthreshold;
  bool was_saturated = saturated;
  bool was_reversed = reversed;
  bool was_sbfwd = sbfwd;
  polarity_t polarity = m->polarity;

  if (_sim->is_initial_step()) {
    reversed = false;
    vds = vgs = vbs = 0.;
  }else{
    double Vds, Vgs, Vbs;
    if (reversed) {
      Vds = polarity * volts_limited(_n[n_is],_n[n_id]);
      Vgs = polarity * volts_limited(_n[n_g],_n[n_id]);
      Vbs = polarity * volts_limited(_n[n_b],_n[n_id]);
    }else{
      Vds = polarity * volts_limited(_n[n_id],_n[n_is]);
      Vgs = polarity * volts_limited(_n[n_g],_n[n_is]);
      Vbs = polarity * volts_limited(_n[n_b],_n[n_is]);
    }
    vgs = fet_limit_vgs(Vgs, vgs, von);
    if (_n[n_d].n_() == _n[n_g].n_()) {
      vds = Vds + (vgs - Vgs);
    }else{
      // Spice hacks Vds here, but my tests show that it often makes
      // convergence worse, and never improves it.
      // I am guessing that it does help when drain and gate are connected,
      // and Spice does it here in case they are and cannot be determined
      // whether they are or not.
      // The hack maintains Vdg after Vgs limiting.
      //Vds = Vds + (vgs - Vgs);
      vds = fet_limit_vds(Vds, vds);
    }
    vbs = std::min(Vbs, 0.);
    //vbs = pnj_limit(double Vbs, double vbs, double vt, double vcrit);
    //vds = Vds;
    //vgs = Vgs;
    //vbs = Vbs;
  }

  assert(qgate == qgate);
  assert(qgs == qgs);
  assert(qgd == qgd);
  assert(qdrn == qdrn);
  assert(qbulk == qbulk);
  assert(qbs == qbs);
  assert(qbd == qbd);

  m->tr_eval(this);

  assert(qgate == qgate);
  assert(qgs == qgs);
  assert(qgd == qgd);
  assert(qdrn == qdrn);
  assert(qbulk == qbulk);
  assert(qbs == qbs);
  assert(qbd == qbd);

  if (reversed) {
    idsxxx = ids + vds*gds + vgs*gmr + vbs*gmbr;
    isbxxx = isb - vds*gsbsd - vgs*gsbgd - vbs*gsbbd;
    idbxxx = 0.;
  }else{
    idsxxx = ids - vds*gds - vgs*gmf - vbs*gmbf;
    idbxxx = idb - vds*gdbds - vgs*gdbgs - vbs*gdbbs;
    isbxxx = 0.;
  }
  ids *= polarity;
  idsxxx *= polarity;
  assert(subckt());

#ifdef BTI_LATE_EVAL
  std::list<CARD*>::iterator ci;

  bool isconverged = true;
  for (ci=subckt()->begin(); ci!=subckt()->end(); ++ci) {
    if ((**ci).tr_needs_eval() || !OPT::bypass ) {

      if (*ci != _BTI){
        isconverged &= (**ci).do_tr();
      }else{
      }
    }
  }

  set_converged( isconverged );

  if( m->use_bti() ){
    if(  converged() ){
      std::cout << "* btieval " << _sim->iteration_number() << " \n";
      set_converged(_BTI->do_tr());
    } else {
      std::cout << "* not btieval " << _sim->iteration_number() << " \n";
    }
  }

#else
  set_converged(subckt()->do_tr());
#endif

  // BTI_HACK
#ifndef BTI_IN_SUBCKT
 // if( m->use_bti &&  converged() )
   // set_converged(_BTI->do_tr());
#endif
  
  trace3(long_label().c_str(), vds, vgs, vbs);
  trace4("", ids, gmf, gds, gmbf);
  trace4("", ids, gmr, gds, gmbr);
  if (was_cutoff != cutoff  ||  was_subthreshold != subthreshold  
  	||  was_saturated != saturated  ||  was_reversed != reversed  
	||  was_sbfwd != sbfwd) {
    if (OPT::dampstrategy & dsDEVREGION) {
      _sim->_fulldamp = true;
    }else{
    }
    #if defined(DO_TRACE)
      error(bTRACE,"%s: region change\n", long_label().c_str());
    #endif
  }else{
  }
#ifdef BTI_IN_SUBCKT
  return converged();
#else
  return (converged() && _BTI->converged());
#endif
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::stress_apply( )
{
  const COMMON_BUILT_IN_MOS* c = (const COMMON_BUILT_IN_MOS*) common();
  const MODEL_BUILT_IN_MOS_BASE* m = (const MODEL_BUILT_IN_MOS_BASE*)(c->model());
  assert(m);
  BASE_SUBCKT::stress_apply();
  m->do_stress_apply(this);
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::tr_stress( )
{
  const COMMON_COMPONENT* cc = common();
  const MODEL_BUILT_IN_MOS_BASE* m = ( const MODEL_BUILT_IN_MOS_BASE*)(cc->model());
  assert(m);

  m->do_tr_stress( this );
}

/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS::wdT() const{
  return ids_stress->wdT();
}
/*--------------------------------------------------------------------------*/
//expand?
void ADP_BUILT_IN_MOS::init(const COMPONENT* c)
{
  trace0("ADP_BUILT_IN_MOS::init");
  const COMMON_COMPONENT* cc = prechecked_cast<const COMMON_COMPONENT*>(c->common());
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(cc->model());
  assert(m);
  assert(cc);

  // std::cerr << "ADP_BUILT_IN_MOS::init( " <<c<< " ) " << c->short_label() << "\n" ;
  bti_stress = new ADP_NODE( c, "bti" );
  // ADP_NODE_LIST::adp_node_list.push_back( bti_stress );

  // only mos>0?
  ids_stress = new ADP_NODE( c, "ids" );
  igd_stress = new ADP_NODE( c, "igs" );

  vthscale_bti = 1;
  vthdelta_bti = 0;

  delta_vth=0;

  btistress_taken = 0;
  bti_eff_voltage = 0;
}

/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::tt_prepare() // NOT const
{
  BASE_SUBCKT::tt_prepare();

  const COMMON_BUILT_IN_MOS* c = (const COMMON_BUILT_IN_MOS*)(common());
  assert(c);
  assert(c->model());

  const MODEL_BUILT_IN_MOS_BASE* m = (const MODEL_BUILT_IN_MOS_BASE*)(c->model());
  assert(m);

  m->do_tt_prepare(this);
}
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS::tt_accept()
{
  //FIXME: move c to ADP_CARD. merge ADP_card with DEV?
  // const DEV_BUILT_IN_MOS* c = (const DEV_BUILT_IN_MOS*) (bti_stress->c());
  //SIM_DATA* sim = c->_sim;
//  std::cerr << "ADP_BUILT_IN_MOS::tt_accept " << c->long_label() << "\n";
//  std::cerr << "ADP_BUILT_IN_MOS::tt_accept time " << sim->_Time0 << "\n";
//  std::cerr << "ADP_BUILT_IN_MOS::tt_accept stress " << bti_stress->get() << "\n";

  btistress_taken  = bti_stress->get();

}
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS::tt_commit()
{
  //FIXME: move c to ADP_CARD. merge ADP_card with DEV?
  //const DEV_BUILT_IN_MOS* c = (const DEV_BUILT_IN_MOS*) (bti_stress->c());
  SIM_DATA* sim = CKT_BASE::_sim;
//  std::cerr << "ADP_BUILT_IN_MOS::tt_commit " << c->long_label() << "\n";
//  std::cerr << "ADP_BUILT_IN_MOS::tt_commit time " << sim->_Time0 << "\n";
//  std::cerr << "ADP_BUILT_IN_MOS::tt_commit stress " << bti_stress->get() << "\n";
//  std::cerr << "ADP_BUILT_IN_MOS::tt_commit deltatime " << sim->_dT0 << "\n";

  double stressdelta = bti_stress->get() - btistress_taken ;
  bti_eff_voltage = log(stressdelta/sim->_dT0);

//  std::cerr << "ADP_BUILT_IN_MOS::tt_commit stressdelta " << stressdelta << "\n";
//  std::cerr << "ADP_BUILT_IN_MOS::tt_commit eff_voltage " << bti_eff_voltage << "\n";

  // parameters FIXME. put somewhere else.
  // ble D_0 = 5.64e-4;  // cm cm / s


}
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS::tr_probe_num(const std::string& x)const
{
  if( Umatch("bti ", x) ){
    if(bti_stress) return bti_stress->tr_get();
    return NA;
  } else if( Umatch("dvth_bti ", x) ) {
    return vthdelta_bti;
  }else{
     return 888;   //    return ADP_BUILT_IN_MOS::tr_probe_num(x); diode?
  }

}
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS::tt_probe_num(const std::string& x)const
{
  unreachable(); //?
  if( Umatch("bti ", x) ){
    return bti_stress->get();
  } else if( Umatch("dvth_bti ", x) ) {
    return bti_stress->get();
  } else if( Umatch("dvth_bti ", x) ) {
    return vthdelta_bti;
  } else if( Umatch("bti_eff ", x) ) {
    return bti_eff_voltage;
  }else{
     return 999;    //    return ADP_BUILT_IN_MOS::tr_probe_num(x); diode?
  }
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::tt_commit(){
  BASE_SUBCKT::tt_commit();

  //if(m->use_bti){
  //  _BTI->tt_commit();
  //}
}
/*--------------------------------------------------------------------------*/
void DEV_BUILT_IN_MOS::tr_save_amps(int trstep)
{
  trace1(("DEV_BUILT_IN_MOS::tr_save_amps()" + long_label()).c_str(),trstep);
  const COMMON_COMPONENT* cc = common();
  const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(cc->model());
  assert(m);
  int j = 3;
  int k = j;

  incomplete();

  hp_float_t tramps0= tr_probe_num("id");
  trace2(("DEV_BUILT_IN_MOS::tr_save_amps " + long_label()).c_str(),trstep, tramps0);
  hp_float_t tramps1=  ((ELEMENT*)_Cgb)->tr_amps() 
                     +((_Cgd) ? ((ELEMENT*)_Cgd)->tr_amps() : 0 )
                     +((_Cgs) ? ((ELEMENT*)_Cgs)->tr_amps() : 0 )                   ;  
  trace2(("DEV_BUILT_IN_MOS::tr_save_amps " + long_label()).c_str(),trstep, tramps1);

  // bloeder bug
  hp_float_t tramps2 = 0;
  //hp_float_t tramps2=  (_Rs) ? ((ELEMENT*)_Rs )->tr_amps() :
  //                           - ((ELEMENT*)_Ids)->tr_amps()
  //                           - ((ELEMENT*)_Cgs)->tr_amps()
  //                           - ((ELEMENT*)_Dsb)->tr_amps() * m->polarity;

  hp_float_t tramps3= tramps0 + tramps1 ;
  trace2(("DEV_BUILT_IN_MOS::tr_save_amps" + long_label()).c_str(),trstep, tramps3);
  //
  // double tramps0= tr_probe_num("id");
  // double tramps1=0; // tr_probe_num("ig");
  // double tramps2=0; // tr_probe_num("is");
  // double tramps3= tramps0 + tramps1 + tramps2;


  //std::cerr << "saving _amps[ " << n << " ]. have " << net_nodes() << " nodes\n";


  if (_amps==0) _tr_amps_diff_cur = 0;
  _tr_amps_diff_max = 0; // maximum der delta i in diesem zeitschritt.
  _tr_amps_scale_max = 0; 


  if(_amps!=0) {
    double _amps3 = _amps[trstep*k + 0] + _amps[trstep*k + 1] + _amps[trstep*k + 2] ;

    double diff0= _amps[trstep*k + 0] - tramps0;
    double diff1= _amps[trstep*k + 1] - tramps1;
    double diff2= _amps[trstep*k + 2] - tramps2;
    double diff3= _amps3              - tramps3;

    double sum0= fabs(_amps[trstep*k + 0]) + fabs(tramps0);
    double sum1= fabs(_amps[trstep*k + 1]) + fabs(tramps1);
    double sum2= fabs(_amps[trstep*k + 2]) + fabs(tramps2);
    double sum3= fabs(_amps3)              + fabs(tramps3);

    _tr_amps_diff_max =  fabs(diff0);
    _tr_amps_diff_max =  fmax( _tr_amps_diff_max, fabs(diff1));
    _tr_amps_diff_max =  fmax( _tr_amps_diff_max, fabs(diff2));
    _tr_amps_diff_max =  fmax( _tr_amps_diff_max, fabs(diff0 + diff1 + diff2));

    _tr_amps_scale_max =                           fabs(diff0/sum0) ;
    _tr_amps_scale_max =  fmax( _tr_amps_diff_max, fabs(diff1/sum1) ) ;
    _tr_amps_scale_max =  fmax( _tr_amps_diff_max, fabs(diff2/sum2) ) ;
    _tr_amps_scale_max =  fmax( _tr_amps_diff_max, fabs(diff3/sum3) ) ;
  }


 // _amps_new[ trstep*k+0] = tramps0;
 // _amps_new[ trstep*k+1] = tramps1;
 // _amps_new[ trstep*k+2] = tramps2;

  tr_behaviour_del = _tr_amps_diff_max;
  tr_behaviour_rel = _tr_amps_scale_max;

  assert( tr_behaviour_del >= 0 );

  CKT_BASE::tr_behaviour_del = fmax( CKT_BASE::tr_behaviour_del, tr_behaviour_del);
  CKT_BASE::tr_behaviour_rel = fmax( CKT_BASE::tr_behaviour_rel, tr_behaviour_rel);
//  trace10 << "DEV_BUILT_IN_MOS::tr_save_amps " << trstep << short_label() << 
//    " " << tr_behaviour_del<< "rel: " << tr_behaviour_rel << "\n";

  trace1(("DEV_BUILT_IN_MOS::tr_save_amps tr_behaviour" + long_label()).c_str(),trstep);

  tr_behaviour();

  trace1(("DEV_BUILT_IN_MOS::tr_save_amps done " + long_label()).c_str(),trstep);
}
/*--------------------------------------------------------------------------*/
void      DEV_BUILT_IN_MOS::dc_advance() {set_not_converged(); BASE_SUBCKT::dc_advance();
#ifndef BTI_IN_SUBCKT
  const COMMON_COMPONENT* cc = common();
  MODEL_BUILT_IN_MOS_BASE* m = ( MODEL_BUILT_IN_MOS_BASE*)(cc->model());
  if(m->use_bti() )
    _BTI->dc_advance();
#endif
  
  }
  void      DEV_BUILT_IN_MOS::tr_advance() {set_not_converged(); BASE_SUBCKT::tr_advance();}
  void      DEV_BUILT_IN_MOS::tr_regress() {set_not_converged(); BASE_SUBCKT::tr_regress();}
/*--------------------------------------------------------------------------*/
void      DEV_BUILT_IN_MOS::precalc_last() {
  COMPONENT::precalc_last();
  if(subckt()) subckt()->precalc_last();
#ifndef BTI_IN_SUBCKT
  trace0("DEV_BUILT_IN_MOS::precalc_last bti notin sckt");
  const COMMON_COMPONENT* cc = common();
  MODEL_BUILT_IN_MOS_BASE* m = ( MODEL_BUILT_IN_MOS_BASE*)(cc->model());
  //if(m->use_bti())
  //  _BTI->precalc_last();
#endif
}
/*--------------------------------------------------------------------------*/
void      DEV_BUILT_IN_MOS::precalc_first() {
  COMPONENT::precalc_first();
  if(subckt()) subckt()->precalc_first();
#ifndef BTI_IN_SUBCKT
  trace0("DEV_BUILT_IN_MOS::precalc_first bti notin sckt");
  // const COMMON_COMPONENT* cc = common();
  //MODEL_BUILT_IN_MOS_BASE* m = ( MODEL_BUILT_IN_MOS_BASE*)(cc->model());
  //if(m->use_bti())
  //  _BTI->precalc_first();
#endif
}
