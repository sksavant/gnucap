/* vim:ts=8:sw=2:et:
 *
 * (c)2010 felix salfelder
 * nonGPL ?
 */

#ifndef D_RCD_SYM_V2_H_INCLUDED
#define D_RCD_SYM_V2_H_INCLUDED

#include "d_rcd_sym.h"

class DEV_BUILT_IN_RCD;

class MODEL_BUILT_IN_RCD_SYM_V2 : public MODEL_BUILT_IN_RCD_SYM {
  protected:
    explicit MODEL_BUILT_IN_RCD_SYM_V2(const MODEL_BUILT_IN_RCD_SYM_V2& p);
  public:
    explicit MODEL_BUILT_IN_RCD_SYM_V2(const BASE_SUBCKT* p);
//    virtual void     do_tt_prepare(COMPONENT*)const;
    virtual void     do_precalc_last(COMMON_COMPONENT*, const
        CARD_LIST*)const;
    virtual bool v2() const{return true;}
    // ~MODEL_BUILT_IN_RCD_SYM_V2() : ~MODEL_BUILT_IN_RCD {}
    bool use_net() const { return(0); }
    void do_stress_apply( COMPONENT* d ) const;
    void do_tr_stress( const COMPONENT*) const;        
    std::string dev_type()const ;
    void      set_dev_type(const std::string& nt )
    {
      assert(&nt);
     trace0(("MODEL_BUILT_IN_RCD_SYM_V2::set_dev_type() " + nt).c_str()); 
    };
    CARD* clone()const {return new MODEL_BUILT_IN_RCD_SYM_V2(*this);}
    void do_expand( COMPONENT*) const;
    ADP_NODE_RCD* new_adp_node(const COMPONENT*) const;
//    region_t region(const COMPONENT*) const;
    int  tt_region(const COMPONENT*) const;
    double __Re(double uin, const COMMON_COMPONENT* cc)const;
    double __Rc(double uin, const COMMON_COMPONENT* cc)const;
    double __Ge(double uin, const COMMON_COMPONENT* cc)const;
    double dvth( const COMPONENT* cc)const;
};
/*--------------------------------------------------------------------------*/
class DEV_BUILT_IN_RCD_SYM_V2 : public DEV_BUILT_IN_RCD{
  explicit DEV_BUILT_IN_RCD_SYM_V2(const DEV_BUILT_IN_RCD_SYM_V2& p);
  public:
  explicit DEV_BUILT_IN_RCD_SYM_V2();
  private:
  TIME_PAIR  tr_review(){ return BASE_SUBCKT::tr_review();}
  void expand();
  void    tr_stress();  
  public:
};
/*--------------------------------------------------------------------------*/
#endif
