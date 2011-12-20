// vim:ts=8:sw=2:et:
// logic port devices.
#include "d_ivl_ports.h"
#include "d_subckt.h"
#include "u_xprobe.h"
#include "d_ivl.h"

/*--------------------------------------------------------------------------*/
#define INNODE BEGIN_IN
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int DEV_LOGIC_AD::_count = -1;
int DEV_LOGIC_DA::_count = -1;
/*--------------------------------------------------------------------------*/
namespace logicports {
  static LOGIC_NONE Default_LOGIC(CC_STATIC);
  static DEV_LOGIC_AD p1;
  static DISPATCHER<CARD>::INSTALL
    d1(&device_dispatcher, "port_to_ivl", &p1);

  static DEV_LOGIC_DA p2;
  static DISPATCHER<CARD>::INSTALL
    d2(&device_dispatcher, "port_from_ivl", &p2);
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC_AD::DEV_LOGIC_AD()
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
DEV_LOGIC_AD::DEV_LOGIC_AD(const DEV_LOGIC_AD& p)
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
void DEV_LOGIC_AD::expand()
{
  trace1("DEV_LOGIC_AD::expand", hp(common()));
  ELEMENT::expand();
  trace1("DEV_LOGIC_AD::expand 2", hp(common()));
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
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
void DEV_LOGIC_AD::tr_iwant_matrix()
{
  if (subckt()) {
    subckt()->tr_iwant_matrix();
  }else{
  }
  tr_iwant_matrix_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_AD::tr_begin()
{
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  ELEMENT::tr_begin();
  trace5("DEV_LOGIC_AD::tr_begin", lvtoivl, _n[INNODE]->lv(), _n[INNODE]->v0(),
                                            _n[OUTNODE]->lv(), _n[OUTNODE]->v0());
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
void DEV_LOGIC_AD::tr_restore()
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
void DEV_LOGIC_AD::dc_advance()
{
  trace0("DEV_LOGIC_AD::dc_advance");
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
                    if ( _n[OUTNODE]->in_transit()) {
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
void DEV_LOGIC_AD::tr_advance()
{
  trace0("DEV_LOGIC::tr_advance");
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
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_AD::tr_regress()
{untested();
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{untested();
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:  untested();
                    assert(subckt());
                    subckt()->tr_regress();
                    break;
    case moDIGITAL: untested();
                    if (_n[OUTNODE]->in_transit()) {untested();
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {untested();
                        _n[OUTNODE]->propagate();
                      }else{untested();
                      }
                    }else{untested();
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC_AD::tr_needs_eval()const
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
void DEV_LOGIC_AD::tr_queue_eval()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:	  unreachable(); break;
    case moDIGITAL: ELEMENT::tr_queue_eval(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_AD::tr_eval_digital()
{
  trace0("DEV_LOGIC_AD::tr_eval_digital");
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

  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  _y[0].x = 0.;
  _y[0].f1 = _n[OUTNODE]->to_analog(m);
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
bool DEV_LOGIC_AD::do_tr()
{  
  trace0("DEV_LOGIC_AD::do_tr");
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
void DEV_LOGIC_AD::tt_advance()
{
  assert(subckt());
  subckt()->tt_advance();
  ELEMENT::tt_advance();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_AD::tr_load()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: tr_load_passive(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_LOGIC_AD::tr_review()
{
  //propagate change of  input state here?
//  q_accept();

  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL:
                    _time_by.reset(); 
                    tr_review_digital();
                    break;
    case moANALOG:  assert(subckt()); _time_by = subckt()->tr_review(); break;
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
// tell icarus about state change, before we accept
void DEV_LOGIC_AD::tr_review_digital(){ // to ivl
  trace1("DEV_LOGIC_AD::tr_review_digital()", _sim->_time0);
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  const COMPONENT* o = prechecked_cast<const COMPONENT*>(owner());
  const COMMON_IVL* oc = prechecked_cast<const COMMON_IVL*>(o->common());
  assert(oc);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  //_n[OUTNODE]->to_logic(m);

  _n[OUTNODE]->to_logic(m);
  _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = "ok"; // _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
  int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */

  // LOOP
  _n[INNODE]->to_logic(m);
  trace3("DEV_LOGIC_AD::tr_review_digital, latching innode", _n[INNODE]->v0(),
      _n[INNODE]->lv(), _n[INNODE]->quality());
  if (_n[INNODE]->quality() < _quality) {
    _quality = _n[INNODE]->quality();
    _failuremode = _n[INNODE]->failure_mode();
  }else{
  }
  if (_n[INNODE]->d_iter() >= lastchangeiter) {
    trace2("DEV_LOGIC_AD::tr_accept inchange", _sim->_time0, _n[INNODE]->d_iter());
    lastchangeiter = _n[INNODE]->d_iter();
    _lastchangenode = INNODE;
  }else{
  }

  trace7("DEV_LOGIC_AD::tr_review_digital", _sim->_time0,
      _n[INNODE]->quality(), 
      _n[OUTNODE]->quality(), 
      _n[INNODE]->v0(),
      _n[INNODE]->lv(),
      _n[OUTNODE]->v0(),
      _n[OUTNODE]->lv()
      );
/* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
   * Otherwise, an input changed.  Need to evaluate.
   * If all quality are good, can evaluate as digital.
   * Otherwise need to evaluate as analog.
   */
  trace3("DEV_LOGIC_AD::tr_accept " + _failuremode, _lastchangenode, lastchangeiter, _quality);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  assert(want_digital());
  assert(_gatemode == moDIGITAL);

  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (!_sim->_bypass_ok
      || _lastchangenode != OUTNODE
      || _sim->analysis_is_static()
      || _sim->analysis_is_restore()) {
    LOGICVAL future_state = _n[INNODE]->lv();
    //		         ^^^^^^^^^^
    if ((_n[OUTNODE]->is_unknown() 
        ) &&
        (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
      trace1("DEV_LOGIC_AD::tr_accept, Forcing", future_state);

      assert(_sim->_time0 == 0); // maybe resume as well?


      switch (future_state) {
        case lvSTABLE0:	/*nothing*/		break;
        case lvRISING:  future_state=lvSTABLE1;	break;
        case lvFALLING: future_state=lvSTABLE0;	break;
        case lvSTABLE1:	/*nothing*/		break;
        case lvUNKNOWN: unreachable();		break;
      }

      _n[OUTNODE]->force_initial_value(future_state);
      //time0==0

      int fut = future_state;
      if (fut==3) fut=1;

      //prop immediately, its just a shadow value.
      untested();
      vvp_time64_t dly = 0;
      vvp_vector4_t val(1, fut);
      trace4("DEV_LOGIC_AD, plucking static EVAL_IVL...", _sim->_time0, 0, dly, fut);
      c->schedule_transition(H,dly,val,0,1);
      trace_queue(c->schedule_list());

      /* This happens when initial DC is digital.
       * Answers could be wrong if order in netlist is reversed 
       */
    }
                //else removed!
    if (future_state != _n[OUTNODE]->lv()) {
      trace0("DEV_LOGIC_AD::tr_accept ...");
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

        trace5("DEV_LOGIC_AD::tr_accept, setting event... ", 
            _sim->_time0, future_state, _n[OUTNODE]->lv(), c->digital_time(), c->SimTimeD);
        _n[OUTNODE]->set_event(0, future_state);
        trace0("DEV_LOGIC_AD::tr_accept, prop " );
        _n[OUTNODE]->propagate();
        q_eval();

        _sim->new_event(_n[OUTNODE]->final_time());
        lvtoivl = future_state;

        int l=future_state;
        if (l==3) l=1;

        assert(l==0 || l ==1 );
        assert(H);

        // BUG: sim->time0 is not necessarily the time of this event

        //need floor. event needs to be executed during DEV_LOGIC_VVP::tr_accept
        vvp_time64_t  i_dly = (int)floor(_sim->_time0 /
            c->time_precision()) - c->schedule_time();
        vvp_time64_t  dly(i_dly);
        vvp_vector4_t val(1,l);

        trace4("DEV_LOGIC_AD, scheduling ivl event", _sim->_time0, i_dly, c->schedule_time(), c->time_precision());
        c->schedule_transition(H,dly,val,0,1);

        // CLEAR: what if step is rejected?

        assert (_lastchangenode != OUTNODE);
      }else{
      }
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC_AD::tr_accept() // to ivl
{
  return;

  trace1("DEV_LOGIC_AD::tr_accept()", _sim->_time0);
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
//  const COMPONENT* o = prechecked_cast<const COMPONENT*>(owner());
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  //_n[OUTNODE]->to_logic(m);

  _n[OUTNODE]->to_logic(m);
  _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = "ok"; // _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
  int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */

  // LOOP
  _n[INNODE]->to_logic(m);
  trace1("DEV_LOGIC_AD::tr_accept, latching innode", _n[INNODE]->lv());
  if (_n[INNODE]->quality() < _quality) {
    _quality = _n[INNODE]->quality();
    _failuremode = _n[INNODE]->failure_mode();
  }else{
  }
  if (_n[INNODE]->d_iter() >= lastchangeiter) {
    trace2("DEV_LOGIC_AD::tr_accept inchange", _sim->_time0, _n[INNODE]->d_iter());
    lastchangeiter = _n[INNODE]->d_iter();
    _lastchangenode = INNODE;
  }else{
  }

  trace7("DEV_LOGIC_AD::tr_accept", _sim->_time0,
      _n[INNODE]->quality(), 
      _n[OUTNODE]->quality(), 
      _n[INNODE]->v0(),
      _n[INNODE]->lv(),
      _n[OUTNODE]->v0(),
      _n[OUTNODE]->lv()
      );
/* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
   * Otherwise, an input changed.  Need to evaluate.
   * If all quality are good, can evaluate as digital.
   * Otherwise need to evaluate as analog.
   */
  trace3("DEV_LOGIC_AD::tr_accept " + _failuremode, _lastchangenode, lastchangeiter, _quality);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  assert(want_digital());
  assert(_gatemode == moDIGITAL);

  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (!_sim->_bypass_ok
      || _lastchangenode != OUTNODE
      || _sim->analysis_is_static()
      || _sim->analysis_is_restore()) {
    LOGICVAL future_state = _n[INNODE]->lv();
    //		         ^^^^^^^^^^
    if ((_n[OUTNODE]->is_unknown() 
        ) &&
        (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
      trace1("DEV_LOGIC_AD::tr_accept, Forcing", future_state);


      switch (future_state) {
        case lvSTABLE0:	/*nothing*/		break;
        case lvRISING:  future_state=lvSTABLE1;	break;
        case lvFALLING: future_state=lvSTABLE0;	break;
        case lvSTABLE1:	/*nothing*/		break;
        case lvUNKNOWN: unreachable();		break;
      }

      _n[OUTNODE]->force_initial_value(future_state);
      //time0==0

      //prop immediately, its just a shadow value.
//      __vpiSignal* HS = (__vpiSignal*) H;
 //     vvp_sub_pointer_t<vvp_net_t> ptr(HS->node,0);
      int l = future_state;
      if (l==3) l=1;

      //need floor. event needs to be executed during DEV_LOGIC_VVP::tr_accept
      vvp_time64_t  dly= 0; // BUG??
      vvp_vector4_t val(1,l);
      trace4("DEV_LOGIC_AD, plucking ivl", _sim->_time0, 0, dly, l);
      c->schedule_transition(H,dly,val,0,1);

      /* This happens when initial DC is digital.
       * Answers could be wrong if order in netlist is reversed 
       */
    }
                //else removed!
    if (future_state != _n[OUTNODE]->lv()) {
      trace0("DEV_LOGIC_AD::tr_accept ...");
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

        trace5("DEV_LOGIC_AD::tr_accept, setting event... ", 
            _sim->_time0, future_state, _n[OUTNODE]->lv(), c->digital_time(), c->SimTimeD);
        _n[OUTNODE]->set_event(0, future_state);
        trace0("DEV_LOGIC_AD::tr_accept, prop " );
        _n[OUTNODE]->propagate();
        q_eval();

        _sim->new_event(_n[OUTNODE]->final_time());
        lvtoivl = future_state;

        int l=future_state;
        if (l==3) l=1;

        assert(l==0 || l ==1 );
        assert(H);


        //need floor. event needs to be executed during DEV_LOGIC_VVP::tr_accept
        vvp_time64_t  i_dly = (int)floor(_sim->_time0 /
            c->time_precision()) - c->schedule_time();
        vvp_time64_t  dly(i_dly);
        vvp_vector4_t val(1,l);
        trace2("DEV_LOGIC_AD, scheduling ivl event", _sim->_time0, i_dly);
        c->schedule_transition(H,dly,val,0,1);

        assert (_lastchangenode != OUTNODE);
      }else{
      }
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_AD::tr_unload()
{
  if (subckt()) {
    subckt()->tr_unload();
  }else{untested();
  }
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_AD::ac_iwant_matrix()
{
  if (subckt()) {
    subckt()->ac_iwant_matrix();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_AD::ac_begin()
{untested();
  if (subckt()) {untested();
    subckt()->ac_begin();
  }else{untested();
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC_AD::tr_probe_num(const std::string& what)const
{
  if (Umatch(what, "ivl ")) { 
    return lvtoivl;
  } 
  return _n[OUTNODE]->tr_probe_num(what);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC_AD::ac_probe_ext(const std::string& what)const
{untested();
  return _n[OUTNODE]->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_AD::want_analog()const
{
  untested();
  return false;
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_AD::want_digital()const
{
  return !subckt() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_LOGIC_DA::DEV_LOGIC_DA()
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
DEV_LOGIC_DA::DEV_LOGIC_DA(const DEV_LOGIC_DA& p)
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
void DEV_LOGIC_DA::expand()
{
  trace1("DEV_LOGIC_DA::expand", hp(common()));
  ELEMENT::expand();
  trace1("DEV_LOGIC_DA::expand 2 ", hp(common()));
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);

  attach_model();
  assert(c->model());

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
          long_label() + "DEV_LOGIC_DA::expand: " + subckt_name + " is not a subckt, forcing digital\n");
    }else{
      _gatemode = OPT::mode;    
      renew_subckt(model, this, scope(), NULL/*&(c->_params)*/);    
      subckt()->expand();
    }
  }catch (Exception_Cant_Find&) {
    error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)), 
        long_label() + ": can't find subckt: " + subckt_name + ", forcing digital\n");
  }

  trace2("DEV_LOGIC_DA::expand " + long_label(),_gatemode, hp(c));

  assert(!is_constant()); /* is a BUG */
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tr_iwant_matrix()
{
  if (subckt()) {
    subckt()->tr_iwant_matrix();
  }else{
  }
  tr_iwant_matrix_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tr_begin()
{
  trace0("DEV_LOGIC_DA::tr_begin");
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
  q_eval();

}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tr_restore()
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
void DEV_LOGIC_DA::dc_advance()
{
  ELEMENT::dc_advance();
  trace5("DEV_LOGIC_DA::dc_advance", 
      lvfromivl,
      _n[OUTNODE]->is_digital(),
      _n[INNODE]->v0(),
      _gatemode,
      _oldgatemode );

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
void DEV_LOGIC_DA::tr_advance()
{
  trace0("DEV_LOGIC_DA::tr_advance");
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
                    //if (_n[OUTNODE]->in_transit()) {
                    if (_n[OUTNODE]->in_transit() || _n[OUTNODE]->final_time_a()  < NEVER ) {
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
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tr_regress()
{untested();
  trace0("DEV_LOGIC_DA::tr_regress");
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{untested();
  }
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moANALOG:  untested();
                    assert(subckt());
                    subckt()->tr_regress();
                    break;
    case moDIGITAL: untested();
                    if (_n[OUTNODE]->in_transit()) {untested();
                      q_eval();
                      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {untested();
                        _n[OUTNODE]->propagate();
                      }else{untested();
                      }
                    }else{untested();
                    }
                    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC_DA::tr_needs_eval()const
{
  trace1("DEV_LOGIC_DA::tr_needs_eval",_gatemode);
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
void DEV_LOGIC_DA::tr_queue_eval()
{
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED: unreachable(); break;
    case moDIGITAL: ELEMENT::tr_queue_eval(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_DA::tr_eval_digital()
{
  trace0("DEV_LOGIC_DA::tr_eval_digital");
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

  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  const COMPONENT* o = prechecked_cast<const COMPONENT*>(owner());
  const COMMON_IVL* oc = prechecked_cast<const COMMON_IVL*>(o->common());
  assert(oc);
  const MODEL_LOGIC* om = prechecked_cast<const MODEL_LOGIC*>(oc->model());
  assert(om);
  _y[0].x = 0.;
  _y[0].f1 = _n[OUTNODE]->to_analog(om);
  trace1("DEV_LOGIC_DA::tr_eval_digital", _y[0].f1);
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
bool DEV_LOGIC_DA::do_tr()
{  
 //bm tr_eval
 //  ExtSigTrEval(_ext,const_cast<std::vector<DPAIR>*>(&_num_table),d);

  trace2("DEV_LOGIC_DA::do_tr " + long_label(),_sim->_time0, _gatemode);
  assert(_gatemode=moDIGITAL);

  //  case moDIGITAL:
  set_converged(tr_eval_digital());
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tt_advance()
{
  assert(subckt());
  subckt()->tt_advance();
  ELEMENT::tt_advance();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tr_load()
{

  return tr_load_passive();

// d_logic
  switch (_gatemode) {
    case moUNKNOWN: unreachable(); break;
    case moMIXED:   unreachable(); break;
    case moDIGITAL: tr_load_passive(); break;
    case moANALOG:  assert(subckt()); subckt()->tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_LOGIC_DA::tr_review()
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

  // q_accept();
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
// stupid hack. doesnt do the right thing.
// need: mark_for_q_eval (or something like that.)
void DEV_LOGIC_DA::qe() 
{
  trace1("DEV_LOGIC_DA::qe",_n[OUTNODE]->is_digital());
  //  tr_queue_eval();
  //  q_eval();
  _sim->_evalq_uc->push_back(owner());
}
/*--------------------------------------------------------------------------*/
  // enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC_DA::tr_accept()
{
  trace4("DEV_LOGIC_DA::tr_accept() " +long_label(),
      _gatemode,  _n[OUTNODE]->is_digital(),_sim->_time0,lvfromivl);
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  //   not necessary probably...
//   _n[OUTNODE]->to_logic(m); 
  _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
  _failuremode = _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
  _lastchangenode = OUTNODE;		/* which node changed most recently */
//  int lastchangeiter = _n[OUTNODE]->d_iter();/* iteration # when it changed */
  trace0(long_label().c_str());
  trace2(_n[OUTNODE]->failure_mode().c_str(), OUTNODE, _n[OUTNODE]->quality());

  if (lvfromivl == lvRISING || lvfromivl == lvFALLING)
  {
    trace1("DEV_LOGIC_DA::tr_accept", lvfromivl);
    _lastchangenode = INNODE;
  }

  trace2(_failuremode.c_str(), _lastchangenode, _quality);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  { // always mixed.
    assert(want_digital());
    trace0("DEV_LOGIC_DA want_digital");
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
    trace2("DEV_LOGIC_DA, transition?", _lastchangenode, _sim->_time0);
    if (!_sim->_bypass_ok
        || _lastchangenode != OUTNODE
        || _sim->analysis_is_static()
        || _sim->analysis_is_restore()) {
      trace1("DEV_LOGIC_DA transition",lvfromivl);
      LOGICVAL future_state = lvfromivl;

// enum smode_t   {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};
// enum _LOGICVAL {lvSTABLE0,lvRISING,lvFALLING,lvSTABLE1,lvUNKNOWN};

      if ((_n[OUTNODE]->is_unknown()) &&
          (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
        trace3("DEV_LOGIC_DA force",future_state, _sim->_time0, _gatemode);
        _n[OUTNODE]->force_initial_value(future_state);
        /* This happens when initial DC is digital.
         * Answers could be wrong if order in netlist is reversed 
         */
      }else if (future_state != _n[OUTNODE]->lv()) {
        trace4("DEV_LOGIC_DA, new state", future_state, _n[OUTNODE]->lv(),
            _sim->_time0, future_state.lv_future());
        assert(future_state != lvUNKNOWN);
        switch (future_state) {
          case lvSTABLE0:	/*nothing*/		break;
          case lvRISING:  future_state=lvSTABLE1;	break;
          case lvFALLING: future_state=lvSTABLE0;	break;
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
        trace3("DEV_LOGIC_DA debugging",  future_state.lv_future() ,
            _n[OUTNODE]->lv_future(),_n[OUTNODE]->lv() );
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
          trace1("DEV_LOGIC_DA new event", _n[OUTNODE]->final_time());
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
unsigned DEV_LOGIC_DA::edge(int in, unsigned delay)
{
  const EVAL_IVL* c = prechecked_cast<const EVAL_IVL*>(common());
  assert(c);
  const COMPONENT* o = prechecked_cast<const COMPONENT*>(owner());
  const COMMON_IVL* oc = prechecked_cast<const COMMON_IVL*>(o->common());
  assert(oc);
  const MODEL_LOGIC* om = prechecked_cast<const MODEL_LOGIC*>(oc->model());
  assert(om);

  LOGICVAL future_state;
  double ev = double (delay+c->schedule_time()) * c->prec();
  double now=CKT_BASE::_sim->_time0;

  if (_n[OUTNODE]->in_transit() ){
    double ft = (_n[OUTNODE]->final_time()); 
    trace2("DEV_LOGIC_DA::edge in transit....",  c->schedule_time(), ft );

    if (ev>ft+om->margin){
      unsigned tryagain = 1+ static_cast<unsigned>((ft+om->margin-now)/c->prec());
      trace1("DEV_LOGIC_DA::edge again", tryagain);
      return tryagain;
    } else untested();
  } else {
    trace3("DEV_LOGIC_DA::edge....", _n[OUTNODE]->final_time(), CKT_BASE::_sim->_time0, ev );
  }

  _lastchangeiter = _sim->iteration_tag();
  _lastchangenode = INNODE;
  switch (in) {
    case 0: lvfromivl = lvFALLING;
            future_state = lvSTABLE0;
            break;
    case 1: lvfromivl = lvRISING;
            future_state = lvSTABLE1;
            break;
    case 3: lvfromivl = lvUNKNOWN;
            future_state = lvUNKNOWN;
            break;
    default: 
             error(bDANGER, "got bogus value %i from ivl\n", in);
             break;
  }

  CKT_BASE::_sim->new_event( ev );
  _n[OUTNODE]->set_event_abs(ev, future_state);
  qe();
  trace4("DEV_LOGIC_DA::edge ", in, delay, ev, future_state);
  return 0;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::tr_unload()
{
  if (subckt()) {
    subckt()->tr_unload();
  }else{untested();
  }
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::ac_iwant_matrix()
{
  if (subckt()) {
    subckt()->ac_iwant_matrix();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC_DA::ac_begin()
{untested();
  if (subckt()) {untested();
    subckt()->ac_begin();
  }else{untested();
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC_DA::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "ivl ")) { 
    return lvfromivl;
  } else if (Umatch(x, "last ")) { 
    return _lastchangeiter;
  }
  return _n[OUTNODE]->tr_probe_num(x);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC_DA::ac_probe_ext(const std::string& what)const
{untested();
  return _n[OUTNODE]->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_DA::want_analog()const { return false; }
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC_DA::want_digital()const
{
  return !subckt() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
}
/*--------------------------------------------------------------------------*/

//unneeded. remove
struct opcode_table_s {                                                                                                                                                                        
  const char*mnemonic;                                                                                                                                                                     
  vvp_code_fun opcode;                                                                                                                                                                     

  unsigned argc;                                                                                                                                                                           
  unsigned  argt[3];                                                                                                                                                        
};        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ivl related.
// maybe this should go to a seperate file.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
extern bool of_NOTIFY(vthread_t thr, vvp_code_t code);
extern bool of_NOTIFY_I(vthread_t thr, vvp_code_t code);
extern bool of_LRI(vthread_t thr, vvp_code_t code);
extern bool of_LNI(vthread_t thr, vvp_code_t code);
extern bool of_LNI_pad(vthread_t thr, vvp_code_t code);
/*--------------------------------------------------------------------------*/
// too complicated. opa not needed...
//void COMPILE_WRAP::notify ( comp_operands_t opa, COMPONENT* daport)
void COMPILE_WRAP::notify( uint32_t delay, uint32_t bit, COMPONENT* daport)
{
  assert(daport);
  vvp_code_t cod = codespace_allocate();
  cod->opcode = of_NOTIFY;
  // pointer hack,
  cod->net =  (vvp_net_t*)  daport; 
  cod->bit_idx[0] = delay;
  cod->bit_idx[1] = bit;

  trace2( "COMPILE_WRAP::notify ",  cod->bit_idx[0], cod->bit_idx[1] );
}  
/*--------------------------------------------------------------------------*/
void COMPILE_WRAP::notify_i( uint32_t delay, uint32_t bit, COMPONENT* daport)
{
  assert(daport);
  vvp_code_t cod = codespace_allocate();
  cod->opcode = of_NOTIFY_I;
  // pointer hack,
  cod->net =  (vvp_net_t*)  daport; 
  cod->bit_idx[0] = delay;
  cod->bit_idx[1] = bit;
  trace2( "COMPILE_WRAP::notify ",  cod->bit_idx[0], cod->bit_idx[1] );
}  
/* --------------------------------- */
void COMPILE_WRAP::load_number_parameter( const int32_t* d, unsigned reg, unsigned wid )
{
  trace2("loading immediately, ", hp(d), *d);
  vvp_code_t cod = codespace_allocate();
  if (wid==64){
    cod->opcode = of_LNI_pad;
  }else{
    cod->opcode = of_LNI;
  }
  cod->ip = (const int64_t*) d;
  
  cod->bit_idx[0] = static_cast<uint32_t>(reg);
}
/* --------------------------------- */
void COMPILE_WRAP::load_number_parameter( const int64_t* , unsigned  )
{
  assert(false);
}
/* --------------------------------- */
void COMPILE_WRAP::load_real_parameter( const double* d, unsigned reg )
{
  trace2("loading immediately, ", hp(d), *d);
  vvp_code_t cod = codespace_allocate();
  cod->opcode = of_LRI;
  cod->dp = d;
  cod->bit_idx[0] = static_cast<uint32_t>(reg);
}
/* --------------------------------- */
// vthread.cc
struct vthread_s {
  vvp_code_t pc;
  vvp_vector4_t bits4;
  union {                                                                                                    
    int64_t  w_int;                                                                                      
    uint64_t w_uint;                                                                                     
    double   w_real;                                                                                     
  } words[16];
  // truncated
  // ... 
  // ...
};
/* --------------------------------- 
struct event_s {
      struct event_s*next;
      virtual ~event_s() { }
      virtual void run_run(void) =0;

	// Write something about the event to stderr
      virtual void single_step_display(void);

	// Fallback new/delete
      static void*operator new (size_t size) { return ::new char[size]; }
      static void operator delete(void*ptr)  { ::delete[]( (char*)ptr ); }
};
--------------------------------- */
struct notify_event_s : public event_s {
  vthread_t thr;
  vvp_code_t cp;
  void run_run(void);
  void single_step_display(void){}

//  static void* operator new(size_t);
//  static void operator delete(void*);
};
struct notify_i_event_s : public event_s {
  vthread_t thr;
  vvp_code_t cp;
  void run_run(void);
  void single_step_display(void){}

//  static void* operator new(size_t);
//  static void operator delete(void*);
};

//static const size_t NOTIFY_CHUNK_COUNT = 8192 / sizeof(struct notify_event_s);

#define SEQ_ACTIVE 1

/*--------------------------------- */
bool of_NOTIFY_I(vthread_t thr, vvp_code_t cp)
{
  unsigned delay = cp->bit_idx[0];
  unsigned bit = cp->bit_idx[1];

  DEV_LOGIC_DA* daport = (DEV_LOGIC_DA*) cp->net;
  assert(daport);
  unsigned val = thr->bits4.value(bit);
  trace5("NOTIFY_I "+ daport->long_label(), bit, delay, val, CKT_BASE::_sim->_time0, hp(daport));

  unsigned try_again_in = daport->edge(val, delay );

//  maybe a transition is not possible,
//  then we need to do something smarter.
  if ( try_again_in != 0 ){
    trace2("of_NOTIFY requeue...", try_again_in, CKT_BASE::_sim->_time0);
    notify_i_event_s* E = new notify_i_event_s();
    E->thr = thr;
    E->cp = new vvp_code_s(*cp);
    E->cp->bit_idx[0]-=try_again_in;
    ex_schedule_event_(E, try_again_in, SEQ_ACTIVE);
  }

  return true;
}
/*--------------------------------- */
bool of_NOTIFY(vthread_t thr, vvp_code_t cp)
{
  unsigned delay_index = cp->bit_idx[0];
  unsigned delay = thr->bits4.value(delay_index) ;
  unsigned bit = cp->bit_idx[1];

  DEV_LOGIC_DA* daport = (DEV_LOGIC_DA*) cp->net;
  assert(daport);
  unsigned val = thr->bits4.value(bit);
  trace5("NOTIFY "+ daport->long_label(), bit, delay, val, CKT_BASE::_sim->_time0, hp(daport));

  unsigned try_again_in = daport->edge(val, delay );

//  maybe a transition is not possible,
//  then we need to do something smarter.
  if ( try_again_in != 0 ){
    trace2("of_NOTIFY requeue...", try_again_in, CKT_BASE::_sim->_time0);
    notify_i_event_s* E = new notify_i_event_s();
    E->thr = thr;
    E->cp = new vvp_code_s(*cp);
    E->cp->bit_idx[0] = delay - try_again_in;
    ex_schedule_event_(E, try_again_in, SEQ_ACTIVE);
  }

  return true;
}
/*------------------*/
bool of_LRI(vthread_t thr, vvp_code_t cp)
{
  const double* d = cp->dp;
  thr->words[cp->bit_idx[0]].w_real = *d;
  return true;
}
/*------------------*/
// ripped from vthread.
static inline void thr_check_addr(struct vthread_s*thr, unsigned addr)
{
        if (thr->bits4.size() <= addr)
                      thr->bits4.resize(addr+1);
}
/*------------------*/
bool of_LNI(vthread_t thr, vvp_code_t cp)
{
  const int64_t* d = cp->ip;
  trace3("LNI", *d, cp->bit_idx[0], cp->bit_idx[1]);

  unsigned dst = cp->bit_idx[0];
  unsigned wid = 32; // fixme bit_idx[1]
  // doesnt work. have no arithmetic registers for ints?
  // thr->words[cp->bit_idx[0]].w_int = *d;
  //
  //
  //taken from MOVI
  thr_check_addr(thr, dst+wid-1);
  static unsigned long val[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  val[0] = *d;
  thr->bits4.setarray(dst, 64, val);
  return true;
}
/*------------------*/
bool of_LNI_pad(vthread_t thr, vvp_code_t cp)
{
  const int32_t* d = (int32_t*) cp->ip;
  trace3("LNI", *d, cp->bit_idx[0], cp->bit_idx[1]);

  unsigned dst = cp->bit_idx[0];
  unsigned wid = 64;

  thr_check_addr(thr, dst+wid-1);
  static unsigned long val[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  val[0] = *d;
  thr->bits4.setarray(dst, 64, val);
  return true;
}
/*------------------*/
/*------------------*/
void notify_event_s::run_run(){
  trace0("running respawned notify");
  of_NOTIFY(thr,cp);
  delete cp;
}
/*------------------*/
void notify_i_event_s::run_run(){
  trace0("running respawned notify");
  of_NOTIFY_I(thr,cp);
  delete cp;
}
