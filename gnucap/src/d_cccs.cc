/*$Id: d_cccs.cc,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
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
 * functions for cccs
 * It is really voltage controlled, taking into account the sense element.
 * Then adjust the gain to account for the sense element.
 */
//testing=script,complete 2008.10.09
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CCCS : public CCSRC_BASE {
private:
  explicit DEV_CCCS(const DEV_CCCS& p) :CCSRC_BASE(p) {}
public:
  explicit DEV_CCCS()		:CCSRC_BASE() {}
private: // override virtual
  char	   id_letter()const	{return 'F';}
  std::string value_name()const {return "gain";}
  std::string dev_type()const	{return "cccs";}
  //int	   max_nodes()const	//CCSRC_BASE=4
  //int	   num_nodes()const	//CCSRC_BASE=4
  //int	   min_nodes()const	//CCSRC_BASE=4
  //int	   matrix_nodes()const	//CCSRC_BASE=4
  //int	   net_nodes()const	//CCSRC_BASE=2
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_CCCS(*this);}
  //void   expand();		//CCSRC_BASE
  void     precalc();
  //void   map_nodes();		//ELEMENT

  void	   tr_iwant_matrix()	{tr_iwant_matrix_active();}
  void     tr_begin();
  //void   tr_restore();	//ELEMENT
  //void   dc_advance();	//ELEMENT
  //void   tr_advance();	//ELEMENT
  //void   tr_regress();	//ELEMENT
  //bool   tr_needs_eval();	//CCSRC_BASE
  //void   tr_queue_eval();	//ELEMENT
  bool     do_tr()		{SIM::late_evalq.push_back(this); return true;}
  bool	   do_tr_last();
  void	   tr_load()		{tr_load_active();}
  //TIME_PAIR tr_review();	//ELEMENT
  //void   tr_accept();		//CARD/nothing
  //void   tr_unload();		//CCSRC_BASE
  //double tr_involts()const;	//CCSRC_BASE
  //double tr_input()const	//CCSRC_BASE
  //double tr_involts_limited()const;	//CCSRC_BASE
  //double tr_input_limited()const //CCSRC_BASE
  //double tr_amps()const	//ELEMENT
  //double tr_probe_num(const std::string&)const;//ELEMENT

  void	    ac_iwant_matrix()	{ac_iwant_matrix_active();}
  void	    ac_begin()		{_ev = _y[0].f1;}
  void	    do_ac();
  void	    ac_load()		{ac_load_active();}
  //COMPLEX ac_involts()const;	//CCSRC_BASE
  //COMPLEX ac_amps()const;	//ELEMENT
  //XPROBE  ac_probe_ext(const std::string&)const;//ELEMENT

  std::string port_name(int i)const {untested();
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"sink", "src"};
    return names[i];
  }
  std::string current_port_name(int i)const {untested();
    assert(i >= 0);
    assert(i < 1);
    static std::string names[] = {"in"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_CCCS::precalc()
{
  CCSRC_BASE::precalc();
  set_converged();
  assert(!is_constant()); /* because of incomplete analysis */
}
/*--------------------------------------------------------------------------*/
void DEV_CCCS::tr_begin()
{
  CCSRC_BASE::tr_begin();
  _m0.x  = _y[0].x;
  _m0.c1 = _y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
}
/*--------------------------------------------------------------------------*/
bool DEV_CCCS::do_tr_last()
{
  assert(_input);
  if (using_tr_eval()) {
    _m0.x = tr_involts_limited();
    _y[0].x = tr_input_limited();
    tr_eval();
    assert(_y[0].f0 != LINEAR);
    _m0 = CPOLY1(_y[0]);
  }else{
    assert(_y[0].f0 == LINEAR);
    assert(_y[0].f1 == value());
    _m0.c0 = 0.;
    assert(converged());
  }
  if (_input->has_inode()) {untested();
    // nothing
  }else if (_input->has_iv_probe()) {
    _m0.c0 += _y[0].f1 * _input->_m0.c0;
    _m0.c1  = _y[0].f1 * (_input->_loss0 + _input->_m0.c1);
  }else{unreachable();
  }
  store_values();
  q_load();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CCCS::do_ac()
{
  assert(_input);
  if (!_input->evaluated()) {	/* patch for forward reference */
    ELEMENT* input = const_cast<ELEMENT*>(_input);
    input->do_ac();		/* make sure sense elt is evaluated first */
  }else{
  }
  if (using_ac_eval()) {
    ac_eval();
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  if (_input->is_source()) {	/* if the sense elt is a fixed source.. */
    _acg = _ev * _input->_acg;	/* then part of this one can be modeled */
    ac_load_source();		/* as a fixed source. ...		*/
    _acg = _ev * _input->_loss0;/* so load it in 2 pieces		*/
  }else if (_input->has_inode()) {untested();
    _acg = _ev;
  }else if (_input->has_iv_probe()) {
    _acg = _ev * _input->_acg;
  }else{unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CCCS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "F|cccs", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
