// vim:ts=8:sw=2:et:
#ifndef U_ADP_H
#define U_ADP_H
#include "s__.h"
#include "e_compon.h"
#include "u_time_pair.h"
#include "io_trace.h"
/*--------------------------------------------------------------------------*/
class ADP_NODE_LIST;
class ADP_LIST;
class ADP_CARD;
/*--------------------------------------------------------------------------*/
// collects transient (stress) data and extrapolates.
class ADP_NODE {
  protected:
    hp_float_t tr_expect;
    hp_float_t tr_value;
    hp_float_t tr_value0;
    hp_float_t tr_value1;
    hp_float_t tr_value2, tr_dd12;
    hp_float_t tr_value3, tr_dd23, tr_dd123;
    hp_float_t tt_expect;
    hp_float_t tt_value;
    hp_float_t tt_value0;
    hp_float_t tt_value1;
    hp_float_t tt_value2;
    double tt_value3;
    double tt_first_time;
    double tt_first_value;
    double _Time_delta_old;
    double _abs_tt_err;
    double _abs_tr_err;
    double _rel_tt_err;
    double _rel_tr_err;
    double _wdT;

    double Time_delta()  const
    { return ( CKT_BASE::_sim->_dT0 ); }

    double Time_delta_old()const { return CKT_BASE::_sim->_dT1 ;}

    inline double dT0()const { return ( CKT_BASE::_sim->_dT0 ); }
    inline double dT1()const { return ( CKT_BASE::_sim->_dT1 ); }
    inline double dT2()const { return ( CKT_BASE::_sim->_dT2 ); }

    virtual void tt_expect0(){ tt_expect0_const(); }
    void tt_expect0_const();
    virtual void tt_expect1(){ tt_expect1_linear(); }
    void tt_expect1_linear();
    void tt_expect1_square();
    void tt_expect1_something();
    virtual void tt_expect2(){ tt_expect2_quadratic(); }
    void tt_expect2_quadratic();
    void tt_expect2_something();
    void tt_expect2_exp();

    std::string name;
    const COMPONENT* _c;
  public:
    std::string short_label() {return name;}
#ifdef DO_TRACE
    virtual double debug();
#endif

    ADP_NODE( ):
      tt_value(0),
      tt_value0(0),
      tt_value1(1)
  { init(); 
  unreachable(); }
    const COMPONENT* c(){return _c;}

    void init();
    explicit ADP_NODE( const COMPONENT* cin2 ) { init(); _c = cin2; }
    explicit ADP_NODE( const COMPONENT* cin3, std::string name_in2 ) { init(); _c=cin3; name=name_in2;}
    explicit ADP_NODE( const COMPONENT* cin, const char name_in[] ) 
      { init(); _c=cin;  name=std::string(name_in); }
    explicit ADP_NODE( ADP_CARD* ac, const COMPONENT* cin, const char name_in[] )
      { init(); _c=cin;  name=std::string(name_in); a=ac; }
    hp_float_t get_total() const { return( get_tr() + get_tt() );}
    hp_float_t get() const {return get_tt();}
    hp_float_t get_tt() const { assert( tt_value == tt_value ); return tt_value; }
    hp_float_t get0()const { assert( tt_value0 == tt_value0 ); return tt_value0; }
    hp_float_t get1()const { assert( tt_value1 == tt_value1 ); return tt_value1; }
    hp_float_t tt_get_sum()const  {return tt_value0 + tr_value0; }
    void tr_add(double x ){tr_value += x;}
    void tr_set(double x ){tr_value = x;}
    double tt_review( );
    TIME_PAIR tt_preview( );
    void reset(); //{tr_value = 0; tt_value1 = 0; tt_value0 =0;}
    void reset_tr() { tr_value = 0;
      trace0(("ADP_NODE::reset_tr() " + short_label()).c_str() );
    }
    hp_float_t tr_get( ) const { return ( tr_value );}
    hp_float_t get_tr( ) const { return ( tr_value );}
    double tr_get_old(){ return (tr_value1);}
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

    void tt_accept( );
    void tt_clear( ) { tr_value = tr_value0 = tr_value1 = tr_value2 = 0;
                       tt_value = tt_value0 = tt_value1 = tt_value2 = 0;
    }

    void print()
    {
      std::cerr << name << ": " << tr_value << "\n";		
    }

  private:
    ADP_CARD* a;
};
/*--------------------------------------------------------------------------*/
class BTI_ADP : public ADP_NODE {

        // nonsense  

};
/*--------------------------------------------------------------------------*/
class ADP_NODE_RCD : public ADP_NODE {
 public:
   ADP_NODE_RCD(COMPONENT*x): ADP_NODE(x) { }

   void tt_expect2() { tt_expect2_exp(); }
        // nonsense  


};
/*--------------------------------------------------------------------------*/
class ADP_CARD {
// manages stress data and stores device parameters.
  private:
    explicit ADP_CARD(const ADP_CARD&) {unreachable();}  
    static int _tt_order;
    double _wdT;
  public:
    explicit ADP_CARD() {unreachable();}
    explicit ADP_CARD(const COMPONENT*) {}
    virtual ~ADP_CARD() {}
    virtual void init(const COMPONENT*) {}

    virtual double tr_probe_num(const std::string& x)const { unreachable(); return 888; }
    virtual double tt_probe_num(const std::string& x)const { unreachable(); return 888; }

    virtual void tt_commit_first(){ }
    virtual void tt_commit(){ }
    virtual void tt_accept_first(){ }
    virtual void tr_commit_first(){ }
    virtual void tr_accept(){ }
    virtual void tt_accept(){ }
    virtual double wdT() const {return 999;}
    virtual double tt_review(){ return 888; }
    virtual TIME_PAIR tt_preview( ){ unreachable(); return TIME_PAIR(); }
};
/*--------------------------------------------------------------------------*/
#endif
