/*$Id: s_dc.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
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
 * dc analysis top
 */
//testing=script,complete 2006.07.14
#include "u_status.h"
#include "u_prblst.h"
#include "u_cardst.h"
#include "e_elemnt.h"
#include "e_storag.h"
#include "s__.h"
#include "io_matrix.h"
#include "m_matrix_extra.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DDCOP : public SIM {
public:
  void	finish();
protected:
  void	fix_args(int);
  void	options(CS&, int);
private:
  void	sweep();
  void	sweep_recursive(int);
  void	first(int);
  bool	next(int);
  void do_tran_step();
  void undo_time_step();
  explicit DDCOP(const DDCOP&): SIM() {unreachable(); incomplete();}
protected:
  explicit DDCOP();
  ~DDCOP() {}
  
protected:
  enum {DCNEST = 4};
  int _n_sweeps;
  PARAMETER<double> _start[DCNEST];
  PARAMETER<double> _stop[DCNEST];
  PARAMETER<double> _step_in[DCNEST];
  double _step[DCNEST];
  bool _linswp[DCNEST];
  double _sweepval[DCNEST];	/* pointer to thing to sweep, dc command */
//  typedef void (*p)(double);
  ELEMENT* (_pushel[DCNEST]);	/* pointer to thing to sweep, dc command */
  ELEMENT* (_zap[DCNEST]);	/* to branch to zap, for re-expand */
  CARDSTASH _stash[DCNEST];	/* store std values of elements being swept */
  bool _loop[DCNEST];		/* flag: do it again backwards */
  bool _reverse_in[DCNEST];	/* flag: sweep backwards, input */
  bool _reverse[DCNEST];	/* flag: sweep backwards, working */
  bool _cont;			/* flag: continue from previous run */
  TRACE _trace;			/* enum: show extended diagnostics */
  enum {ONE_PT, LIN_STEP, LIN_PTS, TIMES, OCTAVE, DECADE} _stepmode[DCNEST];
  static double temp_c_in;	/* ambient temperature, input and sweep variable */
  bool _do_tran_step;
  bool _dump_matrix;
};
/*--------------------------------------------------------------------------*/
double	DDCOP::temp_c_in = 0.;
/*--------------------------------------------------------------------------*/
class DDC : public DDCOP {
public:
  explicit DDC(): DDCOP() {}
  ~DDC() {}
  void	do_it(CS&, CARD_LIST*);
private:
  void	setup(CS&);
  explicit DDC(const DDC&): DDCOP() {unreachable(); incomplete();}
};
/*--------------------------------------------------------------------------*/
/*
class DOP : public DDCOP {
public:
  explicit DOP(): DDCOP() {}
  ~DOP() {}
  void	do_it(CS&, CARD_LIST*);
private:
  void	setup(CS&);
  explicit DOP(const DOP&): DDCOP() {unreachable(); incomplete();}
};
*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DDC::do_it(CS& Cmd, CARD_LIST* Scope)
{
  trace0("doing ddc");
  _scope = Scope;
  _sim->_time0 = 0.;
  //_sim->set_command_ddc();
  _sim->set_command_dc();
  _sim->_phase = p_INIT_DC;
  //_sim->_ddc = true;
  ::status.ddc.reset().start();
  _sim->_temp_c = temp_c_in;
  _do_tran_step=0;
  _dump_matrix=0;
  command_base(Cmd);
  ::status.ddc.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DDCOP::DDCOP()
  :SIM(),
   _n_sweeps(1),
   _cont(false),
   _trace(tNONE)
{

  for (int ii = 0; ii < DCNEST; ++ii) {
    _loop[ii] = false;
    _reverse_in[ii] = false;
    _reverse[ii] = false;
    _step[ii]=0.;
    _linswp[ii]=true;
    _sweepval[ii]=_sim->_genout;
    _zap[ii]=NULL; 
    _stepmode[ii] = ONE_PT;
  }
  
  //BUG// in SIM.  should be initialized there.
  _sim->_genout=0.;
  temp_c_in=OPT::temp_c;
  _out=IO::mstdout;
  _sim->_uic=_sim->_more_uic=false;
}
/*--------------------------------------------------------------------------*/
void DDCOP::finish(void)
{

  for (int ii = 0;  ii < _n_sweeps;  ++ii) {
    if (exists(_zap[ii])) { // component
      _stash[ii].restore();
      _zap[ii]->dec_probes();
      _zap[ii]->precalc_first();
      _zap[ii]->precalc_last();
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DDCOP::do_tran_step()
{

  SIM_PHASE old_phase = _sim->_phase;
  trace1("DDCOP::do_tran_step", OPT::dtddc);
  _sim->_phase = p_TRAN;
  _sim->_mode=s_TRAN;
  _sim->_time0 = _sim->_dt0 = OPT::dtddc;
  //_sim->_genout = gen();

  assert(_sim->analysis_is_tran());
  int tr_converged = solve(OPT::TRHIGH, _trace);

  if (!tr_converged) {
    error(bWARNING, "did not converge\n");
  }else{
  }
  ::status.accept.start();
  trace0("DDCOP::sweep_recursive solved a transient step");

  cerr << "DDCOP v0 = ( " << _sim->_v0[0];
  for(unsigned a=1;a <= _sim->_total_nodes; ++a){
    cerr << " " <<  _sim->_v0[a];
  } cerr << ") \n";

  cerr << "DDCOP vt1 = ( " << _sim->_vt1[0];
  for(unsigned a=1;a <= _sim->_total_nodes; ++a){
    cerr << " " <<  _sim->_vt1[a];
  } cerr << ") \n";
  cerr << "i = ( " << _sim->_i[0];
  for(unsigned a=1;a <= _sim->_total_nodes; ++a){
    cerr << " " <<  _sim->_i[a];
  }
  cerr << ") \n";
  _sim->set_limit();


  CARD_LIST::card_list.tr_accept();
  trace0("DDCOP::sweep_recursive itr_accepted");
  cerr << "_i= ( " << _sim->_i[0];
  for(unsigned a=1; a <= _sim->_total_nodes; ++a){
    cerr << " " <<  _sim->_i[a];
  } cerr << ") \n";

  ::status.accept.stop();

  _sim->_time0 = _sim->_dt0 = 0.0;

  _sim->_mode = s_DC;
  _sim->_phase = p_RESTORE;
  //_sim->restore_voltages(); ????
  _sim->keep_voltages(); //  vdc  = v0
  _sim->put_v1_to_v0(); // v0 = vt1
  _sim->_phase = old_phase;
}
/*--------------------------------------------------------------------------*/
void DDC::setup(CS& Cmd)
{
  _cont = false;
  _trace = tNONE;
  _out = IO::mstdout;
  _out.reset(); //BUG// don't know why this is needed */
  bool ploton = IO::plotset  &&  plotlist().size() > 0;

  if (Cmd.more()) {
    for (_n_sweeps = 0; Cmd.more() && _n_sweeps < DCNEST; ++_n_sweeps) {
      CARD_LIST::fat_iterator ci = findbranch(Cmd, &CARD_LIST::card_list);
      if (!ci.is_end()) {			// sweep a component
	if (ELEMENT* c = dynamic_cast<ELEMENT*>(*ci)) {
	  _zap[_n_sweeps] = c;
          trace0("DDC::setup " + _zap[_n_sweeps]->long_label());
	}else{untested();
	  throw Exception("dc/op: can't sweep " + (**ci).long_label() + '\n');
	}
      }else if (Cmd.is_float()) {		// sweep the generator
	_zap[_n_sweeps] = NULL;
      }else{
	// leave as it was .. repeat Cmd with no args
      }
      
      if (Cmd.match1("'\"({") || Cmd.is_float()) {	// set up parameters
	_start[_n_sweeps] = "NA";
	_stop[_n_sweeps] = "NA";
	Cmd >> _start[_n_sweeps] >> _stop[_n_sweeps];
	_step[_n_sweeps] = 0.;
      }else{
	// leave it as it was .. repeat Cmd with no args
      }
      
      _sim->_genout = 0.;
      temp_c_in = OPT::temp_c;
      _sim->_temp_c = temp_c_in;
      options(Cmd,_n_sweeps);
    }
  }else{ 
  }
  Cmd.check(bWARNING, "what's this?");

  _sim->_uic = _sim->_more_uic = true;

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);

  assert(_n_sweeps > 0);
  for (int ii = 0;  ii < _n_sweeps;  ++ii) {
    _start[ii].e_val(0., _scope);
    fix_args(ii);

    if (exists(_zap[ii])) { // component
      trace1("zap" + _zap[ii]->long_label(), ii );
      _stash[ii] = _zap[ii];			// stash the std value
      _zap[ii]->inc_probes();			// we need to keep track of it


//      urghs. hack
      STORAGE* s = dynamic_cast<STORAGE*>(_zap[ii]);
      if(s != 0){
        _zap[ii]->set_constant(false);		   // so it will be updated
        _pushel[ii] = _zap[ii];	           // point to value to patch
      }else{
        _zap[ii]->set_value(_zap[ii]->value(),0);  // zap out extensions
        _zap[ii]->set_constant(false);		   // so it will be updated
        _pushel[ii] = _zap[ii];	                   // element to patch
      }
      _sweepval[ii] = 0;	        
      
      //_zap[ii]->set_value(_zap[ii]->value(),0);	// zap out extensions
      //_zap[ii]->set_constant(false);		// so it will be updated
      //_sweepval[ii] = _zap[ii]->set__value();	// point to value to patch
      //_sweepval[ii] = _zap[ii]->set__ic();	// point to value to patch
    }else{ // generator
      _sweepval[ii] = 0;
      incomplete();
      _pushel[ii] = NULL; //&_sim->set_gen;			// point to value to patch
    }
  }
  _sim->_freq = 0;
}
/*--------------------------------------------------------------------------*/
void DDCOP::fix_args(int Nest)
{

  _stop[Nest].e_val(_start[Nest], _scope);
  _step_in[Nest].e_val(0., _scope);
  _step[Nest] = _step_in[Nest];
  
  switch (_stepmode[Nest]) {
  case ONE_PT:
  case LIN_STEP:
    _linswp[Nest] = true;
    break;
  case LIN_PTS:untested();
    if (_step[Nest] <= 2.) {untested();
      _step[Nest] = 2.;
    }else{untested();
    }
    _linswp[Nest] = true;
    break;
  case TIMES:untested();
    if (_step[Nest] == 0.  &&  _start[Nest] != 0.) {untested();
      _step[Nest] = _stop[Nest] / _start[Nest];
    }else{untested();
    }
    _linswp[Nest] = false;
    break;
  case OCTAVE:
    if (_step[Nest] == 0.) {untested();
      _step[Nest] = 1.;
    }else{
    }
    _step[Nest] = pow(2.00000001, 1./_step[Nest]);
    _linswp[Nest] = false;
    break;
  case DECADE:
    if (_step[Nest] == 0.) {
      _step[Nest] = 1.;
    }else{
    }
    _step[Nest] = pow(10., 1./_step[Nest]);
    _linswp[Nest] = false;
    break;
  };
  
  if (_step[Nest] == 0.) {	// prohibit log sweep from 0
    _step[Nest] = _stop[Nest] - _start[Nest];
    _linswp[Nest] = true;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DDCOP::options(CS& Cmd, int Nest)
{

  _sim->_uic = _loop[Nest] = _reverse_in[Nest] = false;
  _sim->_more_uic = true;
  unsigned here = Cmd.cursor();
  do{
    ONE_OF
      || (Cmd.match1("'\"({")	&& ((Cmd >> _step_in[Nest]), (_stepmode[Nest] = LIN_STEP)))
      || (Cmd.is_float()	&& ((Cmd >> _step_in[Nest]), (_stepmode[Nest] = LIN_STEP)))
      || (Get(Cmd, "*",		  &_step_in[Nest]) && (_stepmode[Nest] = TIMES))
      || (Get(Cmd, "+",		  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || (Get(Cmd, "by",	  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || (Get(Cmd, "step",	  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || (Get(Cmd, "d{ecade}",	  &_step_in[Nest]) && (_stepmode[Nest] = DECADE))
      || (Get(Cmd, "ti{mes}",	  &_step_in[Nest]) && (_stepmode[Nest] = TIMES))
      || (Get(Cmd, "lin",	  &_step_in[Nest]) && (_stepmode[Nest] = LIN_PTS))
      || (Get(Cmd, "o{ctave}",	  &_step_in[Nest]) && (_stepmode[Nest] = OCTAVE))
      || Get(Cmd, "c{ontinue}",   &_cont)
      || Get(Cmd, "tr{s}",        &_do_tran_step)
      || Get(Cmd, "dm",           &_dump_matrix)
      || Get(Cmd, "dt{emp}",	  &temp_c_in,   mOFFSET, OPT::temp_c)
      || Get(Cmd, "lo{op}", 	  &_loop[Nest])
      || Get(Cmd, "re{verse}",	  &_reverse_in[Nest])
      || Get(Cmd, "te{mperature}",&temp_c_in)
      // FIXME
      //|| Get(Cmd, "uic",	   &_sim->_uic)
      //|| Get(Cmd, "more_uic",	   &_sim->_more_uic)
      || (Cmd.umatch("tr{ace} {=}") &&
	  (ONE_OF
	   || Set(Cmd, "n{one}",      &_trace, tNONE)
	   || Set(Cmd, "o{ff}",       &_trace, tNONE)
	   || Set(Cmd, "w{arnings}",  &_trace, tUNDER)
	   || Set(Cmd, "i{terations}",&_trace, tITERATION)
	   || Set(Cmd, "v{erbose}",   &_trace, tVERBOSE)
	   || Cmd.warn(bWARNING, 
		       "need none, off, warnings, iterations, verbose")
	   )
	  )
      || outset(Cmd,&_out)
      ;
  }while (Cmd.more() && !Cmd.stuck(&here));

  //_sim->_uic|=_sim->_more_uic;

}
/*--------------------------------------------------------------------------*/
void DDCOP::sweep()
{
  head(_start[0], _stop[0], " ");
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
  if (_cont) {untested();
    _sim->restore_voltages();
  }else{
  }
  
  _sim->clear_limit();
  CARD_LIST::card_list.tr_begin();
  sweep_recursive(_n_sweeps);
}
/*--------------------------------------------------------------------------*/
void DDCOP::sweep_recursive(int Nest)
{
  trace1("DDCOP::sweep_recursive", Nest);
  --Nest;
  assert(Nest >= 0);
  assert(Nest < DCNEST);

  OPT::ITL itl = OPT::DCBIAS;
  
  first(Nest);
  do {
    trace0("DDCOP::sweep_recursive loopstart");
    _sim->_temp_c = temp_c_in;
    if (Nest == 0) {
      _sim->_time0 = _sim->_dt0 = 0.0;
      _sim->_last_time = 0.0;
      // _sim->zero_currents();
      _sim->_uic=_sim->_more_uic=true;
      int converged = solve_with_homotopy(itl,_trace);
      if (!converged) {itested();
	error(bWARNING, "did not converge\n");
        throw(Exception("foobar"));
      }
      ::status.accept.start();
      _sim->set_limit();

      CARD_LIST::card_list.tr_accept();
      ::status.accept.stop();
      _sim->keep_voltages(); // vdc = v0

      _sim->_uic=_sim->_more_uic=false;


      _sim->init();

      _sim->_acx.reallocate();
      _sim->_jomega = COMPLEX(0., 1.0);
      _sim->_mode=s_AC;
      // _sim->_acx.set_min_pivot(OPT::pivtol);
      {// AC::sweep
        CARD_LIST::card_list.ac_begin();
        //...
      }

      trace0("solved with homotopy");

      // if verbose
      cerr << "v0 = ( " << _sim->_v0[0];
      for(unsigned a=1;a <= _sim->_total_nodes; ++a){
        cerr << " " <<  _sim->_v0[a];
      }
      cerr << ") \n";
      cerr << "i = ( " << _sim->_i[0];
      for(unsigned a=1;a <= _sim->_total_nodes; ++a){
        cerr << " " <<  _sim->_i[a];
      }
      cerr << ") \n";
      _sim->_uic=_sim->_more_uic=false;

      trace0(" done tr");
      cerr << "_i= ( " << _sim->_i[0];
      for(unsigned a=1; a <= _sim->_total_nodes; ++a){
        cerr << " " <<  _sim->_i[a];
      } cerr << ") \n";

      if(1){ // AC::solve
        trace0("AC::solve");
        _sim->_acx.zero();
        std::fill_n(_sim->_ac, _sim->_total_nodes+1, 0.);

        ::status.load.start();
        _sim->count_iterations(iTOTAL);
        CARD_LIST::card_list.do_ac();
        CARD_LIST::card_list.ac_load();
        ::status.load.stop();
      }

      BSMATRIX<double> G = _sim->_acx.real();
      BSMATRIX<double> C = _sim->_acx.imag();

      if(_dump_matrix){
        _out << "G\n" << G << "\n";
        _out << "C\n" << C << "\n";
      }

      double Gu[_sim->_total_nodes];

      G.rmul(Gu, _sim->_v0);

      for(unsigned a=0;a <= _sim->_total_nodes; ++a){
        // Gu[a] = - Gu[a] +  _sim->_ac[a].real() ;
        Gu[a] = - Gu[a] +  _sim->_i[a] ;
      }
      cerr << "Gu= ( " << Gu[0];
      for(unsigned a=1; a <= _sim->_total_nodes; ++a){
        cerr << " " << Gu[a];
      }
      cerr << ") \n";

      cerr << "_i= ( " << _sim->_i[0];
      for(unsigned a=1; a <= _sim->_total_nodes; ++a){
        cerr << " " <<  _sim->_i[a];
      }
      cerr << ") \n";


      C.dezero( OPT::cmin ); 
      C.lu_decomp();

      _sim->_bypass_ok = false;

      if (_do_tran_step) { 
        do_tran_step();
      } 
        _sim->_mode = s_DC;

      { // some more AC stuff


        cerr << "_ac = ( " << _sim->_ac[0];
        for(unsigned a=1; a <= _sim->_total_nodes; ++a){
          cerr << " " <<  _sim->_ac[a];
        }
        cerr << ") \n";

        if(_dump_matrix){
          _out  << "RS\n";
          //_out << vector<voltage_t>(Gu,_sim->_total_nodes) << "\n";
        }

        cerr << ") \n";
        double dv[_sim->_total_nodes];
        C.fbsub( dv, Gu , dv );

        cerr << " solution = ( " << dv[0];
        for(unsigned a=1; a <= _sim->_total_nodes; ++a){
          cerr << " " << dv[a];
        }
        cerr << ") \n";


        for(unsigned a=0; a <= _sim->_total_nodes; ++a){
          // stash hack
          _sim->_vt1[a] = dv[a];
        }
      }
       
      outdata(_sweepval[Nest]);

      // here??
      CARD_LIST::card_list.tr_regress();
      itl = OPT::DCXFER;
    }else{
      sweep_recursive(Nest);
    }
  } while (next(Nest));
}
/*--------------------------------------------------------------------------*/
void DDCOP::first(int Nest)
{
  trace2("DDCOP::first", Nest, _start[Nest]);
  assert(Nest >= 0);
  assert(Nest < DCNEST);
  assert(_start);
  assert(_sweepval);
// assert(_pushel[Nest]);

  _sweepval[Nest] = _start[Nest];
  // here? (hack...)
  if(_pushel[Nest])
     _pushel[Nest]->set_ic(_start[Nest]);
  _reverse[Nest] = false;
  if (_reverse_in[Nest]) {itested();
    while (next(Nest)) {itested();
      /* nothing */;
    }
    _reverse[Nest] = true;
    next(Nest);
  }else{
  }
  _sim->_phase = p_INIT_DC;
}
/*--------------------------------------------------------------------------*/
bool DDCOP::next(int Nest)
{

  bool ok = false;
  if (_linswp[Nest]) {
    double fudge = _step[Nest] / 10.;
    if (_step[Nest] == 0.) {
      ok = false;
    }else{
      if (!_reverse[Nest]) {
	_sweepval[Nest] += _step[Nest];
	fixzero(_sweepval[Nest], _step[Nest]);
	ok=in_order(_start[Nest]-fudge,_sweepval[Nest],_stop[Nest]+fudge);
        _pushel[Nest]->set_ic(_sweepval[Nest]);
        trace1("DDCOP::next " + _pushel[Nest]->long_label(), _sweepval[Nest]);
	if (!ok  &&  _loop[Nest]) {
	  _reverse[Nest] = true;
	}else{
	}
      }else{
      }
      if (_reverse[Nest]) {
	_sweepval[Nest] -= _step[Nest];
	fixzero(_sweepval[Nest], _step[Nest]);
	ok=in_order(_start[Nest]-fudge,_sweepval[Nest],_stop[Nest]+fudge);
        _pushel[Nest]->set_ic(_sweepval[Nest]);
      }else{
      }
    }
  }else{
    double fudge = pow(_step[Nest], .1);
    if (_step[Nest] == 1.) {untested();
      ok = false;
    }else{
      if (!_reverse[Nest]) {
	_sweepval[Nest] *= _step[Nest];
	ok=in_order(_start[Nest]/fudge,_sweepval[Nest],_stop[Nest]*fudge);
        _pushel[Nest]->set_ic(_sweepval[Nest]);
	if (!ok  &&  _loop[Nest]) {untested();
	  _reverse[Nest] = true;
	}else{
	}
      }else{
      }
      if (_reverse[Nest]) {untested();
	_sweepval[Nest] /= _step[Nest];
	ok=in_order(_start[Nest]/fudge,_sweepval[Nest],_stop[Nest]*fudge);
        _pushel[Nest]->set_ic(_sweepval[Nest]);
      }else{
      }
    }
  }
  _sim->_phase = p_DC_SWEEP;
  return ok;
}
/*--------------------------------------------------------------------------*/
static DDC p2;
//static DOP p4;
static DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "ddc", &p2);
//static DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "dop", &p4);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
