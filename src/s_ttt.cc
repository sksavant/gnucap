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
#include "s_tr.h"



// #include "globals.h" ??
#include "e_adplist.h"
/*--------------------------------------------------------------------------*/
#define sanitycheck()  assert ( 0.99 * _sim->_Time0 <= _Time1 + _sim->_dT0 );\
                       assert ( 1.01 * _sim->_Time0 >= _Time1 + _sim->_dT0 );\
                       assert ( _sim->_Time0 > _Time1 || _sim->_Time0==0 );
/*--------------------------------------------------------------------------*/

namespace {
/*--------------------------------------------------------------------------*/
class TTT : public TRANSIENT {
  public:
    void	do_it(CS&, CARD_LIST*);
    std::string status()const;

    explicit TTT():
      TRANSIENT(),
      _Tstart(0.), // ?? unused?
      _Tstop(0.),
      _Tstep(0.),
      _timesteps(0),
      _fdata_tt(NULL),
      _tt_store(0)  { }
    ~TTT() {}
    void tt_advance_time();
  public:
    int step_tt_cause()const;
    void	advance_Time();

  private:
    explicit TTT(const TTT&): 
      TRANSIENT(),
      _Tstart(0.), // ?? unused?
      _Tstop(0.),
      _Tstep(0.),
      _timesteps(0),
      _fdata_tt(NULL)
    {unreachable(); incomplete();}
    void	setup(CS&);	/* s_fo_set.cc */
    void	allocate();
    void  rescale_behaviour();
    void	unallocate();
    bool	next();
    void	out_tt();
    void	outdata_tt(double);
    void	outdata_b4(double);
    bool	review_tt();
    void	options(CS&);
    void	sweep();
    void	sweep_tt();
    double get_new_dT();
    void	accept_tt();
    void	print_head_tr();
    void	head(double,double,const std::string&);
    void	head_tt(double,double,const std::string&);
    void	set_step_tt_cause(STEP_CAUSE);
    void	first();
    void  first_after_interruption();
    void	fohead(const PROBE&);
    void	store_results(double); // override virtual
    void	store_results_tt(double); 
    void	print_stored_results_tt(double); 
  private:
    PARAMETER<double> _Tstart;	// unused?
    PARAMETER<double> _Tstop;	/* user stop Time */
    PARAMETER<double> _Tstep;	/* user Tstep */
    PARAMETER<double> _dTmin_in;
    PARAMETER<double> _dTratio_in;
    int    _timesteps;	/* number of time steps in tran analysis, incl 0 */
    int    _Timesteps;	/* number of time steps in tt analysis, incl 0 */
    int print_tr_probe_number;
    double _Time1;
    int    steps_total_tt;
    double behaviour_time();
    double _time_by_adp;
    double _dT_by_adp;
    double _time_by_beh;
    double _dT_by_beh;
    double _dTmin;
    double _dTmax;
    void   outdata(double);
    void   print_results(double);
    void   print_results_tt(double);
    double _Time_by_user_request;
    bool   _accepted_tt;
    COMPLEX** _fdata_tt;	/* storage to allow postprocessing */
    double*   _tt_store;
}; // TTT : TRANSIENT
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
  ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_clear ); 

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

  outdata_b4(_sim->_Time0); // first output tt data
  ::status.tran.reset().start();

  CARD_LIST::card_list.tr_begin();

// dashier nach TTT::first()?
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

  if (OPT::tracett)
    _out << "* accepting first " << _sim->tt_iteration_number() << "\n";


  accept_tt();
  outdata_tt(_sim->_Time0); // first.

} //first
/*--------------------------------------------------------------------------*/
// why not directly run into loop?
void TTT::first_after_interruption(){
    _sim->force_tt_order(0); assert(_sim->get_tt_order() == 0 );
    time1 = 0.;
    _sim->_dT0=_sim->_last_time;

    if (OPT::tracett)
      _out << "* first after int " << _sim->tt_iteration_number() << "\n";

    // huh?
    // CARD_LIST::card_list.do_forall( &CARD::stress_apply );

    outdata_b4(_sim->_Time0);
    CARD_LIST::card_list.do_forall( &CARD::tt_next );

    _inside_tt=true;
    TTT::sweep();
    _inside_tt=false;

    set_step_tt_cause(scUSER);
    ++::status.hidden_steps;
    ::status.review.stop();

    _accepted_tt = true;
    accept_tt(); 
    outdata_tt(_sim->_Time0); // first.
}
/*--------------------------------------------------------------------------*/
// do a 2nd tt to check aging impact.
// ie. take some norm of node-voltage difference over time.
/*--------------------------------------------------------------------------*/
void TTT::sweep_tt()
{
// FIXME  if no first then recycle data
  trace6( "TTT::sweep_tt() begin ", _Time1, _sim->_Time0, _Tstart, _Tstop, _Tstep, _sim->tt_iteration_number());
  if (OPT::tracett)
    _out << "* TTT::sweep_tt iteration:" << _sim->tt_iteration_number() << " "<<  _sim->_Time0 << "\n";
//  sanitycheck();
  _sim->_tt_iter = 0;
  _sim->_tt_done = 0;


  //end fixme
  head_tt(_tstart, _tstop, "TTime");
  assert( _sim->_mode  == s_TTT );
  assert(_sim->_Time0 >= 0 );

  if (_power_down){
    if (OPT::tracett)
      _out << "* TTT::sweep_tt after int power down\n";

    print_results_tt( _sim->_Time0 );
    _sim->zero_some_voltages();
    _sim->_dt0 = _Tstop;
    _sim->_dT0 = 0;

 //    ADP_LIST::adp_list.do_forall( &ADP_CARD::tt_commit );
    CARD_LIST::card_list.do_forall( &CARD::tr_stress );
    CARD_LIST::card_list.do_forall( &CARD::tr_stress_last );

    print_results_tt( _Tstop );
    _sim->_last_Time = _Tstop;

    return;
    
  //something.
  }else if( _Tstop == _Tstart ){
     trace0("TTT::sweep_tt just printing...");
     outdata_b4(_sim->_Time0); // first output tt data
     return;
  }else if ( !_tt_cont  ){
    if (OPT::tracett)
      _out << "* TTT::sweep_tt from 0\n";
    _sim->_dT0 = 0;
    _sim->_dT1 = 0;
    _sim->_dT2 = 0;
    CARD_LIST::card_list.do_forall( &CARD::tt_begin );

    first();
    trace0("TTT::sweep_tt first done");
  } else { // strange way of doing first next loop
    if (OPT::tracett)
      _out << "* TTT::sweep_tt after int" << _sim->tt_iteration_number() << " "<<  _sim->_Time0 << "\n";
    _sim->_dT0 = 0;
    _sim->_dT1 = 0;
    _sim->_dT2 = 0;
    first_after_interruption();

  }

  // assert (_sim->_loadq.empty());
  assert(_sim->_Time0 == _Tstart);

  //_time_by_beh=NEVER;
  //_time_by_adp=NEVER;

  assert (_Time1 == _Tstart);
  trace6( "TTT::sweep_tt entering next loop ", _sim->_Time0,  _Time1 ,
      _sim->_dT0 , _sim->_dT1 , _accepted , _accepted_tt ); 
  while(next())
  {
    trace5( "TTT::sweep_tt loop start ", _sim->_Time0, _Time1, _sim->_dT0, _accepted, _accepted_tt ); 
    sanitycheck();

 // FIXME   tt_eval_q
 // FIXME advance_Time vs tt_advance
    advance_Time();
    ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_commit );
    CARD_LIST::card_list.do_forall( &CARD::tt_commit ); // ?
    
    trace1( "TTT::sweep adp stuff ", _sim->_Time0 );
    ADP_LIST::adp_list.do_forall( &ADP_CARD::tt_commit );
    CARD_LIST::card_list.do_forall( &CARD::stress_apply );

    trace2( "TTT::sweep calling TTT::sweep", _cont, _sim->_Time0 );
    store_results_tt(_sim->_Time0); // first output tt data
    _inside_tt=true;
    // ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tr_reset );
    TTT::sweep();
    _inside_tt=false;

    if(!_accepted) 
    {
      std::cerr << "TTT::sweep: sweep complains\n";
      set_step_tt_cause(scREJECT);
      continue;
    }

    _accepted_tt = review_tt();

    if(! _accepted_tt ){
      trace3( "TTT::sweep_tt (loop) NOT accepted ", _sim->_Time0, _sim->_dT0, _dT_by_adp );
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
    print_stored_results_tt(_sim->_Time0);
    outdata_tt(_sim->_Time0); // first output tt data

  }

  _sim->_Time0=_sim->_Time0+_tstop;
  //advance_Time();
  ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tt_last );
  //CARD_LIST::card_list.do_forall( &CARD::tt_stress_last );
}
/*--------------------------------------------------------------------------*/
void TTT::sweep() // tr sweep wrapper.
{
  if (OPT::tracett) _out<< "sweep \n";
  // ADP_NODE_LIST::adp_node_list.do_forall( &ADP_NODE::tr_reset );
  //
  _sim->_mode=s_TRAN;
  int i= storelist().size();
  while(i--> 0) _sim->_waves[i].initialize();
  _sim->_mode=s_TTT;

  trace1("TTT::sweep() ", _inside_tt);

  CKT_BASE::tt_behaviour_rel = 0; // *= new_dT/(time0-time1);
  CKT_BASE::tt_behaviour_del = 0; //  *= new_dT/(time0-time1);
  //  ADP_LIST::adp_list.do_forall( &ADP::tt_commit );
  //
  if (_sim->_Time0 !=0 ){
    //_out << "stress  "  << Time0 << " , "<< Time1 <<"\n";
    //ADP_LIST::adp_list.do_forall( &ADP::tt_commit );
    CARD_LIST::card_list.tt_behaviour_commit( );
  }
  _sim->_time0=0.0;

  // if (_tt_cont) _inside_tt = true;
  try{
    TRANSIENT::sweep();
    if (OPT::tracett) _out<< "done sweep \n";
    CARD_LIST::card_list.do_forall( &CARD::tr_stress_last );
  }catch (Exception& e) {
    untested();
    std::cerr << "* " << e.message() <<  "\n";
    error(bDANGER, e.message() + '\n');
    _accepted=_accepted_tt=false;
    ::status.review.stop();

  //  _sim->restore_voltages();
  }

  trace1("TTT done TRANSIENT::sweep", _sim->_last_time);
//  std::cout << "* sweep done. last time: " << _sim->_last_time << " tstop " << _tstop << "\n";

  CKT_BASE::tt_behaviour_rel /= (_sim->_dT0);
  CKT_BASE::tt_behaviour_del /= (_sim->_dT0);
//  std:: << "TTT::sweep_tt brel: " << CKT_BASE::tt_behaviour_rel << "\n";
//  std:: << "TTT::sweep_tt bdel: " << CKT_BASE::tt_behaviour_del << "\n";
//  std:: << "TTT::sweep_tt brel normalized: " << CKT_BASE::tt_behaviour_rel << "\n";

  // hier gibs noch kein behaviour.  
  // ::status.tran.stop();
}
/*--------------------------------------------------------------------------*/
void TTT::accept_tt()
{
  if (OPT::tracett)
    _out << "* aceppting step " <<_sim->_tt_accepted << " at " <<  _sim->_Time0 <<"\n";
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

}
/*--------------------------------------------------------------------------*/
bool TTT::review_tt()
{
  bool tmp=true;
  trace2 ( "TTT::review_tt " , _sim->_Time0, _sim->_dT0 );
  if (OPT::tracett)
    _out << "review_tt " << _sim->_Time0 << "\n";
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
  if(OPT::tracett)
    _out << "review_tt done "<< tmp <<" "<< _time_by_adp << " " << _dT_by_adp <<"\n";
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
    if(OPT::tracett)
      _out << "* done setup\n";
  }catch (Exception& e) {itested();
    error(bDANGER, e.message() + '\n');
    return;
  }

  try {
    allocate();

    ::status.set_up.stop();
    switch (ENV::run_mode) {untested();
      case rPRE_MAIN:	unreachable();		break;
      case rBATCH:	untested();
      case rINTERACTIVE:  itested();
      case rSCRIPT:  sweep_tt(); //out_tt();
                     break;
      case rPRESET:	untested(); /*nothing*/ break;
    }

    unallocate();

  } catch (Exception& e) {itested();
    error(bDANGER, e.message() + '\n');
  }

  if(OPT::tracett)
    _out << "* unallocating\n";

  _sim->unalloc_vectors();
  _sim->_lu.unallocate();
  _sim->_aa.unallocate();
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
}
/*--------------------------------------------------------------------------*/
static TTT p10;
DISPATCHER<CMD>::INSTALL d10(&command_dispatcher, "twotimetran", &p10);
/*--------------------------------------------------------------------------*/
//testing=script 2007.11.21
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void TTT::setup(CS& Cmd)
{
  _tstart.e_val(NOT_INPUT, _scope);
  _tstop.e_val(NOT_INPUT, _scope);
  _tstep.e_val(NOT_INPUT, _scope);
  _Tstop.e_val(NOT_INPUT, _scope);
  _Tstep.e_val(NOT_INPUT, _scope);
  

  // nonsense
  // TRANSIENT::setup( Cmd);

  _cont = true;
  _tt_cont = true;
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
    }else{
    }
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg6;
    }else{
    }

    trace4("TTT::setup args ", arg1, arg2 , arg3 , arg4);

    if (arg4.has_hard_value()) {	    /* 4 args: all */
      _tt_cont=false;
      assert(arg3.has_hard_value());
      assert(arg2.has_hard_value());
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg3.e_val(0.,_scope);

      _tstep  = arg1;
      _tstop  = arg2;
      _Tstep  = arg3;
      _Tstop  = arg4;
      _Tstart = 0; //HACK?
      assert((double)_tstep!=0);

      trace4("TTT::setup ", _tstep, _tstop , _Tstep , _Tstop);

    } else if (arg3.has_hard_value() ) {
      _tt_cont = false;
      _Tstart = arg1;
      _Tstep = arg2;
      _Tstop = arg3;

    } else if (arg2.has_hard_value() ) {
      _Tstart = _sim->_last_Time;

      if(arg1<arg2){
        _Tstop  = arg2; 
        _Tstep = arg1;
      }else{
        _Tstop  = arg1; 
        _Tstep = arg2;
      }


      trace4("TTT::setup 2 args ", _tstep, _tstop , _Tstep , _Tstop);


    } else if (arg1.has_hard_value() ) {
      _Tstart = _sim->_last_Time;
      _Tstop  = arg1; // as tran

      // to trigger prints... (hack?)
      if(double(_Tstop) == 0) _Tstop = double( _Tstart );

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
  }
  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    Cmd >> _dtmax_in;
  }else{
  }

  if(OPT::tracett)
    _out << "* opt\n";
  options(Cmd);


  _tstart.e_val(0., _scope);
  _tstop.e_val(NOT_INPUT, _scope);
  _tstep.e_val(NOT_INPUT, _scope);

  //Time1 = 
  _sim->_Time0 = _Tstart;
  _Time1 = _Tstart;


  if (!_tstep.has_good_value()) {
    throw Exception("transient: Time step is required");
  }else if (_tstep==0.) {itested();
    throw Exception("Time step = 0");
  }else{
  }

  if (_dtmax_in.has_hard_value()) {
    _dtmax = _dtmax_in;
  }else if (_skip_in.has_hard_value()) {
    _dtmax = _tstep / double(_skip_in);
  }else{
    _dtmax = std::min(_dtmax_in, _tstep);
  }

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
    _tt_cont = false;
    _Time1 = _sim->_Time0 = 0.;
  }else{
    untested();
    _tt_cont = true;
    _Time1 = _sim->_Time0 = _sim->_last_Time;
  }

  // TRANSIENT setup

  _tstart.e_val(0., _scope);
  _tstop.e_val(NOT_INPUT, _scope);
  _tstep.e_val(NOT_INPUT, _scope);

  if  (_tt_cont){

  }else{
    _cont = false;
    time1 = _sim->_time0 = 0.;
  }
  //{}else{
  //  untested();
   // _cont = true;
  //  time1 = _sim->_time0 = _sim->_last_time;
 // }
  _sim->_freq = ((_tstop > _tstart) ? (1 / (_tstop - _tstart)) : (0.));

  if (!_tstep.has_good_value()) {
    throw Exception("transient: time step is required");
  }else if (_tstep==0.) {itested();
    throw Exception("time step = 0");
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
  trace4( "TTT::setup ",  steps_total_out_ , _tstep , _tstop ,_tstart );

}
/*--------------------------------------------------------------------------*/
void TTT::options(CS& Cmd)
{
  //good idea??
  TRANSIENT::options(Cmd); // parse options from tran. 
  if (OPT::tracett)
    _out << "*  pd "<< _power_down << " \n";
  return;

  _out = IO::mstdout;

  return;
  // _out.reset(); //BUG// don't know why this is needed
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  _trace = tNONE;
  _power_down = false;
  unsigned here = Cmd.cursor();
  do{
    ONE_OF
      || Get(Cmd, "p{owerdown}", &_power_down)
      || Get(Cmd, "c{old}",      &_cold)
      || Get(Cmd, "dte{mp}",	   &_sim->_temp_c,  mOFFSET, OPT::temp_c)
      || Get(Cmd, "dtma{x}",	   &_dtmax_in)
      || Get(Cmd, "dtmi{n}",	   &_dtmin_in)
      || Get(Cmd, "dtr{atio}",	 &_dtratio_in)
      || Get(Cmd, "pl{ot}",	     &ploton)
      || Get(Cmd, "sk{ip}",	     &_skip_in)
      || Get(Cmd, "te{mperature}", &_sim->_temp_c)
      || Get(Cmd, "uic",	   &_sim->_uic)
      || outset(Cmd,&_out)
      ;
  } while (Cmd.more() && !Cmd.stuck(&here));
  Cmd.check(bWARNING, "wHat's this?");

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);

  _dtmax_in.e_val(BIGBIG, _scope);
  _dTmin_in.e_val(OPT::dTmin, _scope);
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
  double step_grow_fact = 1.5;

  trace3( "TTT::next()", _sim->_Time0 ,  _Time1, _sim->tt_iteration_number() );

  assert(_sim->_Time0 >=0);

  if( !_accepted_tt ){

    new_dT = 0.9 * ( _sim->_dT0 - _tstop ) + _tstop; // back at least this much.
    if( _time_by_beh < _sim->_Time0 ){
      assert(_time_by_beh >= 0);
      new_dT= fmin(new_dT,_dT_by_beh);
    }
    if ( _time_by_adp < _sim->_Time0 ){
      assert(_time_by_adp >= 0);
      new_dT = fmin(new_dT, _dT_by_adp);
    } 

    assert(new_dT == new_dT );
    trace1( "TTT::next after reject: ", _Time1 );

    if ( OPT::tracett) 
      _out << "* retry step " << new_dT << " ( " << _dT_by_adp << " " <<
        _sim->_dT1  << " " << _sim->_dT0   << " )\n";
   
  } else { // accepted step. calculating new_dT

    _Time1 = _sim->_Time0;       // FIXME

    assert ( _Time1 == _sim->_Time0 ); // advance ...
    new_dT = min( (double) _dT_by_adp, (_sim->_dT0 + _Tstep)/2 ) ; 
    new_dT = min( (double) new_dT, _sim->_dT0 * step_grow_fact) ; 
    new_dT = max( new_dT,  (double) _tstop ) ; // fmin( get_new_dT(), _Tstep );

    if ( OPT::tracett) 
      _out << "* new step " << new_dT << " ( just accepted: " << _sim->_dT0   << 
        ", adp "<< _dT_by_adp <<" )\n";


    if(new_dT < _dTmin){
      error(bWARNING, "step too small %e %e adp %e\n", new_dT, _dTmin, _dT_by_adp );
      return false;
    }

    // last step handler.
    new_dT = min(new_dT, _Tstop - _Time1 - _tstop );
    if (_sim->_dT1 > 0)
      assert( new_dT < (step_grow_fact+1) * _sim->_dT0 );


    trace3( "TTT::next Time1: ", _Time1 , new_dT, _dTmin );
//    trace0 << "TTT::next @" << _sim->_Time0 << " step " <<_Tstep << " not too small: " << _dTmin <<"\n";
    ::status.review.start();
    ++::status.hidden_steps_tt;
    ++steps_total_tt;
    ::status.review.stop();

  }

  new_dT = max (new_dT, (double) _tstop );
  trace1("TTT::next ", new_dT);
  new_Time0 = _Time1 + new_dT;


  bool another_step=( ( new_dT >= _dTmin  )
                 && ( _Tstop - _Time1 >= _dTmin ) 
                 && (  new_Time0 <= _Tstop - _tstop   ));

  if(!another_step) {
    trace6( "TTT::next no next @ Time0: " , _sim->_Time0,  _sim->_dT0, new_dT, _dTmin, _tstop, new_Time0 );
    trace5( "TTT::next no next @ Time0: " ,\
        _Time1, _Tstop,  new_dT >= _dTmin ,  _Tstop - _Time1 >= _dTmin ,  new_Time0 + _tstop <= _Tstop );
    _sim->_last_Time = _Time1 + _tstop; // FIXME
      
    return (false);
  } else {
    trace3("TTT::next another step ", new_dT, _Time1, _Tstop);
  }

  // new_dT = max(0.0,new_dT);

  _Time1 = _sim->_Time0;
  _sim->_Time0 = new_Time0;

  _sim->_dT3 = _sim->_dT2;
  _sim->_dT2 = _sim->_dT1;
  _sim->_dT1 = _sim->_dT0;
  _sim->_dT0 = new_dT;


  time1 = 0.;

  if ( OPT::tracett) 
    _out << "* exiting next, next at " << _sim->_Time0 << 
      "( steps " << _sim->_dT0  << " " <<  _sim->_dT1 << " " <<  _sim->_dT2 << "\n";
  trace6( "TTT::next: exiting next: " , _sim->tt_iteration_number(), _sim->_Time0, _Time1, _sim->_dT0, _sim->_dT1 , _sim->_dT2 );

  assert( _sim->_Time0 < 1+ _Time1 + _sim->_dT0 );
  assert( _sim->_Time0 >= _sim->_dT0 );
  assert( _sim->_Time0 > 0 );

  //advance_Time();
  _sim->restore_voltages();
  if (another_step && _accepted_tt){
    CARD_LIST::card_list.do_forall( &CARD::tt_next ); // fixme: merge to tt_adv.
  }

  if ( OPT::tracett) 
    _out << "* committing for " << _sim->_Time0 << "(step "  << tt_iteration_number() <<  ")\n";
  

  if ( OPT::tracett) _out << "*exiting next " << another_step << "\n";
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
    cout.flush(); // Absoluter ober Hack (Lars)  wenn man das nicht macht
                  // kommt die Ausgabe ei umleitung in ein File in der Kopfzeile
    // bei z.B. udc(MP1.BTI.R_2_9) durcheinander 
    // cerr << "S_TTT::head_tt: output label" << format << "w" << width << std::endl;
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
    _out << "* no trprint\n";
    return;
  }

  std::string foo="* TTime-----";

  _out << foo;
  {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);
//    _out.form(format, "", "tiime");
    _out << "time      ";
    for (PROBELIST::const_iterator
        p=printlist().begin();  p!=printlist().end();  ++p) {
      _out.form(format, ' ', (*p)->label().c_str());
    }
    _out << '\n';
  }
  _sim->_mode=oldmode;
}

/*--------------------------------------------------------------------------*/
void TTT::outdata_b4(double time)
{
  assert( _sim->_mode  == s_TTT );
  ::status.output.start();
  trace1("TTT::outdata_b4", time);
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

  if (!IO::plotout.any()) {
    _out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);

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
      _out << _sim->_Time0;
      _out << i->first;

      ii=0;
      for (PROBELIST::const_iterator
          p=printlist().begin();  p!=printlist().end();  ++p) {

        _out << myiterators[ii]->second;
        myiterators[ii]++;
        ii++;
      }

        _out << "\n";
    }
    _out << '\n';
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
  assert( _sim->_mode  == s_TTT );
  ::status.output.start();
 // plottr(x, plotlist());
  assert( _sim->_mode  == s_TTT );

  // SIM::alarm();
  _sim->_mode=s_TRAN;
  if ( OPT::printrejected ) { //FIXME
//    TRANSIENT::print_results(x);
  }
  store_results(x);
  _sim->_mode=s_TTT;
  

  _sim->reset_iteration_counter(iPRINTSTEP);
  ::status.hidden_steps = 0;
  ::status.output.stop();
  assert( _sim->_mode  == s_TTT );
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
  // trace0("TTT::store_results()");
  _sim->_mode=s_TRAN;
  int ii = 0;
  for (PROBELIST::const_iterator
	 p=storelist().begin();  p!=storelist().end();  ++p) {
    _sim->_waves[ii++].push(x, (*p)->value());
  }
}
/*--------------------------------------------------------------------------*/
void TTT::advance_Time(void)
{
  static double last_iter_time;
  trace2("TTT::advance_Time()", _sim->_Time0, last_iter_time);
  ::status.tt_advance.start();
  if (_sim->_Time0 > 0) {
    if (_sim->_Time0 > last_iter_time) {	/* moving forward */
      _sim->_tt_iter++;
      _sim->_tt_rejects=0;
      _sim->update_tt_order();

      if (OPT::tracett)
        _out << "* advance_Time to " << _sim->_Time0 << 
          " iteration number " << _sim->tt_iteration_number() << 
          " have "<<  _sim->_adp_nodes << " nodes " << "\n";

      trace2("TTT::advance_Time ", _sim->_tr[0], _sim->_tt[0]);
      trace2("TTT::advance_Time ", _sim->_tr1[0], _sim->_tt1[0]);

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

    }else{				/* moving backward */
      /* don't save voltages.  They're wrong! */
      /* instead, restore a clean start for iteration */
      // notstd::copy_n(_sim->_vt1, _sim->_total_nodes+1, _sim->_v0);
      // CARD_LIST::card_list.tr_regress();
    }
  }else{
    if (OPT::tracett) _out << "* advancing from " << _sim->_Time0 << "\n" ;
    CARD_LIST::card_list.do_forall( &CARD::tt_prepare );
  }
  last_iter_time = _sim->_Time0;
  ::status.tt_advance.stop();
  trace0("TTT::advance_Time done\n");
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

} // namespace
