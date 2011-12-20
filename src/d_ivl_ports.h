/*$Id: d_logic.h,v 1.4 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
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
 * data structures and defaults for logic ports.
 *
 *  FIXME: - merge into d_vvp.h
 *         - remove unchanged members
 *         - make obsolete(?)
 */
//testing=script,sparse 2006.07.17
#ifndef D_LOGIC_H_IVL
#define D_LOGIC_H_IVL
#include "e_model.h"
#include "e_elemnt.h"
#include "d_logic.h"
// #include "vvp/vpi_priv.h"
#include "vpi_user.h"
/*--------------------------------------------------------------------------*/
class DEV_LOGIC_AD : public DEV_LOGIC {
public:
  enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
private:
  int		_lastchangenode;
  int		_quality;
  std::string	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
  node_t	nodes[5];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
public:
  explicit	DEV_LOGIC_AD();
  explicit	DEV_LOGIC_AD(const DEV_LOGIC_AD& p);
		~DEV_LOGIC_AD() {--_count; }
private: // override virtuals
  char	   id_letter()const	{return '\0';}
  std::string value_name()const	{return "#";}
  bool	      print_type_in_spice()const {return true;}
  std::string dev_type()const {assert(has_common());
    return (common()->modelname() + " " + common()->name()).c_str();}
  uint_t	   tail_size()const	{return 2;}
  uint_t	   max_nodes()const	{return 5;}
  uint_t	   min_nodes()const	{return 5;}
  virtual uint_t matrix_nodes()const	{return 2;} // for OUTPORT...
  uint_t	   net_nodes()const	{return 5;}
  CARD*	   clone()const		{return new DEV_LOGIC_AD(*this);}
  void	   precalc_first() {ELEMENT::precalc_first(); if (subckt()) {subckt()->precalc_first();}}
  void	   expand();
  void	   precalc_last() {ELEMENT::precalc_last(); if (subckt()) {subckt()->precalc_last();}}
  //void   map_nodes();

  void	   tr_iwant_matrix();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  void	   tr_regress();
  bool	   tr_needs_eval()const;
  void	   tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  TIME_PAIR tr_review();
  void     tr_review_digital();
  void	   tr_accept();
  void	   tr_unload();
  hp_float_t   tr_involts()const		{unreachable(); return 0;}
  //double tr_input()const		//ELEMENT
  hp_float_t   tr_involts_limited()const	{unreachable(); return 0;}
  //double tr_input_limited()const	//ELEMENT
  //double tr_amps()const		//ELEMENT
  double   tr_probe_num(const std::string&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac()	{untested();  assert(subckt());  subckt()->do_ac();}
  void	   tt_advance();
  void	   ac_load()	{untested();  assert(subckt());  subckt()->ac_load();}
  XPROBE   ac_probe_ext(const std::string&)const;

  std::string port_name(uint_t)const {untested();
    incomplete();
    return "";
  }
public:
  static int count()			{return _count;}
  LOGICVAL lvtoivl;
  vpiHandle H;
private:
  bool	   tr_eval_digital();
  bool	   want_analog()const;
  bool	   want_digital()const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class DEV_LOGIC_DA : public DEV_LOGIC {
public:
  enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
private:
  int		_lastchangenode;
  unsigned	_lastchangeiter;
  int		_quality;
  std::string	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
  node_t	nodes[5];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
public:
  explicit	DEV_LOGIC_DA();
  explicit	DEV_LOGIC_DA(const DEV_LOGIC_DA& p);
		~DEV_LOGIC_DA()		{--_count;
                trace0("~DEV_LOGIC_DA");
                }
private: // override virtuals
  char	   id_letter()const	{return '\0';}
  std::string value_name()const	{return "#";}
  bool	      print_type_in_spice()const {return true;}
  std::string dev_type()const {assert(has_common());
    return (common()->modelname() + " " + common()->name()).c_str();}
  uint_t	   tail_size()const	{return 2;}
  uint_t	   max_nodes()const	{return 5;}
  uint_t	   min_nodes()const	{return 5;}
  virtual uint_t matrix_nodes()const	{return 2;} // for OUTPORT...
  uint_t	   net_nodes()const	{return 5;}
  CARD*	   clone()const		{return new DEV_LOGIC_DA(*this);}
  void	   precalc_first() {ELEMENT::precalc_first(); if (subckt()) {subckt()->precalc_first();}}
  void	   expand();
  void	   precalc_last() {ELEMENT::precalc_last(); if (subckt()) {subckt()->precalc_last();}}
  //void   map_nodes();

  void	   tr_iwant_matrix();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  void	   tr_regress();
  bool	   tr_needs_eval()const;
  void	   tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  TIME_PAIR tr_review();
  void	   tr_accept();
  void	   tr_unload();
  double   tr_probe_num(const std::string&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac()	{untested();  assert(subckt());  subckt()->do_ac();}
  void	   tt_advance();
  void	   ac_load()	{untested();  assert(subckt());  subckt()->ac_load();}
  XPROBE   ac_probe_ext(const std::string&)const;

  std::string port_name(uint_t)const {untested();
    incomplete();
    return "";
  }
public:
  static int count()			{return _count;}
//  vpiHandle H; // ?
  LOGICVAL lvfromivl;
  unsigned edge(int, unsigned delay=0);
  void	   qe(); //{tr_queue_eval();}
private:
  bool	   tr_eval_digital();
  bool	   want_analog()const;
  bool	   want_digital()const;
};
#endif
