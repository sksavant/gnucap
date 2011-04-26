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
 * data structures and defaults for logic model.
 */
//testing=script,sparse 2006.07.17
#ifndef D_DIGITAL_H
#define D_DIGITAL_H
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_GATE = 10};
/*--------------------------------------------------------------------------*/
class DEV_DIGITAL : public ELEMENT {
public:
  enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
private:
  int		_lastchangenode;
  int		_quality;
  std::string	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
  node_t	nodes[PORTS_PER_GATE];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
public:
  explicit	DEV_DIGITAL();
  explicit	DEV_DIGITAL(const DEV_DIGITAL& p);
		~DEV_DIGITAL()		{--_count;}
private: // override virtuals
  char	   id_letter()const	{return 'U';}
  std::string value_name()const	{return "#";}
  bool	      print_type_in_spice()const {return true;}
  std::string dev_type()const {assert(has_common());
    return (common()->modelname() + " " + common()->name()).c_str();}
  int	   tail_size()const	{return 2;}
  int	   max_nodes()const	{return PORTS_PER_GATE;}
  int	   min_nodes()const	{return BEGIN_IN+1;}
  virtual int matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return _net_nodes;}
  CARD*	   clone()const		{return new DEV_DIGITAL(*this);}
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
  hp_float_t   tr_involts()const		{unreachable(); return 0;}
  //double tr_input()const		//ELEMENT
  hp_float_t   tr_involts_limited()const	{unreachable(); return 0;}
  //double tr_input_limited()const	//ELEMENT
  //double tr_amps()const		//ELEMENT
  double   tr_probe_num(const std::string&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac()	{untested();  assert(subckt());  subckt()->do_ac();}
  void	   tt_next();
  void	   ac_load()	{untested();  assert(subckt());  subckt()->ac_load();}
  COMPLEX  ac_involts()const		{unreachable(); return 0.;}
  COMPLEX  ac_amps()const		{unreachable(); return 0.;}
  XPROBE   ac_probe_ext(const std::string&)const;

  std::string port_name(int)const {untested();
    incomplete();
    return "";
  }
public:
  static int count()			{return _count;}
private:
  bool	   tr_eval_digital();
  bool	   want_analog()const;
  bool	   want_digital()const;
};
/*--------------------------------------------------------------------------*/
class MODEL_DIGITAL : public MODEL_CARD {
private:
  explicit	MODEL_DIGITAL(const MODEL_DIGITAL& p);
public:
  explicit MODEL_DIGITAL(const DEV_DIGITAL*);
	   ~MODEL_DIGITAL()		{--_count;}
private: // override virtuals
  std::string	dev_type()const		{return "logic";}
  CARD*		clone()const		{return new MODEL_DIGITAL(*this);}
  void		precalc_first();
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int		param_count()const	{return (13 + MODEL_CARD::param_count());}
public:
  static int	count()			{return _count;}
public:
			/* ----- digital mode ----- */
  PARAMETER<double> delay;	/* propagation delay */
			/* -- conversion parameters both ways -- */
  PARAMETER<double> vmax;	/* nominal volts for logic 1 */
  PARAMETER<double> vmin;	/* nominal volts for logic 0 */
  PARAMETER<double> unknown;	/* nominal volts for unknown (bogus) */
			/* ---- D to A conversion ---- */
  PARAMETER<double> rise;	/* rise time (time in slope) */
  PARAMETER<double> fall;	/* fall time (time in slope) */
  PARAMETER<double> rs; 	/* series resistance -- strong */
  PARAMETER<double> rw; 	/* series resistance -- weak */
			/* ---- A to D conversion ---- */
  PARAMETER<double> th1;	/* threshold for 1 as fraction of range */
  PARAMETER<double> th0;	/* threshold for 0 as fraction of range */
  	        	/* ---- quality judgement parameters ---- */
  PARAMETER<double> mr;		/* margin rise - how much worse rise can be */
  PARAMETER<double> mf;		/* margin fall - how much worse fall can be */
  PARAMETER<double> over;	/* overshoot limit - as fraction of range */
public: // calculated parameters
  double range;			/* vmax - vmin */
private:
  static int _count;
};
/*--------------------------------------------------------------------------*/
class INTERFACE COMMON_DIGITAL : public COMMON_COMPONENT {
protected:
  explicit	COMMON_DIGITAL(int c=0)
    :COMMON_COMPONENT(c), incount(0) {++_count;}
  explicit	COMMON_DIGITAL(const COMMON_DIGITAL& p)
    :COMMON_COMPONENT(p), incount(p.incount) {++_count;}
public:
		~COMMON_DIGITAL()			{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  static  int	count()				{return _count;}
  virtual LOGICVAL logic_eval(const node_t*)const	= 0;
public:
  int		incount;
protected:
  static int	_count;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
class LOGIC_INV : public COMMON_LOGIC {
private:
  explicit LOGIC_INV(const LOGIC_INV& p) :COMMON_LOGIC(p){++_count;}
  COMMON_COMPONENT* clone()const	{return new LOGIC_INV(*this);}
public:
  explicit LOGIC_INV(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t* n)const {
    return ~n[0]->lv();
  }
  virtual std::string name()const	  {return "inv";}
};
--------------------------------------------------------------------------*/
#endif
