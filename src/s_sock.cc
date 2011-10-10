/*$Id: s_dc.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
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
#include <unistd.h>
#include "u_prblst.h"
#include "u_cardst.h"
#include "u_nodemap.h"
#include "e_elemnt.h"
#include "e_storag.h"
#include "e_subckt.h"
#include "u_sock.h"
#include "s_ddc.h"
#include "io_error.h"
#include "resolv.h"
#include "s__.h"
#include "io_matrix.h"
#include "m_matrix_extra.h"
#include <iomanip>
#include "d_cap.h"


using namespace std;
using namespace SOME_CAP_HACK;
extern "C" {
#include "atlas/clapack.h"


int dgelss_(int *m, int *n, int *nrhs, double *a, int *lda, double *b, int
    *ldb, double *s, double *rcond, int *rank, double *work, int *lwork, int
    *info);

void dgels_(const char *trans, const int *M, const int *N, const int *nrhs,
    double *A, const int *lda, double *b, const int *ldb, double *work, const
    int * lwork, int *info);
} // exC

//
//extern "C" int clapack_dgesv(const enum CBLAS_ORDER Order, const int N, const int NRHS,
  //                    double *A, const int lda, int *ipiv,
    //                                    double *B, const int ldb);

/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
typedef union
{
  double double_val;
  int32_t   int_val;
} di_union_t;

/*--------------------------------------------------------------------------*/
class SOCK : public DDC_BASE {
public:
  void	finish();
  explicit SOCK();
  ~SOCK() {}
protected:
  void	fix_args(int);
  void	options(CS&, int x=0);
private:
  void	sweep();
  void	sweep_recursive(int);
  void	first(int);
  bool	next(int);
  void do_tran_step();
  void send_matrix();
  void undo_time_step();
  // explicit SOCK(const SOCK&): DDC_BASE() {unreachable(); incomplete();}
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
  unsigned port;
  double* U;
  double* CU;
  double* CUTCU;
public:
  void	do_it(CS&, CARD_LIST*);
private:
  void	setup(CS&);
  void fillnames( const CARD_LIST* scope);
  void findcaps( CARD_LIST* scope);
  vector<string> var_namen_arr;
  vector<DEV_CAPACITANCE*> cap_list;
  uint16_t var_namen_total_size; 

private: //vera stuff.
  void main_loop();
  void verainit();
  void verakons();
  void veraop();
  void verainit_send();
  void verakons_send();
  void veraop_send();

  char* var_names_buf;

  uint16_t verbose;
  size_t total;
  uint16_t n_inputs;
  vector<string> input_names;
  vector<CARD*> input_devs;
  uint16_t n_vars;
  uint16_t n_vars_square;
  uint16_t n_eingaenge;
  uint16_t length;

  SocketStream stream;
  //unsigned BUFSIZE;
  unsigned n_bytes;
  uint16_t error;

  double *dc_werteA,*dc_loesungA,*kons_loesungA,*kons_residuumA;

  double*x_schaetz;
  double*q_punkt;
  double*G;
  double*C;

  int channel;
  int frame_number;
  Socket* socket;
  short unsigned _port_range;
  string _port;   // kommt ueber die Uebergabeparamter port
                              // globale Variable daher (default: port=1400)
  bool _client_mode;
  string _host;
  int reuseaddr;
  struct sockaddr_in sin;

  unsigned char opcode;

  double *matrixg, *matrixc,*vectorq;

  static const int printlevel=0;
#define userinfo( a,b,c,d,e,f ) 

};
/*--------------------------------------------------------------------------*/
double	SOCK::temp_c_in = 0.;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SOCK::do_it(CS& Cmd, CARD_LIST* Scope)
{
  trace0("SOCK::do_it");
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
  reuseaddr=0;
  _port = "1400";   // kommt ueber die Uebergabeparamter -p als
  _port_range = 1;
  _client_mode = false;
  _host = "localhost";

  command_base(Cmd);

  //cleanup
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SOCK::SOCK() :DDC_BASE(),
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
    ::error(bWARNING, "did not converge\n");
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
  trace0("SOCK::setup");
  _cont = false;
  _trace = tNONE;
  _out = IO::mstdout;
  _out.reset(); //BUG// don't know why this is needed */
  bool ploton = IO::plotset  &&  plotlist().size() > 0;

  options(Cmd);

  if (Cmd.more()) {
  }else{ 
  }
  Cmd.check(bWARNING, "what's this?");

  _sim->_uic = _sim->_more_uic = true;
  CKT_BASE::_sim->init();

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);

  error = 0; /* verainit(v_flag, n_inputs, &n_vars, charbuf, &length); */
  n_vars = static_cast<uint16_t>( _sim->_total_nodes) ; // _sim->total_nodes doesnt include gnd
  var_namen_arr.resize( n_vars, string("unset"));
//  var_namen_arr[0]="0";
  fillnames( &CARD_LIST::card_list );
  n_vars_square = (uint16_t)(n_vars * n_vars);

#ifndef NDEBUG
    for (unsigned i=0; i < n_vars; i++)
      trace0("name: " + var_namen_arr[i]);
#endif


  assert(_n_sweeps > 0);
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
  trace0("SOCK::options... ");

  _sim->_uic = _loop[Nest] = _reverse_in[Nest] = false;
  _sim->_more_uic = true;
  _port = "1400";
  unsigned here = Cmd.cursor();
  do{
    ONE_OF
      || (Cmd.match1("'\"({")	&& ((Cmd >> _step_in[Nest]), (_stepmode[Nest] = LIN_STEP)))
      || (Cmd.is_float()	&& ((Cmd >> _step_in[Nest]), (_stepmode[Nest] = LIN_STEP)))
      || (Get(Cmd, "*",		  &_step_in[Nest]) && (_stepmode[Nest] = TIMES))
      || (Get(Cmd, "+",		  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || Get(Cmd, "c{ontinue}",   &_cont)
      || Get(Cmd, "port" ,        &_port)
      || Get(Cmd, "listen{port}", &_port)
      || Get(Cmd, "host" ,        &_host)
      || Get(Cmd, "tr{s}",        &_do_tran_step)
      || Get(Cmd, "dm",           &_dump_matrix)
      || Get(Cmd, "client",       &_client_mode)
      || Get(Cmd, "dt{emp}",	  &temp_c_in,   mOFFSET, OPT::temp_c)
      || Get(Cmd, "lo{op}", 	  &_loop[Nest])
      || Get(Cmd, "re{verse}",	  &_reverse_in[Nest])
      || Get(Cmd, "te{mperature}",&temp_c_in)
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
  // later... FIXME
  //
  frame_number=0;
  n_bytes = 0;
  x_schaetz = new double[BUFSIZE];
  q_punkt = new double[BUFSIZE];
  G = new double[BUFSIZE*BUFSIZE];
  C = new double[BUFSIZE*BUFSIZE];
  trace1("SOCK::do_it", _port);

  if (_client_mode){
    socket = new ClientSocket(Socket::TCP, _port, _host);
    trace1("connected to "+ _host, _port );
    stream = *socket;

  } else {
    stringstream p(_port);
    uint16_t _port_;
    p>>_port_;
    socket = new ServerSocket(Socket::TCP, _port_, _port_range);
    ServerSocket* sock=prechecked_cast<ServerSocket*>(socket);

    trace0("SOCK::do_it waiting");
    stream = sock->listen();
  }
  trace0("SOCK::do_it have stream");
    
  if(!socket){
    ::error(bDANGER,"Error, cannot create Socket\n");
    throw Exception("foo");
  }

  /* Wiederverwendung der lokalen Adresse erlauben */
//  if (setsockopt(channel, SOL_SOCKET, SO_REUSEADDR, (void *) &reuseaddr, 
//		 sizeof(reuseaddr)) == -1)
//  {
//    printf("Error, cannot set Socketoptions\n");
//    exit(1);
//  }

  //setup(Cmd);
  main_loop();
  // what am i doing here??
  return ;
  assert(false);
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
  assert(false);
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
	::error(bWARNING, "did not converge\n");
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
      ac_snapshot();

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
      _sim->set_inc_mode_bad();

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
  assert(false);
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
// fetch names from circuit recursively. fill into local vector.
void SOCK::fillnames( const CARD_LIST* scope){
  trace0("SOCK::fillnames");

  const NODE_MAP * nm = scope->nodes();
  for (NODE_MAP::const_iterator i = nm->begin(); i != nm->end(); ++i) {
    if (i->first != "0") {
      stringstream s;
      string myname(i->second->long_label());

      var_namen_arr[i->second->matrix_number()-1] = myname;
      var_namen_total_size = static_cast<uint16_t>( var_namen_total_size + static_cast<uint16_t>(myname.length()) + 1 );

    }else{
      // _out << "Zero Node  "  << "\n";
    }
  }

  //for (CARD_LIST::const_iterator i = scope->begin(); i != scope->end(); ++i) {
  for (CARD_LIST::const_iterator i = scope->begin(); i != scope->end(); ++i) {
    if (   const BASE_SUBCKT* s = dynamic_cast<const BASE_SUBCKT*>(*i) )
    {
      fillnames( s->subckt() );
    }
  }
}
/*--------------------------------------------------------------------------*/
void SOCK::findcaps( CARD_LIST* scope){
  for (CARD_LIST::iterator i = scope->begin(); i != scope->end(); ++i) {
    if ( DEV_CAPACITANCE* cap = dynamic_cast< DEV_CAPACITANCE*>(*i) )
    {
      trace1("found cap", cap->long_label());
      cap_list.push_back( cap );
    }
    if ( BASE_SUBCKT* s = dynamic_cast< BASE_SUBCKT*>(*i) )
    {
      findcaps( s->subckt() );
    }
  }
}
/*--------------------------------------------------------------------------*/
bool SOCK::next(int Nest)
{

  assert(false);

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

#if 0

// Treiber zum Einlesen der Daten aus Maple               
//		          vera_nl
// Er formt die TParameter in C-Datentypen um. Dabei werden auch
// die Nummern der Variablen neu vergeben von 0-1000
// Die Nummern der Parmeter werden ebenfalls neu vergeben von 1000- 
// Die Variablen werden dann in dieser Reihenfolge in Felder geschrieben.
//                              
// Parameter:  TParameter: Liste der Parameter in der Reihenfolge:
//                         A_Variablennamensliste,
//                         A_Gleichungen,
//                         A_Jacobimatrix,
//                         A_symbolisches,linearisiertes Gleichungssystem
//                         A_Startvektor
//                           
// Rueckgabewert : TParamter: NULL
//   
// L.Hedrich 2002

#define BUFSIZE 256


TParameter *vera_titan_ak(TParameter *parameter)
{


  char input_namen[BUFSIZE*128];
  char var_names[BUFSIZE*128];
 
  di_union_t buffer[3*BUFSIZE*BUFSIZE];
  // werden beneotigt um im DC-OP ein Hold node bzw
  // Schalten eines Transistors fuer z.B. den VCO zu bewirken
  // dazu im Gl-System einen Parameter init_sw so einbauen,
  // dass er im DC-OP auf 1 einen stabilen OP ergibt
  // und sonst im kons_op auf 0 das instabile System hervorruft. 
  double *init_sw=(double*)malloc(sizeof(double)*1);
  char **init_sw_name;
  init_sw_name=(char **)malloc(1*sizeof(char*));
  init_sw_name[0]=(char*)"init_sw";


  if (printlevel >= 3)
  {
    outfile=fopen("call_c.log","w");
  }
  else
  {
    outfile=stderr;
  }

  // Daten einlesen
  para_obj=new get_tpara(parameter);   // Objekt vom Typ Parameter anlegen

  // Hier erfolgt der Aufruf eines Objektes welches die globale Datenstruktur
  // enthaelt. Der Konstruktor erhaelt die Paramerter und arbeitet sie ab.

  A=new data(&para_obj);         // Daten fuer die Schaltung A einlesen

  //printlevel=3;
  delete para_obj;              // letztes PArameterobjekt zerstoeren


  dc_sysA=A->make_dc_system();

  // Gleichungssystem erstellen
  kons_sysA=A->make_dc_system();


  if (printlevel >= 3)
  {
    userinfo(3,"vera_titan_ak","Jetzt das DC-System:\n");
    dc_sysA->print();
  }

  matrixg=(double*)malloc(sizeof(double)*A->n_var*A->n_var);
  matrixc=(double*)malloc(sizeof(double)*A->n_var*A->n_var);
  vectorq=(double*)malloc(sizeof(double)*A->n_var*A->n_eingaenge);



#if 0
  sleep(1);
  if (printlevel >= 2)
  {
    double *dc_werteA=(double*)malloc(sizeof(double)*A->n_eingaenge);
    dc_werteA[0]=0.1;     

    // DC-Arbeitspunkt berechnen
    dc_loesungA = dcop(A, dc_sysA ,dc_werteA);

    // Die Variablenwerte stehen schon durch solve_system in var_werte
    // in dc_sysA und muessen noch nach A kopiert werden:
    A->var_werte=dc_loesungA;

    A->eval_lin_gl(dc_werteA,&matrixg,&matrixc,&vectorq);
    if (printlevel >= 1) 
    {
      userinfo(1,"vera_titan_ak","G und C und q Matrizen:");  
      print_array_name (stderr,matrixg,A->n_var,A->n_var,(char*)"G");
      print_array_name (stderr,matrixc,A->n_var,A->n_var,(char*)"C");
      print_array_name (stderr,vectorq,A->n_var,A->n_eingaenge,(char*)"q");
    }       

    // Nun auch den konsistenten Arbeitspunkt testen:
    for (i=0; i < A->n_var; i++)
    {
      x_schaetz[i] = dc_loesungA[i]+0.01*i;
    }


    kons_loesungA=konsop(A, kons_sysA,dc_werteA,x_schaetz,&kons_residuumA);


    // jetzt noch G,C und q wie bei DC-Loesung

    // Die Variablenwerte stehen schon durch solve_system in var_werte
    // in dc_sysA und muessen noch nach A kopiert werden:
    A->var_werte=kons_loesungA;
    A->eval_lin_gl(dc_werteA,&matrixg,&matrixc,&vectorq);
    if (printlevel >= 1) 
    {
      userinfo(1,"vera_titan_ak","G und C und q Matrizen:");  
      print_array_name (stderr,matrixg,A->n_var,A->n_var,(char*)"G");
      print_array_name (stderr,matrixc,A->n_var,A->n_var,(char*)"C");
      print_array_name (stderr,vectorq,A->n_var,A->n_eingaenge,(char*)"q");
    }          
  } // if printlevel >= 2
#endif



#endif

void SOCK::main_loop(){
  trace0("SOCK::main_loop");

  bool init_done=false;
  while(1) 
  {
    trace0("SOCK::main_loop waiting for opcode");
       
    stream >> opcode;
    stream >> 7;

    trace1("SOCK::main_loop", (int)opcode);
    //trace1(" Naechste Anforderung \n",n_bytes);

    switch (opcode)  
    {
      case '3': // 51
        if(init_done) throw Exception("init twice??");
        verainit();
        verainit_send();
        init_done=true;
        break;
      case '4':  // 52
        veraop();
        veraop_send();
        break;
      case '5':  // 53
        verakons();
        verakons_send();
        break;

      default:
        trace0("error");
        ::error(bDANGER, "unknown opcode %i\n", opcode);
        throw Exception("unknown opcode");
        break;

    }
  }
}
//==========================================================
// very clever way to transfer strings.
static void putstring8(SocketStream* s, const string x){
  const char* A = x.c_str();

  while(*A){
    *s<<*A<<*A<<*A<<*A<<*A<<*A<<*A<<*A;
    A++;
  }
}
//==========================================================
void SOCK::verainit(){
  trace0("SOCK::verainit");
  stream >> verbose >> 6;
  stream >> n_inputs >> 6;
  stream >> length >> 6;

  trace3("SOCK::verainit", verbose, n_inputs, length );

  char input_namen[length+1];
  unsigned here =0;
  unsigned n=0;
  input_names.resize(n_inputs);
  input_devs.resize(n_inputs);

  for (unsigned i=0; i < length; i++)
  {
    stream >> input_namen[i] >> 7;
    if(input_namen[i] == '\t'){
      input_namen[i]=0;
      input_names[n++]=string(input_namen+here);
      here = i;
      trace0(input_names[n-1]);

      trace1("looking out for", input_names[n-1]);
      CARD_LIST::fat_iterator ci = findbranch(input_names[n-1], &CARD_LIST::card_list);
      if (ci.is_end()){
        throw Exception("cannot find voltage source \"" +  input_names[n-1] +"\", giving up");
      }
      input_devs[n-1] = (*ci);
      assert(input_devs[n-1]);


    }
  }


  //trace0("input_namen " + string(input_namen) );
  total = (unsigned) (length+4);
  assert(3*BUFSIZE*BUFSIZE >= total);

  if (!stream.at_end())
  {
    printf("Error in Verainit! no of bytes received %i <> expected %i\n",
        n_bytes, (int)(total*sizeof(di_union_t)));
    throw Exception("bloed\n");
  }


  assert(!var_names_buf);
  var_names_buf = (char*) malloc( BUFSIZE * sizeof(char));
  strcpy(var_names_buf,"");
  for (unsigned i=0; i < n_vars; i++)
  {  
    trace0("name: " + var_namen_arr[i]);
    strcat(var_names_buf, var_namen_arr[i].c_str());
    strcat(var_names_buf, "\t");
  }
  //length = static_cast<uint16_t>( strlen(var_names_buf) );
  // userinfo(1,"vera_titan_ak","Variablennamen %s\n",var_names_buf);
}

/*-------------------------------------------------------------*/
void SOCK::veraop(){
  total = n_vars;
  assert(3*BUFSIZE*BUFSIZE >= total);

  dc_werteA = (double*) malloc(sizeof(double)*n_vars);
  trace1("fetching ",n_vars);
  assert(_sim->_vdc[0] == 0 );
  for (unsigned i=0; i < n_inputs; i++)
  {
    double d;
    stream >> d;
    trace2("setting input " + input_devs[i]->long_label(), i, d);
    asserted_cast<ELEMENT*>(input_devs[i])->set_value(d);
  }

  error = 0; /* veraop(sweep_val, x_new, G, C); */

  // ================do_dc========
  _sim->_uic = false;
  _sim->_more_uic = false;
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
  OPT::ITL itl = OPT::DCBIAS;

  trace0("SOCK::veraop, hot");
  _trace=tVERBOSE;
  CARD_LIST::card_list.tr_begin();  // hier muesste eigentlich eine dc hin.
  try{
    solve_with_homotopy(itl,_trace);
  }catch( Exception e) {
    ::error(bDANGER, "hot failed\n");
    throw e;
  }
  trace0("SOCK::veraop, hot done");

  ::status.accept.start();
  _sim->set_limit();
  CARD_LIST::card_list.tr_accept();
  ::status.accept.stop();

  _sim->keep_voltages();
  //========================

  // Die Variablenwerte stehen schon durch solve_system in var_werte
  // in dc_sysA und muessen noch nach A kopiert werden:
  //
  //A->var_werte = x_neu;
  //free(dc_loesungA);
  //
  //        A->var_werte = dc_loesungA;
  // diff_werte, par_werte muessen auch gesetzt sein!
  // in diesem Falle sind sie per default = 0
  //
  //  solve  somthing
  //
  //  A->eval_lin_gl(dc_werteA,&matrixg,&matrixc,&vectorq);

  trace1("matrix", 1);
}
//==========================================================
void SOCK::verakons() {
  //        n_eingaenge == #caps?
  total =  n_eingaenge + n_vars + 1;


  for (unsigned i=0; i < n_inputs; i++)
  {
    double d;
    stream >> d;
    trace2("setting input " + input_devs[i]->long_label(), i, d);
    asserted_cast<ELEMENT*>(input_devs[i])->set_value(d);
  }

  for (unsigned i=0; i < n_vars; i++)
  {
    stream >> _sim->_vdc[i];
  }

  error = 0; /* verakons(Dwork, x_new, q_dot, G, C); */
  //	n_vars = A->n_var;

  // Konsistenten Arbeitspunkt und q_punkt  berechnen
  // Es wird davon ausgegangen, das dc_werteA noch stimmt
  //


  for( unsigned i = 0; i<cap_list.size(); i++)
  {
    cap_list[i]->keep_ic(); // latch voltage applied to _vdc
  }
  //
  //================do_dc========
  _sim->_uic = true;
  _sim->_more_uic = true;
  OPT::ITL itl = OPT::DCBIAS;

  trace0("SOCK::verakons, hot");
  _trace=tVERBOSE;
  CARD_LIST::card_list.tr_begin();
  try{
    solve_with_homotopy(itl,_trace);
  }catch( Exception e) {
    ::error(bDANGER, "hot failed\n");
    throw e;
  }

  _sim->keep_voltages();
}
/*-==========================================================*/
void SOCK::verakons_send()
{
  stream << ((uint16_t)error); stream.pad(6);
  for (unsigned i=0; i < n_vars; i++)  
  {
    stream << _sim->_vdc[i];
  }

  // BUG
  // vera wants just cap stamps
  //
  // do something like
  // _i=0;
  // cap_list->tr_load();
  //
  // problem: tr_load just cares about differences, 
  // this call would only add 0
  //

  // this could work:
  // std::fill_n(_i,  _total_nodes+1, 0);
  // cap_list->tr_unload();
  //
  // now i contains the negative sum of the cap value.
  // does this break anything?

  for (unsigned i=0; i < n_vars; i++)         /* Q-Punkt == I == RS */
  {
    //buffer[i+n_vars+1].double_val = q_punkt[i];
    trace1("sending dqdt\n", _sim->_i[i]);
    stream << _sim->_i[i];
  }

  ac_snapshot(); // FIXME: to verakons()

  send_matrix();

  assert(3*BUFSIZE*BUFSIZE >= total);
  stream << SocketStream::eol;
  trace0("done SOCK::verakons_send");
}
/*--------------------------------------------*/
void SOCK::verainit_send()
{
  stream << error;   stream.pad(6);
  stream << int32_t (n_vars);  stream.pad(4);
  stream << int32_t (var_namen_total_size);  stream.pad(4);     /* Laenge des Namen Feldes */

  trace4("SOCK::verainit_send ", error, n_vars, length, var_namen_total_size);

  assert(stream.tcur() == 24);

  for (unsigned i = 0; i < n_vars; i++) 
  {
    putstring8( &stream, var_namen_arr[i]);
    stream << '\t'; stream.pad(7);
  }

//good idea?? not yet. vera insists on branch node
//
//  for (unsigned i=0; i < n_inputs; i++)  
//  {
//    trace1("putting name " +  input_names[i], i);
//    putstring8( &stream, input_names[i]);
//    stream << '\t'; stream.pad(7);
//  }
  stream.flush();

  trace0("done verainit_send");
}
/*------------------------------------*/
void SOCK::veraop_send()
{
  assert(n_vars==_sim->_total_nodes);
  trace1("SOCK::veraop_send ", n_vars);
  stream << error; stream.pad(6);
  for (unsigned i=0; i < n_vars; i++)         /* Variablen-Werte */
  {
    stream << _sim->_vdc[i];
  }
  if(_dump_matrix){
    _out << "i,u: \n";
    for (unsigned i=0; i < n_vars; i++)         /* Variablen-Werte */
    {
      _out << _sim->_i[i] << "," <<  _sim->_vdc[i] << "\n" ;
    }
  }

  trace0("SOCK::veraop_send taking ac snapshot (matrix only?).");
  ac_snapshot();
  send_matrix();
  stream << SocketStream::eol;

  trace0("SOCK::veraop_send sent");

  total = 2*n_vars_square+n_vars+1;
  assert(3*BUFSIZE*BUFSIZE >= total);
  if (printlevel >= 1)
  {
    userinfo(1,"vera_titan_ak","Sende: Error %i Framenumber %i, Laenge %i\n",
        error,frame_number,total); 
  }
}
/*-----------------------------------------*/
void SOCK::send_matrix()
{
  const BSMATRIX<double> G = _sim->_acx.real();
  const BSMATRIX<double> C = _sim->_acx.imag();
  if(_dump_matrix){
    _out << "G\n" << G << "\n";
    _out << "C\n" << C << "\n";
  }
  trace0("SOCK::send_matrix G");
  for (unsigned i=1; i <= n_vars; i++){
    for (unsigned j=1; j <= n_vars; j++) {
      stream << G.s(j,i);
    }
  }
  trace0("SOCK::send_matrix C");
  for (unsigned i=1; i <= n_vars; i++){
    for (unsigned j=1; j <= n_vars; j++) {
      stream << C.s(j,i);
    }
  }
}

// vim:ts=8:sw=2:et:
