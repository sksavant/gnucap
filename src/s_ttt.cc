/*$Id: s_ttt.cc,v 1.33 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
 * vim:ts=2:sw=2:et:
 * Copyright (C) 2010
 * Author: Felix Salfelder
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
 * Foundation, Inc., 53 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * performs tt analysis
 */
#include "m_phase.h"
#include "declare.h"	/* plclose, plclear, fft */
#include "u_prblst.h"
#include "s_tr.h"
#include "u_nodemap.h"
#include "e_node.h"
#include "u_sim_data.h"
#include "s__.h"
#include "u_status.h"
#include "u_function.h" // after?
#include "m_wave.h"
#include "u_prblst.h"
#include "ap.h"
#include "s_tt.h"



// #include "globals.h" ??
#include "e_adplist.h"
/*--------------------------------------------------------------------------*/
#define sanitycheck()  assert ( 0.99 * _sim->_Time0 <= _Time1 + _sim->_dT0 );\
                       assert ( 1.01 * _sim->_Time0 >= _Time1 + _sim->_dT0 );\
                       assert ( _sim->_Time0 > _Time1 || _sim->tt_iteration_number()==0 );\
                       assert ( _sim->_dT0 == 0 || _sim->tt_iteration_number()!=0 );
/*--------------------------------------------------------------------------*/

namespace TT {
/*--------------------------------------------------------------------------*/

OMSTREAM TTT::mstdout(stdout);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#define check_consistency_tt() {						\
  trace4("", __LINE__, newTime, almost_fixed_Time, fixed_Time);	\
  assert(almost_fixed_Time <= fixed_Time);				\
  assert(newTime <= fixed_Time);					\
  /*assert(newTime == fixed_Time || newTime <= fixed_Time -_dTmin);*/	\
  assert(newTime <= almost_fixed_Time);				\
  /*assert(newTime == almost_fixed_Time || newTime <= almost_fixed_Time - _dTmin);*/ \
  assert(newTime > Time1);						\
  assert(newTime > refTime);						\
  assert(new_dT > 0.);						\
  assert(new_dT >= _dTmin);						\
  assert(newTime <= _Time_by_user_request);				\
  /*assert(newTime == _Time_by_user_request*/				\
  /*	   || newTime < _Time_by_user_request - _dTmin);	*/	\
}
  /*--------------------------------------------------------------------------*/
int TTT::step_tt_cause()const
{
  return ::status.control_tt;
}
/*--------------------------------------------------------------------------*/
double behaviour_time()
{
  return 80000;
}
/*--------------------------------------------------------------------------*/
void TTT::set_step_tt_cause(STEP_CAUSE C)
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
double TTT::get_new_dT()
{
  double factor=  (double) steps_total();
  double buf= 100 / CKT_BASE::tt_behaviour_rel * factor;
  return buf;
}
/*--------------------------------------------------------------------------*/
void TTT::rescale_behaviour()
{
  incomplete();
}
/*--------------------------------------------------------------------------*/
void TTT::first()
{
  trace0("TTT::first()");
  _sim->force_tt_order(0);
  _sim->zero_voltages();

 // ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_clear ); 

  _Time1 = 0;
  _sim->_dT0 = 0;
  _sim->_dT1 = 0;
  _sim->_dT2 = 0;
  _dT_by_adp = _tstop;

  if (_sim->_loadq.size()){
    untested();
    trace0("TTT::first loadq nonempty -- clearing");
    _sim->_loadq.clear() ; // why?
  }

  // tell gnuplot that we start from the beginning.
  _out << "\n";


  assert(_sim->get_tt_order() == 0 );

  assert(_sim->_Time0 <= _Tstart);
  _Time_by_user_request = _sim->_Time0 + _Tstep; /* set next user step */

  if (_Tstop>0) assert(_Tstep>0);

  _sim->_tt_accepted = 0;
  _sim->_tt_rejects = 0;
  _sim->_tt_rejects_total = 0;
// _stepno_tt = 0;

  _sim->_Time0 = _Tstart; //  _Time_by_user_request;
  assert(_sim->_Time0 >= 0 );

  trace0("TTT::first no prepare...");
  // CARD_LIST::card_list.do_forall( &CARD::tt_prepare );
  advance_Time();

  _sim->set_command_tt();

  trace1("TTT::first", _sim->_Time0);

  // if (_trace >= tGUESS) 
  outdata_b4(_sim->_Time0);
  ::status.tran.reset().start();

  CARD_LIST::card_list.tr_begin();
  trace0("TTT::first sweep");
  TTT::sweep();
  trace0("TTT::first sweep done");


  // assert (_sim->_loadq.empty());

  set_step_tt_cause(scUSER);
  ++::status.hidden_steps;
  ::status.review.stop();

  //ADP_LIST::adp_list.do_forall( &ADP::tt_accept_first );
  //CARD_LIST::card_list.do_forall( &CARD::tt_accept ); //?


  _sim->set_command_tt();

  assert(_sim->_Time0 >= 0 );
  assert( _sim->_mode  == s_TTT );
  _accepted_tt = true;

  if (_trace>0 )
    _out << "* accepting first " << _sim->tt_iteration_number() << "\n";


  accept_tt();
  outdata_tt(_sim->_Time0); // first.

} //first
/*--------------------------------------------------------------------------*/
// FIXME: directly run into loop
void TTT::first_after_interruption(){

  _Time1 = _sim->_last_Time;
  _sim->_Time0 = _sim->_last_Time;
  assert(tt_iteration_number() == 0);
  assert(tt_iteration_number() == 0);

  _sim->_dT0 = 0;
  _sim->_dT1 = 0;
  _sim->_dT2 = 0;
  _sim->_time0 = 0;
  _sim->force_tt_order(0); assert(_sim->get_tt_order() == 0 );
  time1 = 0.;
  //    _sim->_dT0=_sim->_last_time;
  advance_Time(); // fix last_iter time (dT0==0);

  if (_trace>0 )
    _out << "* first after int " << _sim->tt_iteration_number() << "\n";

  outdata_b4(_sim->_Time0);
  CARD_LIST::card_list.do_forall( &CARD::tt_next ); // doest things with tr history
  CARD_LIST::card_list.do_forall( &CARD::tt_prepare ); // lasts==0 hack

  for (uint_t ii = 1;  ii <= _sim->_total_nodes;  ++ii) {
    _sim->_nstat[_sim->_nm[ii]].set_last_change_time(0);
    _sim->_nstat[_sim->_nm[ii]].store_old_last_change_time();
    _sim->_nstat[_sim->_nm[ii]].set_final_time(0);
  }

  TTT::sweep();

  set_step_tt_cause(scUSER);
  ++::status.hidden_steps;
  ::status.review.stop();

  _accepted_tt = true;
  accept_tt(); 
  outdata_tt(_sim->_Time0); // first.
  _sim->_last_Time = _sim->_Time0+_tstop;
  trace0("TTT::first_after_interruption done");
}

void TTT::power_down(double time)
{
    if (_trace>0 )
      _out << "* TTT::sweep_tt after int power down\n";

    _sim->zero_some_voltages();
    _sim->_dt0 = time;
    _sim->_dT0 = 0;

    // CARD_LIST::card_list.do_forall( &CARD::stress_apply ); // nicht gut.
    CARD_LIST::card_list.do_forall( &CARD::tt_prepare ); // lasts==0 hack
    print_results_tt( _sim->_Time0 );

    CARD_LIST::card_list.do_forall( &CARD::tr_stress );
    CARD_LIST::card_list.do_forall( &CARD::tr_stress_last );
    CARD_LIST::card_list.do_forall( &CARD::stress_apply );

    print_results_tt( _Tstop );
    _sim->_last_Time = _Tstop;

}
/*--------------------------------------------------------------------------*/
// do a 2nd tt to check aging impact.
// ie. take some norm of node-voltage difference over time.
/*--------------------------------------------------------------------------*/
void TTT::sweep_tt()
{
  assert(_sim->tt_iteration_number() == 0);
  if (_trace>0 )
    _out << "* TTT::sweep_tt from " <<  (double) _Tstart << " to " << (double)_Tstop <<
      " step " << (double)_Tstep <<  "\n";
  if (_trace>5 )
    _out << "* TTT::sweep_tt trstep " <<  (double) _tstep << " trlen " << (double)_tstop 
      <<  "\n";
//  sanitycheck();
  _sim->_tt_iter = 0;
  _sim->_tt_done = 0;


  //end fixme
  head_tt(_tstart, _tstop, "TTime");
  assert( _sim->_mode  == s_TTT );
  assert(_sim->_Time0 >= 0 );

  if (_power_down){
    power_down(  _Tstop - _Tstart  );
    return;
  }else if( _Tstop == _Tstart ){
    trace0("TTT::sweep_tt just printing");
    if (_trace > 0 )
      _out << "* TTT::sweep_tt just printing " << _Tstart << " " << _sim->_Time0 << "\n";

    outdata_b4(_sim->_Time0); // first output tt data
    return;
  }else if ( !_tt_cont  ){
    trace0("TTT::sweep_tt from 0");
    if (_trace>5 )
      _out << "* TTT::sweep_tt from 0\n";
    _sim->_dT0 = 0;
    _sim->_dT1 = 0;
    _sim->_dT2 = 0;
    CARD_LIST::card_list.do_forall( &CARD::tt_begin );

    first();
    trace0("TTT::sweep_tt first done");
  } else { // strange way of doing first next loop
    if (_trace>0 )
      _out << "* TTT::sweep_tt after int" << _sim->tt_iteration_number() << " "<<  _sim->_last_Time << "\n";
    first_after_interruption();
  }

  // assert (_sim->_loadq.empty());
  assert(_sim->_Time0 == _Tstart);

  //_time_by_beh=NEVER;
  //_time_by_adp=NEVER;

  assert (_Time1 == _Tstart);

  while(next())
  {
    if (_trace > 2)
      _out << "* loop start _Time0 = " << _sim->_Time0 << " _Time1 " << _Time1 
        << " step " << _sim->_dT0 << " last " << _Time1 <<"\n";

    trace7( "TTT::sweep_tt loop start ", _sim->_Time0, _Time1, _sim->_dT0,
        _accepted, _accepted_tt, tt_iteration_number(), _sim->_last_Time ); 
    sanitycheck();
    ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_commit );
    CARD_LIST::card_list.do_forall( &CARD::tt_commit ); // ?
    
    trace1( "TTT::sweep adp stuff ", _sim->_Time0 );
    ADP_LIST::adp_list.do_forall( &ADP_CARD::tt_commit );
    
    trace0("TTT::sweep CARD::stress_apply");
    CARD_LIST::card_list.do_forall( &CARD::stress_apply );

    trace2( "TTT::sweep calling TTT::sweep", _cont, _sim->_Time0 );
    store_results_tt(_sim->_Time0); // first output tt data
    // ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tr_reset );
    TTT::sweep();

    if(!_accepted) 
    {
      if ( _trace>0 ) 
        _out << "transient problem " << _sim->_Time0 << _sim->_dT0 <<  _dT_by_adp  << "\n";
      set_step_tt_cause(scREJECT);
      continue;
    }

    _accepted_tt = review_tt();

    if(! _accepted_tt ){
      if ( _trace>0 ) 
        _out << "TTT::sweep_tt (loop) NOT accepted " << _sim->_Time0 << _sim->_dT0 <<  _dT_by_adp << "\n" ;

      // step_cause_tt = trfail;
      _sim->_tt_rejects++;
      _sim->_tt_rejects_total++;
      trace0("TTT::sweep calling tt_next on CL");
      CARD_LIST::card_list.do_forall( &CARD::tt_next );
      if(OPT::printrejected){
        _out <<"*";
        print_stored_results_tt(_sim->_Time0);
        _out <<"*";
        outdata_tt(_sim->_Time0); // first output tt data
      }
      continue;  
    }

    trace3( "TTT::sweep_tt sweep at " , _sim->_Time0, _sim->_dT0, _sim->_dT1 );
    //   ADP_REWIEW
    accept_tt(); 
    assert( _sim->_mode  == s_TTT );
    // 
    // if OPT::printbefore
    //
    //
    if (_trace >= tGUESS) print_stored_results_tt(_sim->_Time0);
    outdata_tt(_sim->_Time0); // first output tt data

    _sim->_last_Time = _sim->_Time0+_tstop;
    trace2("TTT::sweep_tt end loop", _sim->_last_Time, _sim->_Time0 );
  }

  if ( _trace>0 ) 
    _out << "* TTT::sweep_tt =================== endof loop "<<_sim->_last_Time<<"\n";
  _sim->_Time0 = _sim->_last_Time;
  _sim->_dT0=0;

  // advance_Time needed to update last_time
  advance_Time(); 

#ifndef NDEBUG
/// invalidate history
  std::fill_n(_sim->_tr1, _sim->_adp_nodes, NAN);
  std::fill_n(_sim->_tt1, _sim->_adp_nodes, NAN);
#endif
}
/*--------------------------------------------------------------------------*/
void TTT::sweep() // tr sweep wrapper.
{
  if (_trace>0 ) _out<< "* sweep \n";
  // ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tr_reset );
  //
  _sim->_mode=s_TRAN;
  for ( int i = storelist().size(); i --> 0; ) _sim->_waves[i].initialize();
  _sim->_mode=s_TTT;

  trace1("TTT::sweep() ", _inside_tt);

  CKT_BASE::tt_behaviour_rel = 0; // *= new_dT/(time0-time1);
  CKT_BASE::tt_behaviour_del = 0; //  *= new_dT/(time0-time1);
  //  ADP_LIST::adp_list.do_forall( &ADP::tt_commit );
  //
  if (_sim->_Time0 !=0 ){
    CARD_LIST::card_list.tt_behaviour_commit( );
  }
  _sim->_time0 = 0.0;

  // if (_tt_cont) _inside_tt = true;
  try{
    trace3("calling sweep", _sim->_time0, _sim->_last_time, _tstep);
    _inside_tt=true;
    TRANSIENT::sweep();
    assert(_accepted);
    if (_trace>0 ) _out<< "* done sweep "<<tt_iteration_number()<<  "\n";
    trace0("done sweep, tr_stress_last");
    CARD_LIST::card_list.do_forall( &CARD::tr_stress_last );
  }catch (Exception& e) {
    untested();
    error(bDANGER, "Exception %s at %E, dT0 %E, step %i\n",
        e.message().c_str(), _sim->_Time0, _sim->_dT0, tt_iteration_number());
    _out << "* tt sweep failed\n";
    _accepted=_accepted_tt=false;
    ::status.review.stop();
    _sim->invalidate_tt();
    // throw(e); go on with smalller step?
  }

  trace2("TTT done TRANSIENT::sweep", _sim->_last_time, _sim->_Time0);
//  std::cout << "* sweep done. last time: " << _sim->_last_time << " tstop " << _tstop << "\n";

  CKT_BASE::tt_behaviour_rel /= (_sim->_dT0);
  CKT_BASE::tt_behaviour_del /= (_sim->_dT0);
//  std:: << "TTT::sweep_tt brel: " << CKT_BASE::tt_behaviour_rel << "\n";
//  std:: << "TTT::sweep_tt bdel: " << CKT_BASE::tt_behaviour_del << "\n";
//  std:: << "TTT::sweep_tt brel normalized: " << CKT_BASE::tt_behaviour_rel << "\n";

  // hier gibs noch kein behaviour.  
  // ::status.tran.stop();
  _inside_tt=false;
}
/*--------------------------------------------------------------------------*/
void TTT::accept_tt()
{
  if (_trace>0 )
    _out << "* accepting step " <<_sim->tt_iteration_number()
      << " at " <<  _sim->_Time0  << " - " << " _sim->_Time0 + _sim->_last_time " <<"\n";
  //_sim->keep_voltages(); // bloss nicht. wegen last_time
 // CARD_LIST::card_list.do_forall( &CARD::tt_accept ); //?
  ADP_LIST::adp_list.do_forall( &ADP_CARD::tt_accept ); // schiebt tt_valuei weiter.

  //??
  ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_accept ); // schiebt tt_valuei weiter.

//  FIXME:  _tt_acceptq -> corrector?

  while (!_sim->_tt_acceptq.empty()) {
    _sim->_tt_acceptq.back()->tt_accept();
    _sim->_tt_acceptq.pop_back();
  }
  _sim->_tt_accepted++;
  _sim->_last_Time = _sim->_Time0+_tstop;

}
/*--------------------------------------------------------------------------*/
bool TTT::review_tt()
{
  bool tmp=true;
  trace2 ( "TTT::review_tt " , _sim->_Time0, _sim->_dT0 );
  if (_trace>0 )
    _out << "* review_tt " << _sim->_Time0 << "\n";
  sanitycheck();

  assert(_sim->_Time0 >= _sim->_dT0);
  assert(_sim->_Time0 - _sim->_dT0 >=0 );

  //FIXME
//  _dT_by_adp = ADP_LIST::adp_list.tt_review()           + _tstop; 
  _dT_by_adp = (ADP_NODE_LIST::adp_node_list.tt_review())._event; 
  _dT_by_beh = OPT::behreltol/CKT_BASE::tt_behaviour_rel * _sim->_dT0;
  _dT_by_beh = max ( _dT_by_beh, (double) _tstop );


  _time_by_adp = _Time1 + _dT_by_adp;
  _time_by_beh = _Time1 + _dT_by_beh + 1e9;

  trace5("Times ", _Time1, _time_by_adp, _time_by_beh, _dT_by_adp, _dT_by_beh);
  // ? _sim->_Time0 - _sim->_dT0 + _dT_by_beh; // 1e-2/CKT_BASE::tt_behaviour_rel + Time1;
  //
  if ((_dT_by_adp < _tstop )&& (_tstop == _sim->_dT0 )) return true;

  if( _time_by_beh < _sim->_Time0 ){
    _out << "* beh " << _sim->_dT0 << " " << _dT_by_adp << " \n";
    tmp=false;
  } else {
    trace2( "TTT::review_tt: behaviourtime ok: ", _time_by_beh,  _sim->_Time0);
  }
  if( _time_by_adp < _sim->_Time0 ){
    tmp=false;
//    _out << "* tt reject (adp) timestep " << _sim->_dT0 << " want dT " << _dT_by_adp << " \n";
    trace3( "TTT::review_tt: reject adptime", _sim->_Time0, _dT_by_adp , _sim->_dT0 );
  }
  if(_trace>0 )
    _out << "* review_tt done "<< tmp <<" "<< _time_by_adp << " " << _dT_by_adp <<"\n";
  return tmp;
}
/*--------------------------------------------------------------------------*/
// copied from ttf
void TTT::do_it(CS& Cmd, CARD_LIST* Scope)
{
  trace0("TTT::do_it() ");
  _scope = Scope;
  _sim->set_command_tt();

  reset_timers();
  ::status.ttt.reset().start();
  ::status.tt_tries=0;

  try {
    _sim->init();
    _sim->alloc_vectors();
    _sim->_aa.reallocate();
    _sim->_aa.dezero(OPT::gmin);
    _sim->_aa.set_min_pivot(OPT::pivtol);
    _sim->_lu.reallocate();
    _sim->_lu.dezero(OPT::gmin);
    _sim->_lu.set_min_pivot(OPT::pivtol);

    setup(Cmd);
    if(_trace>0 )
      _out << "* done setup\n";
  }catch (Exception& e) {itested();
    error(bDANGER, e.message() + '\n');
    throw(Exception("error TTT::do_it"));
  }

  try {
    allocate();

    ::status.set_up.stop();
    switch (ENV::run_mode) {untested();
      case rPRE_MAIN:	unreachable();		break;
      case rBATCH:	untested();
      case rINTERACTIVE:  itested();
      case rSCRIPT:  sweep_tt();
                     break;
      case rPRESET:	untested(); /*nothing*/ break;
    }
  } catch (Exception& e) {itested();
    error(bDANGER, e.message() + '\n');
    throw(Exception("do_it failed at %E, step %i\n", _sim->_Time0, tt_iteration_number()));
  }
  if(_trace>0 )
    _out << "* unallocating\n";
  unallocate();

  ::status.four.stop();
  ::status.total.stop();
}
/*--------------------------------------------------------------------------*/
/* allocate:  allocate space for tt
*/
void TTT::allocate()
{
  int probes = printlist().size();
  assert(!_tt_store);
  trace1("TTT::allocate allocating probes ", probes);
  _tt_store = new double[probes];
}
/*--------------------------------------------------------------------------*/
/* unallocate:  unallocate space for tt
*/
void TTT::unallocate()
{
  // int probes = printlist().size();
  assert (_tt_store);
  trace2("TTT::unallocate freeing _tt_store", printlist().size(), storelist().size());
  incomplete(); //unallocate at deconstruction...
                // make consecutive sims faster.
  delete[] _tt_store;
  _tt_store = NULL;

  PROBE_LISTS::store[s_TRAN].clear();


  //FIXME: delete waves;

//  if (_fdata_tt) {
//    for (int ii = 0;  ii < printlist().size();  ++ii) {
//      delete [] _fdata_tt[ii];
//    }
//    delete [] _fdata_tt;
//    _fdata_tt = NULL;
//  }else{itested();
//  }
//  TRANSIENT::unallocate?
  _sim->unalloc_vectors();
  _sim->_lu.unallocate();
  _sim->_aa.unallocate();
}
/*--------------------------------------------------------------------------*/
static TTT p10;
DISPATCHER<CMD>::INSTALL d10(&command_dispatcher, "twotimetran", &p10);
/*--------------------------------------------------------------------------*/
//testing=script 2007.11.21
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void TTT::options(CS& Cmd)
{
  trace0(( "TTT::options rest ||| " +Cmd.tail() ).c_str());
  _out = TTT::mstdout;
  _out.reset();
  bool tr=false;

  _sim->_temp_c = OPT::temp_c;
  _trace = tNONE;
  _power_down = false;
  unsigned here = Cmd.cursor();
  do{
    ONE_OF
      || Get(Cmd, "p{owerdown}",   &_power_down)
      || Get(Cmd, "pd",   &_power_down)
      || Get(Cmd, "uic",	   &_sim->_uic)
      || (Cmd.umatch("tr{ace} {=}") &&
	  (ONE_OF
	   || Set(Cmd, "n{one}",      &_trace, tNONE)
	   || Set(Cmd, "o{ff}",       &_trace, tNONE)
	   || Set(Cmd, "w{arnings}",  &_trace, tUNDER)
	   || Set(Cmd, "g{uess}",     &_trace, tGUESS)
	   || Set(Cmd, "a{lltime}",   &_trace, tALLTIME)
	   || Set(Cmd, "r{ejected}",  &_trace, tREJECTED)
	   || Set(Cmd, "i{terations}",&_trace, tITERATION)
	   || Set(Cmd, "v{erbose}",   &_trace, tVERBOSE)
	   || Set(Cmd, "d{ebug}",   &_trace, tDEBUG)
	   || Cmd.warn(bWARNING, "need none, off, warnings, alltime, "
		       "rejected, iterations, verbose")
	   )
	  )
      ;
      if (!( Get(Cmd , "tra{n}",	   &tr) ) )
        _out.outset(Cmd);
  }while (Cmd.more() && !Cmd.stuck(&here) && !tr);

  initio(_out);

  trace1(( "TTT::options rest ||| " +Cmd.tail() ).c_str(), tr);

  trace0("TTT::options tr options");
  TRANSIENT::options(Cmd); // parse options from tran. 
  trace0("TTT::options tr opt done");

  _dtmax_in.e_val(BIGBIG, _scope);
  // _dTmin_in.e_val(OPT::dTmin, _scope);
  _dtratio_in.e_val(OPT::dtratio, _scope);
  _skip_in.e_val(1, _scope);

}
/*--------------------------------------------------------------------------*/
double behaviour_timestep()
{
  return 1000000;
}
/*--------------------------------------------------------------------------*/
bool TTT::next()
{
  double new_dT;
  double new_Time0;

  trace4( "TTT::next() " , tt_iteration_number() ,  _sim->_Time0 , _Time1 ,   _sim->_last_Time);

  assert(_sim->_Time0 >=0 || tt_iteration_number() == 0);

  if( !_accepted_tt ){
    if ( _trace>5 ) _out << "NOT accepted " << _sim->_Time0 << "\n";

    new_dT = 0.9 * ( _sim->_dT0 - _tstop ) + _tstop; // back at least this much.
    if( _time_by_beh < _sim->_Time0 ){
      assert(_time_by_beh >= 0);
      new_dT = fmin(new_dT,_dT_by_beh);
    }
    if ( _time_by_adp < _sim->_Time0 ){
      assert(_time_by_adp >= 0);
      new_dT = fmin(new_dT, _dT_by_adp);
    } 
    if (!_accepted){
      untested();
      new_dT = _sim->_dT0/2.0;
    }

    assert( is_number(new_dT));

    if ( _trace>5 ) 
      _out << "* retry step " << new_dT << " at " << _sim->_Time0 << " ( " <<
        _dT_by_adp << " " << _sim->_dT1  << " " << _sim->_dT0   << " )\n";
   
  } else { // accepted step. calculating new_dT
    // _sim->_last_Time = _sim->_Time0+_sim->_last_time;   

    _Time1 = _sim->_Time0;

    new_dT = min( (double) _dT_by_adp, (_sim->_dT0 + _Tstep)/2 ) ; 
    new_dT = min( (double) new_dT, _sim->_dT0 * OPT::ttstepgrow) ; 
    new_dT = max( new_dT, (double) _tstop ) ; // fmin( get_new_dT(), _Tstep );

    if ( _trace > 0 ) 
      _out << "* new step " << new_dT << " ( just accepted: " << _sim->_dT0   << 
        ", adp "<< _dT_by_adp <<" last_Time" << _sim->_last_Time << " )\n";


    if(new_dT < _dTmin){
      error(bWARNING, "step too small %e %e adp %e\n", new_dT, _dTmin, _dT_by_adp );
      return false;
    }

    if ( _trace > 5 ) 
      _out << "* last step handler? Tstop " << (double)_Tstop << " tstop " << _tstop <<
       " last_Time" << _sim->_last_Time << " )\n";
    // last step handler, snap to edge
    if ( ( (double)_Tstop - _sim->_last_Time < 2* _tstop 
          || new_dT +  _sim->_last_Time > _Tstop )
        && (double)_Tstop - _sim->_last_Time >    _tstop   ) {
      trace4("TTT::next last step", _Tstop, _sim->_last_Time, _tstop,_Tstart);
      if ( _trace > 0 ) 
        _out << "* last step handler..." <<  _sim->_last_Time << "\n";

      untested();
      new_dT=_Tstop - _sim->_Time0 - _tstop ;
    } else {
      // make sure there is enough time for another step!
    }

    assert( new_dT < (OPT::ttstepgrow+1) * _sim->_dT0 || _sim->_dT1<=0 );

    ++::status.hidden_steps_tt;
    ++steps_total_tt;
  } // accepted

  if (_sim->_dT1) new_dT = max (new_dT, (double) _tstop );
  trace1("TTT::next ", new_dT);
  // new_Time0 = _sim->_Time0 + new_dT;
  // ???????????
  new_Time0 = _sim->_last_Time - _tstop + (new_dT ) ;

  bool another_step= ( _Tstop - _sim->_last_Time >= _dTmin ) 
                 && (  new_Time0 <= _Tstop - _tstop   )
                 && ( new_dT != 0 || !_sim->_dT0  );

  if ( _trace>5 ) 
    _out << "* next, next at " << _sim->_Time0+new_dT << 
      "( _dT0 " << _sim->_dT0  << " _dT1 " <<  _sim->_dT1 << " " <<  _sim->_dT2 << " new_dT: " << new_dT<< 
     "another " << another_step <<  "\n";

  if(!another_step) {
    trace6( "TTT::next no next @ Time0: " , _sim->_Time0,  _sim->_dT0, new_dT, _dTmin, _tstop, new_Time0 );
    trace5( "TTT::next no next @ Time0: " ,\
        _Time1, _Tstop,  new_dT >= _dTmin ,  _Tstop - _Time1 >= _dTmin ,  new_Time0 + _tstop <= _Tstop );
//    _sim->_last_Time = _Time1 + _tstop; // FIXME
      
    return (false);
  } else {
    trace5("TTT::next another step ", _sim->_Time0, new_dT, _Time1, _Tstop, _sim->_dT0 );
  }

  // new_dT = max(0.0,new_dT);

  _sim->_Time0 = new_Time0;

  if(_accepted_tt){ // fixme: obenhin, oder?
    _sim->_dT3 = _sim->_dT2;
    _sim->_dT2 = _sim->_dT1;
    _sim->_dT1 = _sim->_dT0;
  }
  _sim->_dT0 = new_dT;
  time1 = 0.;

  assert( _sim->_Time0 < 1+ _Time1 + _sim->_dT0 );
  assert( _sim->_Time0 >= _sim->_dT0 );
  assert( _sim->_Time0 > 0  || tt_iteration_number() == 0);

  _sim->restore_voltages();

  for (uint_t ii = 1;  ii <= _sim->_total_nodes;  ++ii) {
    _sim->_nstat[_sim->_nm[ii]].set_last_change_time(0);
    _sim->_nstat[_sim->_nm[ii]].store_old_last_change_time();
    _sim->_nstat[_sim->_nm[ii]].set_final_time(0);
  }

  advance_Time();

  if ( _trace>0 ) 
    _out << "* committing for " << _sim->_Time0 << "(step "  << tt_iteration_number() <<  ")\n";
  
  if ( _trace>0 ) _out << "* exiting next " << another_step << "\n";
  trace7( "TTT::next: exiting next: " , _sim->tt_iteration_number(),
      _sim->_Time0, _Time1, _sim->_dT0, _sim->_dT1 , _sim->_dT2, _sim->_last_Time );
  return another_step;
}
/*--------------------------------------------------------------------------*/
/* SIM::head: print column headings and draw plot borders
*/
void TTT::head_tt(double start, double stop, const std::string& col1)
{
  trace2("TTT::head_tt", start, stop);
  SIM_MODE oldmode=_sim->_mode;
  _sim->_mode=s_TTT;

  if (!plopen(start, stop, plotlist())) {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);
    _out.form(format, '*', col1.c_str());

    for (PROBELIST::const_iterator
        p=printlist().begin();  p!=printlist().end();  ++p) {
      _out.form(format, ' ', (*p)->label().c_str());
      //  _out.flush();
      // cerr << "S_TTT::head_tt: output label"  << (*p)->label().c_str() << std::endl; 
    }
    _out << '\n';
  }else{
  }
  _sim->_mode=s_TRAN;

  // TODO clear store but append probes from measurments
  PROBELIST oldstore;
  // hack (copy constructor?)
  // becomes obsolete after measurement rework.
  // (just merge measurement_probelist then)
  for (PROBELIST::const_iterator p=storelist().begin();
    p!=storelist().end(); ++p) {
    oldstore.push_probe((*p)->clone());
  }
  PROBE_LISTS::store[s_TRAN].clear();
  //end  hack
  

  print_tr_probe_number = printlist().size();

  for (PROBELIST::const_iterator p=printlist().begin();
    p!=printlist().end();  ++p) {
    PROBE_LISTS::store[s_TRAN].push_probe((*p)->clone());
  }

  for (PROBELIST::const_iterator p=oldstore.begin();
    p!=oldstore.end();  ++p) {
    PROBE_LISTS::store[s_TRAN].merge_probe((*p)->clone());
  }

  // TODO2
  //  for p in TTT-probe
  //    transtore.append(p.needed_probes)
  //

  trace3("TTT::tt_head probe TRAN", printlist().size(), storelist().size(), oldstore.size());
  _sim->_waves = new WAVE[storelist().size()];
  _sim->_mode=oldmode;

}
/*--------------------------------------------------------------------------*/
/* SIM::head: initialize waves
*/
void TTT::head(double /* start */ , double /* stop */, const std::string& )
{


  assert( _sim->_mode==s_TTT );
  {
    trace0("TTT::head tr ttt WAVE");

    if (_sim->_waves_tt) {
      int i = storelist().size();
      while(  i-->0 )
        (_sim->_waves_tt)[i].initialize();
    }else{
      trace0("TTT::head no waves yet");
      _sim->_waves_tt = new WAVE [storelist().size()];
    }
  }
  _sim->_mode=s_TRAN;
  if (_sim->tt_iteration_number() == 0){
    TRANSIENT::_out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    print_head_tr();
  }

  _sim->_mode=s_TTT;
  trace2("TTT::head probe TTT", printlist().size(), storelist().size());
}
/*--------------------------------------------------------------------------*/
/* SIM::print_head: print column headers TR
*/
void TTT::print_head_tr()
{
  assert( _sim->_mode==s_TRAN );
  SIM_MODE oldmode=_sim->_mode;
  _sim->_mode=s_TRAN;

  if( printlist().size() == 0 )
  {
    itested();
    _sim->_mode=oldmode;
    TRANSIENT::_out << "* no trprint\n";
    return;
  }

  std::string foo="* TTime-----";

  TRANSIENT::_out << foo;
  {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);
//    _out.form(format, "", "tiime");
    TRANSIENT::_out << "time      ";
    for (PROBELIST::const_iterator
        p=printlist().begin();  p!=printlist().end();  ++p) {
      TRANSIENT::_out.form(format, ' ', (*p)->label().c_str());
    }
    TRANSIENT::_out << '\n';
  }
  _sim->_mode=oldmode;
}

/*--------------------------------------------------------------------------*/
void TTT::outdata_b4(double time)
{
  assert( _sim->_mode  == s_TTT );
  ::status.output.start();
  print_results_tt( time );
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
void TTT::outdata_tt(double x)
{
  trace0("TTT::outdata_tt()");
  assert( _sim->_mode  == s_TTT );
  ::status.output.start();
  print_results(0); //transient print?
  print_results_tt( x + _tstop );
  _sim->reset_iteration_counter(iPRINTSTEP);
  ::status.hidden_steps = 0;
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/

// print during tr. i.e. print from storelist
void TTT::print_results(double )
{
  trace0("TTT::print_results()");
  SIM_MODE oldmode=_sim->_mode;
  _sim->set_command_tran();
  WAVE* w =NULL;

  if ( printlist().size() == 0 )
  {
    _sim->_mode=oldmode;
    return;
  }

  if (!IO::plotout.any() && _sim->tt_iteration_number() > 0 ) {
    TRANSIENT::_out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    w=&(_sim->_waves[0]);
    print_head_tr();

    int ii=0;
    // FIXME: how many?
    WAVE::const_iterator myiterators[100];

    for (PROBELIST::const_iterator p = printlist().begin();
         p!=printlist().end();  ++p) {
      myiterators[ii] = (_sim->_waves[ii]).begin();
      ii++;
    }
    for (WAVE::const_iterator i = w->begin(); i < w->end(); i++ ) {
      TRANSIENT::_out << _sim->_Time0;
      TRANSIENT::_out << i->first;

      ii=0;
      for (PROBELIST::const_iterator
          p=printlist().begin();  p!=printlist().end();  ++p) {

        TRANSIENT::_out << myiterators[ii]->second;
        myiterators[ii]++;
        ii++;
      }

      TRANSIENT::_out << "\n";
    }
    TRANSIENT::_out << '\n';
  }else{
  }
  _sim->set_command_tt(); // FIXME
  _sim->_mode=oldmode;
}
/*--------------------------------------------------------------------------*/
void TTT::print_stored_results_tt(double x)
{
  trace0("TTT::print_stored_results_tt()");
  if ( printlist().size() ==0)
  {
    untested0( "no ttprint" );
    return;
  }
  _out << "*\n";
  assert( _sim->_mode  == s_TTT );
  if (!IO::plotout.any()) {
    assert(x != NOT_VALID);
    int i;
    _out << x;
    for (i=0; i <  printlist().size(); i++ )
    {
        _out << _tt_store[i];
    }
    _out << "\n";
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void TTT::store_results_tt(double x)
{
 // trace0("TTT::store_results_tt()");
  if ( printlist().size() ==0)
  {
    return;
  }

  int i=0;
  assert( _sim->_mode  == s_TTT );
  if (!IO::plotout.any()) {
    assert(x != NOT_VALID);
    for (PROBELIST::const_iterator
        p=printlist().begin();  p!=printlist().end();  ++p) {
        _tt_store[i++] =  (*p)->value();
    }
  }else{
  }

}
/*--------------------------------------------------------------------------*/
void TTT::print_results_tt(double x)
{
  if ( printlist().size() == 0)
  {
    untested0( "no ttprint" );
    return;
  }

  assert( _sim->_mode  == s_TTT );
  if (!IO::plotout.any()) {
    _out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    assert(x != NOT_VALID);
    _out << x;
    for (PROBELIST::const_iterator
        p=printlist().begin();  p!=printlist().end();  ++p) {
      _out <<  (*p)->value();
    }
    _out << '\n';
  }else{
  }
}
/*--------------------------------------------------------------------------*/
// save things during TR
void TTT::outdata(double x)
{
  trace1("TTT::outdata", _sim->tt_iteration_number());
  assert( _sim->_mode  == s_TTT );
  ::status.output.start();
  assert( _sim->_mode  == s_TTT );

  // SIM::alarm();
  _sim->_mode=s_TRAN;
  if ( OPT::printrejected ) { //FIXME
//    TRANSIENT::print_results(x);
  }

  _sim->set_command_tran();
  if(_sim->tt_iteration_number()==0 && printlist().size() != 0 )
  {
    TRANSIENT::_out << (double)_sim->_Time0;
    TRANSIENT::print_results(x);
  } else {
    // store_results(x);
  }
  _sim->set_command_tt();

  // FIXME (only > 0)
  store_results(x);

  _sim->_mode=s_TTT;
  _sim->reset_iteration_counter(iPRINTSTEP);
  ::status.hidden_steps = 0;
  ::status.output.stop();
  assert( _sim->_mode  == s_TTT );
  trace0("TTT::outdata done");
}
/*--------------------------------------------------------------------------*/
std::string TTT::status()const
{
  return "ttransient timesteps: accepted=" + to_string(steps_accepted())
    + ", rejected=" + to_string(steps_rejected())
    + ", total=" + to_string(steps_total()) + "\n";  
}
/*--------------------------------------------------------------------------*/
/* SIM::store: store data in preparation for post processing
 * use storelist as index for waves
 */
void TTT::store_results(double x)
{
//  trace0("TTT::store_results()");
  _sim->_mode=s_TRAN;
  int ii = 0;
  for (PROBELIST::const_iterator
      p=storelist().begin();  p!=storelist().end();  ++p) {
//    trace1(("TTT::store_results()"+ (*p)->label()).c_str(),  (*p)->value());
    _sim->_waves[ii++].push(x, (*p)->value());
  }
//  trace0("TTT::store_results done");
}
/*--------------------------------------------------------------------------*/
void TTT::advance_Time(void)
{
  CARD_LIST::card_list.do_forall( &CARD::tt_next ); // fixme: merge to tt_adv.
  static double last_iter_time;
  ::status.tt_advance.start();
  _sim->_time0 = 0.;
  if (_sim->_Time0 > 0) {
    if (_sim->_dT0 == 0) {
      if (_trace>5 )
        _out << "* advance_Time zero step to "<< _sim->_Time0 << "\n";

    }else if (_sim->_Time0 > last_iter_time && _accepted_tt ) {	/* moving forward */
      _sim->_tt_iter++;
      _sim->_tt_rejects=0;
      _sim->update_tt_order();

      if ( _trace > 0 )
        _out << "* advance_Time to " << _sim->_Time0 << 
          " iteration number " << _sim->tt_iteration_number() << 
          " have "<<  _sim->_adp_nodes << " nodes " << "\n";

      //trace2("TTT::advance_Time ", _sim->_tr[0], _sim->_tt[0]);
      //trace2("TTT::advance_Time ", _sim->_tr1[0], _sim->_tt1[0]);

      notstd::copy_n(_sim->_tr2, _sim->_adp_nodes, _sim->_tr3);
      notstd::copy_n(_sim->_tr1, _sim->_adp_nodes, _sim->_tr2);
      notstd::copy_n(_sim->_tr, _sim->_adp_nodes, _sim->_tr1);

      notstd::copy_n(_sim->_tt, _sim->_adp_nodes, _sim->_tt1);

      std::fill_n(_sim->_tr, _sim->_adp_nodes, NAN);
      std::fill_n(_sim->_tt, _sim->_adp_nodes, NAN);

      //ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_advance ); // HACK!
      //CARD_LIST::card_list.tt_advance(); //necessary??
      trace2("TTT::advance_Time ", _sim->_tr[0], _sim->_tt[0]);
      trace2("TTT::advance_Time ", _sim->_tr1[0], _sim->_tt1[0]);

      assert(is_number(_sim->_tr1[0]));
      assert(is_number(_sim->_tt1[0]));

    }else{				/* moving backward */
      if (_trace>5 )
        _out << "* advance_Time back " << _sim->_Time0 << "  "<<last_iter_time<<" \n";
      /* don't save voltages.  They're wrong! */
      /* instead, restore a clean start for iteration */
      // notstd::copy_n(_sim->_vt1, _sim->_total_nodes+1, _sim->_v0);
      // CARD_LIST::card_list.tr_regress();
    }
  }else{
    if (_trace>0 ) _out << "* advancing from " << _sim->_Time0 << "\n" ;
    CARD_LIST::card_list.do_forall( &CARD::tt_prepare );
  }
  last_iter_time = _sim->_Time0;
  ::status.tt_advance.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

} // namespace
