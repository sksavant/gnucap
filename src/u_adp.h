// vim:ts=8:sw=2:et:
#ifndef U_ADP_H
#define U_ADP_H
#include "s__.h"
#include "e_compon.h"
#include "u_time_pair.h"
#include "io_trace.h"

typedef int adp_region_t;

/*--------------------------------------------------------------------------*/
class ADP_NODE_LIST;
class ADP_LIST;
class ADP_CARD;
/*--------------------------------------------------------------------------*/
// collects transient (stress) data and extrapolates.
class ADP_NODE : CKT_BASE { // better? : NODE
  public:
    explicit ADP_NODE( const COMPONENT* );
    explicit ADP_NODE( const COMPONENT* , std::string name_in2 );//
    explicit ADP_NODE( const COMPONENT* , const char name_in[] );
    explicit ADP_NODE( ADP_CARD* ac, const COMPONENT* cin, const char name_in[] );
    //: dbg(0)
    //  { init(); _c=cin;  name=std::string(name_in); a=ac; }
    ~ADP_NODE( );
    ADP_NODE( const ADP_NODE& );
    virtual void init();
    std::string short_label() const  {return name;}
    std::string label() const;//  {return name;} // fixme
    int order() const; 
    const COMPONENT* c(){return _c;}
  protected:
    int dbg;

    // history needs cleanup.
    hp_float_t _delta_expect;
    hp_float_t tr_value;
    hp_float_t tr_value2, tr_dd12;
    hp_float_t tr_value3, tr_dd23, tr_dd123;
    hp_float_t tt_expect;
    hp_float_t tt_value;

    hp_float_t *_val_bef; // replce _val_bef[0] ... 
    hp_float_t *_val_aft;
    hp_float_t *_der_aft;
    hp_float_t *_delta;

    double tt_value3;
    double tt_first_time;
    double tt_first_value;
    double _Time_delta_old;
    double _abs_tt_err;
    double _abs_tr_err;
    double _rel_tt_err;
    double _rel_tr_err;
    double _wdT;
    bool _positive;
    double _sign;


    double Time_delta()  const
    { return ( CKT_BASE::_sim->_dT0 ); }

    double Time_delta_old()const { return CKT_BASE::_sim->_dT1 ;}

    inline double Time0()const { return ( CKT_BASE::_sim->_Time0 ); }
    inline double dT0()const { return ( CKT_BASE::_sim->_dT0 ); }
    inline double dT1()const { return ( CKT_BASE::_sim->_dT1 ); }
    inline double dT2()const { return ( CKT_BASE::_sim->_dT2 ); }

    void         tr_expect_();
    virtual void tr_expect_1(){ tr_expect_1_const(); }
    void         tr_expect_1_const();
    void         tr_expect_1_exp();

    virtual void tr_expect_2(){ tr_expect_2_linear(); }
    void         tr_expect_2_exp();
    void         tr_expect_2_linear();
    void         tr_expect_2_avg();
    void         tr_expect_2_square();
    void         tr_expect_2_something();

    virtual void tr_expect_3(){ tr_expect_3_quadratic(); }
    void         tr_expect_3_quadratic();
    void         tr_expect_3_linear();
    void         tr_expect_3_something();
    void         tr_expect_3_exp();
    void         tr_expect_3_exp_fit();

    double tt_integrate_( double );

    virtual double tt_integrate_1( double a ) { return tt_integrate_1_const( a ); }
    double tt_integrate_1_const( double );
    double tt_integrate_1_exp( double );
    double tt_integrate_1_linear( double );

    virtual double tt_integrate_2( double a ) { return tt_integrate_2_exp( a ); }
    double tt_integrate_2_exp(double);
    double tt_integrate_2_exp_different(double);
    double tt_integrate_2_linear(double);
    double tt_integrate_2_linear2(double);

    virtual double tt_integrate_3( double a ) { return tt_integrate_3_exp( a ); }
    double tt_integrate_3_exp(double);

    double tr_correct_3_exp( );
    double tr_correct_2_exp( );
    double tr_correct_1_exp( );
    double tr_correct_1_const( );
    double tr_correct_generic( );

    std::string name;
    const COMPONENT* _c;
    hp_float_t _debug;
    double (ADP_NODE::*_integrator)( double );
    double (ADP_NODE::*_corrector)( );
    int _region;

  public:
    void apply(); // ?
    virtual int region() const;
    virtual std::string type() const {return "basic";};

    hp_float_t get_total() const;
    hp_float_t get() const {return get_tt();}
    void tt_set(double);
    hp_float_t get_tt() const {
      if ( tt_value == tt_value )       return tt_value; 
      trace1(( "no tt_value" + short_label()).c_str(),tt_value);
      assert(false);
    }
    hp_float_t get0()const { assert( _val_bef[0] == _val_bef[0] ); return _val_bef[0]; }
    hp_float_t get1()const { assert( _val_bef[1] == _val_bef[1] ); return _val_bef[1]; }
    hp_float_t get_aft_1()const;
    hp_float_t tt_get_sum()const  {return _val_bef[0] + _delta[0]; }
    void tr_add(double x );//{tr_value += x;}
    void tr_set(double x ); //{tr_value = x;}
    TIME_PAIR tt_review( );
    TIME_PAIR tt_preview( );
    void reset();
    void reset_tr();
    hp_float_t tr_get( ) const; // { return ( tr_value );}
    hp_float_t get_tr( ) const { return tr_get();}
    double tr_get_old(){ return (_delta[1]);}
    double tr_abs_err()const{ return _abs_tr_err; }
    double tr_rel_err()const{ return _rel_tr_err; }
    double tt_abs_err()const{ return _abs_tt_err; }
    double tt_rel_err()const{ return _rel_tt_err; }
    double tr_duration()const; //{ return _c->_sim->_last_time; }
    double wdT()const{ return _wdT; }
    void tt_commit();
    void tr_stress_last( double );
    void tt_reset();
    void tt_commit_first();
    void tt_accept_first();
    void tt_advance();
    void tt_last();

    void tt_accept( );
    void tt_clear( ) { tr_value = _delta[0] = _delta[1] = tr_value2 = 0;
                       tt_value = _val_bef[0] = _val_bef[1] = _val_bef[2] = 0;
    }

    void print()
    {
      std::cerr << name << ": " << tr_value << "\n";		
    }

  private:
    int _order; // order used for extrapolation.
    ADP_CARD* a;
  public:
    virtual double debug();
};
/*--------------------------------------------------------------------------*/
class BTI_ADP : public ADP_NODE {
        // nonsense   ?
};
/*--------------------------------------------------------------------------*/
class ADP_NODE_RCD : public ADP_NODE {
 public:
   ADP_NODE_RCD( const COMPONENT*x): ADP_NODE(x) { name+=" (rcd)"; }
   ADP_NODE_RCD( const ADP_NODE& );

   // virtual ADP_NODE(){ return ADP_NODE_RCD(*this); };
   void tr_expect_1();
   void tr_expect_2();
   void tr_expect_3();
   virtual std::string type() const {return "rcd";};
};
/*--------------------------------------------------------------------------*/
// ADP card is only a stub...
class ADP_CARD  : public CARD {
// manages stress data and stores device parameters.
  private:
    explicit ADP_CARD(const ADP_CARD&); 
    static int _tt_order;
    double _wdT;
  public:
    explicit ADP_CARD(const COMPONENT*) {}
    explicit ADP_CARD();
    virtual ~ADP_CARD() {}
    virtual void init(const COMPONENT*) {}

    virtual double tr_probe_num(const std::string& )const { unreachable(); return 888; }
    virtual double tt_probe_num(const std::string& )const { unreachable(); return 888; }
    virtual std::string value_name() const{ return "unknown";}


    virtual void tt_commit_first(){ }
    virtual void tt_commit(){ }
    virtual void tt_last(){ }
    virtual void tt_accept_first(){ }
    virtual void tr_commit_first(){ }
    virtual void tr_accept(){ }
    virtual void tt_accept(){ }
    virtual double wdT() const {return 999;}
    virtual TIME_PAIR tt_review(){ return TIME_PAIR(); }
    virtual TIME_PAIR tt_preview( ){ unreachable(); return TIME_PAIR(); }
};
/*--------------------------------------------------------------------------*/
#endif
