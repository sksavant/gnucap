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
#include "d_ivl_ports.h"
#include "d_logic.h"
#include "extlib.h"

// enum smode_t   {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};


/*--------------------------------------------------------------------------
// from t-net.cc
void ivl_nexus_s::operator delete(void*, size_t)   
{                                                                                                                
        assert(0); 
} 
--------------------------------------------------------------------------*/
int DEV_LOGIC_OUT::_count = -1;
int DEV_LOGIC_IN::_count = -1;
/*--------------------------------------------------------------------------*/
namespace{
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
  ELEMENT::tr_begin();
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
void DEV_LOGIC_OUT::tr_advance()
{
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
void DEV_LOGIC_OUT::tr_regress()
{itested();
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {itested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{itested();
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:  itested();
                    assert(subckt());
                    subckt()->tr_regress();
                    break;
    case moDIGITAL: itested();
                    if (_n[OUTNODE]->in_transit()) {itested();
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {itested();
                        _n[OUTNODE]->propagate();
                      }else{itested();
                      }
                    }else{itested();
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
{
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
  _y[0].x = 0.;
//  _y[0].f1 = _n[OUTNODE]->to_analog(m);
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
  // not calling ELEMENT::tr_review();

  q_accept();
  //digital mode queues events explicitly in tr_accept
  //
  //double dt_s = d->_sim->_dtmin;
  //double dt = ExtSigTrCheck(_ext,dt_s,
  //    const_cast<std::vector<DPAIR>*>(&_num_table),d);
  //if (dt < dt_s) {
  //  d->_time_by.min_event(dt + d->_sim->_time0);
 // }

  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: _time_by.reset(); break;
    case moANALOG:  assert(subckt()); _time_by = subckt()->tr_review(); break;
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC_OUT::tr_accept()
{
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  //_n[OUTNODE]->to_logic(m);
  _quality = qGOOD; //_n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = "ok"; // _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
  int lastchangeiter = 17; //_n[OUTNODE]->d_iter();/* iteration # when it changed */

  _n[BEGIN_IN]->to_logic(m);
  if (_n[BEGIN_IN]->quality() < _quality) {
    _quality = _n[BEGIN_IN]->quality();
    _failuremode = _n[BEGIN_IN]->failure_mode();
  }else{
  }
  if (_n[BEGIN_IN]->d_iter() >= lastchangeiter) {
    lastchangeiter = _n[BEGIN_IN]->d_iter();
    _lastchangenode = BEGIN_IN;
  }else{
  }
/* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
   * Otherwise, an input changed.  Need to evaluate.
   * If all quality are good, can evaluate as digital.
   * Otherwise need to evaluate as analog.
   */
  trace3(_failuremode.c_str(), _lastchangenode, lastchangeiter, _quality);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  {
    assert(want_digital());
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
    if (!_sim->_bypass_ok
        || _lastchangenode != OUTNODE
        || _sim->analysis_is_static()
        || _sim->analysis_is_restore()) {
      LOGICVAL future_state = _n[BEGIN_IN]->lv();
      //		         ^^^^^^^^^^
      if ((_n[OUTNODE]->is_unknown()) &&
          (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
        _n[OUTNODE]->force_initial_value(future_state);
        /* This happens when initial DC is digital.
         * Answers could be wrong if order in netlist is reversed 
         */
      }else if (future_state != _n[OUTNODE]->lv()) {
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

          double d_dly = 1e-11; // _sim->_time0 - vvp::SimTimeD;
          double delay = d_dly;
          trace2("DEV_LOGIC_OUT::tr_accept, setting event... ", delay, _sim->_time0);

          _n[OUTNODE]->set_event(delay, future_state);
          _sim->new_event(_n[OUTNODE]->final_time());

          int l=future_state;
          if (l==3) l=1;

          assert(l==0 || l ==1 );
          assert(H);

          __vpiSignal* HS = (__vpiSignal*) H;
          vvp_sub_pointer_t<vvp_net_t> ptr(HS->node,0);
          //    schedule_set_vector(ptr, vvp_vector4_t(1, l) );
          int           i_dly = int(d_dly / pow(10.0,vpip_get_time_precision()));
          vvp_time64_t  dly(i_dly);
          vvp_vector4_t val(1,l);
          schedule_assign_plucked_vector(ptr,dly,val,0,1);

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
                      }else{untested();
                      }
                    }else{
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_IN::tr_regress()
{itested();
  trace0("DEV_LOGIC_IN::tr_regress");
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {itested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{itested();
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:  itested();
                    assert(subckt());
                    subckt()->tr_regress();
                    break;
    case moDIGITAL: itested();
                    if (_n[OUTNODE]->in_transit()) {itested();
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {itested();
                        _n[OUTNODE]->propagate();
                      }else{itested();
                      }
                    }else{itested();
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
                    trace1("ret", (_sim->analysis_is_static() || _sim->analysis_is_restore()));
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
  trace1("DEV_LOGIC_IN::tr_queue_eval", _gatemode);
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

  trace2("DEV_LOGIC_IN::do_tr",_sim->_time0, _gatemode);
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

  q_accept();
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
// stupid hack. doesnt work right
void DEV_LOGIC_IN::qe() 
{
  trace0("DEV_LOGIC_IN::qe");
//  tr_queue_eval();
//  q_eval();
    _sim->_evalq_uc->push_back(this);
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC_IN::tr_accept()
{
  trace0("DEV_LOGIC_IN::tr_accept");
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  {
    _n[OUTNODE]->to_logic(m);
    _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
    _failuremode = _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
    _lastchangenode = OUTNODE;		/* which node changed most recently */
    int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */
    trace0(long_label().c_str());
    trace2(_n[OUTNODE]->failure_mode().c_str(), OUTNODE, _n[OUTNODE]->quality());

    if (lvfromivl == lvRISING || lvfromivl == lvFALLING)
      _lastchangenode=1;


    ///for (uint_t ii = BEGIN_IN;  ii < net_nodes();  ++ii) {
    //  ...
   // }
    /* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
     * Otherwise, an input changed.  Need to evaluate.
     * If all quality are good, can evaluate as digital.
     * Otherwise need to evaluate as analog.
     */
    trace3(_failuremode.c_str(), _lastchangenode, lastchangeiter, _quality);
  }
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
    trace1("DEV_LOGIC_IN, transition?", _lastchangenode);
    if (!_sim->_bypass_ok
        || _lastchangenode != OUTNODE
        || _sim->analysis_is_static()
        || _sim->analysis_is_restore()) {
      trace1("DEV_LOGIC_IN transition",lvfromivl);
      LOGICVAL future_state = lvfromivl;
      //		         ^^^^^^^^^^
      if ((_n[OUTNODE]->is_unknown()) &&
          (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
        _n[OUTNODE]->force_initial_value(future_state);
        /* This happens when initial DC is digital.
         * Answers could be wrong if order in netlist is reversed 
         */
      }else if (future_state != _n[OUTNODE]->lv()) {
        trace2("DEV_LOGIC_IN, new state", future_state, _n[OUTNODE]->lv());
        assert(future_state != lvUNKNOWN);
        switch (future_state) {
          case lvSTABLE0:	/*nothing*/		break;
          case lvRISING:  future_state=lvSTABLE0;	break;
          case lvFALLING: future_state=lvSTABLE1;	break;
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
// ENQUEUE enqueue
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
double DEV_LOGIC_IN::tr_probe_num(const std::string& what)const
{
  return _n[OUTNODE]->tr_probe_num(what);
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
