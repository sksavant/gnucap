/*$Id: s_ttt.cc,v 1.33 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
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
 *
 *	
 *	do this in a loop:
 *
 * - set T, dT
 *	- tt_begin/tt_continue
 *	-  tt_advance
 *	-  tt_apply_stress
 *	-  TR::sweep (calls tr_accept)
 *	-  tr_stress_last
 *	-  tt_review
 *	- tt_last/tt_apply?
 *
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
#include "e_adp.h"
#include "e_adplist.h"
/*--------------------------------------------------------------------------*/
#define sanitycheck()  assert ( is_almost(  _sim->_Time0 , _Time1 + _sim->_dT0 ));\
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
//	_sim->zero_voltages();

	// ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_clear ); 

	_Time1 = 0;
	_sim->_dT0 = 0;
	_sim->_dT1 = 0;
	_sim->_dT2 = 0;
	_dT_by_adp = _tstop;

	if (_sim->_loadq.size()){
		// untested();
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

	advance_Time();

	_sim->set_command_tt();

	trace1("TTT::first", _sim->_Time0);

	if (TRANSIENT::_trace >= tDEBUG) 
		TRANSIENT::outdata(_sim->_Time0);

	outdata(_sim->_Time0);
	::status.tran.reset().start();

	//CARD_LIST::card_list.tr_begin(); ///????
	trace0("TTT::first sweep");
	TTT::sweep();
	trace0("TTT::first sweep done");
	// assert (_sim->_loadq.empty());

	set_step_tt_cause(scUSER);
	++::status.hidden_steps;
	::status.review.stop();

	_sim->set_command_tt();

	assert(_sim->_Time0 >= 0 );
	assert( _sim->_mode  == s_TTT );
	_accepted_tt = true;

	accept_tt();
	outdata_tt(_sim->_Time0 + _tstop); // first.
	_sim->_last_Time = _sim->_Time0 + _tstop;

} //first
/*--------------------------------------------------------------------------*/
// FIXME: directly run into loop
void TTT::first_after_interruption(){
	
	assert(_cont_tt);
	assert(_cont);
	trace3("TTT::first_after_interruption", _cont_tt, _sim->_tt[0], _sim->_tr[0]);
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
	//    _sim->_dT0 =_sim->last_time();
	advance_Time(); // fix last_iter time (dT0==0);

	// bug. too early
	if(_trace>tDEBUG)
		outdata_b4(_sim->_Time0);

	// start at age stored in tt.
	notstd::copy_n(_sim->_tt, _sim->_adp_nodes, _sim->_tt1);

	CARD_LIST::card_list.tt_advance(); // does things with tr history.

	notstd::copy_n(_sim->_tr, _sim->_adp_nodes, _sim->_tr1);
	//  CARD_LIST::card_list.do_forall( &CARD::tt_prepare ); // lasts==0 hack
	//CARD_LIST::card_list.do_forall( &CARD::tt_prepare ); // lasts==0 hack

	for (uint_t ii = 1;  ii <= _sim->_total_nodes;  ++ii) {
		_sim->_nstat[_sim->_nm[ii]].set_last_change_time(0);
		_sim->_nstat[_sim->_nm[ii]].store_old_last_change_time();
		_sim->_nstat[_sim->_nm[ii]].set_final_time(0);
	}
	_sim->force_tt_order(0);

	outdata_b4(_sim->_Time0);

	CARD_LIST::card_list.stress_apply();

	TTT::sweep();

	set_step_tt_cause(scUSER);
	++::status.hidden_steps;
	::status.review.stop();

	_accepted_tt = true;
	accept_tt(); 
	outdata_tt(_sim->_Time0 + _tstop); // first.
	_sim->_last_Time = _sim->_Time0 + _tstop;
	trace0("TTT::first_after_interruption done");
}
/*--------------------------------------------------------------*/
void TTT::tt_begin() {
		_sim->_dT0 = 0;
		_sim->_dT1 = 0;
		_sim->_dT2 = 0;
	trace0("TTT::tt_begin");
	_sim->_stepno = 0;
	_sim->_tt_uic = false;
	CARD_LIST::card_list.do_forall( &CARD::tt_begin );
	CARD_LIST::card_list.stress_apply(); 
}
/*--------------------------------------------------------------*/
void TTT::tt_cont() {
	// continue from externally set adp_node values...
		_sim->_dT0 = 0;
		_sim->_dT1 = 0;
		_sim->_dT2 = 0;
		_sim->_time0 = 0;
	trace0("TTT::cont");
	_sim->_stepno = 0;
	_sim->_tt_uic = true;
	CARD_LIST::card_list.do_forall( &CARD::tt_begin );
	CARD_LIST::card_list.stress_apply(); 
}
/*--------------------------------------------------------------*/
void TTT::do_initial_dc(){
	// set adp_nodes to initial values
	CARD_LIST::card_list.do_forall( &CARD::tr_begin );
	_sim->_phase = p_INIT_DC;
	bool _converged = solve_with_homotopy(OPT::DCBIAS,TRANSIENT::_trace);
	assert(_converged);

//	 review();
	_accepted = true;
	accept();
	//
	_sim->keep_voltages();
	trace0("TTT::do_initial_dc done");
	_sim->_mode = s_TTT;
}
/*--------------------------------------------------------------*/
void TTT::power_down(double until)
{

	_sim->force_tt_order(0);
	double time = until-_sim->_last_Time;
	trace2("TTT::power_down... ", _sim->_last_Time, until);

	/// only if _cont?
	//
	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace2("TTT::power_down... ",i, _sim->_tt[i]);
	}
	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace2("TTT::power_down... ",i, _sim->_tr[i]);
	}

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

	advance_Time(); // fix last_iter time (dT0==0);

	// bug. too early
	if(_trace>tDEBUG)
		outdata_b4(_sim->_Time0);

	// start at age stored in tt.
	notstd::copy_n(_sim->_tt, _sim->_adp_nodes, _sim->_tt1);

	CARD_LIST::card_list.tt_advance(); // does things with tr history?
	CARD_LIST::card_list.stress_apply();

	for (uint_t ii = 1;  ii <= _sim->_total_nodes;  ++ii) {
		_sim->_nstat[_sim->_nm[ii]].set_last_change_time(0);
		_sim->_nstat[_sim->_nm[ii]].store_old_last_change_time();
		_sim->_nstat[_sim->_nm[ii]].set_final_time(0);
	}
	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace2("TTT::power_down... ",i, _sim->_tr[i]);
	}

	// NOW do the powerdown.
	_sim->_phase = p_PD;

	_sim->zero_some_voltages();

	print_results_tt( _sim->_Time0 );

	trace0("TTT::power_down accept...");
	CARD_LIST::card_list.tr_accept();

	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace2("TTT::power_down... ",i, _sim->_tr[i]);
	}
	trace0("TTT::power_down last...");

	CARD_LIST::card_list.do_forall( &CARD::tr_stress_last );

	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace2("TTT::power_down... ",i, _sim->_tr[i]);
	}

	_sim->_dt0 = 0;
	_sim->_dT0 = time;
	_sim->_Time0 = until;

	advance_Time();
	_sim->force_tt_order(0);

	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace3("TTT::power_down advanced ",i, _sim->_tt[i], hp(_sim->_tt));
	}
	for(unsigned i=0; i<_sim->_adp_nodes;i++ ){
		trace3("TTT::power_down advanced ",i, _sim->_tt1[i], hp(_sim->_tt1));
	}

	trace0("TTT::power_down apply");
	CARD_LIST::card_list.do_forall( &CARD::stress_apply );

	trace0("TTT::power_down done");
	tt_accept();

	// go on with old voltages (good idea?)
	
	_sim->restore_voltages();
	print_results_tt( _Tstop );
	_sim->_last_Time = _Tstop;

}
/*--------------------------------------------------------------------------*/
// do a 2nd tt to check aging impact.
// ie. take some norm of node-voltage difference over time.
/*--------------------------------------------------------------------------*/
void TTT::sweep_tt()
{
	_out << "* sweep_tt cont=" << _cont << " cont_tt=" << _cont_tt << " new=" << _new << "\n";
	trace4("TTT::sweep_tt", _Tstop, _cont, _cont_tt, _new );
	assert(_sim->tt_iteration_number() == 0);

	//  sanitycheck();
	_sim->_tt_iter = 0;
	_sim->_tt_done = 0;

	head_tt(_tstart, _tstop, "TTime");
	assert( _sim->_mode  == s_TTT );
	assert( _sim->_Time0 >= 0 );

	assert(!_new || !_cont_tt);

	if ( _new ){
		trace0("TTT::sweep_tt tt_begin");
		tt_begin();
	} else {
		tt_cont();
	}
	if(_new){
		trace0("_new");
		untested();
		CARD_LIST::card_list.stress_apply(); 
	}

	if( _power_down ){
		trace1("TTT::sweep_tt pd until", _Tstop );
		power_down(  _Tstop  );
		trace0("TTT::sweep_tt pd done");
		return;
	}else if( _Tstop == _Tstart || double(_Tstop) == 0 ){
		trace0("TTT::sweep_tt just apply");
		advance_Time();
		CARD_LIST::card_list.stress_apply();
		_sim->_acceptq.clear();
		return;
	}else if( _Tstop == _Tstart ){
		trace0("TTT::sweep_tt just printing");
		outdata_b4(_sim->_Time0); // first output tt data
		return;
	}else if ( !_cont_dc ){
		trace0("TTT::sweep_tt from 0");
		print_head_tr();
		do_initial_dc();

		outdata_tt( _sim->_Time0);
		trace0("initial done");
		if(_trace)
			outdata_b4( 0 );
		_cont = true;

		first();
		trace0("TTT::sweep_tt first done");
	}else if(_tstop==0. && _tstep==0. && (!_cont_tt) ){

		print_head_tr();
		do_initial_dc();
		return;

	} else { // strange way of doing first next loop
		trace0("TTT::sweep_tt 1st next loop");
		assert(false);
		first_after_interruption();
	}

	// assert (_sim->_loadq.empty());
	assert(_sim->_Time0 == _Tstart);

	//_time_by_beh=NEVER;
	//_time_by_adp=NEVER;

	assert (_Time1 == _Tstart);

	while(next())
	{
		trace7( "TTT::sweep_tt loop start ", _sim->_Time0, _Time1, _sim->_dT0,
				_accepted, _accepted_tt, tt_iteration_number(), _sim->_last_Time ); 
		sanitycheck();


		trace0("TTT::sweep_tt ADP_NODE::tt_commit");
//		ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_commit );
		trace2("TTT::sweep CARD::stress_apply", _sim->_dT0, _sim->last_time());
		assert(  _sim->_dT0 - _sim->last_time() >= 0);
		CARD_LIST::card_list.stress_apply();

		trace2( "TTT::sweep calling TTT::sweep", _cont, _sim->_Time0 );
		store_results_tt(_sim->_Time0); // first output tt data
		// ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tr_reset );
		TTT::sweep();

		if(!_accepted) 
		{
			set_step_tt_cause(scREJECT);
			continue;
		}

		_accepted_tt = review_tt();

		if(! _accepted_tt ){
			//	_out << "TTT::sweep_tt (loop) NOT accepted " << _sim->_Time0 << _sim->_dT0 <<  _dT_by_adp << "\n" ;

			// step_cause_tt = trfail;
			_sim->_tt_rejects++;
			_sim->_tt_rejects_total++;
			trace0("TTT::sweep calling tt_next on CL");
			CARD_LIST::card_list.tt_advance();
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
		//
		if (_trace >= tGUESS) print_stored_results_tt(_sim->_Time0);
		outdata_tt(_sim->_Time0+_tstop); // first output tt data
		_sim->_last_Time = _sim->_Time0+_tstop;
		trace2("TTT::sweep_tt end loop", _sim->_last_Time, _sim->_Time0 );
	}

//		_out << "* TTT::sweep_tt =================== endof loop "<<_sim->_last_Time<<"\n";
	_sim->_Time0 = _sim->_last_Time;
	_sim->_dT0 = 0;

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
	_sim->_mode = s_TRAN;
	trace2("TTT::sweep", storelist().size() , _sim->tt_iteration_number());
	for ( int i = storelist().size(); i --> 0; ) _sim->_waves[i].initialize();
	_sim->_mode = s_TTT;

	trace1("TTT::sweep() ", _inside_tt);

	CKT_BASE::tt_behaviour_rel = 0; // *= new_dT/(time0-time1);
	CKT_BASE::tt_behaviour_del = 0; //  *= new_dT/(time0-time1);
	//  ADP_LIST::adp_list.do_forall( &ADP::tt_commit );
	//
	if (_sim->_Time0 !=0 ){
		CARD_LIST::card_list.tt_behaviour_commit( );
	}
	_sim->_time0 = 0.0;

	try{
		_cont = true;
		trace3("TTT::sweep calling sweep", _sim->_time0, _sim->last_time(), _tstep);
		_inside_tt=1;
		TRANSIENT::sweep();
		assert(_accepted);
		CARD_LIST::card_list.tr_stress_last();

	}catch (Exception& e) {
		untested();
		assert(_sim->_dT0 > _sim->last_time()); // = means zero step
		                                       // if this fails, something is wrong.
		error(bDANGER, "Sweep exception %s at %E, dT0 %E, step %i\n",
				e.message().c_str(), _sim->_Time0, _sim->_dT0, tt_iteration_number());
		_out << "* tt sweep failed: "<< e.message() <<"\n";
		_accepted=_accepted_tt=false;
		::status.review.stop();
		_sim->invalidate_tt();
		// throw(e); go on with smalller step?
		if (!tt_iteration_number()) {
			_out << "* first one may not fail\n";
			throw(e); 
		}
	}

	trace2("TTT done TRANSIENT::sweep", _sim->last_time(), _sim->_Time0);
	//  std::cout << "* sweep done. last time: " << _sim->last_time() << " tstop " << _tstop << "\n";

	CKT_BASE::tt_behaviour_rel /= (_sim->_dT0);
	CKT_BASE::tt_behaviour_del /= (_sim->_dT0);
	//  std:: << "TTT::sweep_tt brel: " << CKT_BASE::tt_behaviour_rel << "\n";
	//  std:: << "TTT::sweep_tt bdel: " << CKT_BASE::tt_behaviour_del << "\n";
	//  std:: << "TTT::sweep_tt brel normalized: " << CKT_BASE::tt_behaviour_rel << "\n";

	// hier gibs noch kein behaviour.  
	// ::status.tran.stop();
	_inside_tt=false;
	_sim->_mode = s_TTT;
}
/*--------------------------------------------------------------------------*/
void TTT::accept_tt()
{
	ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_accept ); // schiebt tt_valuei weiter.

	//  FIXME:  _tt_acceptq -> corrector?

	while (!_sim->_tt_acceptq.empty()) {
		_sim->_tt_acceptq.back()->tt_accept();
		_sim->_tt_acceptq.pop_back();
	}
	_sim->_tt_accepted++;
	//  _sim->_last_Time = _sim->_Time0+_tstop;
}
/*--------------------------------------------------------------------------*/
double TTT::time_by_voltages(){
	// we started at vdc (unchanged), before keep, compare v0<->vdc

	double d=0;
	USE(d);
	for(unsigned i=1; i<=_sim->_total_nodes; ++i){
		double delta = fabs(_sim->_v0[i]-_sim->_vdc[i]);
		double sum = fabs(_sim->_v0[i])+fabs(_sim->_vdc[i]);
		USE(sum);

		double err = delta ;
		USE(err);
	}

	incomplete();
	return(0);

}
/*-----------------------------------------------------------*/
// need to check that boundaries are consistent.
bool TTT::conchk() const{

	double a = OPT::abstol;
	double r = OPT::reltol;

	for(unsigned i=1; i<=_sim->_total_nodes; ++i){
		double n = fabs(_sim->_v0[i]);
		double o = fabs(_sim->_vdc[i]);

		if (std::abs(n-o) > (r * std::abs(n) + a))
			return false;
	}
	return true;
}
/*--------------------------------------------------------------------------*/
bool TTT::review_tt()
{
	bool tmp=true;
	trace2 ( "TTT::review_tt " , _sim->_Time0, _sim->_dT0 );
//	if (_trace>0 )
//		_out << "* review_tt " << _sim->_Time0 << "\n";
	sanitycheck();

	assert(_sim->_Time0 >= _sim->_dT0);
	assert(_sim->_Time0 - _sim->_dT0 >=0 );

	//FIXME
	//  _dT_by_adp = ADP_LIST::adp_list.tt_review()           + _tstop; 
	_dT_by_adp = (ADP_NODE_LIST::adp_node_list.tt_review())._event; 
	_dT_by_beh = OPT::behreltol/CKT_BASE::tt_behaviour_rel * _sim->_dT0;
	_dT_by_beh = max ( _dT_by_beh, (double) _tstop );

	//  double _dT_by_nodes = time_by_voltages();// 1.0 / (1e-20+voltage_distance());//

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
//	if(_trace>0 )
//		_out << "* review_tt done "<< tmp <<" "<< _time_by_adp << " " << _dT_by_adp <<"\n";
	return tmp;
}
/*--------------------------------------------------------------------------*/
// copied from ttf
void TTT::do_it(CS& Cmd, CARD_LIST* Scope)
{
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
	}catch (Exception& e) {itested();
		error(bDANGER, "error: " + e.message() + '\n');
		throw(Exception("error TTT::do_it"));
	}

	try {
		allocate();
		::status.set_up.stop();
		switch (ENV::run_mode) {untested();
			case rPRE_MAIN:	unreachable();		break;
			case rPIPE:
			case rBATCH:
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
void TTT::probeexpand()
{
	trace0("TTT::probeexpand");

	_sim->_mode=s_TRAN;
	PROBELIST* transtore = &PROBE_LISTS::store[s_TRAN];

	// append prior prints to store (aligned)
	for (PROBELIST::const_iterator p=printlist().begin();
			p!=printlist().end();  ++p) {
		PROBE* x=((*p)->clone());
		transtore->push_probe(x);
	}
	
	// additionally store during TR as user wishes. (if necessary)
	for (PROBELIST::const_iterator p=oldstore.begin();
			p!=oldstore.end();  ++p) {
		PROBE* x=((*p)->clone());
		transtore->merge_probe(x);
	}

	_sim->_mode = s_TTT;


	PROBELIST measstore;
	for (PROBELIST::const_iterator
			p=printlist().begin();  p!=printlist().end();  ++p) {
		MEAS_PROBE* w = dynamic_cast<MEAS_PROBE*>(*p);
		if(w){
			w->expand(); 
			string probe_name = w->probe_name;
			trace1("TTT::probeexpand", probe_name);
			CS* c = new CS(CS::_STRING,probe_name);
			measstore.add_list(*c);
			delete c;
		}
	}

// merge some more measure nodes.
	for (PROBELIST::const_iterator p=measstore.begin();
			p!=measstore.end();  ++p) {
		PROBE* x=((*p)->clone());
		transtore->merge_probe(x);
	}
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

	PROBELIST* transtore = &PROBE_LISTS::store[s_TRAN];

	for (PROBELIST::const_iterator p=transtore->begin();
			p!=transtore->end(); ++p) {
		oldstore.push_probe((*p)->clone());
	}
	PROBE_LISTS::store[s_TRAN].clear();


	probeexpand();

	assert(!_sim->_waves_tt);
	assert(!_sim->_waves);
	_sim->_waves_tt = new WAVE [storelist().size()];
	_sim->_mode=s_TRAN;
	_sim->_waves = new WAVE[storelist().size()];
	_sim->_mode=s_TTT;

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

	PROBELIST* transtore = &PROBE_LISTS::store[s_TRAN];

	transtore->clear();
	// insert previous store probes
	for (PROBELIST::const_iterator p=oldstore.begin();
			p!=oldstore.end(); ++p) {
		transtore->push_probe((*p)->clone());
	}
}
/*--------------------------------------------------------------------------*/
static TTT p10;
DISPATCHER<CMD>::INSTALL d10(&command_dispatcher, "twotimetran", &p10);
/*--------------------------------------------------------------------------*/
//testing=script 2007.11.21
/*--------------------------------------------------------------------------*/
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
	_inside_tt=true;

	trace4( "TTT::next() " , tt_iteration_number() ,  _sim->_Time0 , _Time1 ,   _sim->_last_Time);

	assert(_sim->_Time0 >=0 || tt_iteration_number() == 0);

	if( !_accepted_tt ){
		// if ( _trace>5 ) _out << "NOT accepted " << _sim->_Time0 << "\n";

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
		// _sim->_last_Time = _sim->_Time0+_sim->last_time();   

		_Time1 = _sim->_Time0;

		new_dT = min( (double) _dT_by_adp, (_sim->_dT0 + _Tstep)/2 ) ; 
		new_dT = min( (double) new_dT, _sim->_dT0 * OPT::ttstepgrow) ; 
		new_dT = max( new_dT, (double) _tstop ) ; // fmin( get_new_dT(), _Tstep );
		new_dT = max( new_dT, (double) _sim->last_time() ) ; // fmin( get_new_dT(), _Tstep );

//		if ( _trace > 0 ) 
//			_out << "* new step " << new_dT << " ( just accepted: " << _sim->_dT0   << 
//				", adp "<< _dT_by_adp <<" last_Time" << _sim->_last_Time << " )\n";

		if(new_dT < _dTmin){
			error(bWARNING, "step too small %e %e adp %e\n", new_dT, _dTmin, _dT_by_adp );
			return false;
		}

//		if ( _trace > 5 ) 
//			_out << "* last step handler? Tstop " << (double)_Tstop << " tstop " << _tstop <<
//				" last_Time" << _sim->_last_Time << " )\n";
		// last step handler, snap to edge
		if ( ( (double)_Tstop - _sim->_last_Time < 2* _tstop 
					|| new_dT +  _sim->_last_Time > _Tstop )
				&& (double)_Tstop - _sim->_last_Time >    _tstop   ) {
			trace4("TTT::next last step", _Tstop, _sim->_last_Time, _tstop,_Tstart);
//			if ( _trace > 0 ) 
//				_out << "* last step handler..." <<  _sim->_last_Time << "\n";

			untested();
			new_dT = _Tstop - _sim->_Time0 - _tstop ;
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
	new_Time0 = _sim->_last_Time - _tstop + new_dT ;

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
				_Time1, _Tstop, new_dT >= _dTmin ,  _Tstop - _Time1 >= _dTmin ,  new_Time0 + _tstop <= _Tstop );
		//    _sim->_last_Time = _Time1 + _tstop; // FIXME

		return (false);
	} else {
		trace5("TTT::next another step ", _sim->_Time0, new_dT, _Time1, _Tstop, _sim->_dT0 );
	}

	_sim->_Time0 = new_Time0;

	if(_accepted_tt){ // fixme: obenhin, oder?
		_sim->_dT3 = _sim->_dT2;
		_sim->_dT2 = _sim->_dT1;
		_sim->_dT1 = _sim->_dT0;
	}
	_sim->_dT0 = new_dT;

	assert(  _sim->_dT0 - _sim->last_time() >= 0);

	time1 = 0.;

	assert( _sim->_Time0 < 1+ _Time1 + _sim->_dT0 );
	assert( _sim->_Time0 >= _sim->_dT0 );
	assert( _sim->_Time0 > 0  || tt_iteration_number() == 0);

	_sim->restore_voltages();

	trace0("praparing nodes nodes");

	for (uint_t ii = 1;  ii <= _sim->_total_nodes;  ++ii) {
		_sim->_nstat[_sim->_nm[ii]].set_last_change_time(0);
		_sim->_nstat[_sim->_nm[ii]].store_old_last_change_time();
		_sim->_nstat[_sim->_nm[ii]].set_final_time(0);
	}

	advance_Time();

//	if ( _trace>0 ) 
//		_out << "* committing for " << _sim->_Time0 << "(step "  << tt_iteration_number() <<  ")\n";

//	if ( _trace>0 ) _out << "* exiting next " << another_step << "\n";
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
	_sim->_mode=s_TTT;
	//PROBELIST* transtore = &PROBE_LISTS::store[s_TRAN];

	print_tr_probe_number = printlist().size();

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
			if ( !(--print_tr_probe_number) ) break;
		}
		_out << '\n';
	}else{
	}
	_sim->_mode=s_TRAN;


	trace3("TTT::tt_head probe TRAN", printlist().size(), storelist().size(), oldstore.size());

//	_sim->_waves = new WAVE[storelist().size()];

	_sim->_mode=s_TTT;
	for (PROBELIST::const_iterator
			p=printlist().begin();  p!=printlist().end();  ++p) {
		_sim->_mode=s_TRAN;
		(*p)->precalc_last();
		_sim->_mode=s_TTT;
	}
}
/*--------------------------------------------------------------------------*/
/* SIM::head: initialize waves
*/
void TTT::head(double /* start */ , double /* stop */, const std::string& )
{
	trace0("TTT::head");
	assert( _sim->_mode==s_TTT );
	assert(_sim->_waves_tt);
	{
		trace0("TTT::head tr ttt WAVE");

		if (_sim->_waves_tt) {
			int i = storelist().size();
			while( i-->0 ){
				(_sim->_waves_tt)[i].initialize();
			}
		}else{
			trace0("TTT::head no waves yet");
		}
	}
	_sim->_mode=s_TRAN;
	if (_sim->tt_iteration_number() == 0){
		TRANSIENT::_out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
		// print_head_tr();
	}

	_sim->_mode=s_TTT;
	trace2("TTT::head probe TTT", printlist().size(), storelist().size());
}
/*--------------------------------------------------------------------------*/
/* SIM::print_head: print column headers TR
*/
void TTT::print_head_tr()
{
	trace0("TTT::print_head_tr");

	//assert( _sim->_mode==s_TRAN );
	_sim->_mode = s_TRAN;
	SIM_MODE oldmode = _sim->_mode;

	if( printlist().size() == 0 )
	{
		itested();
		_sim->_mode=oldmode;
		TRANSIENT::_out << "* no trprint\n";
		return;
	}
	TRANSIENT::_out << "* TTime";
	{
		// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
		char format[20];
		for (int i=0; i< OPT::numdgt; i++) {
			TRANSIENT::_out << "-";
		}  
		sprintf(format, "%%c%%-%us", width);
		TRANSIENT::_out << "time      ";
		for (int i=0; i< OPT::numdgt-5; i++) {
			TRANSIENT::_out << " ";
		}  
		for (PROBELIST::const_iterator
				p=printlist().begin();  p!=printlist().end();  ++p) {
			TRANSIENT::_out.form(format, ' ', (*p)->label().c_str());
		}
		TRANSIENT::_out << '\n';
	}
	_sim->_mode=oldmode;
}
/*--------------------------------------------------------------------------*/
// save things during TR
void TTT::outdata(double time0)
{
	/*   
	 *   untested();
	 *   if( _sim->_mode  == s_TTT && OPT::behave )
	 CARD_LIST::card_list.do_forall( &CARD::tr_save_amps, (_sim->_stepno) );
	 CKT_BASE::tt_behaviour_del +=  CKT_BASE::tr_behaviour_del; // hack
	 CKT_BASE::tt_behaviour_rel +=  CKT_BASE::tr_behaviour_rel;
	 */

	_sim->_mode = s_TTT;
	trace3("TTT::outdata", _sim->tt_iteration_number(), _sim->iteration_number(), time0);
	::status.output.start();
	assert( _sim->_mode  == s_TTT );

	// SIM::alarm();
	_sim->_mode=s_TRAN;
	if ( _trace>=tDEBUG) { 
		  TRANSIENT::print_results(time0);
	}

	_sim->set_command_tran();
	if(_sim->tt_iteration_number()==0 && printlist().size() != 0 )
	{
		TRANSIENT::_out << (double)_sim->_Time0 ;
		TRANSIENT::print_results(time0);
	} else {
		// store_results(time0);
		if (TRANSIENT::_trace >= tDEBUG){
			TRANSIENT::_out << "*" << (double)_sim->_Time0;
			TRANSIENT::print_results(time0);
		}
	}
	_sim->set_command_tt();

	// FIXME (only > 0)
	store_results(time0);

	_sim->_mode = s_TTT;
	_sim->reset_iteration_counter(iPRINTSTEP);
	::status.hidden_steps = 0;
	::status.output.stop();
	assert( _sim->_mode  == s_TTT );
	trace0("TTT::outdata done");
}
/*--------------------------------------------------------------------------*/
void TTT::outdata_b4(double time)
{
	trace0("TTT::outdata_b4()");
	assert( _sim->_mode  == s_TTT );
	::status.output.start();
	print_results_tt( time );
	::status.output.stop();
}
/*--------------------------------------------------------------------------*/
void TTT::outdata_tt(double now) // print at end of timeframe.
{
	assert( _sim->_mode  == s_TTT );
	if ( _sim->_dT0 &&  !is_almost (_sim->_dT0 + _sim->_last_Time , now )){
		error(bWARNING, "EOF: %.9f, last_Time: %.9f, dT0: %f, now: %f\n", _sim->_Time0+_tstop, _sim->_last_Time, _sim->_dT0,now );
	}
	trace0("TTT::outdata_tt()");
	::status.output.start();
	print_results_tr( now ); //transient print?
	print_results_tt( now );
	_sim->reset_iteration_counter(iPRINTSTEP);
	::status.hidden_steps = 0;
	::status.output.stop();
}
/*--------------------------------------------------------------------------*/
// print during tr. i.e. print from storelist
void TTT::print_results(double time)
{
	// deprecated call.
	unreachable();
	print_results_tr(time);
}
/*--------------------------------------------------------------------------*/
void TTT::print_results_tr(double )
{
	trace1("TTT::print_results_tr()", tt_iteration_number());
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
		w = &(_sim->_waves[0]);
		print_head_tr();

		int ii=0;
		// FIXME: how many?
		WAVE::const_iterator myiterators[_sim->_waves->size()];

		for (PROBELIST::const_iterator p = printlist().begin();
				p!=printlist().end();  ++p) {
			myiterators[ii] = (_sim->_waves[ii]).begin();
			ii++;
		}
		for (WAVE::const_iterator i = w->begin(); i < w->end(); i++ ) {
			trace1("TRANSIENT::_out" , _sim->_Time0);
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
	trace0("TTT::store_results_tt()");
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
	trace1("TTT::print_results_tt()",x);

	if ( printlist().size() == 0)
	{
		untested0( "no ttprint" ); 
		//return; // FIXME: implement quiet switch or redirect to /dev/null
	}

	assert( _sim->_mode  == s_TTT );
	if (!IO::plotout.any()) {
		_out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
		assert(x != NOT_VALID);
		_out << x;
		for (PROBELIST::const_iterator
				p=printlist().begin();  p!=printlist().end();  ++p) {
			trace1("TTT::print_results_tt", (*p)->label());
			_out <<  (*p)->value();
		}
		_out << '\n';
	}else{
	}
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
	trace2("TTT::store_results()", tt_iteration_number(), iteration_number() );
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
	trace3("TTT::advance_Time", _sim->_tr[0], _sim->_tt[0], _sim->_Time0);
	::status.tt_advance.start();

	static double last_iter_time;
	_sim->_time0 = 0.;
	if (_sim->_Time0 > 0) {
		if (_sim->_dT0 == 0) {

		}else if (_sim->_Time0 > last_iter_time && _accepted_tt ) {	/* moving forward */
			_sim->_tt_iter++;
			_sim->_tt_rejects = 0;
			_sim->update_tt_order();

//			trace2("TTT::advance_Time", _sim->_tr1[0], _sim->_tt1[0]);

			notstd::copy_n(_sim->_tr2, _sim->_adp_nodes, _sim->_tr3);
			notstd::copy_n(_sim->_tr1, _sim->_adp_nodes, _sim->_tr2);
			assert(is_number(_sim->_tr[0]));

			notstd::copy_n(_sim->_tr, _sim->_adp_nodes, _sim->_tr1);

			notstd::copy_n(_sim->_tt, _sim->_adp_nodes, _sim->_tt1);

			// invalidate ....
			std::fill_n(_sim->_tr, _sim->_adp_nodes, NAN);
			std::fill_n(_sim->_tt, _sim->_adp_nodes, NAN);

			trace3("TTT::advance_Time done", _sim->_tr[0], _sim->_tt[0], _sim->_Time0);
			trace2("TTT::advance_Time done", _sim->_tr1[0], _sim->_tt1[0]);

			assert(is_number(_sim->_tr1[0]));
			assert(is_number(_sim->_tt1[0]));

		}else{				/* moving backward */

		}
	}else{
		trace0("TTT::advance_Time obsolete call?");
		untested();
	}
	last_iter_time = _sim->_Time0;
	trace0("TTT::advance_Time stop");

	ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_advance ); 

	/// AFTER tr and tt have been shifted.
	CARD_LIST::card_list.tt_advance();

	//else
	//CARD_LIST::card_list.tt_regress();
	//?

	trace0("TTT::advance_Time() done tt_advance");
	::status.tt_advance.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

} // namespace
// vim anything.
