
// this is a workaround, to have a cast to DEV_CAPACITANCE in s_sock.cc
//
#include "e_storag.h"
namespace SOME_CAP_HACK 
{
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class DEV_CAPACITANCE : public STORAGE {
protected:
  explicit DEV_CAPACITANCE(const DEV_CAPACITANCE& p) :STORAGE(p) {}
public:
  explicit DEV_CAPACITANCE()	:STORAGE() {}
protected: // override virtual
  char	   id_letter()const	{return 'C';}
  std::string value_name()const {return "c";}
  std::string dev_type()const	{return "capacitor";}
  uint_t	   max_nodes()const	{return 2;}
  uint_t	   min_nodes()const	{return 2;}
  uint_t	   matrix_nodes()const	{return 2;}
  uint_t	   net_nodes()const	{return 2;}
  bool	   has_iv_probe()const  {return true;}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_CAPACITANCE(*this);}
  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  bool	   do_tr();
  void	   tr_accept(); // uic. possibly a hack
  void	   tr_load()		{tr_load_passive();}
//  void	   tr_init(double )		;
  void	   tr_unload()		{tr_unload_passive();}
  hp_float_t   tr_involts()const	{
    if(_n[OUT1].v0() != _n[OUT1].v0()){
      error(bDANGER, "involts %E %s\n", _sim->_time0 , long_label().c_str());
      assert(false);
      //throw(Exception("Error in " + long_label()));
    }
    return tr_outvolts();
  }
  hp_float_t   tr_involts_limited()const {return tr_outvolts_limited();}
  double   tr_probe_num(const std::string&)const;
  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void	   ac_begin()		{_ev = _y[0].f1;}
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{itested();return ac_outvolts();}

  std::string port_name(uint_t i)const {
    assert(i != INVALID_NODE);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};

} // namespace
