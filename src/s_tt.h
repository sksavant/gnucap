
#ifndef S_TT_H
#define S_TT_H

#include "s_tr.h"
#include "u_probe.h"
#include "u_prblst.h"

namespace TT {

class TTT : public TRANSIENT {
  public:
    void	do_it(CS&, CARD_LIST*);
    std::string status()const;

    explicit TTT():
      TRANSIENT(),
      _trace(tNONE),
      _Tstart(0.), // ?? unused?
      _Tstop(0.),
      _Tstep(0.),
      _timesteps(0),
      _fdata_tt(NULL),
      _tt_store(0)
  { }
    ~TTT() {}
    void tt_advance_time();
  public:
    int step_tt_cause()const;
    void	advance_Time();

  private:
    static OMSTREAM mstdout;
    OMSTREAM _out;
    explicit TTT(const TTT&): 
      TRANSIENT(),
      _trace(tNONE),
      _Tstart(0.),
      _Tstop(0.),
      _Tstep(0.),
      _timesteps(0),
      _fdata_tt(NULL)
    {incomplete();}
    void	setup(CS&);	/* s_fo_set.cc */
    void	allocate();
    void  rescale_behaviour();
    void	unallocate();
    bool	next();
    void	out_tt();
    void	outdata_tt(double);
    void	outdata_b4(double);
    bool	review_tt();
    void tt_begin();
    void	options(CS&);
    void	sweep();
	 void do_initial_dc();
    void	sweep_tt();
    double get_new_dT();
    void	accept_tt();
    void	print_head_tr();
    void	head(double,double,const std::string&);
    void	head_tt(double,double,const std::string&);
    void	set_step_tt_cause(STEP_CAUSE);
    void	first();
    void after_interruption_prep();
    void first_after_interruption();
    void	fohead(const PROBE&);
    void	store_results(double); // override virtual
    void	store_results_tt(double); 
    void	print_stored_results_tt(double); 
    void	power_down(double ); 
  private:
	 bool conchk() const;
    TRACE _trace;		// enum: show extended diagnostics
    bool _power_down;
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
    double time_by_voltages();
    double _time_by_adp;
    double _dT_by_adp;
    double _time_by_beh;
    double _dT_by_beh;
    double _dTmin;
    double _dTmax;
    void   outdata(double);
    void   print_results(double); // stupid?
    void   print_results_tr(double);
    void   print_results_tt(double);
    double _Time_by_user_request;
    bool   _accepted_tt;
    COMPLEX** _fdata_tt;	/* storage to allow postprocessing */
    double*   _tt_store;
	 PROBELIST oldstore; //save tr_store (which is abused for caching/measurements)
}; // TTT : TRANSIENT

}
#endif
