

#include "s_tt.h"

namespace TT {

void TTT::options(CS& Cmd)
{
	trace0(( "TTT::options rest ||| " +Cmd.tail() ).c_str());
	_out = TTT::mstdout;
	_out.reset();
	bool tr =false;
	_new = false;
	bool _continue = false;

	_sim->_temp_c = OPT::temp_c;
	_trace = tNONE;
	_power_down = false;
	unsigned here = Cmd.cursor();
	do{
		ONE_OF
			|| Get(Cmd, "p{owerdown}", &_power_down)
			|| Get(Cmd, "pd",          &_power_down)
			|| Get(Cmd, "n{ew}",       &_new)
			|| Get(Cmd, "c{ont}",      &_continue) // continue from adp_node values.
			|| Get(Cmd, "uic",         &_sim->_uic)
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
					 || Set(Cmd, "d{ebug}",     &_trace, tDEBUG)
					 || Cmd.warn(bWARNING, "need none, off, warnings, alltime, "
						 "rejected, iterations, verbose")
					)
				)
			;
		if (!( Get(Cmd , "tran",	   &tr) ) ) // tr means 'trace'
			_out.outset(Cmd);
	}while (Cmd.more() && !Cmd.stuck(&here) && !tr);

	initio(_out);
	TRANSIENT::options(Cmd); // parse options from tran. 

	if (_new){
		_cont_tt = false;
		if(_continue) incomplete();
		_continue = false;
	}
	if(_continue){
		trace0("continuing with aging state...");
		_cont_tt = true;
	}

	_dtmax_in.e_val(BIGBIG, _scope);
	// _dTmin_in.e_val(OPT::dTmin, _scope);
	_dtratio_in.e_val(OPT::dtratio, _scope);
	_skip_in.e_val(1, _scope);

}
/*--------------------------------------------------------------------------*/
void TTT::setup(CS& Cmd)
{
	trace1("TTT::setup", _cont_tt);
	_tstart.e_val(NOT_INPUT, _scope);
	_tstop.e_val(NOT_INPUT, _scope);
	_tstep.e_val(NOT_INPUT, _scope);
	_Tstop.e_val(NOT_INPUT, _scope);
	_Tstep.e_val(NOT_INPUT, _scope);


	// nonsense
	// TRANSIENT::setup( Cmd);

	_cont = true;
	_cont_tt = true;

	if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
		PARAMETER<double> arg1, arg2, arg3, arg4, arg5, arg6;
		Cmd >> arg1;
		arg1.e_val(0.0,_scope);
		if (Cmd.match1("'\"({") || Cmd.is_float()) {
			Cmd >> arg2;
			arg2.e_val(0.0,_scope);
		}else{itested();
		}
		if (Cmd.match1("'\"({") || Cmd.is_float()) {
			Cmd >> arg3;
			arg3.e_val(0.0,_scope);
		}else{
		}
		if (Cmd.match1("'\"({") || Cmd.is_float()) {
			Cmd >> arg4;
			arg4.e_val(0.0,_scope);
		}else{
		}
		if (Cmd.match1("'\"({") || Cmd.is_float()) {
			Cmd >> arg5;
			arg5.e_val(0.0,_scope);
		}else{
		}
		if (Cmd.match1("'\"({") || Cmd.is_float()) {
			Cmd >> arg6;
			arg6.e_val(0.0,_scope);
		}else{
		}

		trace5(("TTT::setup args " + std::string(Cmd)).c_str(), arg1, arg2 , arg3 , arg4, arg5);

		if (arg5.has_hard_value()) {	    // 5 args  tt tt TTstart TTstop TTstep
			trace0("TTT::setup have 5");
			untested();
			assert(arg5.has_hard_value());
			assert(arg4.has_hard_value());
			assert(arg3.has_hard_value());
			assert(arg2.has_hard_value());
			assert(arg1.has_hard_value());
			arg1.e_val(0.,_scope);
			arg2.e_val(0.,_scope);
			arg3.e_val(0.,_scope);
			arg4.e_val(0.,_scope);
			_sim->_last_Time = .0; // ?
			_sim->tr_reset();

			_tstep  = arg1;
			_tstop  = arg2;

			_Tstart = arg3;
			_Tstep  = arg4;
			_Tstop  = arg5;
			assert((double)_tstep!=0 || !_tstop);
			if (!_tstop) untested();

			if(double( _Tstart) == 0) 
				_cont_tt = false;

			trace4("TTT::setup ", _tstep, _tstop , _Tstep , _Tstop);

		} else if (arg4.has_hard_value()) {	    // 4 args: TT TT tt tt
			trace0("TTT::setup have 4");
			assert(arg3.has_hard_value());
			assert(arg2.has_hard_value());
			assert(arg1.has_hard_value());
			arg1.e_val(0.,_scope);
			arg2.e_val(0.,_scope);
			arg3.e_val(0.,_scope);
			_sim->_last_Time = .0;
			_sim->tr_reset();

			_tstep  = arg1;
			_tstop  = arg2;

			if (_tstep>_tstop){
				error(bWARNING, "_tstep > _tstop. really?\n");
			}
			_Tstep  = arg3;
			_Tstop  = arg4;
			_Tstart = 0; //HACK?
			assert((double)_tstep!=0 || !_tstop);
			if (!_tstop) untested();

			_cont_tt = false;
			trace4("TTT::setup ", _tstep, _tstop , _Tstep , _Tstop);

		} else if (arg3.has_hard_value() ) {
			trace0("TTT::setup have 3");
			_cont_tt = false;
			_Tstart = arg1;
			_Tstep = arg2;
			_Tstop = arg3;

		} else if (arg2.has_hard_value() ) {
			trace0("TTT::setup have 2");
			_Tstart = _sim->_last_Time;
			_sim->_time0 = 0;
			_sim->tr_reset();
			if ((double)_Tstart == 0){
				trace0("TTT::setup latching tr times");
				_tstep = arg1;
				_tstop = arg2;
				_Tstop = 0;

			}else{
				trace1("TTT::setup ran already", (double)_Tstart );
				if((double)arg1==0){
					_Tstop  = arg2; 
					_Tstart =0;
				}else if(arg1<arg2){
					_Tstop  = arg2; 
					_Tstep = arg1;
				}else{
					_Tstop  = arg1; 
					_Tstep = arg2;
				}
			}

			_cont_tt = true;
			_cont = true; untested();
			trace4("TTT::setup 2 args ", _tstep, _tstop , _Tstep , _Tstop);

		} else if (arg1.has_hard_value() ) {
			trace1("TTT::setup have 1", _sim->_last_Time);
			_Tstart = _sim->_last_Time;
			_Tstop  = arg1; // as tran
			_sim->_time0 = 0;
			_sim->tr_reset();

			// to trigger prints... (hack?)
			if(double(_Tstop) == 0) _Tstop = double( _Tstart );

			if((!_Tstep.has_hard_value() )|| ((double)_Tstep == 0)) {
				trace1("set Tstep ", _Tstop);
				_Tstep=_Tstop;
			} else {
				trace1("set Tstep ", _Tstep);
			}
			if(_Tstart!=0){
				_cont_tt = true;
				_cont = true; untested();
			}

		} else {
			unreachable();
			assert (!arg1.has_hard_value());   // for now...

			double oldrange = _Tstop - _Tstart;
			_Tstart = _sim->_last_Time;
			_Tstop  = _sim->_last_Time + oldrange;
		}
	}else{ /* no args */
		// std::cerr << "setup ttt -- no args\n";
		double oldrange = _Tstop - _Tstart;
		_Tstart = _sim->_last_Time;
		_Tstop  = _sim->_last_Time + oldrange;

		if(_sim->_last_Time==0){
			trace1("TTT::setup no args at beginning", _cont_tt);
			_Tstop=0;
			_tstep=0;
			_tstop=0;

		}
	}

	if (_tstep>_tstop){
		error(bWARNING, "_tstep > _tstop. really?\n");
	}

	options(Cmd);

	_tstart.e_val(0., _scope);
	_tstop.e_val(NOT_INPUT, _scope);
	_tstep.e_val(NOT_INPUT, _scope);

	//Time1 = 
	_sim->_Time0 = _Tstart;
	_Time1 = _Tstart;


	if (!_tstep.has_good_value()) {
		throw Exception("transient: Time step is required");
	}else if (_tstep==0. && _tstop ) {
		untested();
		throw Exception("Time step == 0 while tend");
	}else{
	}

//		if (_dtmax_in.has_hard_value()) {
//			_dtmax = _dtmax_in;
//		}else if (_skip_in.has_hard_value()) {
//			_dtmax = _tstep / double(_skip_in);
//		}else{
//			_dtmax = std::min(_dtmax_in, _tstep);
//		}

	_dTmin= _tstop;
	_sim->_dTmin= _tstop;

	if (_dTmin_in.has_hard_value()) {
		_dTmin = _dTmin_in;
	}else if (_dtratio_in.has_hard_value()) {
		_dTmin = _dTmax / _dTratio_in;
	}else{
		// use larger of soft values
		// _dTmin = std::max(double(_dTmin_in), _dTmax/_dTratio_in);
		// _dTmin=0.5; // HACK
	}


	if  ( _Tstart < _sim->_last_Time  ||  _sim->_last_Time <= 0.) {
		//    _out << "* last_Time " << _sim->_last_Time << "\n";
		trace3("TTT::setup no cont ", _Tstart, _sim->_last_Time, _cont_tt );
		//_cont_tt = false;
		_Time1 = _sim->_Time0 = 0.;
	}else{
		_cont_tt = true;
		_Time1 = _sim->_Time0 = _sim->_last_Time;
	}

	// TRANSIENT setup

	_tstart.e_val(0., _scope);
	_tstop.e_val(NOT_INPUT, _scope);
	_tstep.e_val(NOT_INPUT, _scope);

	if  (_cont_tt){
		_cont = false;

	}else{
		_cont = false;
	}

	time1 = _sim->_time0 = 0.;
	//{}else{
	//  untested();
	// _cont = true;
	//  time1 = _sim->_time0 = _sim->_last_time;
	// }
	_sim->_freq = ((_tstop > _tstart) ? (1 / (_tstop - _tstart)) : (0.));

	if (!_tstep.has_good_value()) {
		throw Exception("transient: time step is required");
	}else if (_tstep==0. && _tstop ) {itested();
		throw Exception("time step == 0 and tstop");
	}else{
	}

	if (_dtmax_in.has_hard_value()) {
		_dtmax = _dtmax_in;
	}else if (_skip_in.has_hard_value()) {
		_dtmax = _tstep / double(_skip_in);
	}else{
		_dtmax = std::min(_dtmax_in, _tstep);
	}

	if (_dtmin_in.has_hard_value()) {
		_sim->_dtmin = _dtmin_in;
	}else if (_dtratio_in.has_hard_value()) {
		_sim->_dtmin = _dtmax / _dtratio_in;
	}else{
		// use larger of soft values
		_sim->_dtmin = std::max(double(_dtmin_in), _dtmax/_dtratio_in);
	}

	steps_total_out_ = (int) (1 + ceil( ( (_tstop - _tstart ) / _tstep ) ));
	steps_total_out_ = steps_total_out_ ;
	trace6( "TTT::setup done ",  steps_total_out_ , _tstep , _tstop ,_tstart, _cont, _tt_cont );

}
	/*--------------------------------------------------------------------------*/
}
