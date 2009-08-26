/*$Id: d_vs.cc,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
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
 * functions for fixed voltage sources
 * temporary kluge: it has resistance
 */
//testing=script 2006.07.17
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_VS : public ELEMENT {
private:
  explicit DEV_VS(const DEV_VS& p) :ELEMENT(p) {}
public:
  explicit DEV_VS()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const	{return 'V';}
  std::string value_name()const {return "dc";}
  std::string dev_type()const	{return "vsource";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   is_source()const	{return true;}
  bool	   f_is_value()const	{return true;}
  bool	   has_iv_probe()const  {return true;}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_VS(*this);}
  //void   expand();		//COMPONENT
  void     precalc();
  //void   map_nodes();		//ELEMENT

  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  void	   tr_begin();
  //void   tr_restore();	//ELEMENT
  //void   dc_advance();	//ELEMENT
  //void   tr_advance();	//ELEMENT
  //void   tr_regress();	//ELEMENT
  //bool   tr_needs_eval();	//ELEMENT
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_shunt(); tr_load_source();}
  //TIME_PAIR tr_review();	//ELEMENT
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{untested();tr_unload_source();}
  double   tr_involts()const	{return 0.;}
  //double tr_input()const	//ELEMENT
  double   tr_involts_limited()const {unreachable(); return 0.;}
  //double tr_input_limited()const //ELEMENT
  //double tr_amps()const	//ELEMENT
  //double tr_probe_num(const std::string&)const;//ELEMENT

  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void	   ac_begin()	{_loss1 = _loss0 = 1./OPT::shortckt; _acg = _ev = 0.;}
  void	   do_ac();
  void	   ac_load()		{ac_load_shunt(); ac_load_source();}
  COMPLEX  ac_involts()const	{return 0.;}
  COMPLEX  ac_amps()const	{return (_acg + ac_outvolts()*_loss0);}
  //XPROBE ac_probe_ext(const std::string&)const;//ELEMENT

  std::string port_name(int i)const {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_VS::precalc()
{
  ELEMENT::precalc();
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_VS::tr_begin()
{
  ELEMENT::tr_begin();
  _y1.f0 = _y[0].f0 = 0.; // override
  _loss1 = _loss0 = 1./OPT::shortckt;
  _m0.x  = 0.;
  _m0.c0 = -_loss0 * _y[0].f1;
  _m0.c1 = 0.;
  _m1 = _m0;    
  if (!using_tr_eval()) {
    if (_n[OUT2].m_() == 0) {
      set_limit(value());
    }else if (_n[OUT1].m_() == 0) {untested();
      set_limit(-value());
    }else{untested();
      //BUG// don't set limit
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_VS::do_tr()
{
  assert(_m0.x == 0.);
  if (using_tr_eval()) {
    _y[0].x = SIM::time0;
    tr_eval();
    if (_n[OUT2].m_() == 0) {
      set_limit(_y[0].f1);
    }else if (_n[OUT1].m_() == 0) {
      set_limit(-_y[0].f1);
    }else{
      //BUG// don't set limit
    }
    store_values();
    q_load();
    _m0.c0 = -_loss0 * _y[0].f1;
    assert(_m0.c1 == 0.);
  }else{itested();
    assert(conchk(_loss0, 1./OPT::shortckt));
    assert(_y[0].x == 0.);
    assert(_y[0].f0 == 0.);
    assert(_y[0].f1 == value());
    assert(_m0.x == 0.);
    assert(conchk(_m0.c0, -_loss0 * _y[0].f1));
    assert(_m0.c1 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VS::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = -_loss0 * _ev;
  }else{itested();
    assert(_acg == 0.);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_VS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "V|vsource", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
