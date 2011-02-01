/*$Id: d_res.cc,v 1.20 2010-08-30 09:14:04 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * the Free Software Foundation; either version 3, or (at your option)
 *
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
 * functions for resistor.
 * y.x = amps,  y.f0 = volts, ev = y.f1 = ohms
 * m.x = volts, m.c0 = amps, acg = m.c1 = mhos.
 */
//testing=script,complete 2006.07.17
#include "e_elemnt.h"
#include "e_adp.h"
#include "e_adplist.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class ADP_RESISTANCE : public ADP_CARD {
public:
  explicit ADP_RESISTANCE(const COMPONENT* c);
  void init(const COMPONENT*);
public: 
  ADP_NODE* i_stress;
};
/*--------------------------------------------------------------------------*/
ADP_RESISTANCE::ADP_RESISTANCE(const COMPONENT* c) : ADP_CARD(c)
{

  init(c);
}
/*--------------------------------------------------------------------------*/

class DEV_RESISTANCE : public ELEMENT {
private:
  explicit DEV_RESISTANCE(const DEV_RESISTANCE& p) :ELEMENT(p) { }
public:
  explicit DEV_RESISTANCE()	:ELEMENT() { }
  void tt_prepare();
  double _value_new;

private: // override virtual
  char	   id_letter()const	{return 'R';}
  std::string value_name()const {return "r";}
  std::string dev_type()const	{return "resistor";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   has_iv_probe()const  {return true;}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_RESISTANCE(*this);}
  void   expand() { COMPONENT::expand(); attach_adp(new ADP_RESISTANCE(this)); }
  void     precalc_last();
  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  void     tr_begin();
  bool	   do_tr();
  void	   tr_load()		{tr_load_passive();}
  void	   tr_unload()		{untested();tr_unload_passive();}
  hp_float_t tr_involts()const {return tr_outvolts();}
  hp_float_t tr_input()const   {untested(); return _m0.c0 + _m0.c1 * tr_involts();}
  hp_float_t tr_involts_limited()const {return tr_outvolts_limited();}
  hp_float_t tr_input_limited()const {return _m0.c0+_m0.c1*tr_involts_limited();}
  double tr_probe_num(const std::string&)const;
  double tt_probe_num(const std::string&)const;

  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void     ac_begin()           {_ev = _y[0].f1; _acg = 1. / (COMPLEX) _ev;} 
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{return ac_outvolts();}

  std::string port_name(int i)const {itested();
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }

public:
  void stress_apply();
  // void stress_apply()const { std::cerr<< "DEV_RESISTANCE::stress_apply const?? \n";}
  void tr_stress()const;
};
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::tt_prepare()
{
  ELEMENT::tt_prepare();

  trace0( "DEV_RESISTANCE::tt_prepare" );
  _value_new=value();
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::tr_stress()const
{
  // belastung berechnen:
  // grundlage:
  // 5V - 300 Ohm halten 50 Jahre... -> Belastung ist dann bei 73.000
  // --> bei 1000V wird die Belastung schon nach 6 Minuten erreicht.
  // U: anliegende Spannung
  // R_aktuell: aktueller Widerstand
  // stress_now: neue Belastung
  // stress_gesamt: gesamte Belastung
  double I = tr_amps(); // liefert den das Bauelement duchflossenen Strom 
  //double U;
  // double R_aktuell ;
  double delta_t = _sim->_dt0;
  // double stress_now = delta_t * 0.2 * (U*U*U*3) / R_aktuell;
  double stress_now = delta_t * fabs(I); //0.2 * (U*U*U*3) / R_aktuell;
  // std::cerr << "DEV_RESISTANCE::tr_stress: I :" << fabs(I) << "adding " << stress_now << "\n";
  ((ADP_RESISTANCE*)adp())->i_stress->tr_add(stress_now);
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::stress_apply()
{
	//return; // skip (unfinished)

	double watt_jahre = 1000000000; // entspricht 68 jahre bei 5V und 300 Ohm
        watt_jahre=watt_jahre;
	// double faktor_schlechter = 2; // nach 68 jahren wird der widerstand 10x höher sein
	// R_aktuell: aktueller Widerstand
	// R_anfang: ursprünglicher Widerstand
	double R_aktuell;
	double R_anfang = _value_new;
        double breite=1;
        breite=breite;

        assert(_value_new == _value_new);
	double gesamtstress = ((ADP_RESISTANCE*)adp())->i_stress->get();
        assert (gesamtstress == gesamtstress);
	R_aktuell = R_anfang ; // * exp(gesamtstress/watt_jahre  * breite * R_anfang);
	
	double value_old;
	value_old = R_aktuell; //anfang * exp(SIM::Time0/ 10.0);

        assert (R_aktuell == R_aktuell );

        set_value(value_old);

//        trace(
//        std::cerr << "DEV_RESISTANCE::stress_apply " << "Time " << _sim->_Time0 << " value: " << value() << " at time "  
//          << _sim->_Time0 
//          << " stress " << gesamtstress 
//          << " " << short_label() << ": "
//          << value_old << "new:" <<_value_new << "\n";
        // argh: wie kann man dashier richtig machen?

//        // value = value_0 - stress * 17. oder e(-stress)* v_0
//        _y1.f1 = _y[0].f1 = (newvalue != 0.) ? newvalue : OPT::shortckt;
//        _m0.x  = _y[0].x;  
//        _m0.c1 = 1./_y[0].f1;
//        _m0.c0 = 0.;
//        _m1 = _m0;
//        assert(_loss0 == 0.);
//        assert(_loss1 == 0.);
//        if (newvalue == 0. && !has_common()) {
//          error(bPICKY, long_label() + ": short circuit\n");
//        }else{
//        }
//
        // tr_load_passive(); //MUSS!
        //  do_tr();
        //


        // tr_begin();
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::tr_begin()
{
  ELEMENT::tr_begin();
  _y1.f1 = _y[0].f1 = (value() != 0.) ? value() : OPT::shortckt;
  _m0.x  = _y[0].x;  
  _m0.c1 = 1./_y[0].f1;
  if ( _m0.c1 != _m0.c1 ){
        std::cerr << "DEV_RESISTANCE::tr_begin " << value() << "\n";
    exit(0);
  }
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  if (value() == 0. && !has_common()) {
    error(bPICKY, long_label() + ": short circuit trb\n");
  }else{
  }


  //stress_apply();
}
/*--------------------------------------------------------------------------*/
bool DEV_RESISTANCE::do_tr()
{
  ADP_RESISTANCE* a = (ADP_RESISTANCE*)adp();
  a=a;
  if (using_tr_eval()) {
    untested();
    _m0.x = tr_involts_limited();
    _y[0].x = tr_input_limited();;
    tr_eval();
    assert(_y[0].f0 != LINEAR);
    if (_y[0].f1 == 0.) {
      error(bPICKY, long_label() + ": short circuit tr\n");
      _y[0].f1 = OPT::shortckt;
      set_converged(conv_check());
    }else{
    }
    store_values();
    q_load();
    _m0.c1 = 1./_y[0].f1;
    _m0.c0 = _y[0].x - _y[0].f0 / _y[0].f1;
  }else{
    assert(_y[0].f0 == LINEAR);
    assert(_y[0].f1 == value() || _y[0].f1 == OPT::shortckt); // a value MUSS noch value() bekommen etc
    assert(conchk(_m0.c1, 1./_y[0].f1));
    assert(_m0.c0 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    if ((COMPLEX)_ev == 0.) {
      error(bPICKY, long_label() + ": short circuit\n");
      _ev = OPT::shortckt;
    }else{
    }
    _acg = 1. / (COMPLEX) _ev;
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == hp_float_t(value()) || (COMPLEX)_ev == OPT::shortckt);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_RESISTANCE p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "R|resistor", &p1);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void ADP_RESISTANCE::init(const COMPONENT* c)
{
  const COMMON_COMPONENT* cc = c->common();
  cc=cc;

  i_stress = new ADP_NODE(this, c, "i");

  //ADP_NODE_LIST::adp_node_list.push_back( i_stress );

  // std::cerr << "i_stress@ " << c << "->" << this << "->" << i_stress << "\n";
  
}
/*--------------------------------------------------------------------------*/
double DEV_RESISTANCE::tt_probe_num(const std::string& x)const
{
  ADP_RESISTANCE* a= (ADP_RESISTANCE*) adp();
  if (Umatch(x, "dummy ")) {
    return 18;
  }else if (Umatch(x, "stress ")) {
    return a->i_stress->get();
 } 

    return ELEMENT::tt_probe_num(x);
}
/*--------------------------------------------------------------------------*/
double DEV_RESISTANCE::tr_probe_num(const std::string& x)const
{
  ADP_RESISTANCE* a= (ADP_RESISTANCE*) adp();
  if (Umatch(x, "dummy ")) {
    return 18;
  }else if (Umatch(x, "stress ")) {
    return a->i_stress->tr_get();
 } 

    return ELEMENT::tr_probe_num(x);
}
/*--------------------------------------------------------------------------*/

