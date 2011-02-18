/*$Id: s_tr_swp.cc,v 1.23 2010-09-07 07:46:25 felix Exp $ -*- C++ -*-
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
 * sweep time and simulate.  output results.
 * manage event queue
 */
//testing=script 2007.11.22
#include "u_time_pair.h"
#include "u_sim_data.h"
#include "u_status.h"
#include "e_card.h" // debugging...
#include "declare.h"	/* gen */
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::sweep(void);
//	void	TRANSIENT::first(void);
//	bool	TRANSIENT::next(void);
//	void	TRANSIENT::accept(void);
//	void	TRANSIENT::reject(void);
/*--------------------------------------------------------------------------*/
namespace TR {
  static std::string step_cause[] = {
    "impossible",
    "user requested",
    "event queue",
    "command line \"skip\"",
    "convergence failure, reducing (itl4)",
    "slow convergence, holding (itl3)",
    "truncation error",
    "ambiguous event",
    "limit growth",
    "initial guess"
  };
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::sweep()
{
  trace4("TRANSIENT::sweep()", _tstart, _tstop, _cont, _inside_tt );
  _sim->_phase = p_INIT_DC;
  head(_tstart, _tstop, "Time");
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
 
  if ( _inside_tt ) {
    trace0("TRANSIENT::sweep _inside_tt");
    CARD_LIST::card_list.do_tr();

  } else if ( _cont ) {  // use the data from last time
    _sim->_phase = p_RESTORE;
    _sim->restore_voltages();
    CARD_LIST::card_list.tr_restore();
  } else {
    // std::cout << "\n";
    _sim->clear_limit();
    CARD_LIST::card_list.tr_begin();
  }
  
  first();
  trace0( "TRANSIENT::sweep first done" );
  _sim->_genout = gen();
  
  // assert (_sim->_loadq.empty());
  if (_sim->uic_now() || _inside_tt ) {
    trace2("TRANSIENT::sweep uic_now solve", time1, _sim->_time0);
    advance_time();
    if (!_inside_tt) {
      std::cerr << "TRANSIENT::sweep zeroing voltages\n";
      _sim->zero_voltages(); // ?
    }
    CARD_LIST::card_list.do_tr();    //evaluate_models
    while (!_sim->_late_evalq.empty()) {itested(); //BUG// encapsulation violation
      _sim->_late_evalq.front()->do_tr_last();
      _sim->_late_evalq.pop_front();
    }
    _converged = true;
  }else{
    _converged = solve_with_homotopy(OPT::DCBIAS,_trace);
    if (!_converged) {
      error(bWARNING, "did not converge\n");
    }else{
    }
  }
  review(); 
  _accepted = true;
  accept();
  trace0("TRANSIENT::sweep accepted 1st");

  {
    bool printnow = (_sim->_time0 == _tstart || _trace >= tALLTIME);
    if (printnow) {
      _sim->keep_voltages();
      outdata(_sim->_time0);
      if( _sim->_mode  == s_TTT && OPT::behave ){
        CARD_LIST::card_list.do_forall( &CARD::tr_save_amps, _sim->_stepno );
        trace0("TRANSIENT::saved amps");
        CKT_BASE::tt_behaviour_del +=  CKT_BASE::tr_behaviour_del;
        CKT_BASE::tt_behaviour_rel +=  CKT_BASE::tr_behaviour_rel;
        trace0("TRANSIENT::sweep done things");
      }
      CKT_BASE::tr_behaviour_del = 0;
      CKT_BASE::tr_behaviour_rel = 0;
    }else{untested0("?");
    }
  }
  while (next()) {
    trace2("TRANSIENT::sweep entering loop", _sim->_time0 -_tstop, _sim->_dtmin);
    _sim->_bypass_ok = false;
    _sim->_phase = p_TRAN;
    _sim->_genout = gen();
    _converged = solve(OPT::TRHIGH,_trace);

    _accepted = _converged && review();

    if (_accepted) {
      assert(_converged);
      assert(_sim->_time0 <= _time_by_user_request);
      accept();
      if (step_cause() == scUSER) {
	assert(up_order(_sim->_time0-_sim->_dtmin, _time_by_user_request, _sim->_time0+_sim->_dtmin));
	++_stepno;
	++(_sim->_stepno);
	_time_by_user_request += _tstep;	/* advance user time */
        _time_by_user_request = min(_time_by_user_request, (double)_tstop);
        // std::cerr.precision(19);
        //std::cerr << "TRANSIENT::next set _time_by_user_request: " << _time_by_user_request << " " << _tstop << "\n";
        trace1("TRANSIENT::sweep advance ", _time_by_user_request );
      }else{
      }
      assert(_sim->_time0 <= _time_by_user_request);
    }else{
      reject();
      assert(time1 < _time_by_user_request);
    }
    {
      bool printnow =
	(_trace >= tREJECTED)
	|| (_accepted && ((_trace >= tALLTIME) 
			  || (step_cause() == scUSER && _sim->_time0+_sim->_dtmin > _tstart)));
      if (printnow) {
        _sim->keep_voltages();
        if( _sim->_mode  == s_TTT && OPT::behave )
          CARD_LIST::card_list.do_forall( &CARD::tr_save_amps, (_sim->_stepno) );
        // hack
        //
        CKT_BASE::tt_behaviour_del +=  CKT_BASE::tr_behaviour_del;
        CKT_BASE::tt_behaviour_rel +=  CKT_BASE::tr_behaviour_rel;

        outdata(_sim->_time0);
        CKT_BASE::tr_behaviour_del = 0;
        CKT_BASE::tr_behaviour_rel = 0;

        //SIM_MODE oldmode=_mode;
        //_mode=s_TRAN;
        bool valid = verify(); 
        //_mode=oldmode;

        if( !valid )
        {
          std::cerr << "invalid during tr\n";
          _accepted = false;
          return;
        }
      }else{
      }
    }

    if (!_converged && OPT::quitconvfail) {untested();
      outdata(_sim->_time0);
      throw Exception("convergence failure, giving up");
    }else{
    }
    trace1("endof loop loadq:", _sim->_loadq.size() );

  }//next loop

  trace2("TRANSIENT::sweep done loop ", _sim->_time0, _sim->_last_time);


  trace0("TRANSIENT::sweep BUG ");

  // BUG keep_volteges breaks last_time
  // NOT keep_volteges breaks cap_bug
  // _sim->keep_voltages();


  if (_sim->_loadq.size()){
    untested();
    trace0("loadq nonempty -- clearing");
   // _sim->_loadq.clear() ; why??
  }
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::set_step_cause(STEP_CAUSE C)
{
  switch (C) {
  case scITER_A:untested();
  case scADT:untested();
  case scITER_R:
  case scINITIAL:
  case scSKIP:
  case scTE:
  case scAMBEVENT:
  case scEVENTQ:
  case scUSER:
    ::status.control = C;
    break;
  case scNO_ADVANCE:untested();
  case scZERO:untested();
  case scSMALL:itested();
  case scREJECT:
    ::status.control += C;
    break;
  }
}
/*--------------------------------------------------------------------------*/
int TRANSIENT::step_cause()const
{
  return ::status.control;
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::first()
{
  trace0( "TRANSIENT::first()" );
  /* usually, _sim->_time0, time1 == 0, from setup */
  assert(_sim->_time0 == time1);
  assert(_sim->_time0 <= _tstart);
  ::status.review.start();
  _time_by_user_request = _sim->_time0 + _tstep;	/* set next user step */
  //_eq.Clear();				/* empty the queue */
  while (!_sim->_eq.empty()) {
    _sim->_eq.pop();
  }
  _stepno = 0;
  _sim->_stepno = 0;
  set_step_cause(scUSER);
  ++::status.hidden_steps;
  ::status.review.stop();
}
/*--------------------------------------------------------------------------*/
#define check_consistency() {						\
    trace4("", __LINE__, newtime, almost_fixed_time, fixed_time);	\
    assert(almost_fixed_time <= fixed_time);				\
    assert(newtime <= fixed_time);					\
    /*assert(newtime == fixed_time || newtime <= fixed_time -_sim->_dtmin);*/	\
    assert(newtime <= almost_fixed_time);				\
    /*assert(newtime == almost_fixed_time || newtime <= almost_fixed_time - _sim->_dtmin);*/ \
    assert(newtime > time1);						\
    assert(newtime >= reftime);						\
    assert(new_dt == 0 || new_dt / _sim->_dtmin > 0.999999);	        \
    /*assert(newtime <= _time_by_user_request);*/		        \
    assert(newtime == _time_by_user_request				\
    	   || newtime <= _time_by_user_request + _sim->_dtmin);		\
  }
#define check_consistency2() {						\
    assert(newtime > time1);						\
    assert(new_dt == 0 || new_dt / _sim->_dtmin > 0.999999);	        \
    assert(newtime == _time_by_user_request		          	\
    	   || newtime <= _time_by_user_request + _sim->_dtmin);	        \
  }
/*--------------------------------------------------------------------------*/
/* next: go to next time step
 * Set _sim->_time0 to the next time step, store the old one in time1.
 * Try several methods.  Take the one that gives the shortest step.
 */
bool TRANSIENT::next()
{
  double old_dt = _sim->_time0 - time1;
  trace3("TRANSIENT::next()", time1, _sim->_time0, old_dt);
  ::status.review.start();

  assert(old_dt >= 0);
  
  double newtime = NEVER;
  double new_dt = NEVER;
  STEP_CAUSE new_control = scNO_ADVANCE;

  if (_sim->_time0 == time1) {
    trace0("TRANSIENT::next init");
    // initial step -- could be either t==0 or continue
    // for the first time, just guess
    // make it 100x smaller than expected
    new_dt = std::max(_dtmax/100., _sim->_dtmin);
    newtime = _sim->_time0 + new_dt;
    new_control = scINITIAL;
  }else if (!_converged) {
    trace0("TRANSIENT::next !_converged");
    new_dt = old_dt / OPT::trstepshrink;
    newtime = _time_by_iteration_count = time1 + new_dt;
    new_control = scITER_R;
  }else{
    double reftime;
    if (_accepted) {
      reftime = _sim->_time0;
      trace0("TRANSIENT::next accepted");
    }else{
      reftime = time1;
      trace0("TRANSIENT::next rejected");
    }
    trace2("TRANSIENT::next ", step_cause(), old_dt);
    trace3("TRANSIENT::next ", time1, _sim->_time0, reftime);
    trace1("TRANSIENT::next ", _time_by_user_request);
    //std::cerr.precision(17);
    //std::cerr << "TRANSIENT::next _time_by_user_request: " << _time_by_user_request << "\n";

    newtime = _time_by_user_request;
    trace4("TRANSIENT::next request ", newtime, new_dt, reftime, _sim->_dtmin);
    new_dt = newtime - reftime;
    if(new_dt < _sim->_dtmin){
      new_dt=_sim->_dtmin;
      newtime = reftime + _sim->_dtmin;
    }
    new_control = scUSER;
    double fixed_time = newtime;
    double almost_fixed_time = newtime;

    trace4("TRANSIENT::next ", newtime, new_dt, reftime, _sim->_dtmin);
    check_consistency();
    
    // event queue, events that absolutely will happen
    // exact time.  NOT ok to move or omit, even by _sim->_dtmin
    // some action is associated with it.
    if (!_sim->_eq.empty() && _sim->_eq.top() < newtime) {
      newtime = _sim->_eq.top();
      new_dt = newtime - reftime;
      if (new_dt < _sim->_dtmin) {
	//new_dt = _sim->_dtmin;
	//newtime = reftime + new_dt;
      }else{
      }
      new_control = scEVENTQ;
      fixed_time = newtime;
      almost_fixed_time = newtime;
      check_consistency();
      trace3("TRANSIENT::next top ", newtime, new_dt, _sim->_eq.top());
    }else{
    }

    
    
    // device events that may not happen
    // not sure of exact time.  will be rescheduled if wrong.
    // ok to move by _sim->_dtmin.  time is not that accurate anyway.
    if (_time_by_ambiguous_event < newtime - _sim->_dtmin) {  
      trace3("TRANSIENT::next ambi ", newtime, new_dt, _time_by_ambiguous_event);
      if (_time_by_ambiguous_event < time1 + 2*_sim->_dtmin) {untested();
	double mintime = time1 + 2*_sim->_dtmin;
	if (newtime - _sim->_dtmin < mintime) {untested();
	  newtime = mintime;
	  new_control = scAMBEVENT;
	}else{untested();
	}
      }else{
	newtime = _time_by_ambiguous_event;
	new_control = scAMBEVENT;
      }
      new_dt = newtime - reftime;
      almost_fixed_time = newtime;
      check_consistency();
    }else{
    }
    trace3("TRANSIENT::next after ambi", newtime, new_dt, _time_by_ambiguous_event);
    
    // device error estimates
    if (_time_by_error_estimate < newtime - _sim->_dtmin) {
      newtime = _time_by_error_estimate;
      new_dt = newtime - reftime;
      new_control = scTE;
      check_consistency();
      trace3("TRANSIENT::next err", newtime, new_dt, _time_by_error_estimate);
    }else{
    }
    trace3("TRANSIENT::next ", newtime, new_dt, _time_by_error_estimate);
    
    // skip parameter
    if (new_dt > _dtmax) {
      if (new_dt > _dtmax + _sim->_dtmin) {
	new_control = scSKIP;
      }else{
      }
      new_dt = _dtmax;
      newtime = reftime + new_dt;
      check_consistency();
    }else{
    }

    // converged but with more iterations than we like
    if ((new_dt > (old_dt + _sim->_dtmin) * OPT::trstephold)
	&& _sim->exceeds_iteration_limit(OPT::TRLOW)) {untested();
      assert(_accepted);
      new_dt = old_dt * OPT::trstephold;
      newtime = reftime + new_dt;
      new_control = scITER_A;
      check_consistency();
    }else{
    }

    // limit growth
    if (_sim->analysis_is_tran_dynamic() && new_dt > old_dt * OPT::trstepgrow) {untested();
      new_dt = old_dt * OPT::trstepgrow;
      newtime = reftime + new_dt;
      new_control = scADT;
      check_consistency();
    }else{
    }

    // quantize
    if (newtime < almost_fixed_time) {
      assert(new_dt >= 0);
      if (newtime > reftime + old_dt*.8
	  && newtime < reftime + old_dt*1.5
	  && reftime + old_dt <= almost_fixed_time) {
	// new_dt is close enough to old_dt.
	// use old_dt, to avoid a step change.
	new_dt = old_dt;
	newtime = reftime + new_dt;
	if (newtime > almost_fixed_time) {untested();
	  new_control = scAMBEVENT;
	  newtime = almost_fixed_time;
	  new_dt = newtime - reftime;
	}else{
	}
        trace2("TRANSIENT::next", new_dt, _sim->_dtmin );
	check_consistency();
      }else{
	// There will be a step change.
	// Try to choose one that we will keep for a while.
	// Choose new_dt to be in integer fraction of target_dt.
	double target_dt = fixed_time - reftime;
	assert(target_dt >= new_dt);
	double steps = 1 + floor((target_dt - _sim->_dtmin) / new_dt);
	assert(steps > 0);
	new_dt = target_dt / steps;
        trace3("TRANSIENT::next ", reftime, new_dt, reftime+new_dt);
	newtime = reftime + new_dt;
	check_consistency();
      }
    }else{
      assert(newtime == almost_fixed_time);
    }

    // trap time step too small
    if (!_accepted && new_dt < _sim->_dtmin) {untested();
      new_dt = _sim->_dtmin;
      newtime = reftime + new_dt;
      new_control = scSMALL;
      check_consistency();
      trace1("TRANSIENT::next too small trap", newtime);
    }else{
    }
    trace1("TRANSIENT::next got sth", newtime);

    if ( _time_by_user_request - _sim->_dtmin < newtime  && newtime <  _time_by_user_request ){
       newtime = _time_by_user_request - _sim->_dtmin;
       new_dt = _sim->_dtmin;
       trace2("TRANSIENT::next making step smaller", newtime, new_dt);
    }

    // if all that makes it close to user_requested, make it official
    if (up_order(newtime-_sim->_dtmin, _time_by_user_request, newtime+_sim->_dtmin)) {
      //newtime = _time_by_user_request;
      //new_dt = newtime - reftime;
      new_control = scUSER;
      if (new_dt>0)
      check_consistency();			   
    }else{
    }
    if (new_dt>0){
      check_consistency();
      assert(!_accepted || newtime > _sim->_time0 || time1 <= _time_by_user_request );
      assert( _accepted || newtime <= _sim->_time0);
    }


  }

  
  set_step_cause(new_control);

  //  std::cerr << "TRANSIENT::next time0 " << newtime <<  "\n";
  trace1("TRANSIENT::next got it i think", newtime);
  
  /* check to be sure */
  if (newtime < time1 + _sim->_dtmin) {itested();
    /* It's really bad. */
    /* Reject the most recent step, back up as much as possible, */
    /* and creep along */
    trace3("TRANSIENT::next ", newtime, time1, _sim->_dtmin );
    assert(!_accepted);
    assert(step_cause() < scREJECT);
    assert(step_cause() >= 0);
    error(bDANGER,"non-recoverable: " + TR::step_cause[step_cause()] + "\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e  using=%e\n",
	  newtime, _sim->_time0, time1, time1 + _sim->_dtmin);
    newtime = time1 + _sim->_dtmin;
    set_step_cause(scSMALL);
    //check_consistency2();
    throw Exception(("tried everything, still doesn't work, giving up at " + to_string(time1)).c_str());
    //}else if (newtime <= _sim->_time0 - _sim->_dtmin) {
  }else if (newtime < _sim->_time0) {
    /* Reject the most recent step. */
    /* We have faith that it will work with a smaller time step. */
    assert(!_accepted);
    assert(newtime >= time1 + _sim->_dtmin);
    error(bLOG, "backwards time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n", newtime, _sim->_time0, time1);
    set_step_cause(scREJECT);
    _sim->mark_inc_mode_bad();
    check_consistency2();
  }else if ( _sim->_time0 >= _time_by_user_request ){
    trace3("TRANSIENT::next: already there", _sim->_time0, time1, newtime );
    time1=_sim->_time0;
    new_dt=0;
   
  }else if (newtime < _sim->_time0 + _sim->_dtmin) {untested();
    /* Another evaluation at the same time. */
    /* Keep the most recent step, but creep along. */
    assert(newtime > _sim->_time0 - _sim->_dtmin);
    error(bDANGER, "zero time step\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  time1=%e requested=%e\n",
        newtime, _sim->_time0, time1, _time_by_user_request);
    if (_accepted) {untested();
      time1 = _sim->_time0;
    }else{untested();
      assert(_converged);
    }
    trace3( "TRANSIENT::next:", newtime, time1, new_dt);
    check_consistency2();
    newtime = _sim->_time0 + _sim->_dtmin;
    if (newtime > _time_by_user_request) {
      untested();
      newtime = _time_by_user_request;
      set_step_cause(scUSER);
    }else{untested();
    }
    set_step_cause(scZERO);
    check_consistency2();
  }else{
    assert(_accepted);
    assert(newtime >= _sim->_time0 + _sim->_dtmin);
    /* All is OK.  Moving on. */
    /* Keep value of newtime */
    time1 = _sim->_time0;
    check_consistency2();
  }
  _sim->_time0 = newtime;
  
  /* advance event queue (maybe) */
  /* We already looked at it.  Dump what's on top if we took it. */
  while (!_sim->_eq.empty() && _sim->_eq.top() <= _sim->_time0) {
    trace1("eq", _sim->_eq.top());
    _sim->_eq.pop();
  }
  while (!_sim->_eq.empty() && _sim->_eq.top() < _sim->_time0 + _sim->_dtmin) {itested();
    trace1("eq-extra", _sim->_eq.top());
    _sim->_eq.pop();
  }
  //BUG// what if it is later rejected?  It's lost!

  if (new_dt>0)check_consistency2();
  ++::status.hidden_steps;
  ++steps_total_;
  ::status.review.stop();
  bool ret= _sim->_time0 < _tstop + _sim->_dtmin;
  ret = ret &&  time1 < _time_by_user_request;
  ret = ret &&  newtime <= _time_by_user_request;
 // std::cerr.precision(17);
 // std::cerr << "TRANSIENT::next time0 " << _sim->_time0 << " " << _tstop <<  "\n";
 // std::cerr << "TRANSIENT::next time0 " << _sim->_time0 << " " << _tstop <<  "\n";
  trace4("TRANSIENT::next", _sim->_time0, _tstop, _sim->_dtmin, (double) ret );
  return (ret);
}
/*--------------------------------------------------------------------------*/
bool TRANSIENT::review()
{
  ::status.review.start();
  _sim->count_iterations(iTOTAL);

  TIME_PAIR time_by = CARD_LIST::card_list.tr_review();
  _time_by_error_estimate = time_by._error_estimate;

  // limit minimum time step
  // 2*_sim->_dtmin because _time[1] + _sim->_dtmin might be == _time[0].
  if (time_by._event < time1 + 2*_sim->_dtmin) {
    trace1("TRANSIENT::review event ", time_by._event-time1);
    _time_by_ambiguous_event = time1 + 2*_sim->_dtmin;
  }else{
    _time_by_ambiguous_event = time_by._event;
  }
  // force advance when time too close to previous
  if (std::abs(_time_by_ambiguous_event - _sim->_time0) < 2*_sim->_dtmin) {
    _time_by_ambiguous_event = _sim->_time0 + 2*_sim->_dtmin;
  }else{
  }

  if (time_by._error_estimate < time1 + 2*_sim->_dtmin) {
    _time_by_error_estimate = time1 + 2*_sim->_dtmin;
  }else{
    _time_by_error_estimate = time_by._error_estimate;
  }
  if (std::abs(_time_by_error_estimate - _sim->_time0) < 1.1*_sim->_dtmin) {
    _time_by_error_estimate = _sim->_time0 + 1.1*_sim->_dtmin;
  }else{
  }

  ::status.review.stop();

  return (_time_by_error_estimate > _sim->_time0  &&  _time_by_ambiguous_event > _sim->_time0);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::accept()
{
  trace0("TRANSIENT::accept");
  _sim->_dt0 = _sim->_time0 - time1; // ungefaehrlich
  if(_sim->_dt0 <=0) assert (_sim->_stepno == 0);
  ::status.accept.start();
  _sim->set_limit(); // verstehe/brauch ich nicht.
  if (OPT::traceload) {
    while (!_sim->_acceptq.empty()) {
      _sim->_acceptq.back()->tr_accept();
      _sim->_acceptq.pop_back();
    }
  }else{itested();
    _sim->_acceptq.clear();
    CARD_LIST::card_list.tr_accept();
  }
  ++steps_accepted_;
  if( _sim->analysis_is_tt() || OPT::trage ) {
    // put into tr_accept (good idea?)
//    if ( _sim->_dt0>0) CARD_LIST::card_list.do_forall( &CARD::tr_stress );
    trace0( "TRANSIENT::accept: done stressing cardlist");
    if ( OPT::trage ) {
      untested();
      //incomplete();
      CARD_LIST::card_list.do_forall( &CARD::stress_apply );
//      CARD_LIST::card_list.do_forall( &CARD::tr_reset );
    }
  }
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::reject()
{
  ::status.accept.start();
  _sim->_acceptq.clear();
  ++steps_rejected_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
