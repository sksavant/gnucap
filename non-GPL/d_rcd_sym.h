/* vim:ts=8:sw=2:et:
 *
 * (c)2010 felix salfelder
 * nonGPL ?
 */


#ifndef D_RCD_SYM_H_INCLUDED
#define D_RCD_SYM_H_INCLUDED

#include "d_rcd.h"

class DEV_BUILT_IN_RCD;

class MODEL_BUILT_IN_RCD_SYM : public MODEL_BUILT_IN_RCD {
  protected:
    explicit MODEL_BUILT_IN_RCD_SYM(const MODEL_BUILT_IN_RCD_SYM& p);
  public:
    explicit MODEL_BUILT_IN_RCD_SYM(const BASE_SUBCKT* p);
    // ~MODEL_BUILT_IN_RCD_SYM() : ~MODEL_BUILT_IN_RCD {}
    bool use_net() const { return(0); }
    void do_stress_apply( COMPONENT* d ) const;
    void do_tr_stress( const COMPONENT*) const;        
    std::string dev_type()const ;
    void      set_dev_type(const std::string& nt )
    {
      assert(&nt);
     trace0(("MODEL_BUILT_IN_RCD_SYM::set_dev_type() " + nt).c_str()); 
    };
    CARD* clone()const {return new MODEL_BUILT_IN_RCD_SYM(*this);}
    void do_expand(const COMPONENT*);

};
/*--------------------------------------------------------------------------*/
class DEV_BUILT_IN_RCD_SYM : public DEV_BUILT_IN_RCD{
  explicit DEV_BUILT_IN_RCD_SYM(const DEV_BUILT_IN_RCD_SYM& p);
  public:
  explicit DEV_BUILT_IN_RCD_SYM();
  private:
  TIME_PAIR  tr_review(){ return BASE_SUBCKT::tr_review();}
  void expand();
  void    tr_stress() const;        
};

/*--------------------------------------------------------------------------*/
#endif
