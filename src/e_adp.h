// vim:ts=8:sw=2:et:
#ifndef U_ADP_H
#define U_ADP_H
#include "s__.h"
#include "e_compon.h"
#include "u_time_pair.h"
#include "io_trace.h"

typedef int adp_region_t;
#define E_max 1.0001
#define E_min -0.0001
#define U_min -0.0001
#define U_max 1e5

/*--------------------------------------------------------------------------*/
class ADP_NODE_LIST;
class ADP_LIST;
class ADP_CARD;
/*--------------------------------------------------------------------------*/
// collects transient (stress) data, sometimes extrapolates.
class ADP_NODE: public CKT_BASE {
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
  private:
    int	_number;
  public:
    std::string label() const  {return long_label();}
    int order() const; 

    uint_t	m_()const		{return _number;}

    hp_float_t tr(double time)const;
    hp_float_t& tr()const	{
      assert(m_() >= 0);
      assert(m_() <= _sim->_adp_nodes);
      assert (_sim->_tr);
      return _sim->_tr[m_()];
    }
    hp_float_t tr1( )const	{
      assert(m_() >= 0);
      assert(m_() <= _sim->_adp_nodes);
      assert (_sim->_tr1);
      return _sim->_tr1[m_()];
    }
    hp_float_t tr2( )const	{
      assert(m_() >= 0);
      assert(m_() <= _sim->_adp_nodes);
      return _sim->_tr2[m_()];
    }
    hp_float_t tr3( )const	{
      assert(m_() >= 0);
      assert(m_() <= _sim->_adp_nodes);
      return _sim->_tr3[m_()];
    }

    hp_float_t tt1( )const	{
      assert(m_() >= 0);
      assert(m_() <= _sim->_adp_nodes);
      return _sim->_tt1[m_()];
    }
    hp_float_t&     tt()const	{
      assert(m_() >= 0);
      assert(m_() <= _sim->_adp_nodes);
      assert (_sim->_tt);
      return _sim->_tt[m_()];
    }
    double TR2; //depr.
  protected:
    int dbg;

    // history needs cleanup.
    hp_float_t _delta_expect;
    hp_float_t tr_value;
    double tr_noise;
    float tr_rel_low;
    float tr_rel_high;
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
    inline double Time1()const { return ( CKT_BASE::_sim->_Time0-dT0() ); }
    inline double Time2()const { return ( CKT_BASE::_sim->_Time0-dT0()-dT1() ); }
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

    hp_float_t _debug;
    double (ADP_NODE::*_integrator)( double );
    double (ADP_NODE::*_corrector)( );
    int _region;

  public:
    void apply(); // ?
    virtual int region() const;
    virtual std::string type() const {return "basic";}

    hp_float_t get_total() const;

    hp_float_t get_tr_noise( ) {return tr_noise;}


    hp_float_t get_tt( int i ) const {assert(i==1);
      return( tt1() ); }
    hp_float_t get_tt() const { return tt(); }

    hp_float_t get_aft_1()const;
    hp_float_t tt_get_sum()const  {return _val_bef[0] + _delta[0]; }

    void tr_add( double x ) const { tr() += x;}

    void add_tr( hp_float_t x ) { tr_add(x);}
    void set_tr( hp_float_t x );
    void set_tt( hp_float_t x );

    void set_tr_noise(double x ) {tr_noise = x;}
    virtual TIME_PAIR tt_review( );
    TIME_PAIR tt_preview( );
    void reset();
    void reset_tr();

    //aliases
    hp_float_t get( ) const { return tt();}
    hp_float_t tr_get( ) const { return tr();}
    void tt_set(long double x) { set_tt(x);}
    void tr_set(long double x) { set_tr(x);}
    double tr_get_old(){ return get_tr(1);}
    hp_float_t get1()const { return tt1(); }
    hp_float_t get_tr( int i ) const {assert(i==1); 
      return( tr1() ); }
    hp_float_t get_tr( ) const { return tr();}

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
    void tt_clear( ) {
      unreachable();
    //  tr_value = _delta[0] = _delta[1] = tr_value2 = 0;
     //                  tt_value = _val_bef[0] = _val_bef[1] = _val_bef[2] = 0;
    }

    void print()
    {
      std::cerr << short_label() << ": " << tr_value << "\n";		
    }

  private:
    int _order; // order used for extrapolation.
    ADP_CARD* a;
  public:
    virtual double debug();
    void set_order(uint_t x) {_order=x ;}

  public: // temporary hacks
    double tr_hi;
    double tr_lo;
};
/*--------------------------------------------------------------------------*/
class BTI_ADP : public ADP_NODE {
        // nonsense   ?
};
/*--------------------------------------------------------------------------*/
class ADP_NODE_UDC : public ADP_NODE {
  public:
    explicit ADP_NODE_UDC( const COMPONENT* c);
    ADP_NODE_UDC( const ADP_NODE_UDC& );
    ADP_NODE_UDC( const ADP_NODE& );

    double get_udc() const {return udc;}
    void set_udc(double x) {udc=x;}

   void tr_expect_1();
   void tr_expect_2();
   void tr_expect_3();
   virtual std::string type() const {return "rcd";};
   virtual TIME_PAIR tt_review( );
  private:
    double udc;
};
/*--------------------------------------------------------------------------*/
class ADP_NODE_RCD : public ADP_NODE {
  public:
    explicit ADP_NODE_RCD( const COMPONENT* c);
    ADP_NODE_RCD( const ADP_NODE_RCD& );
    ADP_NODE_RCD( const ADP_NODE& );

    double get_udc() const {return udc;}
    void set_udc(double x) {udc=x;}

   // virtual ADP_NODE(){ return ADP_NODE_RCD(*this); };
   void tr_expect_1();
   void tr_expect_2();
   void tr_expect_3();
   virtual std::string type() const {return "rcd";};
   virtual TIME_PAIR tt_review( );
  private:
    double udc;
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
