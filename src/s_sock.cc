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
extern "C" {
#include "atlas/clapack.h"

int dgelss_(int *m, int *n, int *nrhs, double *a, int *lda, double *b, int
    *ldb, double *s, double *rcond, int *rank, double *work, int *lwork, int
    *info);

void dgels_(const char *trans, const int *M, const int *N, const int *nrhs,
    double *A, const int *lda, double *b, const int *ldb, double *work, const
    int * lwork, int *info);
}

//
//extern "C" int clapack_dgesv(const enum CBLAS_ORDER Order, const int N, const int NRHS,
  //                    double *A, const int lda, int *ipiv,
    //                                    double *B, const int ldb);

/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class SOCK : public SIM {
public:
  void	finish();
  explicit SOCK();
  ~SOCK() {}
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
  explicit SOCK(const SOCK&): SIM() {unreachable(); incomplete();}
protected:
  
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
  double* U;
  double* CU;
  double* CUTCU;
public:
  void	do_it(CS&, CARD_LIST*);
private:
  void	setup(CS&);
};
/*--------------------------------------------------------------------------*/
double	SOCK::temp_c_in = 0.;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SOCK::do_it(CS& Cmd, CARD_LIST* Scope)
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
SOCK::SOCK()
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
void SOCK::finish(void)
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
void SOCK::do_tran_step()
{

  SIM_PHASE old_phase = _sim->_phase;
  trace1("SOCK::do_tran_step", OPT::dtddc);
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
  trace0("SOCK::sweep_recursive solved a transient step");


  _sim->set_limit();


  CARD_LIST::card_list.tr_accept();
  trace0("SOCK::sweep_recursive itr_accepted");

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
void SOCK::setup(CS& Cmd)
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
          trace0("SOCK::setup " + _zap[_n_sweeps]->long_label());
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
void SOCK::fix_args(int Nest)
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
void SOCK::options(CS& Cmd, int Nest)
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
void SOCK::sweep()
{
  head(_start[0], _stop[0], " ");
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
  if (_cont) {untested();
    _sim->restore_voltages();
  }else{
  }

  unsigned d = _sim->_total_nodes; // 3
  U = new double[d*d];
  CU = new double[d*d];
  CUTCU = new double[d*d];
  
  _sim->clear_limit();
  CARD_LIST::card_list.tr_begin();
  sweep_recursive(_n_sweeps);
}
/*--------------------------------------------------------------------------*/
void SOCK::sweep_recursive(int Nest)
{
  unsigned d = _sim->_total_nodes; // 3

  trace1("SOCK::sweep_recursive", Nest);
  --Nest;
  assert(Nest >= 0);
  assert(Nest < DCNEST);

  double iddc[d];
  double dv[_sim->_total_nodes];

  OPT::ITL itl = OPT::DCBIAS;
  
  first(Nest);
  do {
    trace0("SOCK::sweep_recursive loopstart");
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

      if(_dump_matrix){
        _out << " ======================== \n";
        _out << "iddc ( " << _sim->_i[1];
        for(unsigned a=2; a <= d; ++a){
          _out << " " <<  _sim->_i[a];
        }
        _out  << ") \n";
      }

      for(unsigned a=0; a <= _sim->_total_nodes; ++a){
        iddc[a]=_sim->_i[a];
      }


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
      if(_dump_matrix){
        _out << "i ( " << _sim->_i[1];
        for(unsigned a=2; a <= _sim->_total_nodes; ++a){
          _out << " " <<  _sim->_i[a];
        }
        _out  << ") \n";
        _out << "v0 = ( " << _sim->_v0[1];
        for(unsigned a=2;a <= _sim->_total_nodes; ++a){
          _out << " " <<  _sim->_v0[a];
        }
        _out << ") \n";
      }

      // if verbose
      _sim->_uic=_sim->_more_uic=false;


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
  //    BSMATRIX<double> S = _sim->_acx.sum();

      if(_dump_matrix){
        _out << "G\n" << G << "\n";
        _out << "C\n" << C << "\n";
      }

      double Gul[d+1];
      double* Gu = Gul+1;

      // Gu = G * v0
      G.rmul(Gul, _sim->_v0);

      G.lu_decomp();

      double col[d+1];
      double CU[d*d];

      // U = G^{-1} C (column-major)
      for( unsigned i=0; i<d; ++i){
        C.col(col,1+i);
        double buf[d+1];

        // compute ith column of U
        G.fbsub(buf, col);
        for (unsigned k=0; k<d; ++k ){
            U[k+i*d] = buf[k+1];
        }
        if(_dump_matrix){
          _out << "U row " << i << ":  " << U[0 + i*d];
          for(unsigned a=1; a < d; ++a){
            _out << " " <<  U[a + i*d ];
          }
          _out  << ") \n";
        }
      }

      // CU=C*U (col maj)
      for(unsigned i=0; i < d; ++i){
        for(unsigned j=0; j < d; ++j){
          CU[i+j*d] = 0;
          for(unsigned k=0;k < d; ++k){
            CU[i+j*d] += ((const BSMATRIX<double>)C).s(i+1,k+1) * U[k+j*d];
          }
        }
      }

      double RS[d];
      double X[d];
      // fetch rhs
      for(unsigned a = 0; a < d; ++a){
        RS[a] = - Gu[a] + _sim->_i[a+1] ;
        X[a] = - Gu[a] + _sim->_i[a+1] ;
      }

      if(_dump_matrix){
        _out << "RS ( " << RS[0];
        for(unsigned a=1; a < d; ++a){
          _out << " " <<  RS[a];
        }
        _out  << ") \n";
      }

      // X = CU.trans * App
      //cblas_dgemv(CblasColMajor, 
      //    CblasTrans, d, d,
      //    1.0, 
      //    CU, d,
      //    RS, 1, 0,
      //    X,1);

      if(_dump_matrix){
        _out << "X ( " << X[0];
        for(unsigned a=1; a < d; ++a){
          _out << " " <<  X[a];
        }
        _out  << ") \n";
      }

      // U = G^{-1} C
      // want to solve C x = i - Gu, and G x = C y <=> x = U y
      // solving C U y = i - Gu
      
      double alpha=1;
      double wurk;
      int rank;
      int lwork=-1;
      int info=0;
      double rcond=0;
      int D=d;
      int one=1;

      // does not work (no full rank)
      // clapack_dgesv(CblasColMajor, d, 1,
      //             CU, d, ipiv, X, d);



//void dgels_(const char *trans, const int *M, const int *N, const int *nrhs,
//    double *A, const int *lda, double *b, const int *ldb, double *work, const
//    int * lwork, int *info);
/*
      dgels_("No transpose", &D,&D,&one, CU, &D, RS, &D, &wurk, &lwork, &info );
      assert(!info);
      lwork = (int)wurk;
      double work[lwork];
      dgels_("No transpose", &D,&D,&one, CU, &D, RS, &D, work, &lwork, &info );

      */
       double S[d]; //singular values
       dgelss_(&D,&D,&one,CU, &D, RS, &D, S, &rcond, &rank, &wurk, &lwork, &info );
       assert(!info);
       lwork = (int)wurk;
       double work[lwork];
       dgelss_(&D,&D,&one,CU, &D, RS, &D, S, &rcond, &rank, work, &lwork, &info );
      

      if(info){
        cout<<info << "\n";
        exit(info);
      }

      if(_dump_matrix){
        _out << "after dgels " << lwork  << " ( " << RS[0];
        for(unsigned a=1; a < d; ++a){
          _out << " " <<  RS[a];
        }
        _out  << ") \n";
      }

      // dv = U * app 
      cblas_dgemv(CblasColMajor, 
          CblasNoTrans, d, d,
          1.0/alpha, 
          U, d,
          RS, 1, 0,
          dv,1);


      if(_dump_matrix){
        _out << "Gu ( " << Gu[0];
        for(unsigned a=1; a < d; ++a){
          _out << " " <<  Gu[a];
        }
        _out  << ") \n";
      }

      for(unsigned a=0; a < d; ++a){
        Gu[a] = - Gu[a] +  _sim->_i[a+1] ;
      }


      C.dezero( OPT::cmin ); 
      C.lu_decomp();

      _sim->_bypass_ok = false;

      if (_do_tran_step) { 
        do_tran_step();
      } 
        _sim->_mode = s_DC;

      { // some more AC stuff

        if(_dump_matrix){
          _out << "RS ( " << Gu[0];
          for(unsigned a=1; a < d; ++a){
            _out << " " <<  Gu[a];
          }
          _out  << ") \n";
        }

//        irgendwoher di/du holen...
        //C.fbsub( dv, Gu , dv );

        for(unsigned a=0; a < d; ++a){
          // stash hack
          _sim->_vt1[a+1] = dv[a];
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
void SOCK::first(int Nest)
{
  trace2("SOCK::first", Nest, _start[Nest]);
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
bool SOCK::next(int Nest)
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
        trace1("SOCK::next " + _pushel[Nest]->long_label(), _sweepval[Nest]);
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
static SOCK p2;
static DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "sock", &p2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
